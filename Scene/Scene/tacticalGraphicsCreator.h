#ifndef TACTICALGRAPHICSCREATOR_H
#define TACTICALGRAPHICSCREATOR_H

#include <QObject>
#include "ByyEventProcessor.h"

class ByyControlObject;
class ByyIG;

typedef QVector<osg::Vec3d> ByyVec3dArray;

class SCENE_EXPORT ByyTacticalGraphicsCreator : public QObject
{
	Q_OBJECT
public:
	ByyTacticalGraphicsCreator(ByyIG& ig);
	virtual ~ByyTacticalGraphicsCreator();

	virtual ByyControlObject* accept()=0;

	enum Result{Cancel,Ok};

protected:
	virtual bool processEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

	virtual bool mouseClicked(int button,int x,int y,osg::View *view);
	virtual bool mouseMove(int button,int x,int y,osg::View *view);

signals:
	void finished(int);

protected:
	ByyIG& myIG;
	osg::observer_ptr<osgEarth::MapNode> myMapNode;
	osg::observer_ptr<osg::Group> myRoot;

	int myMouseDown;
	float myXDown;
	float myYDown;

	osg::Vec4f myPathColor;
	osg::Vec4f myFillColor;

	typedef ByyHostEventProcessor<ByyTacticalGraphicsCreator> MyEventProcessor;
	friend class MyEventProcessor;

	MyEventProcessor myEventProcessor;
};

class ByySceneObjCreateVisualizer
{
public:
	ByySceneObjCreateVisualizer(ByyIG& ig);

protected:
	osg::observer_ptr<osgEarth::MapNode> myMapNode;
	osg::observer_ptr<osg::Group> myRoot;
	osg::Vec4f myPathColor;
	osg::Vec4f myFillColor;
};

#endif // TACTICALGRAPHICSCREATOR_H
