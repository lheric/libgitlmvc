#ifndef GITLVIEW_H
#define GITLVIEW_H
#include "gitlmodual.h"
#include "gitlrefreshuirequestevt.h"
class GitlView : public GitlModual
{
public:
    GitlView();
    virtual ~GitlView() {}
    virtual bool detonate(GitlEvent& rcEvt);
    virtual void onGitlUiUpdate(GitlRefreshUIRequestEvt& rcEvt);
};

#endif // GITLVIEW_H
