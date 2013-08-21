#include "gitlfrontcontroller.h"
#include "gitlabstractcommand.h"

#include <QSharedPointer>
#include <QDebug>
#include "gitlmvcconst.h"
#include "gitlupdateuievt.h"

SINGLETON_PATTERN_IMPLIMENT(GitlFrontController)

GitlFrontController::GitlFrontController()
{
    subscribeToEvtByName(GITL_EXE_COMMAND_REQUEST_EVENT);
}


bool GitlFrontController::detonate( GitlEvent& rcEvt)
{
    if(rcEvt.getEvtName() == GITL_EXE_COMMAND_REQUEST_EVENT)
    {
       GitlIvkCmdEvt& rcCmdRequestEvt = dynamic_cast<GitlIvkCmdEvt&>(rcEvt);
       onCommandRequestArrive(rcCmdRequestEvt);
    }
    return true;
}

void GitlFrontController::onCommandRequestArrive(GitlIvkCmdEvt &rcEvt)
{
    GitlUpdateUIEvt cRefreshUIEvt;
    GitlCommandParameter& rcRequest = rcEvt.getAllParameters();
    GitlCommandParameter& rcRespond = cRefreshUIEvt.getAllParameters();

    // find command by name
    QVariant vValue;
    if( rcRequest.hasParameter("command_name") )
    {
        vValue = rcRequest.getParameter("command_name");
    }
    else
    {
        qWarning() << QString("No command name in request!");
        return;
    }
    QString strCommandName = vValue.toString();
    rcRespond.setParameter("command_name", strCommandName);
    QHash<QString, QMetaObject*>::iterator i = m_cCommandTable.find(strCommandName);
    if( i != m_cCommandTable.end() )
    {
        //command name matched
        //create  command
        const QMetaObject* pMetaObj = i.value();
        QObject* pObj = pMetaObj->newInstance();
        //if fail to create command class
        if(pObj == NULL)
        {
            qCritical() << QString("Unable to create command class '%1'! Please ensure the constructor have Q_INVOKABLE macro.")
                        .arg(pMetaObj->className());
            return;
        }
        QSharedPointer<GitlAbstractCommand> pCmd(static_cast<GitlAbstractCommand *>(pObj));
        //execute command
        if( pCmd->execute(rcRequest, rcRespond) == false )
        {
            qDebug() << QString("%1 Execution Failed!").arg(pMetaObj->className());
        }
        else
        {
            cRefreshUIEvt.dispatch();
            qDebug() << QString("%1 Execution Success!").arg(pMetaObj->className());
        }
        return;

    }
    qWarning() << QString("No matched command name found. %1").arg(strCommandName);
    return;



}


bool GitlFrontController::registerCommand(const QString cCommandName, const QMetaObject *pMetaObject)
{
    if(m_cCommandTable.contains(cCommandName))
    {
        qCritical() << QString("%1 Register Fail! Duplicated command name!").arg(cCommandName);
        return false;
    }

    QMetaObject *p = const_cast<QMetaObject *>(pMetaObject);
    m_cCommandTable.insert(cCommandName, p);
    qDebug() << QString("%1 Register Success!").arg(pMetaObject->className());
    return true;
}

void GitlFrontController::unregisterAllCommand()
{
    m_cCommandTable.clear();
}



