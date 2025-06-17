#include "stdafx.h"
#include "soundManager.h"
#include "ByyIG.h"
#include "audioSettings.h"
#include "ByyInputController.h"
#include "ByyChannel.h"

#include <osgAudio/SoundManager.h>

ByySoundManager::ByySoundManager( ByyIG& ig )
	:myIG(ig)
	,myInitialized(false)
	,m_last_time(0)
{
	connect(&ByyAudioSettings::instance(),SIGNAL(enableChanged(bool)),SLOT(setEnabled(bool)));
}

ByySoundManager::~ByySoundManager()
{
	 if (osgAudio::SoundManager::instance()->initialized())
	 {
		 osgAudio::SoundManager::instance()->shutdown();
	 }
}

void ByySoundManager::setEnabled( bool enabled )
{
	if (enabled)
	{
		osgAudio::SoundManager::instance()->init( 50, true );
		osgAudio::SoundManager::instance()->getEnvironment()->setDistanceModel(osgAudio::InverseDistance);
		osgAudio::SoundManager::instance()->getEnvironment()->setDopplerFactor(1);
	}
	else
	{
		osgAudio::SoundManager::instance()->shutdown();
	}

	myInitialized=enabled;
}

void ByySoundManager::tick()
{
	if (!myInitialized)
		return;

	const double curr_time( osg::Timer::instance()->time_s());

	if (curr_time-m_last_time<osgAudio::SoundManager::instance()->getUpdateFrequency())
		return;

	osgAudio::SoundManager::instance()->update();

	if(ByyChannel *channel=myIG.inputController().currentChannel())
	{
		osg::Matrixd m(channel->view()->getCamera()->getViewMatrix());

		osgAudio::SoundManager::instance()->setListenerMatrix( m );
	}
}
