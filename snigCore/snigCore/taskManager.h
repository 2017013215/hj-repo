#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QList>
#include "snigcore_global.h"

class ByyTask;

class SNIGCORE_EXPORT ByyTaskManager
{
public:
	ByyTaskManager();
	virtual ~ByyTaskManager();

	void appendTask(ByyTask* task);
	void removeTask(ByyTask* task);

	typedef QList<ByyTask*> TaskList;

	TaskList& taskList();

	bool	taskCanToPrev(ByyTask* task);
	bool	taskCanToNext(ByyTask* task);
	void	taskToPrev(ByyTask* task);
	void	taskToNext(ByyTask* task);
	//TaskList backup();

protected:
	TaskList myTaskList;
};

#endif // TASKMANAGER_H
