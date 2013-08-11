#ifndef TESTMODEL_H
#define TESTMODEL_H
#include "gitldef.h"
#include "gitlmodel.h"
/// model
class TestModel: public GitlModel<TestModel>
{
    ADD_CLASS_FIELD(QString, strDataInModel, getDataInModel, setDataInModel)

protected:
    TestModel() {}
    friend class GitlModel<TestModel>;
};

#endif // TESTMODEL_H
