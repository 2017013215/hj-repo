#include "StdAfx.h"
#include "sphereElectronicVisualizer.h"
#include "entityObject.h"
#include "utils.h"
#include "entitySettings.h"

#include <osg/Shape>
#include <osg/PolygonMode>
#include <osg/ClipPlane>

ByySphereElectronicVisualizer::ByySphereElectronicVisualizer( ByyEntityObject& entObj,ByyIG& ig ) 
	: ByyStateVisualizer(entObj,ig)
{
	osg::Vec3Array* sphereVertexs=new osg::Vec3Array;

	mySphereColors=new osg::Vec4Array;
	mySphereColors->push_back(osg::Vec4(1,1,1,1));

	osg::Vec3Array *sphereNormals=new osg::Vec3Array;
	sphereNormals->push_back(osg::X_AXIS);

	int hPointNum=26;
	int vPointNum=8;

	double radius=10000;
	double hAngle=360.0/(hPointNum-1);
	double vAngle=75.0/vPointNum;

	osg::Vec3 axis(0,radius,0);

	sphereVertexs->resize(vPointNum*hPointNum);

	osg::Vec3Array *sectorVertexs=new osg::Vec3Array;
	sectorVertexs->resize(vPointNum);

	int index=0;

	for (int  i=0;i<vPointNum;++i)
	{
		for (int j=0;j<hPointNum;++j)
		{
			osg::Matrixf matrix=osg::Matrix::rotate(osg::DegreesToRadians(i*vAngle),osg::X_AXIS,
				0,osg::Y_AXIS,
				osg::DegreesToRadians(j*hAngle),osg::Z_AXIS);

			osg::Vec3 aa=matrix.preMult(axis);

			(*sphereVertexs)[index]=matrix.preMult(axis);
			++index;
		}
		(*sectorVertexs)[i]=(*sphereVertexs)[index-1];
	}

	sectorVertexs->push_back(osg::Vec3(0,0,0));

	std::reverse(sectorVertexs->begin(),sectorVertexs->end());

	osg::DrawElementsUShort *primiSet=new osg::DrawElementsUShort(GL_QUADS);

	for (int i=0;i<vPointNum-1;++i)
	{
		for (int j=0;j<hPointNum-1;++j)
		{
			primiSet->push_back(i*hPointNum+j);
			primiSet->push_back((i+1)*hPointNum+j);
			primiSet->push_back((i+1)*hPointNum+j+1);
			primiSet->push_back(i*hPointNum+j+1);
		}
	}

	myGeom=new osg::Geometry;
	myGeom->setUseVertexBufferObjects(true);
	myGeom->setUseDisplayList(false);
	myGeom->setVertexArray(sphereVertexs);
	myGeom->setColorArray(mySphereColors);
	myGeom->setNormalArray(sphereNormals);

	myGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	myGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	myGeom->addPrimitiveSet(primiSet);

	osg::Geode *sphereGeode=new osg::Geode;
	sphereGeode->addDrawable(myGeom);
	sphereGeode->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE));


	osg::Vec4Array *sectorColors=new osg::Vec4Array;
	sectorColors->push_back(osg::Vec4(0,1,0,0.5));

	osg::Vec3Array *sectorNormals=new osg::Vec3Array;
	sectorNormals->push_back(osg::X_AXIS);

	osg::Geometry *sectorGeom=new osg::Geometry;
	sectorGeom->setUseVertexBufferObjects(true);
	sectorGeom->setUseDisplayList(false);
	sectorGeom->setVertexArray(sectorVertexs);
	sectorGeom->setColorArray(sectorColors);
	sectorGeom->setNormalArray(sectorNormals);

	sectorGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	sectorGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

	sectorGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,0,sectorVertexs->size()));
	
	osg::Geode* sectorGeode=new osg::Geode;
	sectorGeode->addDrawable(sectorGeom);

	osg::MatrixTransform *sectorAnimation=new osg::MatrixTransform;
	sectorAnimation->addChild(sectorGeode);

	osg::AnimationPath::TimeControlPointMap tcpm;
	tcpm[0]=osg::AnimationPath::ControlPoint();
	tcpm[0.25]=osg::AnimationPath::ControlPoint(osg::Vec3d(),osg::Matrix::rotate(osg::DegreesToRadians(90.0),osg::Z_AXIS).getRotate());
	tcpm[0.5]=osg::AnimationPath::ControlPoint(osg::Vec3d(),osg::Matrix::rotate(osg::DegreesToRadians(180.0),osg::Z_AXIS).getRotate());
	tcpm[0.75]=osg::AnimationPath::ControlPoint(osg::Vec3d(),osg::Matrix::rotate(osg::DegreesToRadians(270.0),osg::Z_AXIS).getRotate());
	tcpm[1]=osg::AnimationPath::ControlPoint();

	osg::AnimationPath *ap=new osg::AnimationPath;
	ap->setTimeControlPointMap(tcpm);
	ap->setLoopMode(osg::AnimationPath::LOOP);
	sectorAnimation->setUpdateCallback(new osg::AnimationPathCallback(ap));

	osg::Group* halfGroup=new osg::Group;
	halfGroup->addChild(sphereGeode);
	halfGroup->addChild(sectorAnimation);

	osg::MatrixTransform *mySphereMirror=new osg::MatrixTransform;
	mySphereMirror->addChild(sphereGeode);
	mySphereMirror->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(180.0),
		osg::X_AXIS,0,osg::Y_AXIS,
		osg::DegreesToRadians(180.0),osg::Z_AXIS));

	osg::MatrixTransform *mySectorMirror=new osg::MatrixTransform;
	mySectorMirror->addChild(sectorGeode);
	mySectorMirror->setMatrix(osg::Matrix::rotate(osg::DegreesToRadians(180.0),
		osg::X_AXIS,0,osg::Y_AXIS,
		osg::DegreesToRadians(180.0),osg::Z_AXIS));

	osg::MatrixTransform *SectorMirrorAnimation=new osg::MatrixTransform;
	SectorMirrorAnimation->addChild(mySectorMirror);
	SectorMirrorAnimation->setUpdateCallback(new osg::AnimationPathCallback(ap));

	myHalfMirror=new osg::Group;

	myHalfMirror->addChild(mySphereMirror);
	myHalfMirror->addChild(SectorMirrorAnimation);

	root()->addChild(halfGroup);
	root()->addChild(myHalfMirror);

	updateForceType(myEntObj.forceType());
	connect(&myEntObj,SIGNAL(forceTypeChanged(ByyForceType)),this,SLOT(updateForceType(ByyForceType)));

	setVisible(ByyEntitySettings::instance().electronicEnabled());
	connect(&ByyEntitySettings::instance(),SIGNAL(electronicEnabledChanged(bool)),this,SLOT(setVisible(bool)));
}

ByySphereElectronicVisualizer::~ByySphereElectronicVisualizer()
{

}

const QString& ByySphereElectronicVisualizer::theType()
{
	static QString _type("SphereElectronic");
	return _type;
}

const QString& ByySphereElectronicVisualizer::type()
{
	return theType();
}

void ByySphereElectronicVisualizer::updateForceType( ByyForceType ft )
{
	(*mySphereColors)[0]=forceTypeColorAlpha(ft,1.0);
	mySphereColors->dirty();
	myGeom->dirtyBound();
}

bool ByySphereElectronicVisualizer::isLocalized() const
{
	return true;
}