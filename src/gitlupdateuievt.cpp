#include "gitlupdateuievt.h"
#include "gitlmvcconst.h"

GitlUpdateUIEvt::GitlUpdateUIEvt() :
    GitlEvent(GITL_UPDATE_UI_REQUEST_EVENT)
{
}

QString GitlUpdateUIEvt::getCommandName()
{
    return getParameter("command_name").toString();
}

void GitlUpdateUIEvt::setCommandName(const QString& strCommandName)
{
    setParameter("command_name", strCommandName);
}

