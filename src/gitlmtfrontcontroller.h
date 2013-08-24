#ifndef GITLMTFRONTCONTROLLER_H
#define GITLMTFRONTCONTROLLER_H
#include "gitlfrontcontroller.h"
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
/*!
 * \brief The GitlMTFrontController class
 * This is a multi-thread version of front controller. If some commands take too long and
 * you don't want to freeze the UI , you should use this class instead of \see GitlFrontController.
 * This multi-thread front controller will run commands in a seperated thread
 * (run one command at a time, i.e. one after another).
 */
class GitlMTFrontController : public QThread, public GitlFrontController
{

public:
    /*! The concrete action to deal with specific event
      */
    virtual bool detonate( GitlEvent& cEvt );

protected:
    GitlMTFrontController();
    virtual void run();

    //SINGLETON
    SINGLETON_PATTERN_DECLARE(GitlMTFrontController)

    ADD_CLASS_FIELD_PRIVATE(QList<GitlEvent*>, pcEvtQue)
    ADD_CLASS_FIELD_PRIVATE(QMutex, cEvtQueMutex)
    ADD_CLASS_FIELD_PRIVATE(QWaitCondition, cEvtQueNotEmpty)
    ADD_CLASS_FIELD_PRIVATE(QWaitCondition, cEvtQueNotFull)
    ADD_CLASS_FIELD(int, iMaxEvtInQue, setMaxEvtInQue, getMaxEvtInQue)
    
};

#endif // GITLMTFRONTCONTROLLER_H
