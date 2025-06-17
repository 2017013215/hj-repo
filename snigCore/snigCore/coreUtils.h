#ifndef COREUTILS_H
#define COREUTILS_H

#include <QString>
#include "snigcore_global.h"
#include <QMainWindow>

namespace osg
{
	class Vec3d;
}

QString vec3d2Str(const osg::Vec3d& vec);
osg::Vec3d str2Vec3d(const QString& str);

osg::Vec3d swapXY(const osg::Vec3d& vec);

SNIGCORE_EXPORT QMainWindow*	frameWnd();

#define CNTR(str) QString::fromLocal8Bit(str) /*QObject::tr(str)*/

#define ByyDELETE(x) \
	if (x) \
{ \
	delete x; \
	x = NULL; \
}

#define IMP_PROPERTY(type,name)\
protected:\
	type	m_##name;\
public:\
	type&	name(){return m_##name;}\
	void	set##name(type val){m_##name=val;}\


#define DateTime_fmt "yyyy,MM,dd,hh,mm,ss"


SNIGCORE_EXPORT QAction* createAction(QMenu* menu,int cmd,QString txt,QString icon="");
SNIGCORE_EXPORT QAction* createAction(QMenu* menu,QString txt,QObject* recver,char* slot,QString icon="");
SNIGCORE_EXPORT bool isDigitStr(QString str);
SNIGCORE_EXPORT QMenu*	sysMenu(QString id,QMenu* parent=0,bool heading=false);

SNIGCORE_EXPORT char* pos2str( double lon, double lat);

#endif // COREUTILS_H
