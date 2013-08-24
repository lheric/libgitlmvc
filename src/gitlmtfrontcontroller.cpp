#include "gitlmtfrontcontroller.h"
#include <QDebug>
GitlMTFrontController::GitlMTFrontController()
{
    m_iMaxEvtInQue = 1000;
    setModualName("multi_thread_front_controller");
    this->start();
}

bool GitlMTFrontController::detonate(GitlEvent &cEvt )
{

    m_cEvtQueMutex.lock();
    if( m_pcEvtQue.size() >= m_iMaxEvtInQue )
    {
        qCritical() << QString("Too Many Events Pending...Waiting...");
        m_cEvtQueNotFull.wait(&m_cEvtQueMutex);
        qDebug() << QString("Event Queue Not Full...Moving on...");
    }
    m_pcEvtQue.push_back(cEvt.clone());
    m_cEvtQueMutex.unlock();
    m_cEvtQueNotEmpty.wakeAll();
    return true;

}


void GitlMTFrontController::run()
{
    forever
    {
        /// get one event from the waiting queue
        m_cEvtQueMutex.lock();
        if( m_pcEvtQue.empty() )
        {
            m_cEvtQueNotEmpty.wait(&m_cEvtQueMutex);
        }
        GitlEvent* pcEvt = m_pcEvtQue.front();
        m_pcEvtQue.pop_front();
        m_cEvtQueMutex.unlock();
        m_cEvtQueNotFull.wakeAll();


        /// do command & exception handling
        GitlFrontController::detonate(*pcEvt);
        delete pcEvt;

    }
}
