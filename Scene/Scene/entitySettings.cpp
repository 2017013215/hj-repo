#include "stdafx.h"
#include "entitySettings.h"

#include "ByyRelativePath.h"

ByyEntitySettings::ByyEntitySettings()
{
	readSettings();
}

ByyEntitySettings::~ByyEntitySettings()
{
	writeSettings();
}

ByyEntitySettings& ByyEntitySettings::instance()
{
	static ByyEntitySettings s;
	return s;
}

bool ByyEntitySettings::groundClampEnabled()
{
	return myGroundClampEnabled;
}

void ByyEntitySettings::setGroundClampEnabled( bool e )
{
	if (myGroundClampEnabled!=e)
	{
		myGroundClampEnabled=e;

		emit groundClampEnabledChanged(myGroundClampEnabled);
	}
}

bool ByyEntitySettings::entityLabelEnabled()
{
	return myEntityLabelEnabled;
}

void ByyEntitySettings::setEntityLabelEnabled( bool e )
{
	if (myEntityLabelEnabled!=e)
	{
		myEntityLabelEnabled=e;

		emit entityLabelEnabledChanged(myEntityLabelEnabled);
	}
}

bool ByyEntitySettings::entityNameEnabled()
{
	return myEntityNameEnabled;
}

void ByyEntitySettings::setEntityNameEnabled( bool e )
{
	if (myEntityNameEnabled!=e)
	{
		myEntityNameEnabled=e;

		emit entityNameEnabledChanged(myEntityNameEnabled);
	}
}

bool ByyEntitySettings::autoScaleEnabled()
{
	return myAutoScaleEnabled;
}

void ByyEntitySettings::setAutoScaleEnabled( bool e )
{
	if (myAutoScaleEnabled!=e)
	{
		myAutoScaleEnabled=e;

		emit autoScaleEnabledChanged(myAutoScaleEnabled);
	}
}

bool ByyEntitySettings::trackHistoryEnabled()
{
	return myTrackHistoryEnabled;
}

void ByyEntitySettings::setTrackHistoryEnabled( bool e )
{
	if (myTrackHistoryEnabled!=e)
	{
		myTrackHistoryEnabled=e;

		emit trackHistoryEnabledChanged(myTrackHistoryEnabled);
	}
}

bool ByyEntitySettings::entityInfoEnabled()
{
	return myEntityInfoEnabled;
}

void ByyEntitySettings::setEntityInfoEnabled( bool e )
{
	if (myEntityInfoEnabled!=e)
	{
		myEntityInfoEnabled=e;

		emit entityInfoEnabledChanged(myEntityInfoEnabled);
	}
}

bool ByyEntitySettings::electronicEnabled()
{
	return myElectronicEnabled;
}

void ByyEntitySettings::setElectronicEnabled( bool e )
{
	if (myElectronicEnabled!=e)
	{
		myElectronicEnabled=e;

		emit electronicEnabledChanged(myElectronicEnabled);
	}
}

void ByyEntitySettings::readSettings()
{
	QSettings s(myRelativePath.resolvePath("../data/config/Entity.ini"),QSettings::IniFormat);
	s.beginGroup("Entity");
	myGroundClampEnabled=s.value("GroundClampEnabled",true).toBool();
	myEntityLabelEnabled=s.value("EntityLabelVisible",true).toBool();
	myEntityNameEnabled=s.value("EntityNameVisible",true).toBool();
	myAutoScaleEnabled=s.value("AutoScaleEnabled",false).toBool();
	myTrackHistoryEnabled=s.value("TrackHistoryVisible",true).toBool();
	myEntityInfoEnabled=s.value("EntityInfoVisible",true).toBool();
	myElectronicEnabled=s.value("ElectronicVisible",true).toBool();
	mySatelliteOribtVisible=s.value("SatelliteOribtVisible",false).toBool();
	_terrainDeployChecking=s.value("terrainDeployChecking",false).toBool();
	_terrainObstacleLayers=s.value("terrainObstacleLayers",false).toString().split(';');

}

void ByyEntitySettings::writeSettings()
{
	QSettings s(myRelativePath.resolvePath("../data/config/Entity.ini"),QSettings::IniFormat);
	s.beginGroup("Entity");

	s.setValue("GroundClampEnabled",myGroundClampEnabled);
	s.setValue("EntityLabelVisible",myEntityLabelEnabled);
	s.setValue("EntityNameVisible",myEntityNameEnabled);
	s.setValue("AutoScaleEnabled",myAutoScaleEnabled);
	s.setValue("TrackHistoryVisible",myTrackHistoryEnabled);
	s.setValue("EntityInfoVisible",myEntityInfoEnabled);
	s.setValue("ElectronicVisible",myElectronicEnabled);
	s.setValue("SatelliteOribtVisible",mySatelliteOribtVisible);
	s.setValue("terrainDeployChecking",_terrainDeployChecking);
	s.setValue("terrainObstacleLayers",_terrainObstacleLayers.join(";"));
}

bool ByyEntitySettings::satelliteOribtVisible()
{
	return mySatelliteOribtVisible;
}

void ByyEntitySettings::setSatelliteOribtVisible( bool e )
{
	if (e!=mySatelliteOribtVisible)
	{
		mySatelliteOribtVisible=e;
		emit satelliteOribtVisibleChanged(mySatelliteOribtVisible);
	}
}
