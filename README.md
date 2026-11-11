libgitlmvc
==========
[![CI](https://github.com/lheric/libgitlmvc/actions/workflows/ci.yml/badge.svg)](https://github.com/lheric/libgitlmvc/actions/workflows/ci.yml)
[![Documentation](https://readthedocs.org/projects/libgitlmvc/badge/?version=latest)](https://libgitlmvc.readthedocs.io/)

**Documentation and tutorials: https://libgitlmvc.readthedocs.io/**

libgitlmvc is a lightweight MVC framework for Qt applications written in C++, released under the BSD license.

It sits on top of [libgitlevtbus](https://github.com/lheric/libgitlevtbus), an event bus for Qt, and splits an application into four layers that talk to each other only through events:

```
 View  ──request──▶  Front Controller  ──▶  Command  ──▶  Model
  ▲                                            │
  └───────────────── UI update ────────────────┘
```

| Part | Role |
|------|------|
| **View** | The user interface. Sends command requests and listens for the results it cares about. |
| **Front Controller** | Receives every request and runs the command registered under the request's name. |
| **Command** | One class per use case. Reads its input, changes the model, and writes the results that go back to the views. |
| **Model** | Holds the application state. Only commands change it. |

How this differs from classic MVC:

- **Views never touch the model.** Every change goes through a command, and views learn about it from the command's output. That keeps the layers independent, so you can change one without affecting the others.
- **One request, one command.** The front controller maps each request name to one command class (the front controller and command patterns). Each feature is a separate class, so you can add features one at a time.

Requirements
============

- **Qt 5.15 or Qt 6.** The library uses Qt Core and Qt Widgets.
- **A C++17 compiler**, such as GCC, Clang or MSVC. libgitlevtbus requires C++17.
- **qmake** to build.

CI builds and tests Qt 5.15 and Qt 6 on Linux (GCC), macOS (Clang) and Windows (MSVC).

Building
========

libgitlevtbus is a git submodule, so clone with `--recursive`:

```sh
git clone --recursive https://github.com/lheric/libgitlmvc.git
cd libgitlmvc
mkdir build && cd build
qmake ../libgitlmvc.pro
make            # nmake or jom with MSVC
```

If you have already cloned the repository without its submodules, run `git submodule update --init` first.

The build produces these static libraries:

- `libGitlMVC.a` and `libgitlevtbus/libGitlEvtBus.a`, or the `GitlMVCd`/`GitlEvtBusd` variants in debug builds.

It also builds the test suite (`test/TestGitlEvtBus`) and a small GUI demo (`examples/`).

To use the library in your own project, add `src/` and `libgitlevtbus/src/` to `INCLUDEPATH`, then link both libraries. See `examples/examples.pro` for an example setup.

Getting started
===============

The steps below follow the program in `examples/`. Clicking a button sends a string through a command into the model, and the result is shown back in a label.

### 1. Model

Derive your model from `GitlModel<T>`, which makes it a thread-safe singleton:

```c++
#include "gitlmodel.h"

class TestModel : public GitlModel<TestModel>
{
    ADD_CLASS_FIELD(QString, strDataInModel, getDataInModel, setDataInModel)

protected:
    TestModel() {}
    friend class GitlModel<TestModel>;
};
```

### 2. Command

A command derives from `GitlAbstractCommand` and implements `execute()`:

- `rcInputArg` holds the parameters the view sent.
- Whatever you put in `rcOutputArg` is delivered to the views when `execute()` returns `true`.

```c++
#include "gitlabstractcommand.h"

class ShowStringCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    // Q_INVOKABLE is required: the front controller creates commands through Qt's meta-object system.
    Q_INVOKABLE explicit ShowStringCommand(QObject *parent = nullptr) : GitlAbstractCommand(parent) {}

    bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg) override
    {
        QString str = rcInputArg.getParameter("data_to_command").toString();
        TestModel::getInstance()->setDataInModel(str);
        rcOutputArg.setParameter("data_to_view", str);
        return true;
    }
};
```

### 3. Register the command

Register each command with the front controller under a request name, usually in `main()`:

```c++
GitlFrontController::getInstance()->registerCommand("show_string_command",
                                                    &ShowStringCommand::staticMetaObject);
```

### 4. View

A view derives from `GitlView`.

- `listenToParams()` registers a callback that runs whenever a command's output contains the named parameter. If you pass a list of names, the output must contain all of them.
- To make a request, dispatch a `GitlIvkCmdEvt` that names the command.

```c++
#include "gitlview.h"
#include "gitlivkcmdevt.h"

class MainWindow : public QMainWindow, public GitlView
{
public:
    MainWindow()
    {
        listenToParams("data_to_view", MAKE_CALLBACK(MainWindow::onUIUpdate));
    }

    void onUIUpdate(GitlUpdateUIEvt &rcEvt)
    {
        label->setText(rcEvt.getParameter("data_to_view").toString());
    }

    void onButtonClicked()
    {
        GitlIvkCmdEvt cRequest("show_string_command");
        cRequest.setParameter("data_to_command", "Hello GitlMVC");
        cRequest.dispatch();
    }
};
```

That covers the whole round trip. A command can also dispatch other `GitlIvkCmdEvt`s to chain commands; see `NestedCommand` in `test/testcase.cpp`.

Threading
=========

By default, commands run on the thread that dispatched the request, one at a time. Long commands can run in the background instead: choose a policy when you register them.

```c++
pcFC->registerCommand("save_file",    &SaveFileCommand::staticMetaObject,    GitlExec::Worker);      // one at a time
pcFC->registerCommand("thumbnail",    &ThumbnailCommand::staticMetaObject,   GitlExec::Concurrent);  // in parallel
pcFC->registerCommand("export_video", &ExportVideoCommand::staticMetaObject, GitlExec::queue("io"));  // one at a time within "io"
```

All background commands share one bounded thread pool. A full queue rejects new requests instead of blocking the GUI thread, and `shutdown()` runs automatically when the application quits. `Concurrent` and queued commands can overlap with other commands, so protect any data they share. See the [threading docs](https://libgitlmvc.readthedocs.io/en/latest/threading.html).

A command that runs in the background **must not create or touch GUI objects**.

More examples
=============

- `examples/` is a minimal Qt Widgets application.
- `test/testcase.cpp` covers single- and multi-parameter listeners and nested commands.
- The [libgitlmvc documentation](https://libgitlmvc.readthedocs.io/) covers commands, views and threading in more depth.
- The [libgitlevtbus documentation](https://libgitlevtbus.readthedocs.io/) describes the event bus underneath: events, modules, and threading.

License
=======

BSD license. See the copyright header in the source files.
