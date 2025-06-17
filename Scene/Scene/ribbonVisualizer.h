#ifndef RIBBONVISUALIZER_H
#define RIBBONVISUALIZER_H

#include "stateVisualizer.h"

class ByyRibbonVisualizer : public ByyStateVisualizer
{
	Q_OBJECT
public:
	ByyRibbonVisualizer(ByyEntityObject& entObj,ByyIG& ig);
	~ByyRibbonVisualizer();

	static const QString& theType();
	virtual const QString& type();

public slots:
	void updateForceType();
	void setVisible(bool v);
	void update(const osg::Vec3d& position);

protected:
	void init();
	void uninit();

protected:
	osg::ref_ptr<osg::Geode> myGeode;
	osg::ref_ptr<osg::Geometry> myGeometry;
	osg::ref_ptr<osg::Vec3Array> myVertices;
	osg::ref_ptr<osg::Vec4Array> myColors;
	osg::ref_ptr<osg::Vec3Array> myNormals;

	osg::ref_ptr<osg::MatrixTransform> myRibbonMt;

	float myHalfWidth;
	int myNumPoints;

	osg::Vec3d myRibbonOrigin;

};

typedef ByyStateVisualizerCreatorTemplate<ByyRibbonVisualizer> ByyRibbonVisualizerCreator;

#endif // RIBBONVISUALIZER_H
