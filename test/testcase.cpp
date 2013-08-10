#include <QCoreApplication>
#include <iostream>
#include <QtTest/QtTest>
#include <QTest>
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
    TestModel() {}
    friend class GitlModel<TestModel>;
};

/// view
class TestView : public GitlView
{
public:
    /// it receives the result from command
    virtual void onUIUpdate(GitlUpdateUIEvt& rcEvt)
    {
        m_strDataInView = rcEvt.getParameter("data_to_view").toString();
    }
    ADD_CLASS_FIELD_NOSETTER(QString, strDataInView, getDataInView)
};

/// controller
/// the default one is used (GitlFrontController)

/// command, it manipulates the model and writes the result to output parameter. The output parameter will
/// be pass to view automatically.
class TestCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    /// Q_INVOKABLE is necessary for constructor
    Q_INVOKABLE explicit TestCommand(QObject *parent = 0):GitlAbstractCommand(parent) {}
    bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
    {
        QString strDataToCommand = rcInputArg.getParameter("data_to_command").toString();
        TestModel::getInstance()->setDataInModel(strDataToCommand);
        rcOutputArg.setParameter("data_to_view", strDataToCommand);
        return true;
    }

};


/// test case
class TestCase : public QObject
{
    Q_OBJECT

private slots:
    void basicTest()
    {
        /// view
        TestView cView;

        /// controller
        GitlFrontController* pcFC = GitlFrontController::getInstance();
        pcFC->registerCommand("show_string_command", &TestCommand::staticMetaObject);

        /// event (in real case, this event should be dispatch from user interface, i.e. the views)
        GitlIvkCmdEvt cRequestEvt("show_string_command");
        cRequestEvt.setParameter("data_to_command", "Hello GitlMVC");
        cRequestEvt.dispatch();

        /// verify
        ///
        QVERIFY(TestModel::getInstance()->getDataInModel() == "Hello GitlMVC");
        QVERIFY(cView.getDataInView()=="Hello GitlMVC");

    }



};


/// test main
QTEST_MAIN(TestCase)
#include "testcase.moc"

