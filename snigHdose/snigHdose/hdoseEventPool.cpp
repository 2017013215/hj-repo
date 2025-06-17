#include "hdoseEventPool.h"

#include <QMutex>

ByyHDoseEventPool::ByyHDoseEventPool()
	:myMutex(new QMutex)
{

}

ByyHDoseEventPool::~ByyHDoseEventPool()
{
	delete myMutex;
}

ByyHDoseEventPool& ByyHDoseEventPool::instance()
{
	static ByyHDoseEventPool _instance;
	return _instance;
}

QSharedPointer<ByyHdoseEvent> ByyHDoseEventPool::dequeue()
{
	QMutexLocker locker(myMutex);
	return myQueue.dequeue();
}

void ByyHDoseEventPool::enqueue( const QSharedPointer<ByyHdoseEvent> &t )
{
	QMutexLocker locker(myMutex);
	myQueue.enqueue(t);
}

bool ByyHDoseEventPool::isEmpty()
{
	QMutexLocker locker(myMutex);
	return myQueue.isEmpty();
}
