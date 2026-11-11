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

};


/// test main
QTEST_MAIN(TestCase)
#include "testcase.moc"

