#include "gitlcommandrequestevt.h"
#include "gitlmvcconst.h"

GitlCommandRequestEvt::GitlCommandRequestEvt(const QString& strCommandName) :
    GitlEvent(GITL_EXE_COMMAND_REQUEST_EVENT)
{
    m_cCmdRequest.setParameter("command_name", strCommandName);
}

QString GitlCommandRequestEvt::getCommandName()
{
    return m_cCmdRequest.getParameter("command_name").toString();
}

void GitlCommandRequestEvt::setCommandName(const QString& strCommandName)
{
    m_cCmdRequest.setParameter("command_name", strCommandName);
}

bool GitlCommandRequestEvt::hasParameter(QString strParam) const
{
    return m_cCmdRequest.hasParameter(strParam);
}

QVariant GitlCommandRequestEvt::getParameter(const QString& strParam ) const
{
    return m_cCmdRequest.getParameter(strParam);
}

bool GitlCommandRequestEvt::setParameter(const QString& strParam, const QVariant& rvValue)
{
    return m_cCmdRequest.setParameter(strParam, rvValue);
}
