#include "StdAfx.h"
#include "observerSettings.h"
#include "ByyRelativePath.h"
#include "ByyObserver.h"

ByyObserverSettings::ByyObserverSettings()
	:myOceanVisible(true)
{
	myGridOpen=true;
	myOpenScale=50000;
	myLonStart=120;
	myLonEnd=121;
	myLatStart=32;
	myLatEnd=33;
	myResolution=100;
	readSettings();
}

ByyObserverSettings::~ByyObserverSettings()
{
	writeSettings();
}

ByyObserverSettings& ByyObserverSettings::instance()
{
	static ByyObserverSettings _instance;
	return _instance;
}

void ByyObserverSettings::readSettings()
{
	QSettings s(myRelativePath.resolvePath("../data/config/Observer.ini"),QSettings::IniFormat);
	s.beginGroup("Observer");
	myOceanVisible=s.value("OceanVisible",true).toBool();
	myCtrlObjVisible=s.value("ControlObjVisible",true).toBool();
	myCtrlObjLableVisible=s.value("ControlObjLableVisible",true).toBool();
	myEntityVisible=s.value("EntityVisible",true).toBool();
	myGraticuleVisible=s.value("GraticuleVisible",false).toBool();
	my2DProjection=s.value("2DProjection",false).toBool();
	myRadioCommVisible=s.value("RadioCommVisible",false).toBool();
	myTrackVisible=s.value("Subjection",false).toBool();
	mySubjectionVisible=s.value("Subjection",false).toBool();
	myTrackVisible=s.value("TrackVisible",false).toBool();
	myGuidanceVisible=s.value("GuidanceVisible",false).toBool();
	myRoutePlanVisible=s.value("RoutePlanVisible",false).toBool();

	myGridOpen=s.value("GridOpen",false).toBool();
	myOpenScale=s.value("OpenScale",false).toInt();
	myLonStart=s.value("LonStart",false).toDouble();
	myLonEnd=s.value("LonEnd",false).toDouble();
	myLatStart=s.value("LatStart",false).toDouble();
	myLatEnd=s.value("LatEnd",false).toDouble();
	myResolution=s.value("Resolution",false).toInt();
	myClampToTerrain=s.value("ClampToTerrain",false).toBool();

	_viewPoint=s.value("ViewPoint").toString().toStdString();
}

void ByyObserverSettings::writeSettings()
{
	QSettings s(myRelativePath.resolvePath("../data/config/Observer.ini"),QSettings::IniFormat);
	s.beginGroup("Observer");

	s.setValue("OceanVisible",myOceanVisible);
	s.setValue("ControlObjVisible",myCtrlObjVisible);
	s.setValue("ControlObjLableVisible",myCtrlObjLableVisible);
	s.setValue("EntityVisible",myEntityVisible);
	s.setValue("GraticuleVisible",myGraticuleVisible);
	s.setValue("2DProjection",my2DProjection);
	s.setValue("RadioCommVisible",myRadioCommVisible);
	s.setValue("Subjection",mySubjectionVisible);
	s.setValue("TrackVisible",myTrackVisible);
	s.setValue("GuidanceVisible",myGuidanceVisible);
	s.setValue("RoutePlanVisible",myRoutePlanVisible);

	s.setValue("GridOpen",myGridOpen);
	s.setValue("OpenScale",myOpenScale);
	s.setValue("LonStart",myLonStart);
	s.setValue("LonEnd",myLonEnd);
	s.setValue("LatStart",myLatStart);
	s.setValue("LatEnd",myLatEnd);
	s.setValue("Resolution",myResolution);
	s.setValue("ClampToTerrain",myClampToTerrain);

	osgEarth::Config cfg= oberverInstance()->getViewpoint().getConfig();
	
	QString str=QString::fromStdString(cfg.toJSON(false));
	s.setValue("ViewPoint",str);
}

void ByyObserverSettings::setOceanVisible( bool b )
{
	if (b!=myOceanVisible)
	{
		myOceanVisible=b;
		emit oceanVisibleChanged(myOceanVisible);
	}
}

bool ByyObserverSettings::oceanVisible() const
{
	return myOceanVisible;
}

void ByyObserverSettings::setCtrlObjVisible( bool b )
{
	if (b!=myCtrlObjVisible)
	{
		myCtrlObjVisible=b;
		emit ctrlObjVisibleChanged(myCtrlObjVisible);
	}
}

void ByyObserverSettings::setCtrlObjLableVisible(bool b)
{
	if(myCtrlObjLableVisible!=b)
	{
		myCtrlObjLableVisible=b;
		emit ctrlObjLableVisibleChanged(myCtrlObjLableVisible);
	}
}

bool ByyObserverSettings::ctrlObjLableVisible() const
{
	return myCtrlObjLableVisible;
}

bool ByyObserverSettings::ctrlObjVisible() const
{
	return myCtrlObjVisible;
}

bool ByyObserverSettings::entityVisible() const
{
	return myEntityVisible;
}

void ByyObserverSettings::setEntityVisible( bool b )
{
	if (b!=myEntityVisible)
	{
		myEntityVisible=b;
		emit entityVisibleChanged(myEntityVisible);
	}
}

bool ByyObserverSettings::graticuleVisible() const
{
	return myGraticuleVisible;
}

void ByyObserverSettings::setGraticuleVisible( bool b )
{
	if (b!=myGraticuleVisible)
	{
		myGraticuleVisible=b;
		emit graticuleVisibleChanged(myGraticuleVisible);
	}
}

bool ByyObserverSettings::is2D() const
{
	return my2DProjection;
}

void ByyObserverSettings::set2D( bool b )
{
	if (my2DProjection!=b)
	{
		my2DProjection=b;

		emit projectionChanged(my2DProjection);
	}
}

bool ByyObserverSettings::radioCommVisible() const
{
	return myRadioCommVisible;
}

void ByyObserverSettings::setRadioCommVisible( bool b )
{
	if (b!=myRadioCommVisible)
	{
		myRadioCommVisible=b;
		emit radioCommVisibleChanged(myRadioCommVisible);
	}
}

bool ByyObserverSettings::subjectionVisible() const
{
	return mySubjectionVisible;
}

void ByyObserverSettings::setSubjectionVisible( bool b )
{
	if (b!=mySubjectionVisible)
	{
		mySubjectionVisible=b;
		emit subjectionVisibleChanged(mySubjectionVisible);
	}
}

bool ByyObserverSettings::trackVisible() const
{
	return myTrackVisible;
}

bool ByyObserverSettings::guidanceVisible() const
{
	return myGuidanceVisible;
}

void ByyObserverSettings::setTrackVisible( bool b )
{
	if(b!=myTrackVisible)
	{
		myTrackVisible=b;
		emit trackVisibleChanged(myTrackVisible);
	}
}

void ByyObserverSettings::setGuidanceVisible( bool b )
{
	if (b!=myGuidanceVisible)
	{
		myGuidanceVisible=b;
		emit guidanceVisibleChanged(myGuidanceVisible);
	}
}

bool ByyObserverSettings::routePlanVisible() const
{
	return myRoutePlanVisible;
}

void ByyObserverSettings::setRoutePlanVisible( bool b )
{
	if (b!=myRoutePlanVisible)
	{
		myRoutePlanVisible=b;
		emit routePlanVisibleChanged(myRoutePlanVisible);
	}
}
#include "ScaleBar.h"
void ByyObserverSettings::setGridSettings(bool GridOpen,int openScale,double LonStart,double LonEnd,double LatStart,double LatEnd,int Resolution,bool ClampToTerrain)
{
	myOpenScale=openScale;
	myGridOpen=GridOpen;
	myLonStart=LonStart;
	myLonEnd=LonEnd;
	myLatStart=LatStart;
	myLatEnd=LatEnd;
	myResolution=Resolution;
	myClampToTerrain=ClampToTerrain;
	scaleInstance()->updateNet();
}