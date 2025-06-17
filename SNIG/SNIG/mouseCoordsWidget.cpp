#include "StdAfx.h"
#include "mouseCoordsWidget.h"
#include <osgEarth/Terrain>
#include <osgEarth/MapNode>
#include <osgEarth/GeoMath>
ByyMouseCoordsWidget::ByyMouseCoordsWidget(ByyIG &ig,QWidget *parent)
	: QWidget(parent)
	,myIG(ig)
	,myEventProcessor(this)
{
	_view=0;
	ui.setupUi(this);

	myIG.inputController().addEventProcessor(&myEventProcessor);
}

ByyMouseCoordsWidget::~ByyMouseCoordsWidget()
{
	myIG.inputController().removeEventProcessor(&myEventProcessor);
}


double normalizeScaleMeters(double meters)
{
	return meters;
	if (meters <= 3) {
		meters = 1;
	} else if (meters <= 7.5) {
		meters = 5;
	} else if (meters <= 15) {
		meters = 10;
	} else if (meters <= 35) {
		meters = 20;
	} else if (meters <= 75) {
		meters = 50;
	} else if (meters <= 150) {
		meters = 100;
	} else if (meters <= 350) {
		meters = 200;
	} else if (meters <= 750) {
		meters = 500;
	} else if (meters <= 1500) {
		meters = 1000;
	} else if (meters <= 3500) {
		meters = 2000;
	} else if (meters <= 7500) {
		meters = 5000;
	} else if (meters <= 15000) {
		meters = 10000;
	} else if (meters <= 35000) {
		meters = 20000;
	} else if (meters <= 55000) {
		meters = 50000;
	} else if (meters <= 150000) {
		meters = 100000;
	} else if (meters <= 350000) {
		meters = 200000;
	} else if (meters <= 750000) {
		meters = 500000;
	} else if (meters <= 1500000) {
		meters = 1000000;
	} else {
		meters = 2000000;
	}
	return meters;
}
/*
QString ByyMouseCoordsWidget::computeScale()
{
	if(_view==0)
	{
		_view=myIG.display().getViews().at(0);
	}
	int _windowWidth, _windowHeight;
	osg::ref_ptr<osg::GraphicsContext> gc = _view->getCamera()->getGraphicsContext();
	if (gc) {
		auto t = gc->getTraits();
		_windowWidth = t->width;
		_windowHeight = t->height;
	}

	double x, y;
	double pixelWidth = _windowWidth * 0.1 * 2.0;
	if (pixelWidth < 10)
		pixelWidth = 10;
	if (pixelWidth > 150)
		pixelWidth = 150;
	x = (double)(_windowWidth - 1) / 2.0 - pixelWidth / 2.0;
	y = (double)(_windowHeight - 1) / 2.0;

	osg::Vec3d world1, world2;
	osgEarth::MapNode* _mapNode=myIG.terrain()->mapNode();
	if (!_mapNode->getTerrain()->getWorldCoordsUnderMouse(_view->asView(), x, y, world1)) {
		// off map
		//        TRACE("Off map coords: %g %g", x, y);
		//_scaleLabel->setText("");
		//_scaleBar->setWidth(0);
		//return -1.0;
		return "";
	}
	x += pixelWidth;
	if (!_mapNode->getTerrain()->getWorldCoordsUnderMouse(_view->asView(), x, y, world2)) {
		// off map
		//        TRACE("Off map coords: %g %g", x, y);
		//_scaleLabel->setText("");
		//_scaleBar->setWidth(0);
		//return -1.0;
		return "";
	}
	osgEarth::Map* _map=myIG.terrain()->getMap();
	double meters;
	double radius = 6378137.0;
	if (_mapNode->getMapSRS() && _mapNode->getMapSRS()->getEllipsoid()) {
		radius = _mapNode->getMapSRS()->getEllipsoid()->getRadiusEquator();
	}
	if (!_map->isGeocentric() && _mapNode->getMapSRS() && _mapNode->getMapSRS()->isGeographic()) {
		//        TRACE("Map is geographic");
		// World cords are already lat/long
		// Compute great circle distance
		meters = osgEarth::GeoMath::distance(world1, world2, _mapNode->getMapSRS());
	} else if (_mapNode->getMapSRS()) {
		// Get map coords in lat/long
		osgEarth::GeoPoint mapPoint1, mapPoint2;
		mapPoint1.fromWorld(_mapNode->getMapSRS(), world1);
		mapPoint1.makeGeographic();
		mapPoint2.fromWorld(_mapNode->getMapSRS(), world2);
		mapPoint2.makeGeographic();
		// Compute great circle distance
		meters = osgEarth::GeoMath::distance(osg::DegreesToRadians(mapPoint1.y()),
			osg::DegreesToRadians(mapPoint1.x()),
			osg::DegreesToRadians(mapPoint2.y()),
			osg::DegreesToRadians(mapPoint2.x()),
			radius);
	} else {
		// Assume geocentric?
		//        ERROR("No map SRS");
		//_scaleLabel->setText("");
		//_scaleBar->setWidth(0);
		//return -1.0;
		return "";
	}

	double scale = meters / pixelWidth;
	// 1mi = 5280 feet
	//double scaleMiles = scale / 1609.344; // International mile = 1609.344m
	//double scaleNauticalMiles = scale / 1852.0; // nautical mile = 1852m
	//double scaleUSSurveyMiles = scale / 1609.347218694; // US survey mile = 5280 US survey feet
	//double scaleUSSurveyFeet = scale * 3937.0/1200.0; // US survey foot = 1200/3937 m

	meters = normalizeScaleMeters(meters);
	pixelWidth = meters / scale;
	if (meters >= 1000) {
		return QString("%1km").arg(QString::number(meters/1000.0,'f',3));
	} 
	else {
		return QString("%1m").arg(meters);
	}
	return "";
	

}
*/
bool ByyMouseCoordsWidget::processEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	//if(ea.getEventType()==ea.SCROLL|| ea.getEventType() == ea.KEYDOWN||ea.getEventType()==ea.DOUBLECLICK||ea.getEventType()==ea.DRAG)
	//{
	//	((QWidget*)parent())->setWindowTitle(computeScale());
	//}
	if (isHidden()||ui.closedRadioButton->isChecked()) return false;

	if (ui.clickRadioButton->isChecked())
	{
		if (ea.getEventType()!=ea.RELEASE)
			return false;
	}
	else
	{
		if (ea.getEventType()!=ea.MOVE) 
			return false;
	}

	osgEarth::MapNode *mapNode=myIG.terrain()->mapNode();

	if (!mapNode) return false;

	osg::Vec3d world;
	if ( mapNode->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), world) )
	{
		static osgEarth::GeoPoint map;

		map.fromWorld( mapNode->getMapSRS(), world );

		ui.lineEdit->setText(QString::number(map.x(),'f',8));
		ui.lineEdit_2->setText(QString::number(map.y(),'f',8));
		ui.lineEdit_3->setText(QString::number(map.z(),'f',8));
	}
	else
	{
		ui.lineEdit->setText("");
		ui.lineEdit_2->setText("");
		ui.lineEdit_3->setText("");
	}

	return false;
}
