#include "gitlivkcmdevt.h"
#include "gitlmvcconst.h"

GitlIvkCmdEvt::GitlIvkCmdEvt(const QString& strCommandName) :
    GitlEvent(GITL_EXE_COMMAND_REQUEST_EVENT)
{
    GitlEvent::setParameter("command_name", strCommandName);
}

QString GitlIvkCmdEvt::getCommandName()
{
    return GitlEvent::getParameter("command_name").toString();
}

void GitlIvkCmdEvt::setCommandName(const QString& strCommandName)
{
    GitlEvent::setParameter("command_name", strCommandName);
}
