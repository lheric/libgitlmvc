# Views

A view is the part of your user interface that shows results. It derives from `GitlView`, and it never reads the model directly. Instead, it listens to the output of commands.

## Listen to a parameter

`listenToParams()` registers a callback that runs whenever a command's output contains a given parameter:

```cpp
class StatusBar : public QStatusBar, public GitlView
{
public:
    StatusBar()
    {
        listenToParams("document_title", MAKE_CALLBACK(StatusBar::onTitleChanged));
    }

    void onTitleChanged(GitlUpdateUIEvt& rcEvt)
    {
        showMessage(rcEvt.getParameter("document_title").toString());
    }
};
```

`MAKE_CALLBACK` turns a member function into a callback. A lambda works too:

```cpp
listenToParams("document_title", [this](GitlUpdateUIEvt& rcEvt)
{
    showMessage(rcEvt.getParameter("document_title").toString());
});
```

The callback receives a `GitlUpdateUIEvt`:

- `getParameter()` reads any parameter from the command's output.
- `getCommandName()` tells you which command produced it.

## Listen to several parameters

Pass a list to run the callback only when the output contains **all** of the parameters:

```cpp
listenToParams(QStringList() << "width" << "height",
               MAKE_CALLBACK(ImageInfoPanel::onSizeChanged));
```

A view can register as many callbacks as it needs. One command output can trigger several of them. For example, if a view listens to `"width"`, to `"height"`, and to both together, an output containing both parameters runs all three callbacks.

## Stop listening

- `unlistenToParams(list)` removes the callback registered for exactly that list of parameters.
- `isListenToParams(list)` checks whether there is one.
- Registering a callback for a list that already has one replaces the old callback.

## Many views

Every view receives every command output, and each view picks out the parameters it cares about. A command doesn't know which views exist, so you can add a new view without changing a single command.

## Which thread callbacks run on

View callbacks run on the thread that created the view, normally the GUI thread. That holds even when the command ran on a worker thread, so a callback can always update widgets safely. See [Threading](threading.md).
