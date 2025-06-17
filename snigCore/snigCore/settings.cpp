#include "settings.h"

#include <QSettings>
#include "ByyRelativePath.h"
#include <QTextCodec>

ByySettings::ByySettings():
_buildingSettings(new ByyBuildingSettings),
_videoSettings(new ByyVideoSettings),
_oceanSettings(new ByyOceanSettings),
_environmentProcessSettings(new ByyEnvirProcessSettings),
_mouseCoordsSettings(new ByyMouseCoordsSettings)
{

}

ByySettings::~ByySettings()
{
	delete _buildingSettings;
	delete _videoSettings;
	delete _oceanSettings;
	delete _environmentProcessSettings;
	delete _mouseCoordsSettings;
}

ByySettings* ByySettings::instance()
{
	static ByySettings _instance;
	return &_instance;
}



ByyBuildingSettings::ByyBuildingSettings()
	:_showInfo(false),_background_color(0,0,255,50)
{

}

bool ByyBuildingSettings::isShowInfo()
{
	return _showInfo;
}

QColor ByyBuildingSettings::backgroundColor()
{
	return _background_color;
}

void ByyBuildingSettings::showInfo( bool show )
{
	if (show!=_showInfo)
	{
		_showInfo=show;
		emit showInfoChanged(_showInfo);
	}
}

void ByyBuildingSettings::setBackgroundColor( const QColor& color )
{
	if (_background_color!=color)
	{
		_background_color=color;
	}
}

void ByyBuildingSettings::registerShowInfo( const QObject * receiver, const char * method )
{
	connect(this,SIGNAL(showInfoChanged(bool)),receiver,method);
}

ByyVideoSettings::ByyVideoSettings()
	:_show(false)
{

}

bool ByyVideoSettings::isShow()
{
	return _show;
}

void ByyVideoSettings::registerShow( const QObject * receiver, const char * method )
{
	connect(this,SIGNAL(showInfoChanged(bool)),receiver,method);
}

void ByyVideoSettings::show( bool s )
{
	if (s!=_show)
	{
		_show=s;
		emit showInfoChanged(_show);
	}
}

ByyOceanSettings::ByyOceanSettings()
	:_seaLevel(10)
{

}

float ByyOceanSettings::seaLevel()
{
	return _seaLevel;
}

void ByyOceanSettings::registerSeaLevel( const QObject * receiver, const char * method )
{
	connect(this,SIGNAL(seaLevelChanged(float)),receiver,method);
}

void ByyOceanSettings::setSeaLevel( float value )
{
	if (value!=_seaLevel)
	{
		_seaLevel=value;
		emit seaLevelChanged(value);
	}
}

ByyEnvirProcessSettings::ByyEnvirProcessSettings()
	:_show(true)
{

}

bool ByyEnvirProcessSettings::isShow()
{
	return _show;
}

void ByyEnvirProcessSettings::registerShow( const QObject * receiver, const char * method )
{
	connect(this,SIGNAL(showChanged(bool)),receiver,method);
}

void ByyEnvirProcessSettings::show( bool s )
{
	if (s!=_show)
	{
		_show=s;
		emit showChanged(_show);
	}
}

ByyMouseCoordsSettings::ByyMouseCoordsSettings()
	:_enable(false)
{

}

bool ByyMouseCoordsSettings::enabled()
{
	return _enable;
}

void ByyMouseCoordsSettings::registerEnable( const QObject * receiver, const char * method )
{
	connect(this,SIGNAL(enableChanged(bool)),receiver,method);
}

void ByyMouseCoordsSettings::setEnabled( bool e )
{
	if (e!=_enable)
	{
		_enable=e;
		emit enableChanged(_enable);
	}

}

ByyConfigIni::ByyConfigIni()
{
	QSettings s(myRelativePath.resolvePath("../data/config/config.ini"),QSettings::IniFormat);

	s.beginGroup("SNIG");

	_title=QString::fromLocal8Bit(s.value("Title").toByteArray().data());

	_earthFile=QString::fromLocal8Bit(s.value("EarthFile").toByteArray().data());

	_companyName=QString::fromLocal8Bit(s.value("CompanyName").toByteArray().data());

	_cameraCapture=QString::fromLocal8Bit(s.value("CameraCapture").toByteArray().data());

	_enable2d=s.value("Enable2D").toBool();

	_userRule=QString::fromLocal8Bit(s.value("UserRule").toByteArray().data());

	_defense=0;
	_uav=0;
	_defense=s.value("Defense").toInt();
	_uav=s.value("UAV").toInt();

	_scnPath=QString::fromLocal8Bit( s.value("scnPath").toByteArray().data());
	//_recScnFiles=s.value("recScnFiles").toString().split(';');
	QString recfiles = QString::fromLocal8Bit( s.value("recScnFiles").toByteArray().data() );
	_recScnFiles= recfiles.split(';');

	//裁决相关的表单
	_weaponHitProbTable =s.value("WeaponHitProbTable").toString();
	_entityDamageProbTable=s.value("EntityDamageProbTable").toString();

	_isScn = false;
}

ByyConfigIni* ByyConfigIni::instance()
{
	static ByyConfigIni _instance;
	return &_instance;
}

void ByyConfigIni::writeSettings()
{
	QSettings s(myRelativePath.resolvePath("../data/config/config.ini"),QSettings::IniFormat);
	s.setIniCodec(QTextCodec::codecForName("GB2312"));
	s.beginGroup("SNIG");

	s.setValue("title",_title);
	s.setValue("earthFile",_earthFile);
	s.setValue("companyName",_companyName);
	s.setValue("cameraCapture",_cameraCapture);
	s.setValue("enable2d",_enable2d);
	s.setValue("userRule",_userRule);
	s.setValue("Defense",_defense ? 1:0);
	s.setValue("UAV",_uav ? 1:0);
	s.setValue("scnPath",_scnPath);
	s.setValue("recScnFiles",_recScnFiles.join(";"));

}

void ByyConfigIni::addRecScnFile(QString file)
{
	int findIdx = _recScnFiles.indexOf(file);
	if(findIdx>=0)
	{
		_recScnFiles.removeAt(findIdx);
	}
	_recScnFiles.prepend(file);
	if(_recScnFiles.size()>MAX_REC_FILES_SIZE)
		_recScnFiles.removeLast();

	//if(_recScnFiles.contains(file))
	//	return;
	//_recScnFiles.append(file);
	//_recScnFiles.prepend(file);
	//if(_recScnFiles.size()>MAX_REC_FILES_SIZE)
		//_recScnFiles.removeFirst();
	//	_recScnFiles.removeLast();
}

void ByyConfigIni::removeRecScnFile(QString file)
{
	_recScnFiles.removeOne(file);
}