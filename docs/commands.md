# Commands

A command implements one use case of your application: open a file, apply a filter, save the settings. Views ask for commands by name, and the front controller runs them.

## Register commands

The front controller, `GitlFrontController`, is a singleton. Register every command with it once, usually at startup:

```cpp
GitlFrontController* pcFC = GitlFrontController::getInstance();
pcFC->registerCommand("open_file_command", &OpenFileCommand::staticMetaObject);
pcFC->registerCommand("save_file_command", &SaveFileCommand::staticMetaObject);
```

- Each name maps to one command class. Registering a name that's already taken fails and returns `false`.
- `unregisterAllCommand()` clears the table.
- A request for a name that isn't registered logs a warning and does nothing else.

## Request a command

Any code can request a command by dispatching a `GitlIvkCmdEvt`, though usually a view does it:

```cpp
GitlIvkCmdEvt cRequest("open_file_command");
cRequest.setParameter("file_path", "/tmp/image.png");
cRequest.dispatch();
```

Parameters are name–value pairs. The values are `QVariant`s, so any type that `QVariant` can hold works.

## Write a command

```cpp
class OpenFileCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit OpenFileCommand(QObject* parent = nullptr)
        : GitlAbstractCommand(parent) {}

    bool execute(GitlCommandParameter& rcInputArg, GitlCommandParameter& rcOutputArg) override
    {
        QString strPath = rcInputArg.getParameter("file_path").toString();
        if (!DocumentModel::getInstance()->open(strPath))
            return false;                                  // views are not updated

        rcOutputArg.setParameter("document_title", QFileInfo(strPath).fileName());
        return true;
    }
};
```

- **Input:** `rcInputArg` holds the parameters of the request. Use `hasParameter()` to check for optional ones.
- **Output:** the parameters you set on `rcOutputArg` are sent to the views as a `GitlUpdateUIEvt`. The output always includes the parameter `command_name`, holding the name the command was requested under.
- **Return value:** return `true` on success. If you return `false`, the views aren't updated, and the failure is logged.
- **Lifetime:** the front controller creates a new command object for every request, so don't keep state in a command. State belongs in the model.

The class needs `Q_OBJECT`, and its constructor needs `Q_INVOKABLE`, because the front controller creates commands through Qt's meta-object system.

## Chain commands

A command can request other commands from `execute()`. When they run on the same thread, they finish before `dispatch()` returns, each one updating the views with its own output:

```cpp
bool execute(GitlCommandParameter&, GitlCommandParameter&) override
{
    GitlIvkCmdEvt("open_file_command").dispatch();
    GitlIvkCmdEvt("refresh_thumbnails_command").dispatch();
    return true;
}
```

This way, small commands can be combined into larger ones. `NestedCommand` in `test/testcase.cpp` is a complete example.

## One command at a time

The front controller runs commands one at a time. A command that's requested while another is executing on a different thread waits for it to finish. Nested requests from inside `execute()` on the same thread run immediately, as shown above.

As a result, commands don't need locks to protect the model from each other.
