# Threading

By default, every command runs on the thread that dispatched the request, and its results reach the views before `dispatch()` returns. That's simple, but a long command freezes the user interface while it runs. For such commands, libgitlmvc provides a worker thread.

## Run a command on the worker thread

Call `setInWorkerThread(true)` in the command's constructor:

```cpp
class ExportVideoCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit ExportVideoCommand(QObject* parent = nullptr)
        : GitlAbstractCommand(parent)
    {
        setInWorkerThread(true);
    }

    bool execute(GitlCommandParameter& rcInputArg, GitlCommandParameter& rcOutputArg) override
    {
        // long-running work; the GUI stays responsive
        rcOutputArg.setParameter("export_finished", true);
        return true;
    }
};
```

Nothing changes for the view: it dispatches the request the same way. The difference is when things happen:

- `dispatch()` returns as soon as the request is queued.
- The front controller's worker thread runs the command later.
- When the command finishes, its output is delivered to each view on the view's own thread, normally the GUI thread, through that thread's Qt event loop.

```{admonition} No GUI code in worker-thread commands
:class: warning

A command that runs on the worker thread must not create or touch widgets or other GUI objects. Qt only allows that on the GUI thread. Send results to the views instead, and let them update the widgets.
```

## The rules

- **Commands run one at a time.** The front controller serializes commands, whether they run on the GUI thread or the worker thread, so commands don't need locks to protect the model from each other.
- **Worker-thread commands run in request order.** There is one worker thread, and it takes requests from a queue.
- **The queue is bounded.** If 1000 worker-thread requests are already pending, the front controller waits for room before queueing another. It runs on the GUI thread, so the GUI blocks meanwhile. Change the limit with `GitlFrontController::getInstance()->setMaxEvtInQue()`.
- **Views receive results on their own thread.** View callbacks always run on the thread that created the view, so they can update widgets.
- **The GUI thread needs a running event loop.** Results from the worker thread are delivered by the GUI thread's event loop. In a normal application, `QApplication::exec()` provides it.
- **Views shouldn't read the model.** A worker-thread command may be changing it at the same time. Get the data from the command's output instead.

## Under the hood

libgitlmvc delivers requests and results with [libgitlevtbus](https://libgitlevtbus.readthedocs.io/), and follows its threading model. The front controller and the views are event-bus modules. See the libgitlevtbus [threading page](https://libgitlevtbus.readthedocs.io/en/latest/threading.html) for the details.
