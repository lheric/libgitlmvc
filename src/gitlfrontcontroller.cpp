#include "gitlfrontcontroller.h"
#include "gitlabstractcommand.h"

#include <QSharedPointer>
#include <QDebug>
#include <iostream>
#include "gitlmvcconst.h"
#include "gitlupdateuievt.h"

SINGLETON_PATTERN_IMPLIMENT(GitlFrontController)

GitlFrontController::GitlFrontController() :
    m_cCmdExeMutex(QMutex::Recursive)
{
    m_iMaxEvtInQue = 1000;
    subscribeToEvtByName(GITL_EXE_COMMAND_REQUEST_EVENT, MAKE_CALLBACK(GitlFrontController::detonate));
    this->start();
}


bool GitlFrontController::detonate( GitlEvent& rcEvt)
{
    GitlIvkCmdEvt& rcCmdRequestEvt = dynamic_cast<GitlIvkCmdEvt&>(rcEvt);
    QString strCommandName = rcCmdRequestEvt.getCommandName();
    QHash<QString, QMetaObject*>::iterator i = m_cCommandTable.find(strCommandName);
    //command not found
    if( i == m_cCommandTable.end() )
    {
        qWarning() << QString("No matched command name found. %1").arg(strCommandName);
        return true;
    }
    //create command
    const QMetaObject* pMetaObj = i.value();
    QObject* pObj = pMetaObj->newInstance();
    //if fail to create command class
    if(pObj == NULL)
    {
        qCritical() << QString("Unable to create command class '%1'! Please ensure the constructor have Q_INVOKABLE macro.")
                    .arg(pMetaObj->className());
        return true;
    }

    QSharedPointer<GitlAbstractCommand> pCmd(static_cast<GitlAbstractCommand *>(pObj));

    /// execute it in worker thread or main(GUI) thread
    if(pCmd->getInWorkerThread() == false)
    {
        onCommandRequestArrive(rcCmdRequestEvt);
    }
    else
    {
        /// pending for worker thread execution
        m_cEvtQueMutex.lock();
        if( m_pcWorkerThreadEvtQue.size() >= m_iMaxEvtInQue )
        {
            std::cerr << "Too Many Events Pending...Waiting..." << std::endl;
            m_cEvtQueNotFull.wait(&m_cEvtQueMutex);
            std::cerr << "Event Queue Not Full...Moving on..." << std::endl;
        }
        m_pcWorkerThreadEvtQue.push_back(rcEvt.clone());
        m_cEvtQueMutex.unlock();
        m_cEvtQueNotEmpty.wakeAll();
    }

    return true;

}

void GitlFrontController::onCommandRequestArrive(GitlIvkCmdEvt &rcEvt)
{
    QMutexLocker cCmdExeMutexLocker(&m_cCmdExeMutex);
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
        //create  command
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
    return;



}


bool GitlFrontController::registerCommand(const QString cCommandName, const QMetaObject *pMetaObject)
{
    if(m_cCommandTable.contains(cCommandName))
    {
        qCritical() << QString("%1 Register Fail! Duplicated command name!").arg(cCommandName);
        return false;
    }

    QString strAbsCmdClassName = GitlAbstractCommand::staticMetaObject.className();
    if( pMetaObject->className() == strAbsCmdClassName )
    {
        qCritical() << QString("Trying to register name '%1' for abstract class '%2'! "
                               "You may miss the Q_OBJECT marco for the derived class of '%3'")
                       .arg(cCommandName).arg(strAbsCmdClassName).arg(strAbsCmdClassName);
        return false;
    }

    QMetaObject *p = const_cast<QMetaObject *>(pMetaObject);
    m_cCommandTable.insert(cCommandName, p);
    qDebug() << QString("%1 Register Success!").arg(pMetaObject->className());
    return true;
}

void GitlFrontController::unregisterAllCommand()
{
    m_cCommandTable.clear();
}

void GitlFrontController::run()
{
    forever
    {

        /// get one event from the waiting queue
        m_cEvtQueMutex.lock();
        if( m_pcWorkerThreadEvtQue.empty() )
        {
            m_cEvtQueNotEmpty.wait(&m_cEvtQueMutex);
        }
        GitlEvent* pcEvt = m_pcWorkerThreadEvtQue.front();
        m_pcWorkerThreadEvtQue.pop_front();
        m_cEvtQueMutex.unlock();
        m_cEvtQueNotFull.wakeAll();

        /// execute command
        GitlIvkCmdEvt& rcCmdRequestEvt = dynamic_cast<GitlIvkCmdEvt&>(*pcEvt);
        onCommandRequestArrive(rcCmdRequestEvt);
        delete pcEvt;

    }
}


