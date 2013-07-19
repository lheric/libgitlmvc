#ifndef GITLFRONTCONTROLLER_H
#define GITLFRONTCONTROLLER_H


#include <QMetaObject>
#include <QVector>

#include "gitldef.h"
#include "gitlcommandrequest.h"
#include "gitlcommandrespond.h"
#include "gitlmodual.h"


typedef struct GitlCommandFormat_s
{
    QString cCommandName;
    const QMetaObject* pMetaObject;
}GitlCommandFormat;


class GitlFrontController : public GitlModual
{
public:
    virtual ~GitlFrontController() {}
    
    virtual bool detonate(GitlEvent& rcEvt);
    
    bool processRequest( GitlCommandRequest& rcRequest, GitlCommandRespond& rcRespond );
    bool addCommand(GitlCommandFormat cCommandFormat);
    bool addCommand(const QString cCommandFormat, const QMetaObject* pMetaObject);
    void removeAllCommand();

protected:
    explicit GitlFrontController();

protected:

    /// command table
    ADD_CLASS_FIELD(QVector<GitlCommandFormat>, cCommandTable, getCommandTable, setCommandTable)

    SINGLETON_PATTERN_DECLARE(GitlFrontController)

};

#endif // GITLFRONTCONTROLLER_H
