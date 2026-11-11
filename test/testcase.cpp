#include <QCoreApplication>
#include <iostream>
#include <QtTest/QtTest>
#include <QTest>
#include <QStringList>
#include "gitldef.h"
#include "gitlabstractcommand.h"
#include "gitlfrontcontroller.h"
#include "gitlview.h"
#include "gitlivkcmdevt.h"
#include "gitlmodel.h"
#include "gitlmvcconst.h"
#include <QSemaphore>
#include <mutex>
#include <QThread>
#include <thread>
#include <chrono>


/// model
class TestModel: public GitlModel<TestModel>
{
    ADD_CLASS_FIELD(QString, strDataInModel, getDataInModel, setDataInModel)

protected:
    TestModel() { m_strDataInModel = "Hello GitlMVC";}
    friend class GitlModel<TestModel>;
};

/// view
class TestView : public GitlView
{
public:
    TestView()
    {
        listenToParams("fir_param", MAKE_CALLBACK(TestView::firParamListener));
        listenToParams("sec_param", MAKE_CALLBACK(TestView::secParamListener));
        listenToParams(QStringList()<<"fir_param"<<"sec_param",
                       MAKE_CALLBACK(TestView::multiParamListener));
    }


    /// it receives the result from command
    void firParamListener(GitlUpdateUIEvt& rcEvt)
    {
        m_strFirString = rcEvt.getParameter("fir_param").toString();
    }
    ADD_CLASS_FIELD_NOSETTER(QString, strFirString, getFirString)

    void secParamListener(GitlUpdateUIEvt& rcEvt)
    {
        m_strSecString = rcEvt.getParameter("sec_param").toString();
    }
    ADD_CLASS_FIELD_NOSETTER(QString, strSecString, getSecString)


    void multiParamListener(GitlUpdateUIEvt& rcEvt)
    {
        m_strMultiString = rcEvt.getParameter("fir_param").toString() + " " +
                rcEvt.getParameter("sec_param").toString();
    }
    ADD_CLASS_FIELD_NOSETTER(QString, strMultiString, getMultiString)
};

/// controller
/// the default one is used (GitlFrontController)

/// command, it manipulates the model and writes the result to output parameter. The output parameter will
/// be pass to view automatically.
class FirParamCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    /// Q_INVOKABLE is necessary for constructor
    Q_INVOKABLE explicit FirParamCommand(QObject *parent = 0):GitlAbstractCommand(parent) {setInWorkerThread(false);}
    bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
    {
        Q_UNUSED(rcInputArg)
        rcOutputArg.setParameter("fir_param", TestModel::getInstance()->getDataInModel());
        return true;
    }

};

class SecParamCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    /// Q_INVOKABLE is necessary for constructor
    Q_INVOKABLE explicit SecParamCommand(QObject *parent = 0):GitlAbstractCommand(parent) {setInWorkerThread(false);}
    bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
    {
        Q_UNUSED(rcInputArg)
        rcOutputArg.setParameter("sec_param", "this is the second param");
        return true;
    }

};

class NestedCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    /// Q_INVOKABLE is necessary for constructor
    Q_INVOKABLE explicit NestedCommand(QObject *parent = 0):GitlAbstractCommand(parent) {setInWorkerThread(false);}
    bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
    {
        Q_UNUSED(rcInputArg)
        Q_UNUSED(rcOutputArg)

        GitlIvkCmdEvt cFirstNestedEvt("fir_param_command");
        cFirstNestedEvt.dispatch();

        GitlIvkCmdEvt cSecondNestedEvt("sec_param_command");
        cSecondNestedEvt.dispatch();
        return true;
    }

};

class MultiParamCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    /// Q_INVOKABLE is necessary for constructor
    Q_INVOKABLE explicit MultiParamCommand(QObject *parent = 0):GitlAbstractCommand(parent) {setInWorkerThread(false);}
    bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
    {
        Q_UNUSED(rcInputArg)
        rcOutputArg.setParameter("fir_param", TestModel::getInstance()->getDataInModel());
        rcOutputArg.setParameter("sec_param", "this is the second param");
        return true;
    }

};


/// background command for the threading tests: it records which commands overlap,
/// then waits until the test opens the gate
struct GateState
{
    static QSemaphore cGate;
    static std::mutex cMutex;
    static int iRunning, iMaxRunning, iStarted;
    static QStringList cOrder;

    static void reset()
    {
        cGate.acquire(cGate.available());
        std::lock_guard<std::mutex> cLocker(cMutex);
        iRunning = iMaxRunning = iStarted = 0;
        cOrder.clear();
    }
    static int running()  { std::lock_guard<std::mutex> cLocker(cMutex); return iRunning; }
    static int started()  { std::lock_guard<std::mutex> cLocker(cMutex); return iStarted; }
};
QSemaphore GateState::cGate;
std::mutex GateState::cMutex;
int GateState::iRunning = 0, GateState::iMaxRunning = 0, GateState::iStarted = 0;
QStringList GateState::cOrder;

class GateCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit GateCommand(QObject *parent = 0):GitlAbstractCommand(parent) {}
    bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
    {
        QString strTag = rcInputArg.getParameter("tag").toString();
        {
            std::lock_guard<std::mutex> cLocker(GateState::cMutex);
            ++GateState::iStarted;
            GateState::iMaxRunning = qMax(GateState::iMaxRunning, ++GateState::iRunning);
            GateState::cOrder << strTag;
        }
        bool bOpened = GateState::cGate.tryAcquire(1, 5000);
        {
            std::lock_guard<std::mutex> cLocker(GateState::cMutex);
            --GateState::iRunning;
        }
        rcOutputArg.setParameter("gate_done", strTag);
        rcOutputArg.setParameter("gate_on_main_thread",
                                 QThread::currentThread() == QCoreApplication::instance()->thread());
        return bOpened;
    }
};

/// a command written the old way: it asks for the worker thread in its constructor
class WorkerFlagCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit WorkerFlagCommand(QObject *parent = 0):GitlAbstractCommand(parent) {setInWorkerThread(true);}
    bool execute(GitlCommandParameter &, GitlCommandParameter &) { return true; }
};

/// collects gate results and rejected requests, and the thread each callback ran on
class GateView : public GitlView
{
public:
    GateView()
    {
        listenToParams("gate_done", [this](GitlUpdateUIEvt& rcEvt)
        {
            m_cDone << rcEvt.getParameter("gate_done").toString();
            m_cRanOnMainThread << rcEvt.getParameter("gate_on_main_thread").toBool();
            m_bCallbacksOnMainThread &= QThread::currentThread() == QCoreApplication::instance()->thread();
        });
        listenToParams(GITL_COMMAND_ERROR_PARAM, [this](GitlUpdateUIEvt& rcEvt)
        {
            m_cRejected << rcEvt.getCommandName();
        });
    }
    QStringList m_cDone;
    QList<bool> m_cRanOnMainThread;
    QStringList m_cRejected;
    bool m_bCallbacksOnMainThread = true;
};

static void requestGate(const QString& strCommand, const QString& strTag)
{
    GitlIvkCmdEvt cRequestEvt(strCommand);
    cRequestEvt.setParameter("tag", strTag);
    cRequestEvt.dispatch();
}

/// test case
class TestCase : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        /// controller
        GitlFrontController* pcFC = GitlFrontController::getInstance();
        pcFC->registerCommand("fir_param_command",   &FirParamCommand::staticMetaObject);
        pcFC->registerCommand("sec_param_command",   &SecParamCommand::staticMetaObject);
        pcFC->registerCommand("multi_param_command", &MultiParamCommand::staticMetaObject);
        pcFC->registerCommand("nested_command", &NestedCommand::staticMetaObject);

        pcFC->setMaxThreadCount(4);
        QVERIFY(pcFC->registerCommand("gate_concurrent", &GateCommand::staticMetaObject, GitlExec::Concurrent));
        QVERIFY(pcFC->registerCommand("gate_queue_a",    &GateCommand::staticMetaObject, GitlExec::queue("a")));
        QVERIFY(pcFC->registerCommand("gate_queue_a2",   &GateCommand::staticMetaObject, GitlExec::queue("a")));
        QVERIFY(pcFC->registerCommand("gate_queue_b",    &GateCommand::staticMetaObject, GitlExec::queue("b")));
        QVERIFY(pcFC->registerCommand("gate_worker",     &GateCommand::staticMetaObject, GitlExec::Worker));
        QVERIFY(pcFC->registerCommand("worker_flag_command", &WorkerFlagCommand::staticMetaObject));

    }


    void firParamTest()
    {
        /// view
        TestView cView;
        /// event (in real case, this event should be dispatch from user interface, i.e. the views)
        GitlIvkCmdEvt cRequestEvt("fir_param_command");
        cRequestEvt.dispatch();
        /// verify
        QVERIFY(cView.getFirString()=="Hello GitlMVC");
        QVERIFY(cView.getSecString().isEmpty());
    }

    void secParamTest()
    {
        /// view
        TestView cView;
        /// event (in real case, this event should be dispatch from user interface, i.e. the views)
        GitlIvkCmdEvt cRequestEvt("sec_param_command");
        cRequestEvt.dispatch();
        /// verify
        QVERIFY(cView.getFirString().isEmpty());
        QVERIFY(cView.getSecString()=="this is the second param");
    }



    ///TODO check multi parameter listening case
    void multiParamsListening()
    {
        /// view
        TestView cView;

        /// event (in real case, this event should be dispatch from user interface, i.e. the views)
        GitlIvkCmdEvt cRequestEvt("multi_param_command");
        cRequestEvt.dispatch();

        /// verify
        QVERIFY(cView.getFirString()=="Hello GitlMVC");
        QVERIFY(cView.getSecString()=="this is the second param");
        QVERIFY(cView.getMultiString()=="Hello GitlMVC this is the second param");

    }

    void nestedCommandTest()
    {
        /// view
        TestView cView;

        /// event (in real case, this event should be dispatch from user interface, i.e. the views)
        GitlIvkCmdEvt cRequestEvt("nested_command");
        cRequestEvt.dispatch();

        /// verify
        QVERIFY(cView.getFirString()=="Hello GitlMVC");
        QVERIFY(cView.getSecString()=="this is the second param");
    }

    /// regression: ParamNameList::operator< was not a strict weak ordering, so QMap
    /// could fail to find a parameter list that had been registered
    void listenToSeveralParamListsTest()
    {
        GitlView cView;
        const QList<QStringList> acLists = {
            {"c","e"}, {"b","c"}, {"a","d"}
        };
        for(const QStringList& rcList : acLists)
            cView.listenToParams(rcList, [](GitlUpdateUIEvt&) {});

        for(const QStringList& rcList : acLists)
            QVERIFY2(cView.isListenToParams(rcList), qPrintable(rcList.join(",")));

        for(const QStringList& rcList : acLists)
        {
            QVERIFY2(cView.unlistenToParams(rcList), qPrintable(rcList.join(",")));
            QVERIFY(!cView.isListenToParams(rcList));
        }
    }

    /// a parameter list is a set: order and duplicates don't matter
    void paramListIsSetTest()
    {
        GitlView cView;
        int iFirst = 0, iSecond = 0;
        cView.listenToParams(QStringList{"b","a"}, [&](GitlUpdateUIEvt&) { ++iFirst; });
        QVERIFY(cView.isListenToParams({"a","b"}));
        QVERIFY(cView.isListenToParams({"a","b","a"}));

        /// the same set in another order replaces the first callback
        cView.listenToParams(QStringList{"a","b"}, [&](GitlUpdateUIEvt&) { ++iSecond; });

        GitlUpdateUIEvt cEvt;
        cEvt.setParameter("a", 1);
        cEvt.setParameter("b", 2);
        cEvt.dispatch();
        QCOMPARE(iFirst, 0);
        QCOMPARE(iSecond, 1);

        QVERIFY(cView.unlistenToParams({"b","a","b"}));
        QVERIFY(!cView.isListenToParams({"a","b"}));
    }

    void registrationTest()
    {
        GitlFrontController* pcFC = GitlFrontController::getInstance();
        /// a queue needs a name, and names are unique
        QVERIFY(!pcFC->registerCommand("unnamed_queue", &GateCommand::staticMetaObject, GitlExec::queue("")));
        QVERIFY(!pcFC->registerCommand("gate_queue_a", &GateCommand::staticMetaObject, GitlExec::Concurrent));
        /// commands registered without a policy keep their old behaviour
        QVERIFY(pcFC->getExecPolicy("fir_param_command") == GitlExec::Main);
        QVERIFY(pcFC->getExecPolicy("worker_flag_command") == GitlExec::Worker);
        QVERIFY(pcFC->getExecPolicy("gate_queue_a") == GitlExec::queue("a"));
    }

    void concurrentCommandsRunInParallel()
    {
        GateState::reset();
        GateView cView;
        requestGate("gate_concurrent", "1");
        requestGate("gate_concurrent", "2");
        QTRY_COMPARE(GateState::running(), 2);          /// both are inside execute() at once

        GateState::cGate.release(2);
        QVERIFY(GitlFrontController::getInstance()->waitForDone(5000));
        QTRY_COMPARE(cView.m_cDone.size(), 2);          /// results arrive through the event loop
        QVERIFY(!cView.m_cRanOnMainThread.contains(true));
        QVERIFY(cView.m_bCallbacksOnMainThread);
    }

    void sameQueueRunsInOrder()
    {
        GateState::reset();
        GateView cView;
        requestGate("gate_queue_a",  "1");
        requestGate("gate_queue_a2", "2");              /// another command, same queue
        requestGate("gate_queue_a",  "3");
        QTRY_COMPARE(GateState::started(), 1);
        QTest::qWait(100);
        QCOMPARE(GateState::started(), 1);              /// the others wait their turn

        GateState::cGate.release(3);
        QVERIFY(GitlFrontController::getInstance()->waitForDone(5000));
        QCOMPARE(GateState::cOrder.join('|'), QString("1|2|3"));
        QCOMPARE(GateState::iMaxRunning, 1);
        QTRY_COMPARE(cView.m_cDone.join('|'), QString("1|2|3"));
    }

    void differentQueuesRunInParallel()
    {
        GateState::reset();
        requestGate("gate_queue_a", "a");
        requestGate("gate_queue_b", "b");
        QTRY_COMPARE(GateState::running(), 2);
        GateState::cGate.release(2);
        QVERIFY(GitlFrontController::getInstance()->waitForDone(5000));
    }

    void workerRunsOffMainThread()
    {
        GateState::reset();
        GateView cView;
        GateState::cGate.release(1);
        requestGate("gate_worker", "w");
        QVERIFY(GitlFrontController::getInstance()->waitForDone(5000));
        QTRY_COMPARE(cView.m_cDone.join('|'), QString("w"));
        QCOMPARE(cView.m_cRanOnMainThread.size(), 1);
        QVERIFY(!cView.m_cRanOnMainThread.first());
    }

    void fullQueueRejectsRequests()
    {
        GitlFrontController* pcFC = GitlFrontController::getInstance();
        GateState::reset();
        GateView cView;
        pcFC->setMaxEvtInQue(1);
        requestGate("gate_queue_a", "running");
        QTRY_COMPARE(GateState::running(), 1);
        requestGate("gate_queue_a", "waiting");         /// fills the queue
        QVERIFY(cView.m_cRejected.isEmpty());
        requestGate("gate_queue_a", "rejected");        /// reported at once, no blocking
        QCOMPARE(cView.m_cRejected.join('|'), QString("gate_queue_a"));

        GateState::cGate.release(2);
        QVERIFY(pcFC->waitForDone(5000));
        QCOMPARE(GateState::cOrder.join('|'), QString("running|waiting"));
        pcFC->setMaxEvtInQue(1000);
    }

    /// must stay the last test: shutdown() is final
    void shutdownDropsPendingRequests()
    {
        GitlFrontController* pcFC = GitlFrontController::getInstance();
        GateState::reset();
        GateView cView;
        requestGate("gate_queue_b", "running");
        QTRY_COMPARE(GateState::running(), 1);
        requestGate("gate_queue_b", "dropped");

        /// shutdown() waits for the running command, so open its gate from another thread
        std::thread cOpener([]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            GateState::cGate.release(1);
        });
        QVERIFY(pcFC->shutdown(5000));
        cOpener.join();
        QCOMPARE(GateState::cOrder.join('|'), QString("running"));

        requestGate("gate_queue_b", "late");            /// rejected after shutdown
        QCOMPARE(cView.m_cRejected.join('|'), QString("gate_queue_b"));
        QCOMPARE(GateState::started(), 1);
    }

};


/// test main
QTEST_MAIN(TestCase)
#include "testcase.moc"

