#ifndef GUIDANCEVISUALIZER_H
#define GUIDANCEVISUALIZER_H

#include <QObject>

class ByyEntityObject;
class ByyIG;

class ByyGuidanceVisualizer : public QObject
{
	Q_OBJECT

public:
	ByyGuidanceVisualizer(ByyEntityObject* sender,ByyEntityObject* target,ByyIG& ig);
	~ByyGuidanceVisualizer();

public slots:
	void update();
	void setVisible(bool v);

private:
	ByyIG& myIG;
	ByyEntityObject* mySender;
	ByyEntityObject* myTarget;

	osg::ref_ptr<osg::Group> myRoot;
	osg::ref_ptr<osg::Vec3dArray> myVertexs;
	osg::ref_ptr<osg::Geometry> myGeometry;

	double myLastTickTime;
};

#endif // GUIDANCEVISUALIZER_H
