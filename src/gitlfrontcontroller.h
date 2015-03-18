#ifndef GITLFRONTCONTROLLER_H
#define GITLFRONTCONTROLLER_H


#include <QMetaObject>
#include <QHash>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

#include "../libgitlevtbus/src/gitldef.h"
#include "../libgitlevtbus/src/gitlmodual.h"
#include "gitlivkcmdevt.h"


/*!
 * \brief The GitlFrontController class
 *  Front controller pattern. It accepts all command requests and invoke corresponding commands
 *  (according to the <command name>-<command class> table)
 */

class GitlFrontController : public GitlModual, public QThread
{
public:
    virtual ~GitlFrontController() {}
    
    /*!
     * \brief detonate Filter out the command request from other request, and then it calls
     *                 onCommandRequestArrive function
     * \param rcEvt
     * \return
     */
    virtual bool detonate(GitlEvent& rcEvt);

    /*!
     * \brief onCommandRequestArrive One command request is captured by front controller.
     *                               By default, it find out the corrensponding command and execute this command immediately.
     *                               You can override this function to to something else.
     *
     * \param rcRequest
     */
    virtual void onCommandRequestArrive(GitlIvkCmdEvt& rcEvt);

    /*!
     * \brief registerCommand register an command with a command name
     * \param cCommandFormat
     * \param pMetaObject
     * \return
     */
    bool registerCommand(const QString cCommandName, const QMetaObject *pMetaObject);

    /*!
     * \brief unregisterAllCommand unregister all command
     */
    void unregisterAllCommand();


    virtual void run();
protected:
    explicit GitlFrontController();

protected:

    /// <command string>-<command class> table
    ADD_CLASS_FIELD( CONCATE(QHash<QString,QMetaObject*>), cCommandTable, getCommandTable, setCommandTable)

    SINGLETON_PATTERN_DECLARE(GitlFrontController)

    ADD_CLASS_FIELD_PRIVATE(QList<GitlEvent*>, pcMainThreadEvtQue)
    ADD_CLASS_FIELD_PRIVATE(QList<GitlEvent*>, pcWorkerThreadEvtQue)
    ADD_CLASS_FIELD_PRIVATE(QMutex, cEvtQueMutex)
    ADD_CLASS_FIELD_PRIVATE(QWaitCondition, cEvtQueNotEmpty)
    ADD_CLASS_FIELD_PRIVATE(QWaitCondition, cEvtQueNotFull)
    ADD_CLASS_FIELD(int, iMaxEvtInQue, setMaxEvtInQue, getMaxEvtInQue)

    ADD_CLASS_FIELD_PRIVATE(QMutex, cCmdExeMutex)   ///< ensure one command execution at one time

};

#endif // GITLFRONTCONTROLLER_H
