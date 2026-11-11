# Threading

By default, every command runs on the thread that dispatched the request, and its results reach the views before `dispatch()` returns. That's simple, but a long command freezes the user interface while it runs. You can choose, for each command, to run it in the background instead.

## Choose where a command runs

Pass an execution policy when you register the command:

```cpp
GitlFrontController* pcFC = GitlFrontController::getInstance();
pcFC->registerCommand("open_file",    &OpenFileCommand::staticMetaObject);                         // Main (default)
pcFC->registerCommand("save_file",    &SaveFileCommand::staticMetaObject,    GitlExec::Worker);
pcFC->registerCommand("thumbnail",    &ThumbnailCommand::staticMetaObject,   GitlExec::Concurrent);
pcFC->registerCommand("export_video", &ExportVideoCommand::staticMetaObject, GitlExec::queue("io"));
```

| Policy | Where it runs | Runs at the same time as |
|---|---|---|
| `GitlExec::Main` (default) | The dispatching thread, before `dispatch()` returns | Nothing: one `Main` or `Worker` command at a time |
| `GitlExec::Worker` | A background thread, one command at a time, in request order | Nothing: one `Main` or `Worker` command at a time |
| `GitlExec::queue("name")` | A background thread, one command of that queue at a time, in request order | Commands in other queues, `Concurrent` commands, and `Main`/`Worker` commands |
| `GitlExec::Concurrent` | Any free background thread | Everything else, including other `Concurrent` commands |

All background commands share one thread pool, so the number of threads stays bounded however many requests arrive. The pool's size defaults to the number of CPU cores. Change it with `setMaxThreadCount()`.

Nothing changes for the view: it dispatches requests the same way, whatever the policy.

### Which policy to pick

- **Quick commands, and anything that touches widgets → `Main`.**
- **Long commands that change the model → `Worker`.** They never overlap with each other or with `Main` commands, so they need no locks. This is the safe choice for background work.
- **Long commands that only work on their own input → `Concurrent`.** For example, rendering a thumbnail from a file path, or a calculation whose result goes straight into the command's output.
- **Long commands that share some state with each other, but not with the rest → one named queue for them.** For example, all commands that write to the same file or device go in `queue("io")`. They run in order, one at a time, while other queues keep running.

```{admonition} Concurrent and queued commands need care with shared data
:class: warning

`Concurrent` commands, and commands in named queues, can run at the same time as other commands. If they read or write a model that other commands change, protect that data with a lock, or put all the commands that touch it in the same named queue. `Main` and `Worker` commands are serialized for you.
```

### Commands written the old way

A command registered without a policy keeps its old behaviour. If its constructor calls `setInWorkerThread(true)`, it runs as `GitlExec::Worker`; otherwise as `GitlExec::Main`. The front controller reads the flag once, at registration. To use the new policies, pass one to `registerCommand()`; it overrides the flag.

## No GUI code in background commands

A command that runs on a background thread must not create or touch widgets or other GUI objects. Qt only allows that on the GUI thread. Send results to the views instead, and let them update the widgets.

Views always receive results on their own thread, normally the GUI thread, so their callbacks can update widgets safely. Results from background commands are delivered by the GUI thread's event loop; in a normal application, `QApplication::exec()` runs it. Results of `Concurrent` commands, and of commands in different queues, can arrive in any order.

## When too many requests are waiting

Each serial queue (`Worker` and every named queue) holds at most 1000 waiting requests, and at most 1000 `Concurrent` requests can be waiting or running. Change the limit with `setMaxEvtInQue()`.

A request beyond the limit is rejected at once. It's never queued, and the GUI thread doesn't block. The front controller logs a warning and sends the views an update carrying two parameters:

- `command_name`: the rejected command.
- `command_error` (`GITL_COMMAND_ERROR_PARAM`): why it was rejected.

To show such errors, listen for that parameter:

```cpp
listenToParams(GITL_COMMAND_ERROR_PARAM, [this](GitlUpdateUIEvt& rcEvt)
{
    statusBar()->showMessage(rcEvt.getCommandName() + ": " +
                             rcEvt.getParameter(GITL_COMMAND_ERROR_PARAM).toString());
});
```

## Waiting and shutting down

- `waitForDone(msecs)` waits until every background command has finished. It's useful in tests.
- `shutdown(msecs)` stops accepting background requests, drops the ones that haven't started, and waits for the running ones to finish. Requests made afterwards are rejected as above. It's called automatically when the application is about to quit, if the front controller was created after the `QCoreApplication`.

## Under the hood

libgitlmvc delivers requests and results with [libgitlevtbus](https://libgitlevtbus.readthedocs.io/), and follows its threading model. The front controller and the views are event-bus modules. See the libgitlevtbus [threading page](https://libgitlevtbus.readthedocs.io/en/latest/threading.html) for the details.
