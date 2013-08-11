#include "gitldef.h"
#include "gitlabstractcommand.h"
#include "gitlfrontcontroller.h"
#include "gitlview.h"
#include "gitlivkcmdevt.h"
#include "testcommand.h"
#include <QDebug>

/// view
class TestView : public GitlView
{
public:
    /// it receives the result from commands
    virtual void onUIUpdate(GitlUpdateUIEvt& rcEvt)
    {
        m_strDataInView = rcEvt.getParameter("data_to_view").toString();
    }

    ADD_CLASS_FIELD_NOSETTER(QString, strDataInView, getDataInView)
};



int main(int argc, char *argv[])
{
    /// view
    TestView cView;

    /// controller, the default one is used (GitlFrontController)
    GitlFrontController* pcFC = GitlFrontController::getInstance();
    pcFC->registerCommand("show_string_command", &TestCommand::staticMetaObject);

    /// event (in real case, this event should be dispatch from user interface, i.e. the views)
    GitlIvkCmdEvt cRequestEvt("show_string_command");
    cRequestEvt.setParameter("data_to_command", "Hello GitlMVC");
    cRequestEvt.dispatch();

    qDebug() << cView.getDataInView();
    
    return 0;
}
