#ifndef GITLIVKCMDEVT_H
#define GITLIVKCMDEVT_H

#include <QString>
#include "../libgitlevtbus/src/gitlevent.h"


class GitlIvkCmdEvt : public GitlEvent
{
    /// virtual copy pattern, please add this macro to all the subclass
    CLONABLE(GitlIvkCmdEvt)

public:
    GitlIvkCmdEvt(const QString& strCommandName);

    QString getCommandName();
    void setCommandName(const QString& strCommandName);

};

#endif // GITLIVKCMDEVT_H
