#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QtGui/QColor>

#include "snigcore_global.h"

class SNIGCORE_EXPORT ByyBuildingSettings: public QObject
{
	Q_OBJECT
public:
	ByyBuildingSettings();
	inline bool isShowInfo();
	QColor backgroundColor();

	void registerShowInfo(const QObject * receiver, const char * method);

public slots:
	void showInfo(bool show);
	void setBackgroundColor(const QColor& color);

signals:
	void showInfoChanged(bool);

private:
	bool _showInfo;
	QColor _background_color;
};

class SNIGCORE_EXPORT ByyVideoSettings: public QObject
{
	Q_OBJECT
public:
	ByyVideoSettings();
	inline bool isShow();

	void registerShow(const QObject * receiver, const char * method);

public slots:
	void show(bool s);

signals:
	void showInfoChanged(bool);

private:
	bool _show;

};

class SNIGCORE_EXPORT ByyOceanSettings: public QObject
{
	Q_OBJECT
public:
	ByyOceanSettings();
	inline float seaLevel();

	void registerSeaLevel(const QObject * receiver, const char * method);

	public slots:
		void setSeaLevel(float value);

signals:
		void seaLevelChanged(float);

private:
	float _seaLevel;

};

class SNIGCORE_EXPORT ByyEnvirProcessSettings: public QObject
{
	Q_OBJECT

public:
	ByyEnvirProcessSettings();
	inline bool isShow();

	void registerShow(const QObject * receiver, const char * method);

public slots:
	void show(bool s);

signals:
	void showChanged(bool);

private:
	bool _show;
};

class SNIGCORE_EXPORT ByyMouseCoordsSettings: public QObject
{
	Q_OBJECT
public:
	ByyMouseCoordsSettings();
	inline bool enabled();

	void registerEnable(const QObject * receiver, const char * method);

	public slots:
		void setEnabled(bool e);

signals:
		void enableChanged(bool);

private:
	bool _enable;
};

class SNIGCORE_EXPORT ByyConfigIni: public QObject
{
public:
	#define MAX_REC_FILES_SIZE 5
	static ByyConfigIni* instance();

	QString title(){return _title;}
	QString earthFile(){return _earthFile;}
	QString companyName(){return _companyName;}
	QString cameraCapture(){return _cameraCapture;}

	bool enable2d(){return _enable2d;}

	QString userRule(){return _userRule;}
	QStringList	plugins(){return _plugins;}
	bool defense(){return _defense;}
	bool uav(){return _uav;}

	QString	scnPath(){return _scnPath;}
	void	setScnPath(QString val){_scnPath=val;}
	void	setisScn(bool val){_isScn=val;}
	bool	getisScn(){return _isScn;}
	QStringList	recScnFiles(){return _recScnFiles;}
	void	addRecScnFile(QString file);
	void	removeRecScnFile(QString file);

	QString weaponHitProbTable(){return _weaponHitProbTable;}
	QString entityDamageProbTable(){return _entityDamageProbTable;}

private:
	ByyConfigIni();
	~ByyConfigIni(){writeSettings();}
	void writeSettings();

private:
	QString _title;
	QString _earthFile;
	QString _companyName;
	QString _cameraCapture;
	bool	_enable2d;
	QString _userRule;
	bool _defense;
	bool _uav;
	QStringList	_plugins;

	QString	_scnPath;
	QStringList	_recScnFiles;
	bool _isScn;
	
	//裁决提供的相关文件
	QString _weaponHitProbTable;		//武器碰撞概率
	QString _entityDamageProbTable;		//实体毁伤概率表
};

class SNIGCORE_EXPORT ByySettings
{
public:
	virtual ~ByySettings();

	static ByySettings* instance();

	inline ByyBuildingSettings* buildingSettings(){return _buildingSettings;}
	inline ByyVideoSettings* videoSettings(){return _videoSettings;}
	inline ByyOceanSettings* oceanSettings(){return _oceanSettings;}
	inline ByyEnvirProcessSettings* envirProcessSettings(){return _environmentProcessSettings;}
	inline ByyMouseCoordsSettings* mouseCoordsSettings(){return _mouseCoordsSettings;}

private:
	ByySettings();

private:
	ByyBuildingSettings* _buildingSettings;
	ByyVideoSettings* _videoSettings;
	ByyOceanSettings* _oceanSettings;
	ByyEnvirProcessSettings* _environmentProcessSettings;
	ByyMouseCoordsSettings* _mouseCoordsSettings;
};

#endif // SETTINGS_H
