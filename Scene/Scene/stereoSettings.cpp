#include "StdAfx.h"
#include "stereoSettings.h"

ByyStereoSettings::ByyStereoSettings(QObject *parent)
	: QObject(parent)
	,myEnabled(false)
	,myMode(osg::DisplaySettings::QUAD_BUFFER)
{
	osg::DisplaySettings::instance()->setStereoMode(osg::DisplaySettings::QUAD_BUFFER);
}

ByyStereoSettings::~ByyStereoSettings()
{

}

ByyStereoSettings& ByyStereoSettings::instance()
{
	static ByyStereoSettings _instance;
	return _instance;
}

bool ByyStereoSettings::enabled()
{
	return myEnabled;
}

osg::DisplaySettings::StereoMode ByyStereoSettings::mode()
{
	return myMode;
}

void ByyStereoSettings::setEnabled( bool enabled )
{
	if (enabled!=myEnabled)
	{
		myEnabled=enabled;

		osg::DisplaySettings::instance()->setStereo(myEnabled);

		emit enableChanged(myEnabled);
	}
}

void ByyStereoSettings::setMode( const osg::DisplaySettings::StereoMode& mode )
{
	if (myMode!=mode)
	{
		myMode=mode;

		osg::DisplaySettings::instance()->setStereoMode(myMode);

		emit modeChanged(myMode);
	}
}
