#ifndef GITLFRONTCONTROLLER_H
#define GITLFRONTCONTROLLER_H


#include <QMetaObject>
#include <QVector>

#include "gitldef.h"
#include "gitlcommandrequest.h"
#include "gitlcommandrespond.h"

/*!
 * < command name , command class > pair
 */
typedef struct GitlCommandFormat_s
{
    QString cCommandName;               ///< command name
    const QMetaObject* pMetaObject;     ///< command class
}GitlCommandFormat;


class GitlFrontController
{
public:
    /*!
     * \brief processRequest
     * \param rcRequest
     * \param rcRespond
     * \return
     */
    bool processRequest( GitlCommandRequest& rcRequest, GitlCommandRespond& rcRespond );

    /*!
     * \brief addCommand register a command
     * \param cCommandFormat
     * \return
     */
    bool addCommand(GitlCommandFormat cCommandFormat);

    /*!
     * \brief addCommand register a command
     * \param cCommandFormat
     * \param pMetaObject
     * \return
     */
    bool addCommand(const QString cCommandFormat, const QMetaObject* pMetaObject);

    /*!
     * \brief removeAllCommand remove all command
     */
    void removeAllCommand();

protected:
    explicit GitlFrontController();

protected:

    /// command table
    ADD_CLASS_FIELD(QVector<GitlCommandFormat>, cCommandTable, getCommandTable, setCommandTable)

    SINGLETON_PATTERN_DECLARE(GitlFrontController)

};

#endif // GITLFRONTCONTROLLER_H
