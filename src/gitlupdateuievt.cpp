#include "gitlupdateuievt.h"
#include "gitlmvcconst.h"

GitlUpdateUIEvt::GitlUpdateUIEvt() :
    GitlEvent(GITL_UPDATE_UI_REQUEST_EVENT)
{
}

QString GitlUpdateUIEvt::getCommandName()
{
    return m_cCmdRespond.getParameter("command_name").toString();
}

void GitlUpdateUIEvt::setCommandName(const QString& strCommandName)
{
    m_cCmdRespond.setParameter("command_name", strCommandName);
}

bool GitlUpdateUIEvt::hasParameter(QString strParam) const
{
    return m_cCmdRespond.hasParameter(strParam);
}

QVariant GitlUpdateUIEvt::getParameter(const QString& strParam ) const
{
    return m_cCmdRespond.getParameter(strParam);
}

bool GitlUpdateUIEvt::setParameter(const QString& strParam, const QVariant& rvValue)
{
    return m_cCmdRespond.setParameter(strParam, rvValue);
}
