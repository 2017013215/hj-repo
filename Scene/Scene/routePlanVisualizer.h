#ifndef ROUTEPLANVISUALIZER_H
#define ROUTEPLANVISUALIZER_H

#include <QObject>

class ByyIG;
class ByyEntityObject;

class ByyRoutePlanVisualizer : public QObject
{
	Q_OBJECT

public:
	ByyRoutePlanVisualizer(ByyEntityObject* entObj,const QVector<osg::Vec3d>& pnts,ByyIG& ig);
	~ByyRoutePlanVisualizer();

public slots:
	void setVisible(bool b);

private:
	osg::ref_ptr<osg::Group> myRoot;
	ByyIG& myIG;
};

#endif // ROUTEPLANVISUALIZER_H
