#ifndef GITLVIEW_H
#define GITLVIEW_H
#include "../libgitlevtbus/src/gitlmodule.h"
#include "gitlupdateuievt.h"

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

    ParamNameList(const QStringList& rcStrList)
    {
        foreach(const QString& str, rcStrList)
            this->append(str);
    }


    bool operator < (const ParamNameList& cOther) const
    {
        if(this->size() < cOther.size())
            return true;
        if(this->size() > cOther.size())
            return false;
        int iSize = this->size();

        for(int i = 0; i < iSize; i++)
        {
            if(this->at(i) < cOther.at(i))
                return true;
        }

        return false;
    }
};

class GitlView : public GitlModule
{
public:
    GitlView(GitlEventBus* pcEventBus = NULL);
    virtual ~GitlView() {}
    bool detonate(GitlEvent& rcEvt);    


    bool listenToParams(const QString& rcParams, const UIUpdateCallback& rcCallback);
    bool listenToParams(const QStringList& rcParams, const UIUpdateCallback& rcCallback);
    bool unlistenToParams(const QStringList& rcParams);
    bool isListenToParams(const QStringList& rcParams) const;
    ADD_CLASS_FIELD_PRIVATE( CONCATE(QMap<ParamNameList,UIUpdateCallback>), pafCallbacks )
};

#endif // GITLVIEW_H
