#include "gitlview.h"
#include "gitlmvcconst.h"
#include <QDebug>
GitlView::GitlView()
{
    subscribeToEvtByName( GITL_UPDATE_UI_REQUEST_EVENT );
}


bool GitlView::detonate(GitlEvent& rcEvt)
{
    if(rcEvt.getEvtName() == GITL_UPDATE_UI_REQUEST_EVENT )
    {
        GitlRefreshUIRequestEvt& rcUpdateUIEvt = static_cast<GitlRefreshUIRequestEvt&>(rcEvt);
        onGitlUiUpdate(rcUpdateUIEvt);
    }
    return true;
}

void GitlView::onGitlUiUpdate(GitlRefreshUIRequestEvt& rcEvt)
{
    qWarning() << "GitlView::onGitlUiUpdate NOT implemented yet.";
}
