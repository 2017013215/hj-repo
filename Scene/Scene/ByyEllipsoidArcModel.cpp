#include "StdAfx.h"
#include "ByyEllipsoidArcModel.h"

#include <osg/PolygonMode>
#include <osg/FrontFace>
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osgUtil/SmoothingVisitor>

ByyEllipsoidArcModel::ByyEllipsoidArcModel( float azCenter,float azSweep,
	float elCenter,float elSweep,double radius,
	const osg::Vec4& insideColor/*=osg::Vec4(1,0,1,0.75)*/, 
	const osg::Vec4& outsideColor/*=osg::Vec4(1,1,1,0.1)*/ )
	:myAzimuthCenter(azCenter),
	myAzimuthSweep(azSweep),
	myElevationCenter(elCenter),
	myElevationSweep(elSweep),
	myRadius(radius),
	myInsideColor(insideColor),
	myOutSideColor(outsideColor)
{
	myAngleSegmentMax=10.0;

	myGraph=new osg::MatrixTransform;

	addChild(myGraph);

	init();
}

ByyEllipsoidArcModel::~ByyEllipsoidArcModel()
{

}

void ByyEllipsoidArcModel::setAzimuthRange( float center, float sweep )
{
	myAzimuthCenter=center;
	myAzimuthSweep=sweep;

	init();
}

void ByyEllipsoidArcModel::setElevationRange( float center, float sweep )
{
	myElevationCenter=center;
	myElevationSweep=sweep;

	init();
}

void ByyEllipsoidArcModel::setInsideColor( const osg::Vec4& insideColor )
{
	myInsideColor=insideColor;
	init();
}

void ByyEllipsoidArcModel::setOutsideColor( const osg::Vec4& outsideColor )
{
	myOutSideColor=outsideColor;
	init();
}

void ByyEllipsoidArcModel::setRadius( double radius )
{
	myRadius=radius;

	myGraph->setMatrix(osg::Matrix::scale(myRadius,myRadius,myRadius));
}

void ByyEllipsoidArcModel::init()
{
	myGraph->removeChildren(0,myGraph->getNumChildren());

	float  myAzimuthMin=myAzimuthCenter-myAzimuthSweep;
	float  myAzimuthMax=myAzimuthCenter+myAzimuthSweep;
	float  myElevationMin=myElevationCenter-myElevationSweep;
	float  myElevationMax=myElevationCenter+myElevationSweep;

	osg::Vec3Array *vertexs=new osg::Vec3Array;
	osg::Vec4Array *colors=new osg::Vec4Array;
	osg::Vec3Array *normals=new osg::Vec3Array;

	float fRow=(myElevationMax-myElevationMin)/myAngleSegmentMax;
	float fColumn=(myAzimuthMax-myAzimuthMin)/myAngleSegmentMax;


	int row=fRow==(int)fRow?fRow:fRow+1;
	++row;

	int column=fColumn==(int)fColumn?fColumn:fColumn+1;
	++column;

	vertexs->resize(row*column+1);
	colors->resize(row*column+1);
	normals->resize(row*column+1);

	*vertexs->rbegin()=osg::Vec3(0,0,0);
	*colors->rbegin()=myInsideColor;
	*normals->rbegin()=osg::Vec3(0,1,0);

	int cz=row*column;

	int zz=0;

	osg::Vec3 axis(0,1,0);

	for (int i=0;i!=row;++i)
	{
		float curElev=std::min(i*myAngleSegmentMax+myElevationMin,myElevationMax);

		for (int j=0;j!=column;++j)
		{
			float curAzimuth=std::min(j*myAngleSegmentMax+myAzimuthMin,myAzimuthMax);

			osg::Matrixf matrix=osg::Matrix::rotate(osg::DegreesToRadians(curElev),osg::X_AXIS,
				0,osg::Y_AXIS,osg::DegreesToRadians(curAzimuth),osg::Z_AXIS);

			(*vertexs)[zz]=matrix.preMult(axis);

			(*colors)[zz]=myOutSideColor;

			(*normals)[zz]=(*vertexs)[zz];

			++zz;
		}
	}


	osg::Geometry *geometry=new osg::Geometry;

	geometry->setVertexArray(vertexs);
	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	geometry->setNormalArray(normals);
	geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);


	osg::DrawElementsUInt *primitiveSet=new osg::DrawElementsUInt(GL_QUADS);

	for (int i=0;i!=row-1;++i)
	{
		for (int j=0;j!=column-1;++j)
		{
			primitiveSet->push_back(i*column+j);
			primitiveSet->push_back(i*column+j+1);
			primitiveSet->push_back((i+1)*column+j+1);
			primitiveSet->push_back((i+1)*column+j);
		}
	}

	geometry->addPrimitiveSet(primitiveSet);

	osg::DrawElementsUInt *triPset=0;

	if (myAzimuthMax-myAzimuthMin<360.0)
	{
		if (triPset==0)
		{
			triPset=new osg::DrawElementsUInt(GL_TRIANGLES);
		}
		for (int i=0;i!=row-1;++i)
		{
			triPset->push_back(i*column);
			triPset->push_back((i+1)*column);
			triPset->push_back(vertexs->size()-1);
		}

		for (int i=0;i!=row-1;++i)
		{
			triPset->push_back((i+1)*column+column-1);
			triPset->push_back(i*column+column-1);

			triPset->push_back(vertexs->size()-1);
		}
	}

	//	if (myElevationMax-myElevationMin<osg::DegreesToRadians(180.0))
	{


		if (myElevationMin>-90.0)
		{

			if (triPset==0)
			{
				triPset=new osg::DrawElementsUInt(GL_TRIANGLES);
			}

			for (int i=0;i!=column-1;++i)
			{
				triPset->push_back(i);
				triPset->push_back(i+1);
				triPset->push_back(vertexs->size()-1);
			}
		}

		if (myElevationMax<90.0)
		{
			if (triPset==0)
			{
				triPset=new osg::DrawElementsUInt(GL_TRIANGLES);
			}

			for (int i=0;i!=column-1;++i)
			{
				triPset->push_back((row-1)*column+i);
				triPset->push_back((row-1)*column+i+1);
				triPset->push_back(vertexs->size()-1);
			}
		}
	}

	if (triPset)
	{
		geometry->addPrimitiveSet(triPset);
	}

	osg::Geode *geode=new osg::Geode;

	geode->addDrawable(geometry);

	geode->getOrCreateStateSet()->setRenderBinDetails(10,"DepthSortedBin");
	geode->getOrCreateStateSet()->setAttributeAndModes(new osg::FrontFace(osg::FrontFace::COUNTER_CLOCKWISE));

	osg::Geode *geode2=new osg::Geode;
	geode2->getOrCreateStateSet()->setRenderBinDetails(11,"DepthSortedBin");
	geode2->getOrCreateStateSet()->setAttributeAndModes(new osg::FrontFace(osg::FrontFace::CLOCKWISE));
	geode2->addDrawable(geometry);

	osg::Group *g1=new osg::Group;
	g1->getOrCreateStateSet()->setMode(GL_CULL_FACE,osg::StateAttribute::ON);
	g1->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON);
	g1->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);
	g1->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,osg::BlendFunc::ONE_MINUS_SRC_ALPHA));

	g1->addChild(geode);
	g1->addChild(geode2);

	osg::Group *g2=new osg::Group;

	g2->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE));
	g2->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(1));

	g2->addChild(geode);
	g2->addChild(geode2);

	myGraph->setMatrix(osg::Matrix::scale(myRadius,myRadius,myRadius));

	myGraph->addChild(g1);
	myGraph->addChild(g2);

	myGraph->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL,osg::StateAttribute::ON);
	myGraph->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	myGraph->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
	myGraph->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LESS,0,1,false));
	myGraph->getOrCreateStateSet()->setAttributeAndModes(new osg::Program);

	//	osgUtil::SmoothingVisitor sv;

	//	myGraph->accept(sv);
}
