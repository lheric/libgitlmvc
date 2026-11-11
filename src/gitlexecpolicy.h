#ifndef GITLEXECPOLICY_H
#define GITLEXECPOLICY_H

#include <QString>

/*!
 * \brief The GitlExecPolicy class says where a command runs. Pass one to
 *        GitlFrontController::registerCommand(); use the ready-made values in the
 *        GitlExec namespace rather than constructing it yourself.
 *
 *  - GitlExec::Main: on the thread that dispatched the request, before dispatch() returns.
 *  - GitlExec::Worker: the default background queue. Its commands run one at a time, and
 *    never at the same time as a Main command (the behaviour of setInWorkerThread(true)).
 *  - GitlExec::queue("name"): a named serial queue. Commands in the same queue run one at a
 *    time, in request order; different queues run in parallel.
 *  - GitlExec::Concurrent: any free thread of the pool, in parallel with everything else.
 *
 *  Queues and Concurrent commands share one thread pool, so the number of threads is bounded.
 *  Commands outside Main and Worker may run at the same time as other commands: protect any
 *  data they share, or put them in the same named queue.
 */
class GitlExecPolicy
{
public:
    enum Kind
    {
        MainThread,     ///< the dispatching thread
        WorkerQueue,    ///< the default serial background queue
        SerialQueue,    ///< a named serial queue
        Concurrent      ///< the thread pool, no ordering
    };

    explicit GitlExecPolicy(Kind eKind = MainThread, const QString& strQueueName = QString())
        : m_eKind(eKind), m_strQueueName(strQueueName) {}

    Kind getKind() const { return m_eKind; }
    QString getQueueName() const { return m_strQueueName; }

    /// whether commands with this policy hold the global command lock, i.e. never overlap
    /// with one another (the behaviour of libgitlmvc before thread pools)
    bool isExclusive() const { return m_eKind == MainThread || m_eKind == WorkerQueue; }

    bool operator==(const GitlExecPolicy& rcOther) const
    {
        return m_eKind == rcOther.m_eKind && m_strQueueName == rcOther.m_strQueueName;
    }
    bool operator!=(const GitlExecPolicy& rcOther) const { return !(*this == rcOther); }

private:
    Kind m_eKind;
    QString m_strQueueName;
};

namespace GitlExec
{
    /// on the dispatching thread (the default)
    const GitlExecPolicy Main(GitlExecPolicy::MainThread);

    /// the default background queue, one command at a time
    const GitlExecPolicy Worker(GitlExecPolicy::WorkerQueue);

    /// the thread pool, in parallel with other commands
    const GitlExecPolicy Concurrent(GitlExecPolicy::Concurrent);

    /// a named serial queue: one command at a time within the queue
    inline GitlExecPolicy queue(const QString& strName)
    {
        return GitlExecPolicy(GitlExecPolicy::SerialQueue, strName);
    }
}

#endif // GITLEXECPOLICY_H
