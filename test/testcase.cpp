#include <QCoreApplication>
#include <iostream>
#include <QtTest/QtTest>
#include <QTest>
#include "gitldef.h"
#include "gitlabstractcommand.h"
#include "gitlfrontcontroller.h"
#include "gitlview.h"
#include "gitlcommandrequestevt.h"

/// model


/// view
class TestView : public GitlView
{
public:
    TestView()
    {
    }

    virtual void onGitlUiUpdate(GitlRefreshUIRequestEvt& rcEvt)
    {
        m_strViewString = rcEvt.getParameter("string_to_display").toString();
    }

    ADD_CLASS_FIELD_NOSETTER(QString, strViewString, getViewString)
};


/// event


/// command
class TestCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit TestCommand(QObject *parent = 0):GitlAbstractCommand(parent) {}
    Q_INVOKABLE bool execute( GitlCommandRequest& rcRequest, GitlCommandRespond& rcRespond )
    {
        rcRespond.setParameter("string_to_display", "Hello GitlMVC");
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
        GitlCommandRequestEvt cRequestEvt("show_string_command");
        cRequestEvt.dispatch();

        /// verify
        QVERIFY(cView.getViewString()=="Hello GitlMVC");
    }



};


/// test main
QTEST_MAIN(TestCase)
#include "testcase.moc"

