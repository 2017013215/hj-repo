#include "taskRegistration.h"
#include "taskFactory.h"
#include "moveToTask.h"
#include "moveAlongRouteTask.h"
#include "patrolRouteTask.h"
#include "patrolAreaTask.h"
#include "launchMissileTask.h"
#include "followTask.h"
#include "waitTask.h"
#include "cruiseTask.h"
#include "takeOffTask.h"
#include "returnBaseTask.h"
#include "sensorOperTask.h"

void registerTask( ByyTaskFactory& factory )
{
	factory.appenCreator(new ByyMoveToTaskCreator);
	factory.appenCreator(new ByyMoveAlongRouteTaskCreator);
	factory.appenCreator(new ByyPatrolRouteTaskCreator);
	factory.appenCreator(new ByyPatrolAreaTaskCreator);
	factory.appenCreator(new ByyFollowTaskCreator);
	factory.appenCreator(new ByyWaitTaskCreator);
	factory.appenCreator(new ByyCruiseTaskCreator);
	factory.appenCreator(new ByyLaunchMissileTaskCreator);
	factory.appenCreator(new ByyTakeOffTaskCreator);
	factory.appenCreator(new ByyReturnBaseTaskCreator);
	factory.appenCreator(new ByySensorOperTaskCreator);
}
