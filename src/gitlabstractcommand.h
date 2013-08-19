#ifndef GITLABSTRACTCOMMAND_H
#define GITLABSTRACTCOMMAND_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QDebug>
#include <QVariant>
#include "../libgitlevtbus/src/gitldef.h"
#include "gitlcommandparameter.h"


/*!
 * \brief The GitlAbstractCommand class
 *        All command should inherit this class and implement the execute function
 */
class GitlAbstractCommand : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit GitlAbstractCommand(QObject *parent = 0):
        QObject(parent)
    {
    }

    virtual ~GitlAbstractCommand()
    {
    }

    /*!
     * \brief execute All subclass should reimplement this function.
     * \param rcInputArg this parameter is from UI(GitlView)
     * \param rcOutputArg this parameter will be delivered to UI(GitlView)
     * \return
     */
    virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
    {
        qCritical() << "Please reimplement <GitlAbstractCommand::execute>";
        return false;
    }

signals:

public slots:

};

#endif // GITLABSTRACTCOMMAND_H
