#ifndef WAYPOINTEDITOR_H
#define WAYPOINTEDITOR_H

#include "waypointCreator.h"

class ByyWaypoint;

class ByyWaypointEditor: public ByyWaypointCreator
{
public:
	ByyWaypointEditor(ByyIG& ig,ByyWaypoint* waypoint);
	~ByyWaypointEditor();

	virtual ByyControlObject* accept();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig,ByyControlObject* obj);

private:
	ByyWaypoint* myWaypoint;

};

#endif // WAYPOINTEDITOR_H
