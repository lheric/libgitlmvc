#ifndef GITLVIEW_H
#define GITLVIEW_H
#include "gitlmodual.h"
#include "gitlupdateuievt.h"
class GitlView : public GitlModual
{
public:
    GitlView();
    virtual ~GitlView() {}
    virtual bool detonate(GitlEvent& rcEvt);
    virtual void onUIUpdate(GitlUpdateUIEvt& rcEvt);
};

#endif // GITLVIEW_H
