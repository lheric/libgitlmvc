#ifndef GITLVIEW_H
#define GITLVIEW_H
#include "../libgitlevtbus/src/gitlmodule.h"
#include "gitlupdateuievt.h"

#include <algorithm>
#include <functional>
#include <QStringList>
#include <QMap>


///
/// \brief UIUpdateCallback ui update listener
///
typedef std::function<void (GitlUpdateUIEvt&)> UIUpdateCallback;


class ParamNameList: public QStringList
{
public:
    ParamNameList();

    ParamNameList(const QString& rcStr)
    {
        this->append(rcStr);
    }

    /// A listener fires when an update carries ALL of these parameters, so the list is
    /// really a set: sort it and drop duplicates, so that {"a","b"}, {"b","a"} and
    /// {"a","b","a"} all name the same listener.
    ParamNameList(const QStringList& rcStrList)
    {
        foreach(const QString& str, rcStrList)
            this->append(str);
        this->sort();
        this->removeDuplicates();
    }


    bool operator < (const ParamNameList& cOther) const
    {
        if(this->size() < cOther.size())
            return true;
        if(this->size() > cOther.size())
            return false;
        /// same size: compare element by element. The first element that differs decides,
        /// in either direction, so this is a strict weak ordering as QMap requires.
        return std::lexicographical_compare(this->begin(), this->end(),
                                            cOther.begin(), cOther.end());
    }
};

class GitlView : public GitlModule
{
public:
    GitlView(GitlEventBus* pcEventBus = NULL);
    virtual ~GitlView() {}
    bool detonate(const GitlEvent& rcEvt);    


    bool listenToParams(const QString& rcParams, const UIUpdateCallback& rcCallback);
    bool listenToParams(const QStringList& rcParams, const UIUpdateCallback& rcCallback);
    bool unlistenToParams(const QStringList& rcParams);
    bool isListenToParams(const QStringList& rcParams) const;
    ADD_CLASS_FIELD_PRIVATE( CONCATE(QMap<ParamNameList,UIUpdateCallback>), pafCallbacks )
};

#endif // GITLVIEW_H
