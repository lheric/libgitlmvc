# Getting started

## Build the library

libgitlmvc uses [libgitlevtbus](https://libgitlevtbus.readthedocs.io/) as a git submodule, so clone with `--recursive`. Then build with qmake:

```sh
git clone --recursive https://github.com/lheric/libgitlmvc.git
mkdir build && cd build
qmake ../libgitlmvc/libgitlmvc.pro -r "CONFIG+=release"
make            # nmake or jom with MSVC
```

If you have already cloned the repository without its submodules, run `git submodule update --init` first.

The build places two static libraries in the build directory:

- `GitlMVC`, which is libgitlmvc itself.
- `libgitlevtbus/GitlEvtBus`, the event bus it uses.

Debug builds add a `d` to the name: `GitlMVCd` and `GitlEvtBusd`. The build also produces the tests and an example application.

To use the libraries in your own qmake project, add:

```text
QT       += core gui widgets
CONFIG   += c++17
INCLUDEPATH += /path/to/libgitlmvc/src \
               /path/to/libgitlmvc/libgitlevtbus/src
LIBS     += -L/path/to/build -L/path/to/build/libgitlevtbus \
            -lGitlMVC -lGitlEvtBus          # -lGitlMVCd -lGitlEvtBusd for debug builds
```

## A first application

This page builds the program from the repository's `examples/` folder. It's a window with a button and a label. Clicking the button sends a string through a command, which stores it in the model and shows it in the label.

### The model

The model holds the application state. Derive it from `GitlModel<T>`, which turns it into a thread-safe singleton reachable through `TestModel::getInstance()`:

```cpp
#include "gitldef.h"
#include "gitlmodel.h"

class TestModel : public GitlModel<TestModel>
{
    ADD_CLASS_FIELD(QString, strDataInModel, getDataInModel, setDataInModel)

protected:
    TestModel() {}
    friend class GitlModel<TestModel>;
};
```

`ADD_CLASS_FIELD` declares a member `m_strDataInModel` with a getter and a setter. Keep the constructor protected and befriend `GitlModel<TestModel>` so that `getInstance()` is the only way to reach the model.

### The command

A command does one job. It derives from `GitlAbstractCommand` and implements `execute()`:

```cpp
#include "gitlabstractcommand.h"

class ShowStringCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ShowStringCommand(QObject* parent = nullptr)
        : GitlAbstractCommand(parent) {}

    bool execute(GitlCommandParameter& rcInputArg, GitlCommandParameter& rcOutputArg) override
    {
        QString strData = rcInputArg.getParameter("data_to_command").toString();
        TestModel::getInstance()->setDataInModel(strData);   // update the model
        rcOutputArg.setParameter("data_to_view", strData);   // tell the views
        return true;
    }
};
```

- `rcInputArg` holds the parameters that came with the request.
- Whatever you put in `rcOutputArg` is sent to the views when `execute()` returns `true`.

```{admonition} Q_OBJECT and Q_INVOKABLE are required
:class: warning

The front controller creates commands from their `QMetaObject`. Without `Q_OBJECT`, `registerCommand()` refuses the class, and without `Q_INVOKABLE` on the constructor, the command can't be created when it's requested. Both mistakes are reported in the log.
```

### The view

A view derives from `GitlView`. Here the main window is the view, so it derives from both `QMainWindow` and `GitlView`:

```cpp
#include <QMainWindow>
#include "gitlview.h"
#include "gitlivkcmdevt.h"

class MainWindow : public QMainWindow, public GitlView
{
public:
    explicit MainWindow(QWidget* parent = nullptr) : QMainWindow(parent)
    {
        // ... set up the button and the label ...
        listenToParams("data_to_view", MAKE_CALLBACK(MainWindow::onUIUpdate));
    }

    void onUIUpdate(GitlUpdateUIEvt& rcEvt)
    {
        m_pcLabel->setText(rcEvt.getParameter("data_to_view").toString());
    }

    void onButtonClicked()
    {
        GitlIvkCmdEvt cRequest("show_string_command");
        cRequest.setParameter("data_to_command", "Hello GitlMVC");
        cRequest.dispatch();
    }
};
```

- `listenToParams()` registers a callback for command results that contain the parameter `data_to_view`.
- `GitlIvkCmdEvt` is a command request. Its constructor takes the name of the command to run, and `setParameter()` adds the input for that command.

### Putting it together

In `main()`, register the command with the front controller under the name the view uses, then show the view:

```cpp
#include <QApplication>
#include "gitlfrontcontroller.h"

int main(int argc, char* argv[])
{
    QApplication cApp(argc, argv);

    GitlFrontController::getInstance()->registerCommand(
        "show_string_command", &ShowStringCommand::staticMetaObject);

    MainWindow cView;
    cView.show();

    return cApp.exec();
}
```

When the button is clicked, the following happens:

1. The view dispatches a `show_string_command` request.
2. The front controller creates a `ShowStringCommand` and calls `execute()`.
3. The command stores the string in the model and writes `data_to_view` to its output.
4. The output is sent to the views, and `MainWindow::onUIUpdate()` sets the label's text.

Steps 2 to 4 all happen inside `dispatch()`, because the command runs on the same thread by default. See [Threading](threading.md) to run long commands in the background.

## Next steps

- [Commands](commands.md): what else commands can do.
- [Views](views.md): listen to several parameters at once.
- `test/testcase.cpp` in the repository has more small, complete examples.
