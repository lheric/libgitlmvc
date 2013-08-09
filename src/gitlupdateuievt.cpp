#include "gitlupdateuievt.h"
#include "gitlmvcconst.h"

GitlUpdateUIEvt::GitlUpdateUIEvt() :
    GitlEvent(GITL_UPDATE_UI_REQUEST_EVENT)
{
}

QString GitlUpdateUIEvt::getCommandName()
{
    return m_cAllParameters.getParameter("command_name").toString();
}

void GitlUpdateUIEvt::setCommandName(const QString& strCommandName)
{
    m_cAllParameters.setParameter("command_name", strCommandName);
}

bool GitlUpdateUIEvt::hasParameter(QString strParam) const
{
    return m_cAllParameters.hasParameter(strParam);
}

QVariant GitlUpdateUIEvt::getParameter(const QString& strParam ) const
{
    return m_cAllParameters.getParameter(strParam);
}

bool GitlUpdateUIEvt::setParameter(const QString& strParam, const QVariant& rvValue)
{
    return m_cAllParameters.setParameter(strParam, rvValue);
}
