#ifndef AUDIOSETTINGS_H
#define AUDIOSETTINGS_H

#include <QObject>
#include "scene_global.h"

class SCENE_EXPORT ByyAudioSettings : public QObject
{
	Q_OBJECT

public:
	~ByyAudioSettings();

	static ByyAudioSettings& instance();

	inline bool enabled();
	inline float soundVolume();
	inline int range();

public slots:
	void setEnabled(bool enable);
	void setSoundVolume(float sv);
	void setRange(int r);

signals:
	void enableChanged(bool);
	void volumeChanged(float);
	void rangeChanged(int);

protected:
	ByyAudioSettings();

protected:
	bool _enable;
	float _soundVolume;
	int _range;
};

#endif // AUDIOSETTINGS_H
