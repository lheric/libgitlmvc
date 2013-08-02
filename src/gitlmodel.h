#ifndef GITLMODEL_H
#define GITLMODEL_H
#include <QMutex>

template <class T>
class GitlModel
{
public:
    static T* getInstance()
    {
        QMutexLocker cLocker(&m_cMutex);
        if(m_instance == NULL)
            m_instance = new T();
        return m_instance;
    }


protected:
    GitlModel() {}
    static QMutex m_cMutex;
    static T* m_instance;

};

template <class T>
T* GitlModel<T>::m_instance = NULL;

template <class T>
QMutex GitlModel<T>::m_cMutex;

#endif // GITLMODEL_H
