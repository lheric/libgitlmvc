#ifndef GITLREFRESHUIREQUESTEVT_H
#define GITLREFRESHUIREQUESTEVT_H
#include "gitlevent.h"
#include "gitlcommandrespond.h"

class GitlRefreshUIRequestEvt : public GitlEvent
{
    /// virtual copy pattern, please add this macro to all the subclass
    VIRTUAL_COPY_PATTERN(GitlRefreshUIRequestEvt)

public:
    GitlRefreshUIRequestEvt();

    QString getCommandName();
    void setCommandName(const QString& strCommandName);

    bool hasParameter(QString strParam) const;
    QVariant getParameter(const QString& strParam ) const;
    bool setParameter(const QString& strParam, const QVariant& rvValue);


    ADD_CLASS_FIELD_NOSETTER(GitlCommandRespond, cCmdRespond, getCmdRespond)
};

#endif // GITLREFRESHUIREQUESTEVT_H
