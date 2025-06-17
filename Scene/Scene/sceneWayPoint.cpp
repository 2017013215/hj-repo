#include "StdAfx.h"
#include "sceneWayPoint.h"
#include "ByyWaypoint.h"
#include "ByyIG.h"
#include "osgFileCache.h"
#include "observerSettings.h"

ByySceneWayPoint::ByySceneWayPoint(ByyWaypoint* wp,ByyIG& ig)
	:ByySceneControlObject(wp,ig)
	,myWayPoint(wp)
{
	connect(myWayPoint,SIGNAL(positionChanged(const osg::Vec3d&)),SLOT(setPosition(const osg::Vec3d&)));

	myRoot->removeChild(myLabelNode);

	myLabelNode=0;
}

ByySceneWayPoint::~ByySceneWayPoint()
{

}

void ByySceneWayPoint::initiate()
{
	osg::Image* image=myIG.fileCache().readImageFile("../data/Textures/triangle.png");

	osgEarth::Symbology::Style style;
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding()=osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->font()="../data/fonts/msyh.ttf";
	style.getOrCreate<osgEarth::Symbology::IconSymbol>()->alignment()=osgEarth::Symbology::IconSymbol::ALIGN_CENTER_CENTER;
	style.getOrCreate<osgEarth::Symbology::TextSymbol>()->declutter()=true;
	style.getOrCreate<osgEarth::Symbology::IconSymbol>()->declutter()=true;

	updateTextStyle(myWayPoint->forceType(),style);

	QString name=ByyObserverSettings::instance().ctrlObjLableVisible()?myWayPoint->name():"";

	myLocalNode=new osgEarth::Annotation::PlaceNode(mapNode(),osgEarth::GeoPoint(),image,name.toUtf8().data(),style);
	setPosition(myWayPoint->position());
	myLocalNode->getOrCreateStateSet()->setRenderBinDetails(13,"RenderBin");
	myLocalNode->setDynamic(true);

	myRoot->addChild(myLocalNode);
}

osg::Vec3d ByySceneWayPoint::center()
{
	return myLocalNode->getPosition().vec3d();
}

double ByySceneWayPoint::radius()
{
	return 100;
}

void ByySceneWayPoint::setPosition( const osg::Vec3d& pos )
{
	myLocalNode->setPosition(osgEarth::GeoPoint(srs(),pos));
}

void ByySceneWayPoint::setName( const QString& text )
{
	QString name=ByyObserverSettings::instance().ctrlObjLableVisible()?text:"";
	myLocalNode->setText(name.toUtf8().data());
}

void ByySceneWayPoint::setForceType( ByyForceType side )
{
	osgEarth::Symbology::Style style=myLocalNode->getStyle();

	updateTextStyle(side,style);

	myLocalNode->setStyle(style);
}

void ByySceneWayPoint::setClampToTerrain( bool clamp )
{
	osgEarth::Symbology::Style style=myLocalNode->getStyle();

	updateClampStyle(clamp,style);

	myLocalNode->setStyle(style);
}

ByySceneControlObject* ByySceneWayPoint::create( ByyControlObject* obj,ByyIG& ig )
{
	ByyWaypoint* wp=dynamic_cast<ByyWaypoint*>(obj);

	if (wp)
	{
		return new ByySceneWayPoint(wp,ig);
	}

	return 0;
}

void ByySceneWayPoint::setCtrlObjLableVisible(bool visible)
{
	if(myLocalNode==0)
		return;
	QString txt=myWayPoint->name().toUtf8().data();
	if(visible)
		myLocalNode->setText(myWayPoint->name().toUtf8().data());
	else
		myLocalNode->setText("");
}