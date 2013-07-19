#include "gitlrefreshuirequestevt.h"
#include "gitlmvcconst.h"

GitlRefreshUIRequestEvt::GitlRefreshUIRequestEvt() :
    GitlEvent(GITL_UPDATE_UI_REQUEST_EVENT)
{
}

QString GitlRefreshUIRequestEvt::getCommandName()
{
    return m_cCmdRespond.getParameter("command_name").toString();
}

void GitlRefreshUIRequestEvt::setCommandName(const QString& strCommandName)
{
    m_cCmdRespond.setParameter("command_name", strCommandName);
}

bool GitlRefreshUIRequestEvt::hasParameter(QString strParam) const
{
    return m_cCmdRespond.hasParameter(strParam);
}

QVariant GitlRefreshUIRequestEvt::getParameter(const QString& strParam ) const
{
    return m_cCmdRespond.getParameter(strParam);
}

bool GitlRefreshUIRequestEvt::setParameter(const QString& strParam, const QVariant& rvValue)
{
    return m_cCmdRespond.setParameter(strParam, rvValue);
}
