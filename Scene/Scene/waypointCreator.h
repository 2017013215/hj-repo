#ifndef WAYPOINTCREATOR_H
#define WAYPOINTCREATOR_H

#include "tacticalGraphicsCreator.h"

class ByyWaypointCreator : public ByyTacticalGraphicsCreator
{
	Q_OBJECT

public:
	ByyWaypointCreator(ByyIG& ig);
	~ByyWaypointCreator();

	virtual ByyControlObject* accept();

	static ByyTacticalGraphicsCreator* create(ByyIG& ig);

protected:
	virtual bool processEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
	virtual bool mouseClicked(int button,int x,int y,osg::View *view);
	virtual bool mouseMove(int x,int y,osg::View *view);

protected:
	osg::ref_ptr<osgEarth::Annotation::PlaceNode> myPlaceNode;

	int myMouseDown;
	float myXDown;
	float myYDown;
};

#endif // WAYPOINTCREATOR_H
