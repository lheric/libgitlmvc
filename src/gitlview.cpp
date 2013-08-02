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
        GitlUpdateUIEvt& rcUpdateUIEvt = static_cast<GitlUpdateUIEvt&>(rcEvt);
        onUIUpdate(rcUpdateUIEvt);
    }
    return true;
}

void GitlView::onUIUpdate(GitlUpdateUIEvt& rcEvt)
{
    qWarning() << "GitlView::onGitlUiUpdate NOT implemented yet.";
}
