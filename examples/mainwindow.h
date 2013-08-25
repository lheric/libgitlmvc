#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gitlview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public GitlView
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /// it receives the result from commands
    virtual void onUIUpdate(GitlUpdateUIEvt& rcEvt);
    
private slots:
    void on_mtTestButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
