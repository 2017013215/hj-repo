#ifndef TASKFACTORY_H
#define TASKFACTORY_H

#include <QList>
#include "snigcore_global.h"

class ByyTaskCreator;

class SNIGCORE_EXPORT ByyTaskFactory
{
public:
	ByyTaskFactory();
	virtual ~ByyTaskFactory();

	void appenCreator(ByyTaskCreator* creator);

	typedef QList<ByyTaskCreator*> TaskCreatorList;

	TaskCreatorList& creators();

	ByyTaskCreator* findCreator(const QString& type);

private:
	TaskCreatorList myCreators;
};

#endif // TASKFACTORY_H
