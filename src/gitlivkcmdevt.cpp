#include "gitlivkcmdevt.h"
#include "gitlmvcconst.h"

GitlIvkCmdEvt::GitlIvkCmdEvt(const QString& strCommandName) :
    GitlEvent(GITL_EXE_COMMAND_REQUEST_EVENT)
{
    m_cAllParameters.setParameter("command_name", strCommandName);
}

QString GitlIvkCmdEvt::getCommandName()
{
    return m_cAllParameters.getParameter("command_name").toString();
}

void GitlIvkCmdEvt::setCommandName(const QString& strCommandName)
{
    m_cAllParameters.setParameter("command_name", strCommandName);
}

bool GitlIvkCmdEvt::hasParameter(QString strParam) const
{
    return m_cAllParameters.hasParameter(strParam);
}

QVariant GitlIvkCmdEvt::getParameter(const QString& strParam ) const
{
    return m_cAllParameters.getParameter(strParam);
}

bool GitlIvkCmdEvt::setParameter(const QString& strParam, const QVariant& rvValue)
{
    return m_cAllParameters.setParameter(strParam, rvValue);
}
