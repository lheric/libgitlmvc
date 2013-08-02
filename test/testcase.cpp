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
    virtual void onUIUpdate(GitlUpdateUIEvt& rcEvt)
    {
        m_strDataInView = rcEvt.getParameter("data_to_view").toString();
    }
    ADD_CLASS_FIELD_NOSETTER(QString, strDataInView, getDataInView)
};
/// controller


/// command
class TestCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit TestCommand(QObject *parent = 0):GitlAbstractCommand(parent) {}
    Q_INVOKABLE bool execute( GitlCommandRequest& rcRequest, GitlCommandRespond& rcRespond )
    {
        QString strDataToCommand = rcRequest.getParameter("data_to_command").toString();
        TestModel::getInstance()->setDataInModel(strDataToCommand);
        rcRespond.setParameter("data_to_view", strDataToCommand);
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
        pcFC->addCommand("show_string_command", &TestCommand::staticMetaObject);

        /// event
        GitlIvkCmdEvt cRequestEvt("show_string_command");
        cRequestEvt.setParameter("data_to_command", "Hello GitlMVC");
        cRequestEvt.dispatch();

        /// verify
        QVERIFY(cView.getDataInView()=="Hello GitlMVC");
    }



};


/// test main
QTEST_MAIN(TestCase)
#include "testcase.moc"

