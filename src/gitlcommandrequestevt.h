#ifndef GITLCOMMANDREQUESTEVT_H
#define GITLCOMMANDREQUESTEVT_H

#include <QString>
#include "gitlevent.h"
#include "gitlcommandrequest.h"


class GitlCommandRequestEvt : public GitlEvent
{
    /// virtual copy pattern, please add this macro to all the subclass
    VIRTUAL_COPY_PATTERN(GitlCommandRequestEvt)

public:
    GitlCommandRequestEvt(const QString& strCommandName);

    QString getCommandName();
    void setCommandName(const QString& strCommandName);

    bool hasParameter(QString strParam) const;
    QVariant getParameter(const QString& strParam ) const;
    bool setParameter(const QString& strParam, const QVariant& rvValue);


    ADD_CLASS_FIELD_NOSETTER(GitlCommandRequest, cCmdRequest, getCmdRequest)
};

#endif // GITLCOMMANDREQUESTEVT_H
