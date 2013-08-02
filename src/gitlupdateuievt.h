#ifndef GITLUPDATEUIEVT_H
#define GITLUPDATEUIEVT_H
#include "gitlevent.h"
#include "gitlcommandrespond.h"

class GitlUpdateUIEvt : public GitlEvent
{
    /// virtual copy pattern, please add this macro to all the subclass
    VIRTUAL_COPY_PATTERN(GitlUpdateUIEvt)

public:
    GitlUpdateUIEvt();

    QString getCommandName();
    void setCommandName(const QString& strCommandName);

    bool hasParameter(QString strParam) const;
    QVariant getParameter(const QString& strParam ) const;
    bool setParameter(const QString& strParam, const QVariant& rvValue);


    ADD_CLASS_FIELD_NOSETTER(GitlCommandRespond, cCmdRespond, getCmdRespond)
};

#endif // GITLUPDATEUIEVT_H
