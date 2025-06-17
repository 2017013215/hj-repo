#include "taskManager.h"
#include "task.h"

ByyTaskManager::ByyTaskManager()
{

}

ByyTaskManager::~ByyTaskManager()
{
	qDeleteAll(myTaskList);
}

void ByyTaskManager::appendTask( ByyTask* task )
{
	myTaskList.append(task);
}

void ByyTaskManager::removeTask( ByyTask* task )
{
	myTaskList.removeOne(task);

	delete task;
}

ByyTaskManager::TaskList& ByyTaskManager::taskList()
{
	return myTaskList;
}

bool ByyTaskManager::taskCanToPrev(ByyTask* task)
{
	if(!myTaskList.contains(task))
		return false;
	return myTaskList.first()!=task;
}
bool ByyTaskManager::taskCanToNext(ByyTask* task)
{
	if(!myTaskList.contains(task))
		return false;
	return myTaskList.last()!=task;
}

void ByyTaskManager::taskToPrev(ByyTask* task)
{
	int idx=myTaskList.indexOf(task);
	myTaskList.removeOne(task);
	myTaskList.insert(idx-1,task);
}
void ByyTaskManager::taskToNext(ByyTask* task)
{
	int idx=myTaskList.indexOf(task);
	myTaskList.removeOne(task);
	myTaskList.insert(idx+1,task);
}

/*
ByyTaskManager::TaskList ByyTaskManager::backup()
{
	ByyTaskManager::TaskList result;

	for (int i=0;i!=myTaskList.size();++i)
	{
		result.append(myTaskList[i]->clone());
	}

	return result;
}
*/
