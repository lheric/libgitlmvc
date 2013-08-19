#ifndef GITLIVKCMDEVT_H
#define GITLIVKCMDEVT_H

#include <QString>
#include "../libgitlevtbus/src/gitlevent.h"
#include "gitlcommandparameter.h"


class GitlIvkCmdEvt : public GitlEvent
{
    /// virtual copy pattern, please add this macro to all the subclass
    VIRTUAL_COPY_PATTERN(GitlIvkCmdEvt)

public:
    GitlIvkCmdEvt(const QString& strCommandName);

    QString getCommandName();
    void setCommandName(const QString& strCommandName);

    bool hasParameter(QString strParam) const;
    QVariant getParameter(const QString& strParam ) const;
    bool setParameter(const QString& strParam, const QVariant& rvValue);


    ADD_CLASS_FIELD_NOSETTER(GitlCommandParameter, cAllParameters, getAllParameters)
};

#endif // GITLIVKCMDEVT_H
