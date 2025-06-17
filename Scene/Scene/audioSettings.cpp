#include "StdAfx.h"
#include "audioSettings.h"

ByyAudioSettings::ByyAudioSettings()
	:_enable(false),
	_soundVolume(1.0),
	_range(100)
{

}

ByyAudioSettings::~ByyAudioSettings()
{

}

ByyAudioSettings& ByyAudioSettings::instance()
{
	static ByyAudioSettings _instance;
	return _instance;
}
void ByyAudioSettings::setEnabled( bool enable )
{
	if (enable!=_enable)
	{
		_enable=enable;
		emit enableChanged(_enable);
	}
}

bool ByyAudioSettings::enabled()
{
	return _enable;
}

void ByyAudioSettings::setSoundVolume( float sv )
{
	if (sv!=_soundVolume)
	{
		_soundVolume=sv;
		emit volumeChanged(_soundVolume);
	}
}

float ByyAudioSettings::soundVolume()
{
	return _soundVolume;
}

void ByyAudioSettings::setRange( int r )
{
	if (r!=_range)
	{
		_range=r;
		emit rangeChanged(_range);
	}
}

int ByyAudioSettings::range()
{
	return _range;
}