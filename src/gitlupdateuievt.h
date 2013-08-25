#ifndef GITLUPDATEUIEVT_H
#define GITLUPDATEUIEVT_H
#include "../libgitlevtbus/src/gitlevent.h"


class GitlUpdateUIEvt : public GitlEvent
{
    /// virtual copy pattern, please add this macro to all the subclass
    VIRTUAL_COPY_PATTERN(GitlUpdateUIEvt)

public:
    GitlUpdateUIEvt();

    QString getCommandName();
    void setCommandName(const QString& strCommandName);
};

#endif // GITLUPDATEUIEVT_H
