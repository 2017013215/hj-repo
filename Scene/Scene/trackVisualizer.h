#ifndef TRACKVISUALIZER_H
#define TRACKVISUALIZER_H

#include <QObject>

class ByyEntityObject;
class ByyIG;

class ByyTrackVisualizer : public QObject
{
	Q_OBJECT

public:
	ByyTrackVisualizer(ByyEntityObject* sender,ByyEntityObject* target,ByyIG& ig);
	~ByyTrackVisualizer();

public slots:
	void update();
	void setVisible(bool v);

protected slots:
	void tickAnimation();

private:
	ByyIG& myIG;
	ByyEntityObject* mySender;
	ByyEntityObject* myTarget;

	osg::ref_ptr<osg::Group> myRoot;
	osg::ref_ptr<osg::Vec3dArray> myVertexs;
	osg::ref_ptr<osg::Geometry> myGeometry;
	osg::ref_ptr<osg::Geode> myGeode;

	double myLastTickTime;
};

#endif // TRACKVISUALIZER_H
