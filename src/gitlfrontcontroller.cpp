#include "gitlfrontcontroller.h"
#include "gitlabstractcommand.h"

#include <QCoreApplication>
#include <QThread>
#include <QSharedPointer>
#include <QDebug>
#include "gitlmvcconst.h"
#include "gitlupdateuievt.h"

SINGLETON_PATTERN_IMPLIMENT(GitlFrontController)

GitlFrontController::GitlFrontController()
{
    m_iMaxEvtInQue = 1000;
    m_iConcurrentInFlight = 0;
    m_bAccepting = true;
    m_cThreadPool.setMaxThreadCount(QThread::idealThreadCount());
    subscribeToEvtByName(GITL_EXE_COMMAND_REQUEST_EVENT, MAKE_CALLBACK(GitlFrontController::detonate));

    /// let running background commands finish before the application's objects go away
    if(QCoreApplication::instance() != nullptr)
    {
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         QCoreApplication::instance(), [this]() { shutdown(); });
    }
}

GitlFrontController::~GitlFrontController()
{
    shutdown();
}


bool GitlFrontController::detonate(const GitlEvent& rcEvt)
{
    const GitlIvkCmdEvt* pcCmdRequestEvt = dynamic_cast<const GitlIvkCmdEvt*>(&rcEvt);
    if(pcCmdRequestEvt == nullptr)
    {
        qWarning() << "A command request that is not a GitlIvkCmdEvt was ignored.";
        return true;
    }

    QString strCommandName = pcCmdRequestEvt->getCommandName();
    QHash<QString, GitlExecPolicy>::const_iterator i = m_cPolicyTable.constFind(strCommandName);
    //command not found
    if( i == m_cPolicyTable.constEnd() )
    {
        qWarning() << QString("No matched command name found. %1").arg(strCommandName);
        return true;
    }
    const GitlExecPolicy cPolicy = i.value();

    if(cPolicy.getKind() == GitlExecPolicy::MainThread)
    {
        /// the bus delivers a const event shared by all listeners; commands get a private copy
        GitlIvkCmdEvt cCmdRequestEvt(*pcCmdRequestEvt);
        execute(cPolicy, cCmdRequestEvt);
    }
    else
    {
        QString strError = submit(cPolicy, std::make_shared<GitlIvkCmdEvt>(*pcCmdRequestEvt));
        if(!strError.isEmpty())
            reportRejected(strCommandName, strError);
    }
    return true;
}

void GitlFrontController::execute(const GitlExecPolicy& rcPolicy, GitlIvkCmdEvt& rcEvt)
{
    if(rcPolicy.isExclusive())
    {
        std::lock_guard<std::recursive_mutex> cCmdExeMutexLocker(m_cCmdExeMutex);
        onCommandRequestArrive(rcEvt);
    }
    else
    {
        onCommandRequestArrive(rcEvt);
    }
}

QString GitlFrontController::submit(const GitlExecPolicy& rcPolicy, const std::shared_ptr<GitlIvkCmdEvt>& pcEvt)
{
    std::function<void()> cTask = [this, rcPolicy, pcEvt]() { execute(rcPolicy, *pcEvt); };

    std::lock_guard<std::mutex> cLocker(m_cQueueMutex);
    if(!m_bAccepting)
        return QStringLiteral("the front controller is shut down");

    if(rcPolicy.getKind() == GitlExecPolicy::Concurrent)
    {
        if(m_iConcurrentInFlight >= m_iMaxEvtInQue)
            return QStringLiteral("too many concurrent requests pending");
        ++m_iConcurrentInFlight;
        m_cThreadPool.start([this, cTask]()
        {
            cTask();
            std::lock_guard<std::mutex> cLocker(m_cQueueMutex);
            --m_iConcurrentInFlight;
        });
        return QString();
    }

    /// serial queues: the Worker queue, or a named one
    const QString strKey = rcPolicy.getKind() == GitlExecPolicy::WorkerQueue
            ? QString() : QStringLiteral("queue:") + rcPolicy.getQueueName();
    SerialQueue& rcQueue = m_cSerialQueues[strKey];
    if(rcQueue.cTasks.size() >= m_iMaxEvtInQue)
        return QStringLiteral("the command queue is full");
    rcQueue.cTasks.enqueue(cTask);
    if(!rcQueue.bRunning)
    {
        rcQueue.bRunning = true;
        m_cThreadPool.start([this, strKey]() { runNextInQueue(strKey); });
    }
    return QString();
}

void GitlFrontController::runNextInQueue(const QString& strQueueKey)
{
    std::function<void()> cTask;
    {
        std::lock_guard<std::mutex> cLocker(m_cQueueMutex);
        SerialQueue& rcQueue = m_cSerialQueues[strQueueKey];
        if(rcQueue.cTasks.isEmpty())
        {
            rcQueue.bRunning = false;
            return;
        }
        cTask = rcQueue.cTasks.dequeue();
    }

    cTask();

    /// one task per pool slot, so a busy queue does not keep a thread from the others
    std::lock_guard<std::mutex> cLocker(m_cQueueMutex);
    SerialQueue& rcQueue = m_cSerialQueues[strQueueKey];
    if(rcQueue.cTasks.isEmpty())
        rcQueue.bRunning = false;
    else
        m_cThreadPool.start([this, strQueueKey]() { runNextInQueue(strQueueKey); });
}

void GitlFrontController::reportRejected(const QString& strCommandName, const QString& strReason)
{
    qWarning() << QString("Command request '%1' rejected: %2").arg(strCommandName, strReason);
    GitlUpdateUIEvt cRejectedEvt;
    cRejectedEvt.setParameter("command_name", strCommandName);
    cRejectedEvt.setParameter(GITL_COMMAND_ERROR_PARAM, strReason);
    cRejectedEvt.dispatch();
}

void GitlFrontController::onCommandRequestArrive(GitlIvkCmdEvt &rcEvt)
{
    GitlUpdateUIEvt cRefreshUIEvt;
    GitlCommandParameter& rcRequest = rcEvt.getParameters();
    GitlCommandParameter& rcRespond = cRefreshUIEvt.getParameters();

    // find command by name
    QString strCommandName = rcEvt.getCommandName();
    rcRespond.setParameter("command_name", strCommandName);
    QHash<QString, QMetaObject*>::iterator i = m_cCommandTable.find(strCommandName);
    if( i != m_cCommandTable.end() )
    {
        //command name matched
        //create command, on the thread that runs it
        const QMetaObject* pMetaObj = i.value();
        QObject* pObj = pMetaObj->newInstance();
        //if fail to create command class
        if(pObj == NULL)
        {
            qCritical() << QString("Unable to create command class '%1'! Please ensure the constructor have Q_INVOKABLE macro.")
                        .arg(pMetaObj->className());
            return;
        }
        QSharedPointer<GitlAbstractCommand> pCmd(static_cast<GitlAbstractCommand *>(pObj));
        //execute command
        if( pCmd->execute(rcRequest, rcRespond) == false )
        {
            qDebug() << QString("%1 Execution Failed!").arg(pMetaObj->className());
        }
        else
        {
            cRefreshUIEvt.dispatch();
            qDebug() << QString("%1 Execution Success!").arg(pMetaObj->className());
        }
        return;

    }
    qWarning() << QString("No matched command name found. %1").arg(strCommandName);
}


bool GitlFrontController::checkCommandClass(const QString& strCommandName, const QMetaObject *pMetaObject,
                                            bool *pbInWorkerThread)
{
    if(m_cCommandTable.contains(strCommandName))
    {
        qCritical() << QString("%1 Register Fail! Duplicated command name!").arg(strCommandName);
        return false;
    }

    QString strAbsCmdClassName = GitlAbstractCommand::staticMetaObject.className();
    if( pMetaObject->className() == strAbsCmdClassName )
    {
        qCritical() << QString("Trying to register name '%1' for abstract class '%2'! "
                               "You may miss the Q_OBJECT marco for the derived class of '%3'")
                       .arg(strCommandName).arg(strAbsCmdClassName).arg(strAbsCmdClassName);
        return false;
    }

    /// create one now, so a missing Q_INVOKABLE fails at registration, not at the first request
    QObject* pObj = pMetaObject->newInstance();
    if(pObj == nullptr)
    {
        qCritical() << QString("Unable to create command class '%1'! Please ensure the constructor have Q_INVOKABLE macro.")
                    .arg(pMetaObject->className());
        return false;
    }
    *pbInWorkerThread = static_cast<GitlAbstractCommand *>(pObj)->getInWorkerThread();
    delete pObj;
    return true;
}

bool GitlFrontController::registerCommand(const QString cCommandName, const QMetaObject *pMetaObject)
{
    bool bInWorkerThread = false;
    if(!checkCommandClass(cCommandName, pMetaObject, &bInWorkerThread))
        return false;

    m_cCommandTable.insert(cCommandName, const_cast<QMetaObject *>(pMetaObject));
    m_cPolicyTable.insert(cCommandName, bInWorkerThread ? GitlExec::Worker : GitlExec::Main);
    qDebug() << QString("%1 Register Success!").arg(pMetaObject->className());
    return true;
}

bool GitlFrontController::registerCommand(const QString& strCommandName, const QMetaObject *pMetaObject,
                                          const GitlExecPolicy& rcPolicy)
{
    if(rcPolicy.getKind() == GitlExecPolicy::SerialQueue && rcPolicy.getQueueName().isEmpty())
    {
        qCritical() << QString("%1 Register Fail! A command queue needs a name.").arg(strCommandName);
        return false;
    }

    bool bInWorkerThread = false;
    if(!checkCommandClass(strCommandName, pMetaObject, &bInWorkerThread))
        return false;

    m_cCommandTable.insert(strCommandName, const_cast<QMetaObject *>(pMetaObject));
    m_cPolicyTable.insert(strCommandName, rcPolicy);
    qDebug() << QString("%1 Register Success!").arg(pMetaObject->className());
    return true;
}

void GitlFrontController::unregisterAllCommand()
{
    m_cCommandTable.clear();
    m_cPolicyTable.clear();
}

GitlExecPolicy GitlFrontController::getExecPolicy(const QString& strCommandName) const
{
    return m_cPolicyTable.value(strCommandName, GitlExec::Main);
}

void GitlFrontController::setMaxThreadCount(int iCount)
{
    m_cThreadPool.setMaxThreadCount(iCount);
}

int GitlFrontController::getMaxThreadCount() const
{
    return m_cThreadPool.maxThreadCount();
}

bool GitlFrontController::waitForDone(int iMsecs)
{
    return m_cThreadPool.waitForDone(iMsecs);
}

bool GitlFrontController::shutdown(int iMsecs)
{
    {
        std::lock_guard<std::mutex> cLocker(m_cQueueMutex);
        m_bAccepting = false;
        for(SerialQueue& rcQueue : m_cSerialQueues)
            rcQueue.cTasks.clear();
    }
    m_cThreadPool.clear();      /// drop requests that have not started
    return m_cThreadPool.waitForDone(iMsecs);
}
