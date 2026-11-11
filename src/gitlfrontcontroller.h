#ifndef GITLFRONTCONTROLLER_H
#define GITLFRONTCONTROLLER_H


#include <QMetaObject>
#include <QHash>
#include <QQueue>
#include <QThreadPool>

#include <functional>
#include <memory>
#include <mutex>

#include "../libgitlevtbus/src/gitldef.h"
#include "../libgitlevtbus/src/gitlmodule.h"
#include "gitlivkcmdevt.h"
#include "gitlexecpolicy.h"


/*!
 * \brief The GitlFrontController class
 *  Front controller pattern. It accepts all command requests and invoke corresponding commands
 *  (according to the <command name>-<command class> table)
 *
 *  Where a command runs is chosen when it is registered (see GitlExecPolicy). Background
 *  commands run on a thread pool owned by the front controller.
 */

class GitlFrontController : public GitlModule
{
public:
    virtual ~GitlFrontController();

    /*!
     * \brief detonate Filter out the command request from other request, and then it calls
     *                 onCommandRequestArrive function
     * \param rcEvt
     * \return
     */
    virtual bool detonate(const GitlEvent& rcEvt);

    /*!
     * \brief onCommandRequestArrive One command request is captured by front controller.
     *                               By default, it find out the corrensponding command and execute this command immediately.
     *                               It is called on the thread the command's policy names.
     *
     * \param rcRequest
     */
    virtual void onCommandRequestArrive(GitlIvkCmdEvt& rcEvt);

    /*!
     * \brief registerCommand register an command with a command name. The command runs
     *        on a background queue (GitlExec::Worker) if its constructor calls
     *        setInWorkerThread(true), else on the dispatching thread (GitlExec::Main).
     * \return false if the name is taken or the command class cannot be created
     */
    bool registerCommand(const QString cCommandName, const QMetaObject *pMetaObject);

    /*!
     * \brief registerCommand register an command with a command name, and choose where it runs
     * \param rcPolicy GitlExec::Main, GitlExec::Worker, GitlExec::Concurrent or GitlExec::queue("name").
     *                 It overrides the command's setInWorkerThread().
     * \return false if the name is taken, the command class cannot be created, or the policy is invalid
     */
    bool registerCommand(const QString& strCommandName, const QMetaObject *pMetaObject,
                         const GitlExecPolicy& rcPolicy);

    /*!
     * \brief unregisterAllCommand unregister all command
     */
    void unregisterAllCommand();

    /*!
     * \brief getExecPolicy where a registered command runs
     * \return the policy, or GitlExec::Main if no command has this name
     */
    GitlExecPolicy getExecPolicy(const QString& strCommandName) const;

    /*!
     * \brief setMaxThreadCount the number of threads background commands may use at once.
     *        The default is QThread::idealThreadCount().
     */
    void setMaxThreadCount(int iCount);
    int getMaxThreadCount() const;

    /*!
     * \brief waitForDone wait until every background command has finished
     * \param iMsecs time limit in milliseconds, -1 for none
     * \return false on timeout
     */
    bool waitForDone(int iMsecs = -1);

    /*!
     * \brief shutdown stop accepting background requests, drop the ones not started yet,
     *        and wait for the running ones to finish. Called automatically when the
     *        application is about to quit. Requests made afterwards are rejected.
     * \param iMsecs time limit in milliseconds, -1 for none
     * \return false on timeout
     */
    bool shutdown(int iMsecs = -1);

protected:
    explicit GitlFrontController();

    /// check a command class before registering it: not the abstract class, constructible
    /// (Q_INVOKABLE). Reports the command's setInWorkerThread() in *pbInWorkerThread.
    bool checkCommandClass(const QString& strCommandName, const QMetaObject *pMetaObject,
                           bool *pbInWorkerThread);

    /// queue a request on a background policy
    /// \return empty on success, else why the request was rejected
    QString submit(const GitlExecPolicy& rcPolicy, const std::shared_ptr<GitlIvkCmdEvt>& pcEvt);

    /// run a request on the current thread, taking the global lock if its policy says so
    void execute(const GitlExecPolicy& rcPolicy, GitlIvkCmdEvt& rcEvt);

    /// run the next task of a serial queue, then schedule the one after it
    void runNextInQueue(const QString& strQueueKey);

    /// tell the views that a request was rejected
    void reportRejected(const QString& strCommandName, const QString& strReason);

protected:

    /// <command string>-<command class> table
    ADD_CLASS_FIELD( CONCATE(QHash<QString,QMetaObject*>), cCommandTable, getCommandTable, setCommandTable)

    SINGLETON_PATTERN_DECLARE(GitlFrontController)

    /// maximum number of requests waiting in each serial queue, and of Concurrent requests not yet finished
    ADD_CLASS_FIELD(int, iMaxEvtInQue, getMaxEvtInQue, setMaxEvtInQue)

private:
    struct SerialQueue
    {
        QQueue<std::function<void()>> cTasks;
        bool bRunning = false;
    };

    QHash<QString, GitlExecPolicy> m_cPolicyTable;      ///< command name -> where it runs
    QHash<QString, SerialQueue> m_cSerialQueues;        ///< queue key -> pending tasks
    int m_iConcurrentInFlight;                          ///< Concurrent requests queued or running
    bool m_bAccepting;                                  ///< false after shutdown()
    std::mutex m_cQueueMutex;                           ///< guards the four members above
    QThreadPool m_cThreadPool;
    std::recursive_mutex m_cCmdExeMutex;                ///< held by Main and Worker commands

};

#endif // GITLFRONTCONTROLLER_H
