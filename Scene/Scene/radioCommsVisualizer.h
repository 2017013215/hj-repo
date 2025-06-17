#ifndef RADIOCOMMSVISUALIZER_H
#define RADIOCOMMSVISUALIZER_H

#include <QObject>
#include "placeNode.h"

class ByyIG;
class ByyEntityObject;

class ByyRadioCommsVisualizer : public QObject
{
	Q_OBJECT

public:
	ByyRadioCommsVisualizer(ByyEntityObject* sender,ByyEntityObject* target,ByyIG& ig);
	~ByyRadioCommsVisualizer();

public slots:
	void update();
	void setVisible(bool v);

protected:
	ByyIG& myIG;
	ByyEntityObject* mySenderObj;
	ByyEntityObject* myTargetObj;

	osg::ref_ptr<osg::Group> myRoot;
	osg::ref_ptr<osg::Vec3dArray> myVertexs;
	osg::ref_ptr<osg::Geometry> myGeometry;

	osg::ref_ptr<osgEarth::Annotation::ByyPlaceNode> myPlaceNode;
};

#endif // RADIOCOMMSVISUALIZER_H
