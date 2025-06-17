#include "coreUtils.h"
#include <osg/Vec3d>
#include <QApplication>
#include <QtGUI>

QString vec3d2Str( const osg::Vec3d& vec )
{
	return QString("%1,%2,%3").arg(vec.x(),0,'f').arg(vec.y(),0,'f').arg(vec.z(),0,'f');
}

osg::Vec3d str2Vec3d( const QString& str )
{
	double X=0,Y=0,Z=0;

	sscanf(str.toStdString().c_str(),"%lf,%lf,%lf",&X,&Y,&Z);

	return osg::Vec3d(X,Y,Z);
}

osg::Vec3d swapXY( const osg::Vec3d& vec )
{
	return osg::Vec3d(vec.y(),vec.x(),vec.z());
}

QMainWindow*	frameWnd()
{
	QWidgetList buf=QApplication::topLevelWidgets();
	foreach(QWidget* wnd,QApplication::topLevelWidgets())
	{
		if(wnd->inherits("QMainWindow"))
			return qobject_cast<QMainWindow*>(wnd);
	}
	return 0;

}

QAction* createAction(QMenu* menu,int cmd,QString txt,QString icon)
{
	QAction* act=new QAction(txt,menu);
	act->setData(cmd);
	menu->addAction(act);
	if(!icon.isEmpty())
		act->setIcon(QIcon(icon));
	return act;
}

QAction* createAction(QMenu* menu,QString txt,QObject* recver,char* slot,QString icon)
{
	QAction* action=new QAction(txt,menu);
	menu->addAction(action);
	if(!icon.isEmpty())
		action->setIcon(QIcon(icon));
	QObject::connect(action,SIGNAL(triggered(bool)),recver,slot);
	return action;
}

bool isDigitStr(QString str)
{
	QByteArray strArray=str.toLatin1();

	for(int i=0;i<strArray.size();i++)
	{
		char ch=strArray.at(i);
		if(!(ch>='0'&&ch<='9')&&ch!='.')
			return false;
	}

	return true;
}


QMenu*	sysMenu(QString id,QMenu* parent,bool heading)
{
	QObject* owner=parent==0?(QObject*)(frameWnd()->menuBar()):(QObject*)(parent);
	foreach(QObject* item,owner->children()){
		QMenu* menu=qobject_cast<QMenu*>(item);
		if(menu==0)
			continue;
		if(menu->title()==id)			
		{
			return menu;
		}
	}
	//
	QMenu* newmenu=0;
	if(parent==0){
		if(heading){
			newmenu=new QMenu(id,frameWnd()->menuBar());
			QAction* act=frameWnd()->menuBar()->actionAt(QPoint(0,0));
			frameWnd()->menuBar()->insertMenu(act,newmenu);
		}
		else
			newmenu=frameWnd()->menuBar()->addMenu(id);
	}
	else
		newmenu=parent->addMenu(id);
	return newmenu;
}

char* pos2str( double lon, double lat)
{
	static char str[32];

	char	ns = lat < 0 ? 'S':'N';
	lat = fabs(lat);

	int latdeg=(int)lat;
	int latmin = int( ( lat-latdeg )*60 );
	int latminxxx =int(((lat-latdeg)*60-latmin)*60);

	char	ew = lon < 0 ? 'W':'E';
	lon = fabs(lon);

	int londeg=int(lon);
	int lonmin = int((lon-londeg)*60);
	int lonminxxx = int(((lon-londeg)*60-lonmin)*60);

	sprintf( str, "%02d^%02d'%02d\"%c %03d^%02d'%02d\"%c", 
		int(lat), latmin, latminxxx, ns, int(lon), lonmin, lonminxxx, ew );

	return str;
}

