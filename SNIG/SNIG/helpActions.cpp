#include "StdAfx.h"
#include "helpActions.h"
#include "ByyApp.h"
#include "mainWindow.h"

ByyAboutAction::ByyAboutAction( ByyApp& app,QWidget* parent ) 
	: ByyAction(app,"ByyAboutAction",parent)
{
	setText( QString::fromLocal8Bit("关于(&A)") );
	setIcon( QIcon("../data/icons/information.png") );
}

ByyAboutAction::~ByyAboutAction()
{

}

void ByyAboutAction::on_triggered()
{
	QMessageBox::about(&myApp.mainWindow(), QString::fromLocal8Bit("关于"),
		QString::fromLocal8Bit("<p><span style=\" font-size:12pt; font-weight:600; color:#ff0000;\">电子沙盘 v1.0</span></p><p align=\"right\"><span style=\" font-weight:600;\">海军航空工程学院科研部软件中心</span></p>"));
}

ByyShowDocAction::ByyShowDocAction( ByyApp& app,const QString& objectName,const QString& text,const QString& file,QWidget* parent/*=0*/ ) 
	:ByyAction(app,objectName,parent)
	,myDocFile(file)
{
	setText( text );
}

ByyShowDocAction::~ByyShowDocAction()
{

}

void ByyShowDocAction::on_triggered()
{
	QDesktopServices::openUrl(QUrl("file:///"+QFileInfo(myDocFile).absoluteFilePath()));
}
