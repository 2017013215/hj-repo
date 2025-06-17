#include "StdAfx.h"
#include "ByyPolygonMeasure.h"
#include "geoMath.h"
#include <osgEarthAnnotation/FeatureNode>

ByyPolygonMeasure::ByyPolygonMeasure( ByyIG& ig ) 
	: ByyMeasureItem(ig)
	,ByyPolygonVisualizer(ig)
{
	osgEarth::Symbology::Style labelStyle;
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->declutter()=false;
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()=osgEarth::Color("#ffffff");
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_CENTER_CENTER;
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->pixelOffset()=osg::Vec2s(20,0);
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding()=osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->font()="../data/fonts/msyh.ttf";
	labelStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->content()=osgEarth::StringExpression("");

	myLabelNode=new osgEarth::Annotation::LabelNode(myMapNode.get(),labelStyle);
	myLabelNode->setDynamic(true);

	myLabelNode->setNodeMask(0x0);

	myRoot->addChild(myLabelNode);
}

ByyPolygonMeasure::~ByyPolygonMeasure()
{
	myRoot->removeChild(myLabelNode);
	myLabelNode=0;
}

void ByyPolygonMeasure::addPoint( const osgEarth::GeoPoint& point,bool refresh/*=true*/ )
{
	if (myPoints.size()==0)
	{
		myPoints.resize(2);
		myPoints[0]=point.vec3d();
		myPoints[1]=point.vec3d();
	}
	else
	{
		myPoints.last()=point.vec3d();
		myPoints.push_back(point.vec3d());
	}
	updatePolygon(myPoints);
}

void ByyPolygonMeasure::mouseMove( const osgEarth::GeoPoint& point )
{
	if (myLabelNode->getNodeMask()==0x0)
	{
		myLabelNode->setNodeMask(0x1);
	}
	if (myPoints.size())
	{
		myPoints.last()=point.vec3d();
		updatePolygon(myPoints);

		osgEarth::GeoPoint polygonCenter=point;
		polygonCenter.vec3d()=osg::Vec3d(myPolygon->getBounds().center2d(),myPolygon->getBounds().depth()/2.0);

		myLabelNode->setPosition(polygonCenter);

		std::vector<osg::Vec3d> points=myPoints.toStdVector();

		double _distance=osgEarth::GeoMath::distance(points);

		bool km=false;

		if (_distance>1000)
		{
			_distance=_distance/1000;
			km=true;
		}

		double area=ByyGeoMath::calcPolygonArea(points);

		bool km2=false;

		if (area>1000000)
		{
			area=area/1000000;

			km2=true;
		}

		QString _text=QString::fromLocal8Bit("面积:%1 %2\n周长:%3 %4")
			.arg(area,0,'f',0).arg(QString::fromLocal8Bit(km2?"km2":"m2"))
			.arg(_distance,0,'f',0).arg(km?"km":"m");

		myLabelNode->setText(_text.toUtf8().data());
	}
}
