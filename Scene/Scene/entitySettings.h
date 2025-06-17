#ifndef ENTITYSETTINGS_H
#define ENTITYSETTINGS_H

#include <QObject>
#include "scene_global.h"
#include <QtGlobal>

class SCENE_EXPORT ByyEntitySettings : public QObject
{
	Q_OBJECT

public:
	~ByyEntitySettings();

	static ByyEntitySettings& instance();

	inline bool groundClampEnabled();
	inline bool entityLabelEnabled();
	inline bool entityNameEnabled();
	inline bool autoScaleEnabled();
	inline bool trackHistoryEnabled();
	inline bool entityInfoEnabled();
	inline bool electronicEnabled();
	inline bool satelliteOribtVisible();

	bool	terrainDeployChecking(){return _terrainDeployChecking;}
	QStringList	terrainObstacleLayers(){return _terrainObstacleLayers;}	

public slots:
	void setGroundClampEnabled(bool e);
	void setEntityLabelEnabled(bool e);
	void setEntityNameEnabled(bool e);
	void setAutoScaleEnabled(bool e);
	void setTrackHistoryEnabled(bool e);
	void setEntityInfoEnabled(bool e);
	void setElectronicEnabled(bool e);
	void setSatelliteOribtVisible(bool e);

protected:
	ByyEntitySettings();
	void readSettings();
	void writeSettings();

signals:
	void groundClampEnabledChanged(bool);
	void entityLabelEnabledChanged(bool);
	void entityNameEnabledChanged(bool);
	void autoScaleEnabledChanged(bool);
	void trackHistoryEnabledChanged(bool);
	void entityInfoEnabledChanged(bool);
	void electronicEnabledChanged(bool);
	void satelliteOribtVisibleChanged(bool);

protected:
	bool myGroundClampEnabled;
	bool myEntityLabelEnabled;
	bool myEntityNameEnabled;
	bool myAutoScaleEnabled;
	bool myTrackHistoryEnabled;
	bool myEntityInfoEnabled;
	bool myElectronicEnabled;
	bool mySatelliteOribtVisible;
	bool	_terrainDeployChecking;
	QStringList	_terrainObstacleLayers;
};

#endif // ENTITYSETTINGS_H
