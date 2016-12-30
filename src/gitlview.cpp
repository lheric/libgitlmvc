#include "gitlview.h"
#include "gitlmvcconst.h"
#include <QDebug>
GitlView::GitlView(GitlEventBus *pcEventBus):
    GitlModule(pcEventBus)
{
    subscribeToEvtByName( GITL_UPDATE_UI_REQUEST_EVENT, MAKE_CALLBACK(GitlView::detonate) );
}


bool GitlView::detonate(GitlEvent& rcEvt)
{
    GitlUpdateUIEvt& rcUpdateUIEvt = static_cast<GitlUpdateUIEvt&>(rcEvt);

    QMap<ParamNameList,UIUpdateCallback>::iterator iter= m_pafCallbacks.begin();
    /// check every callback functions
    while(iter != m_pafCallbacks.end())
    {
        /// if the event has all the parameters that this callback function listens to, call it.
        bool findAllParams = true;
        const ParamNameList& cList = iter.key();
        for(int i = 0; i < cList.size(); i++)
        {
            if(rcEvt.getParameters().hasParameter(cList.at(i)) == false)
            {
                findAllParams = false;
                break;
            }
        }

        if(findAllParams == true)
            (iter.value())(rcUpdateUIEvt);      /// call

        iter++;     /// check next
    }

    return true;
}

bool GitlView::listenToParams(const QString& rcParams, const UIUpdateCallback& rcCallback)
{
    m_pafCallbacks[rcParams] = rcCallback;
    return true;
}


bool GitlView::listenToParams(const QStringList& rcParams, const UIUpdateCallback& rcCallback)
{
    m_pafCallbacks[rcParams] = rcCallback;
    return true;
}

bool GitlView::unlistenToParams(const QStringList& rcParams)
{
    return m_pafCallbacks.remove(rcParams);
}

bool GitlView::isListenToParams(const QStringList& rcParams) const
{
    return m_pafCallbacks.contains(rcParams);
}
