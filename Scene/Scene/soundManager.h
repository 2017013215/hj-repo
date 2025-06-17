#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <osgAudio/SoundRoot.h>

class ByyIG;

class ByySoundManager: public QObject
{
	Q_OBJECT
public:
	ByySoundManager(ByyIG& ig);
	~ByySoundManager();

public slots:
	void setEnabled(bool enabled);
	void tick();

private:
	osg::ref_ptr<osgAudio::SoundRoot> sound_root;
	ByyIG& myIG;
	bool myInitialized;
	double m_last_time;
};

#endif // SOUNDMANAGER_H
