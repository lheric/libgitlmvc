#include "gitldef.h"
#include "gitlabstractcommand.h"
#include "gitlfrontcontroller.h"
#include "gitlview.h"
#include "testcommand.h"
#include <QDebug>
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication cApp(argc, argv);

    /// controller, the default one is used (GitlFrontController)
    GitlFrontController* pcFC = GitlFrontController::getInstance();
    pcFC->registerCommand("show_string_command", &FirParamCommand::staticMetaObject);

    /// view
    MainWindow cView;
    cView.show();

    cApp.exec();
    return 0;
}
