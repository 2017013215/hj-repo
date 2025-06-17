#include "StdAfx.h"
#include "taskWidgetFactory.h"

#include "moveToTask.h"
#include "moveToTaskWidget.h"
#include "moveAlongRouteTask.h"
#include "moveAlongRouteTaskWidget.h"
#include "patrolAreaTask.h"
#include "patrolAreaTaskWidget.h"
#include "patrolRouteTask.h"
#include "patrolRouteTaskWidget.h"
#include "launchMissileTask.h"
#include "launchMissileTaskWidget.h"
#include "waitTask.h"
#include "waitTaskWidget.h"
#include "followTask.h"
#include "followTaskWidget.h"
#include "cruiseTask.h"
#include "cruiseTaskWidget.h"
#include "takeOffTask.h"
#include "takeOffTaskWidget.h"
#include "returnBaseTask.h"
#include "returnBaseTaskWidget.h"
#include "sensorOperTask.h"
#include "sensorOperTaskWidget.h"

struct RegisterTaskWidgetCreator
{
	RegisterTaskWidgetCreator()
	{
		ByyTaskWidgetFactory::instance().addCreator(new ByyMoveToTaskWidgetCreator);
		ByyTaskWidgetFactory::instance().addCreator(new ByyMoveAlongRouteTaskWidgetCreator);
		ByyTaskWidgetFactory::instance().addCreator(new ByyPatrolRouteTaskWidgetCreator);
		ByyTaskWidgetFactory::instance().addCreator(new ByyPatrolAreaTaskWidgetCreator);
		ByyTaskWidgetFactory::instance().addCreator(new ByyLaunchMissileTaskWidgetCreator);
		ByyTaskWidgetFactory::instance().addCreator(new ByyWaitTaskWidgetCreator);
		ByyTaskWidgetFactory::instance().addCreator(new ByyFollowTaskWidgetCreator);
		ByyTaskWidgetFactory::instance().addCreator(new ByyCruiseTaskWidgetCreator);
		ByyTaskWidgetFactory::instance().addCreator(new ByyTakeOffTaskWidgetCreator);
		ByyTaskWidgetFactory::instance().addCreator(new ByyReturnBaseTaskWidgetCreator);
		ByyTaskWidgetFactory::instance().addCreator(new ByySensorOperTaskWidgetCreator);
	}
};

static RegisterTaskWidgetCreator _register;

ByyTaskWidgetFactory::ByyTaskWidgetFactory()
{

}

ByyTaskWidgetFactory::~ByyTaskWidgetFactory()
{

}

ByyTaskWidgetFactory& ByyTaskWidgetFactory::instance()
{
	static ByyTaskWidgetFactory _instance;
	return _instance;
}

void ByyTaskWidgetFactory::addCreator( ByyTaskEditorWidgetCreator* creator )
{
	myTaskEditorWidgetCreators[creator->type()]=creator;
}

void ByyTaskWidgetFactory::removeCreator( ByyTaskEditorWidgetCreator* creator )
{
	TaskEditorWidgetCreatorMap::iterator it=myTaskEditorWidgetCreators.find(creator->type());

	if (it!=myTaskEditorWidgetCreators.end()&&it.value()==creator)
	{
		myTaskEditorWidgetCreators.erase(it);
	}
}

ByyTaskEditorWidget* ByyTaskWidgetFactory::createTaskEditor( const QString& taskType,ByyIG& ig,QWidget* parent )
{
	TaskEditorWidgetCreatorMap::iterator it=myTaskEditorWidgetCreators.find(taskType);

	if (it!=myTaskEditorWidgetCreators.end())
	{
		return it.value()->create(ig,parent);
	}

	return NULL;
}
