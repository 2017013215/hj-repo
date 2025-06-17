#ifndef HDOSEEVENTPOOL_H
#define HDOSEEVENTPOOL_H

#include <QQueue>
#include <QSharedPointer>

#include "snighdose_global.h"

class QMutex;
class ByyHdoseEvent;

class SNIGHDOSE_EXPORT ByyHDoseEventPool
{
public:
	virtual ~ByyHDoseEventPool();

	static ByyHDoseEventPool& instance();

	QSharedPointer<ByyHdoseEvent> dequeue();
	void enqueue(const QSharedPointer<ByyHdoseEvent> &t);

	bool isEmpty();

protected:
	ByyHDoseEventPool();

protected:
	QMutex* myMutex;
	QQueue<QSharedPointer<ByyHdoseEvent>> myQueue;
};

#endif // HDOSEEVENTPOOL_H
