#include "stdafx.h"
#include "ScaleBar.h"

#include <osg/GraphicsContext>
#include <osgEarth/GeoMath>
#include <osgEarth/Terrain>

#include <Windows.h>
#include "ByyIG.h"
#include "ByyRenderer.h"
#include "terrain.h"
double ScaleBar::normalizeScaleMeters(double meters)
{
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

double ScaleBar::normalizeScaleFeet(double feet)
{
    double feetPerMile = 5280.0;
    if (feet <= 7.5) {
        feet = 5;
    } else if (feet <= 15) {
        feet = 10;
    } else if (feet <= 35) {
        feet = 20;
    } else if (feet <= 75) {
        feet = 50;
    } else if (feet <= 150) {
        feet = 100;
    } else if (feet <= 350) {
        feet = 200;
    } else if (feet <= 750) {
        feet = 500;
    } else if (feet <= 1500) {
        feet = 1000;
    } else if (feet <= 3640) {
        feet = 2000;
    } else if (feet <= 1.5 * feetPerMile) {
        feet = 1 * feetPerMile;
    } else if (feet <= 3.5 * feetPerMile) {
        feet = 2 * feetPerMile;
    } else if (feet <= 7.5 * feetPerMile) {
        feet = 5 * feetPerMile;
    } else if (feet <= 15 * feetPerMile) {
        feet = 10 * feetPerMile;
    } else if (feet <= 35 * feetPerMile) {
        feet = 20 * feetPerMile;
    } else if (feet <= 75 * feetPerMile) {
        feet = 50 * feetPerMile;
    } else if (feet <= 150 * feetPerMile) {
        feet = 100 * feetPerMile;
    } else if (feet <= 350 * feetPerMile) {
        feet = 200 * feetPerMile;
    } else if (feet <= 750 * feetPerMile) {
        feet = 500 * feetPerMile;
    } else if (feet <= 1500 * feetPerMile) {
        feet = 1000 * feetPerMile;
    } else {
        feet = 2000 * feetPerMile;
    }
    return feet;
}

double ScaleBar::normalizeScaleNauticalMiles(double nmi)
{
    //double feetPerMile = 6076.12;
    if (nmi <= 0.0015) {
        nmi = 0.001;
    } else if (nmi <= 0.0035) {
        nmi = 0.002;
    } else if (nmi <= 0.0075) {
        nmi = 0.005;
    } else if (nmi <= 0.015) {
        nmi = 0.01;
    } else if (nmi <= 0.035) {
        nmi = 0.02;
    } else if (nmi <= 0.075) {
        nmi = 0.05;
    } else if (nmi <= 0.15) {
        nmi = 0.1;
    } else if (nmi <= 0.35) {
        nmi = 0.2;
    } else if (nmi <= 0.75) {
        nmi = 0.5;
    } else if (nmi <= 1.5) {
        nmi = 1;
    } else if (nmi <= 3.5) {
        nmi = 2;
    } else if (nmi <= 7.5) {
        nmi = 5;
    } else if (nmi <= 15) {
        nmi = 10;
    } else if (nmi <= 35) {
        nmi = 20;
    } else if (nmi <= 75) {
        nmi = 50;
    } else if (nmi <= 150) {
        nmi = 100;
    } else if (nmi <= 350) {
        nmi = 200;
    } else if (nmi <= 750) {
        nmi = 500;
    } else if (nmi <= 1500) {
        nmi = 1000;
    } else {
        nmi = 2000;
    }
    return nmi;
}
ScaleBar* barIns=0;
ScaleBar*	scaleInstance()
{
	return barIns;
}
ScaleBar::ScaleBar(osgEarth::MapNode* mapNode, osgViewer::View* view)
    : _mapNode(mapNode)
    , _view(view)
    , _windowWidth(500)
    , _windowHeight(500)
    , _scaleBarUnits(UNITS_METERS)
{
	barIns=this;
    _map = mapNode->getMap();

	_scaleLabel=new osgEarth::Util::Controls::LabelControl("");
	_scaleLabel->setFont(osgText::readFontFile("../data/fonts/msyh.ttf"));
	_scaleLabel->setEncoding(osgText::String::ENCODING_UTF8);
	//_scaleLabel->setSize(300,500);
	_scaleLabel->setFontSize(12);
	_scaleLabel->setAbsorbEvents(true);
	_scaleLabel->setAlign(osgEarth::Util::Controls::Control::ALIGN_LEFT,osgEarth::Util::Controls::Control::ALIGN_BOTTOM);

   // _scaleLabel = new osgEarth::Util::Controls::LabelControl("", 12.0f);
	//_scaleLabel->setFont(osgText::readFontFile("../data/fonts/simhei.ttf"));
    //_scaleLabel->setForeColor(osg::Vec4f(0, 0, 0, 1));
    _scaleBar = new osgEarth::Util::Controls::Frame();
    _scaleBar->setVertFill(true);
    _scaleBar->setForeColor(osg::Vec4f(0, 0, 0, 0.8));
    _scaleBar->setBackColor(osg::Vec4f(1, 1, 1, 0.5));
    _scaleBar->setBorderColor(osg::Vec4f(0, 0, 0, 1));
    _scaleBar->setBorderWidth(1.0);


	HDC hdc=::GetDC(HWND(NULL));
	int x1=::GetDeviceCaps(hdc,VERTRES);
	int x2=::GetDeviceCaps(hdc,VERTSIZE);

	barWidth=(int)(10.0/x2*x1);

	_scaleLabel->setBackColor(255,0,0,0.5);

	netRoot=new osg::Group;	
	ByyIG::IGInstance()->renderer()->addNodeToRoot(netRoot,ByyOsgRenderer::Terrain);
	scaleRefreshed=false;
	updateNet();

}

void ScaleBar::setVisible(bool visible)
{
    if (_scaleLabel.valid()) {
        _scaleLabel->setVisible(visible);
    }
    if (_scaleBar.valid()) {
        _scaleBar->setVisible(visible);
    }
}

double ScaleBar::computeScale()
{
    if (!_scaleLabel.valid() || !_scaleLabel->visible()) {
        return -1.0;
    }
    if (!_mapNode.valid() || _mapNode->getTerrain() == NULL) {
        //        ERROR("No map");
        return -1.0;
    }
    if (!_view.valid()) {
        //        ERROR("No viewer");
        return -1.0;
    }

    osg::ref_ptr<osg::GraphicsContext> gc = _view->getCamera()->getGraphicsContext();
    if (gc) {
        auto t = gc->getTraits();
        _windowWidth = t->width;
        _windowHeight = t->height;
    }

	// test
	double w,h;
	osg::Vec3d worlda, worldb;
	w=0;h=0;
	_mapNode->getTerrain()->getWorldCoordsUnderMouse(_view->asView(), w, h, worlda);
	w=0;h=800;
	_mapNode->getTerrain()->getWorldCoordsUnderMouse(_view->asView(), w, h, worldb);
	osgEarth::GeoPoint mapPointa, mapPointb;
	mapPointa.fromWorld(_mapNode->getMapSRS(), worlda);
	mapPointb.fromWorld(_mapNode->getMapSRS(), worldb);
	///////


    double x, y;
//     double pixelWidth = _windowWidth * 0.1 * 2.0;
//     if (pixelWidth < 10)
//         pixelWidth = 10;
//     if (pixelWidth > 150)
//         pixelWidth = 150;
	double pixelWidth=barWidth;
    x = (double)(_windowWidth - 1) / 2.0 - pixelWidth / 2.0;
    y = (double)(_windowHeight - 1) / 2.0;

    osg::Vec3d world1, world2;
    if (!_mapNode->getTerrain()->getWorldCoordsUnderMouse(_view->asView(), x, y, world1)) {
        // off map
        //        TRACE("Off map coords: %g %g", x, y);
        _scaleLabel->setText("");
        _scaleBar->setWidth(0);
        return -1.0;
    }
    x += pixelWidth;
    if (!_mapNode->getTerrain()->getWorldCoordsUnderMouse(_view->asView(), x, y, world2)) {
        // off map
        //        TRACE("Off map coords: %g %g", x, y);
        _scaleLabel->setText("");
        _scaleBar->setWidth(0);
        return -1.0;
    }
	
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
		osg::Vec3d p1(mapPoint1.x(),mapPoint1.y(),0);
		osg::Vec3d p2(mapPoint2.x(),mapPoint2.y(),0);
		meters=osgEarth::GeoMath::distance(p1,p2,_mapNode->getMapSRS());
//         meters = osgEarth::GeoMath::distance(osg::DegreesToRadians(mapPoint1.y()),
//             osg::DegreesToRadians(mapPoint1.x()),
//             osg::DegreesToRadians(mapPoint2.y()),
//             osg::DegreesToRadians(mapPoint2.x()),
//             radius);
    } else {
        // Assume geocentric?
        //        ERROR("No map SRS");
        _scaleLabel->setText("");
        _scaleBar->setWidth(0);
        return -1.0;
    }

	double scale=meters*100;
	scaleRefreshed=true;
	_mapScale = scale;
//     double scale = meters / pixelWidth;
//     
// 	 
//     switch (_scaleBarUnits) {
//     case UNITS_NAUTICAL_MILES: {
//         double nmi = meters / 1852.0;
//         scale = nmi / pixelWidth;
//         nmi = normalizeScaleNauticalMiles(nmi);
//         pixelWidth = nmi / scale;
//         if (_scaleLabel.valid()) {
//             _scaleLabel->setText(osgEarth::Stringify()
//                 << nmi
//                 << " nmi");
//         }
//     } break;
//     case UNITS_US_SURVEY_FEET: {
//         double feet = meters * 3937.0 / 1200.0;
//         scale = feet / pixelWidth;
//         feet = normalizeScaleFeet(feet);
//         pixelWidth = feet / scale;
//         if (_scaleLabel.valid()) {
//             if (feet >= 5280) {
//                 _scaleLabel->setText(osgEarth::Stringify()
//                     << feet / 5280.0
//                     << " miUS");
//             } else {
//                 _scaleLabel->setText(osgEarth::Stringify()
//                     << feet
//                     << " ftUS");
//             }
//         }
//     } break;
//     case UNITS_INTL_FEET: {
//         double feet = 5280.0 * meters / 1609.344;
//         scale = feet / pixelWidth;
//         feet = normalizeScaleFeet(feet);
//         pixelWidth = feet / scale;
//         if (_scaleLabel.valid()) {
//             if (feet >= 5280) {
//                 _scaleLabel->setText(osgEarth::Stringify()
//                     << feet / 5280.0
//                     << " mi");
//             } else {
//                 _scaleLabel->setText(osgEarth::Stringify()
//                     << feet
//                     << " ft");
//             }
//         }
//     } break;
//     case UNITS_METERS:
//     default: {
//        pixelWidth = meters / scale;
//         if (_scaleLabel.valid()) {
//             if (meters >= 1000) {
// 				QString test=QString("%1 km").arg(QString::number(meters/1000.0,'f',2));
//                 _scaleLabel->setText(QString("%1 km").arg(QString::number(meters/1000.0,'f',2)).toStdString());
//             } else {
// 				_scaleLabel->setText(QString("%1 m").arg(QString::number(meters,'f',2)).toStdString());
//             }
//         }
// 		
//     } break;
//     } 
	_scaleLabel->setText(QString("1:%1").arg((int)scale).toStdString());
    if (_scaleBar.valid()) {
        _scaleBar->setWidth(barWidth);
    }
    return scale;
}


#include "observerSettings.h"

void ScaleBar::updateNetVisible()
{
	if(!scaleRefreshed)
		computeScale();
	bool show=true;
	if(!ByyObserverSettings::instance().gridOpen())
	{
		show=false;
	}
	else 
	{
		show=_mapScale<ByyObserverSettings::instance().openScale();
	}
	
	//show=true;
	//show=true;
	netRoot->setNodeMask((int)show);
}

void ScaleBar::updateNet()
{		
	netRoot->removeChildren(0,netRoot->getNumChildren());
	if(!ByyObserverSettings::instance().gridOpen())
		return;
	double	lonStart=ByyObserverSettings::instance().lonStart();
	double	lonEnd=ByyObserverSettings::instance().lonEnd();
	double	latStart=ByyObserverSettings::instance().latStart();
	double	latEnd=ByyObserverSettings::instance().latEnd();
	int		reso=ByyObserverSettings::instance().resolution();

	double	lonLoopEnd=lonEnd+1.0/reso;
	osgEarth::Symbology::AltitudeSymbol::Clamping clamp=ByyObserverSettings::instance().clampToTerrain()?
		osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN:osgEarth::Symbology::AltitudeSymbol::CLAMP_NONE;
	for(double lon=lonStart;lon<=lonLoopEnd;)
	{
		osg::ref_ptr<osgEarth::Symbology::LineString> line=new osgEarth::Symbology::LineString;
		line->push_back(lon,latStart,3000);
		line->push_back(lon,latEnd,3000);	
		osgEarth::Symbology::Style style;
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color(192,192,192);
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width()=1;
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation()=60.0;
		style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=clamp;//CLAMP_RELATIVE_TO_TERRAIN;//CLAMP_NONE
		style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique()=osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;
		osgEarth::Features::Feature* feature=new osgEarth::Features::Feature(line,ByyIG::IGInstance()->terrain()->srs(),style);
		osgEarth::Annotation::FeatureNode* featureNode=new osgEarth::Annotation::FeatureNode(ByyIG::IGInstance()->terrain()->mapNode(),feature);
		netRoot->addChild(featureNode);
		lon+=1.0/reso;
	}

	double latLoopEnd=latEnd+1.0/reso;
	for(double lat=latStart;lat<=latLoopEnd;)
	{
		osg::ref_ptr<osgEarth::Symbology::LineString> line=new osgEarth::Symbology::LineString;
		line->push_back(lonStart,lat,3000);
		line->push_back(lonEnd,lat,3000);	
		osgEarth::Symbology::Style style;
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color()=osgEarth::Symbology::Color(192,192,192);//osgEarth::Symbology::Color::Maroon;
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width()=1;
		style.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation()=60.0;
		style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=clamp;
		style.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique()=osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;
		osgEarth::Features::Feature* feature=new osgEarth::Features::Feature(line,ByyIG::IGInstance()->terrain()->srs(),style);
		osgEarth::Annotation::FeatureNode* featureNode=new osgEarth::Annotation::FeatureNode(ByyIG::IGInstance()->terrain()->mapNode(),feature);
		netRoot->addChild(featureNode);
		lat+=1.0/reso;
	}

}

bool ScaleBarHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if (view) {
        if (ea.getEventType() == ea.SCROLL || ea.getEventType() == ea.KEYDOWN ||ea.getEventType()==ea.DOUBLECLICK) {
            scaleBar_->computeScale();
            return false;
        }
    }
    return false;
}
