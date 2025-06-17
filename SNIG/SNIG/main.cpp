#include "stdafx.h"
#include "mainWindow.h"
#include "settings.h"
#include "ByyApp.h"
#include "stereoSettings.h"
#include "ByyUserOperateLog.h"
#include "ByyRelativePath.h"

#include <QMessageBox>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QDir::setCurrent(a.applicationDirPath());

	putenv("GDAL_DATA=./gdal_data");

	ByyRelativePath::instance().setCurrentPath(a.applicationDirPath());

	ByyUserOperateLog::instance().addLog(QString::fromLocal8Bit("系统启动"));

	a.setOrganizationName("BOYONGYAN");
	a.setApplicationName("SNIG");

	QTranslator translator;
	translator.load(":/ByyMainWindow/Resources/qt_zh_CN.qm");

	a.installTranslator(&translator);

	if (a.arguments().contains("--stereo"))
	{
		ByyStereoSettings::instance().setEnabled(true);

		if (a.arguments().contains("quad_buffer")||a.arguments().contains("QUAD_BUFFER"))
		{
			ByyStereoSettings::instance().setMode(osg::DisplaySettings::QUAD_BUFFER);
		}
		else if (a.arguments().contains("anaglyphic")||a.arguments().contains("ANAGLYPHIC"))
		{
			ByyStereoSettings::instance().setMode(osg::DisplaySettings::ANAGLYPHIC);
		}
		else if (a.arguments().contains("horizontal_split")||a.arguments().contains("HORIZONTAL_SPLIT"))
		{
			ByyStereoSettings::instance().setMode(osg::DisplaySettings::HORIZONTAL_SPLIT);
		}
	}
	
	QString earthFile;

	foreach(const QString& str,a.arguments())
	{
		if (str.contains(".earth"))
		{
			earthFile=str;
			break;
		}
	}

	QSplashScreen splash(QPixmap(myRelativePath.resolvePath("../data/icons/sn-sim ig.png")));
	splash.show();

	a.processEvents();

	if (earthFile.isEmpty())
	{
		earthFile=ByyConfigIni::instance()->earthFile();
	}

	if (earthFile.isEmpty())
	{
		earthFile="../data/earthFile/default.earth";
	}

	ByyApp myApp;

	myApp.initialize();

	myApp.mainWindow().setWindowTitle(ByyConfigIni::instance()->title());

	splash.finish(&myApp.mainWindow());

// 	a.setStyle( new Qtitan::RibbonStyle() );
// 
// 	ByyRibbonMainWindow rw(myApp);
// 
// 	rw.show();

	myApp.mainWindow().showMaximized();

	myApp.ig().terrain()->load(earthFile.toStdString());
	myApp.ig().createScaleBar();

	int r=a.exec();

	ByyUserOperateLog::instance().addLog(QString::fromLocal8Bit("系统退出"));

	return r;
}
