#include "StdAfx.h"
#include "ByyLineMeasure.h"

ByyLineMeasure::ByyLineMeasure( ByyIG& ig ) 
	: ByyMeasureItem(ig)
	,ByyLineStringVisualizer(ig)
{
	osgEarth::Symbology::Style labelStyle;
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->declutter()=false;
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()=osgEarth::Color("#ffffff");
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_LEFT_TOP;
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

ByyLineMeasure::~ByyLineMeasure()
{
	myRoot->removeChild(myLabelNode);
	myLabelNode=0;
}

void ByyLineMeasure::addPoint( const osgEarth::GeoPoint& point,bool refresh/*=true*/ )
{
	if (myPoints.size()==0)
	{
		myPoints.resize(2);
		myPoints[0]=point.vec3d();
		myPoints[1]=point.vec3d();
	}
	else
	{
		myPoints[0]=point.vec3d();
	}
	updateLineString(myPoints);
	myLabelNode->setNodeMask(0x0);
}

void ByyLineMeasure::mouseMove( const osgEarth::GeoPoint& point )
{
	if (myLabelNode->getNodeMask()==0x0)
	{
		myLabelNode->setNodeMask(0x1);
	}
	if (myPoints.size())
	{
		myPoints[1]=point.vec3d();
		updateLineString(myPoints);
		myLabelNode->setPosition(point);
		myPoints[0].z()=0;
		myPoints[1].z()=0;
		double _distance=osgEarth::GeoMath::distance(myPoints[0],myPoints[1],myMapNode->getMapSRS());
		double _bearing=osgEarth::GeoMath::bearing(
			osg::DegreesToRadians(myPoints[0].y()),
			osg::DegreesToRadians(myPoints[0].x()),
			osg::DegreesToRadians(myPoints[1].y()),
			osg::DegreesToRadians(myPoints[1].x()));

		if (_bearing<0)
		{
			_bearing+=osg::PI*2;
		}
		

		bool _km=false;

		if (_distance>1000)
		{
			_distance=_distance/1000;
			_km=true;
		}

		QString _text=QString::fromLocal8Bit("¾àÀë:%1 %2\n·½Î»:%3 deg")
			.arg(_distance,0,'f',0)
			.arg(QString::fromLocal8Bit(_km?"km":"m"))
			.arg(osg::RadiansToDegrees(_bearing),0,'f',0);

		myLabelNode->setText(_text.toUtf8().data());
	}
}
