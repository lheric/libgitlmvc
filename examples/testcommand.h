#ifndef TESTCOMMAND_H
#define TESTCOMMAND_H
#include "gitlabstractcommand.h"
#include "testmodel.h"

/// command, it manipulates the model and writes the result to output parameter. The output parameter will
/// be pass to view automatically.
class FirParamCommand : public GitlAbstractCommand
{
    Q_OBJECT
public:
    /// Q_INVOKABLE is necessary for constructor
    Q_INVOKABLE explicit FirParamCommand(QObject *parent = 0):GitlAbstractCommand(parent) {}
    bool execute(GitlCommandParameter &rcInputArg, GitlCommandParameter &rcOutputArg)
    {
        QString strDataToCommand = rcInputArg.getParameter("data_to_command").toString();
        TestModel::getInstance()->setDataInModel(strDataToCommand);
        rcOutputArg.setParameter("data_to_view", strDataToCommand);
        return true;
    }

};

#endif // TESTCOMMAND_H
