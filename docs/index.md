# libgitlmvc

libgitlmvc is a lightweight MVC framework for Qt applications written in C++. It splits an application into four layers that talk to each other only through events, carried by the [libgitlevtbus](https://libgitlevtbus.readthedocs.io/) event bus:

```text
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

A complete round trip looks like this:

```cpp
// a view asks for something...
GitlIvkCmdEvt cRequest("show_string_command");
cRequest.setParameter("data_to_command", "Hello GitlMVC");
cRequest.dispatch();

// ...a command does it and reports the result...
bool execute(GitlCommandParameter& rcInputArg, GitlCommandParameter& rcOutputArg) override
{
    rcOutputArg.setParameter("data_to_view", rcInputArg.getParameter("data_to_command"));
    return true;
}

// ...and every view listening to "data_to_view" is updated
listenToParams("data_to_view", [this](GitlUpdateUIEvt& rcEvt)
{
    label->setText(rcEvt.getParameter("data_to_view").toString());
});
```

## Why this design

- **Views never touch the model.** Every change goes through a command, and views learn about it from the command's output. That keeps the layers independent, so you can change one without affecting the others.
- **One request, one command.** The front controller maps each request name to one command class (the front controller and command patterns). Each feature is a separate class, so you can add features one at a time.
- **Heavy work off the GUI thread.** A command can run in the background, one at a time or in parallel on a thread pool, chosen with one argument at registration. Its results still reach the views on the GUI thread.

## Requirements

- **Qt 5.15 or Qt 6.** The library uses Qt Core and Qt Widgets.
- **A C++17 compiler**, such as GCC, Clang or MSVC.
- **qmake** to build.

CI builds and tests Qt 5.15 and Qt 6 on Linux (GCC), macOS (Clang) and Windows (MSVC).

## Where next

1. [Getting started](getting-started.md): build the library and write a complete application.
2. [Commands](commands.md): the front controller, command parameters and chaining commands.
3. [Views](views.md): listen to command results.
4. [Threading](threading.md): run commands in the background, one at a time or in parallel.

libgitlmvc is open source under the BSD license. The source code is on [GitHub](https://github.com/lheric/libgitlmvc).

```{toctree}
:hidden:

getting-started
commands
views
threading
```
