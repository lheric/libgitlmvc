#ifndef GITLABSTRACTCOMMAND_H
#define GITLABSTRACTCOMMAND_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QDebug>
#include <QVariant>
#include "../libgitlevtbus/src/gitldef.h"
#include "../libgitlevtbus/src/gitleventparam.h"

typedef GitlEventParam GitlCommandParameter;

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
        m_bInWorkerThread = false;
    }

    virtual ~GitlAbstractCommand()
    {
    }

    /*!
     * \brief Execute All subclass should reimplement this function.
     * \param rcInputArg this parameter is from UI(GitlView)
     * \param rcOutputArg this parameter will be delivered to UI(GitlView)
     * \return
     */
    virtual bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
    {
        qCritical() << "Please reimplement <GitlAbstractCommand::execute>";
        return false;
    }

    /*!
     * \brief Dose this command designed to execute in worker thread.
     * For computational intensive command, it should be set to ture.
     * *****If it involves GUI classes creation, it MUST BE SET TO FALSE*****
     */
    ADD_CLASS_FIELD(bool, bInWorkerThread, getInWorkerThread, setInWorkerThread)

};

#endif // GITLABSTRACTCOMMAND_H
