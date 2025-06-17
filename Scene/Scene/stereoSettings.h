#ifndef STEREOSETTINGS_H
#define STEREOSETTINGS_H

#include <QObject>
#include "scene_global.h"

class SCENE_EXPORT ByyStereoSettings : public QObject
{
	Q_OBJECT

public:
	~ByyStereoSettings();

	static ByyStereoSettings& instance();

	inline bool enabled();
	osg::DisplaySettings::StereoMode mode();

public slots:
	void setEnabled(bool enabled);
	void setMode(const osg::DisplaySettings::StereoMode& mode);

protected:
	ByyStereoSettings(QObject *parent=0);

signals:
	void enableChanged(bool);
	void modeChanged(const osg::DisplaySettings::StereoMode&);

protected:
	bool myEnabled;
	osg::DisplaySettings::StereoMode myMode;
};

#endif // STEREOSETTINGS_H
