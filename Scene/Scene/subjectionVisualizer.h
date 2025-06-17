#ifndef SUBJECTIONVISUALIZER_H
#define SUBJECTIONVISUALIZER_H

#include <QObject>

class ByyEntityObject;
class ByyIG;

class ByySubjectionVisualizer : public QObject
{
	Q_OBJECT

public:
	ByySubjectionVisualizer(ByyEntityObject* parent,ByyEntityObject* child,ByyIG& ig);
	~ByySubjectionVisualizer();

public slots:
	void update();
	void setVisible(bool v);

private:
	ByyIG& myIG;
	ByyEntityObject* myParent;
	ByyEntityObject* myChild;

	osg::ref_ptr<osg::Group> myRoot;
	osg::ref_ptr<osg::Vec3dArray> myVertexs;
	osg::ref_ptr<osg::Geometry> myGeometry;
};

#endif // SUBJECTIONVISUALIZER_H
