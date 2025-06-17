#include "StdAfx.h"
#include "signalSingleton.h"

signalSingleton* signalSingleton::m_instance = 0;

signalSingleton::signalSingleton(QObject* parent)
    : QObject(parent)
{
}

signalSingleton* signalSingleton::instance()
{
    if (!m_instance)
        m_instance = new signalSingleton;
    return m_instance;
}

void signalSingleton::triggerLoadSuccess()
{
    emit loadSuccess();  // 信号只能在类内部发射
}
