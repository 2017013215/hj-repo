#ifndef HDOSEEVENT_H
#define HDOSEEVENT_H

#include <QSharedPointer>
#include <QMap>
#include <windows.h>

class CEngine;
class CObj;

class ByyHdoseEvent
{
public:
	enum Type{EntityAdd,EntityRemoved,CtrlObjAdd,CtrlObjRemoved,End};
	enum CtrlObjType{WayPoint,Phaseline,Route,Circle,Polygon,Ellipse,Rectangle};

	ByyHdoseEvent(Type type);
	ByyHdoseEvent();
	virtual ~ByyHdoseEvent();

	void setType(Type type);
	Type type()const;

	void setId(int id);
	int  id()const;

	void setSide(int side);
	int side()const;

	void setShapeType(CtrlObjType st);
	CtrlObjType shapeType()const;

	void setObj(CObj* obj);
	CObj* getObj();

	static QSharedPointer<ByyHdoseEvent> create(CEngine * pEng, DWORD type, LPVOID pObj);

protected:
	Type myType;
	int myId;
	int mySide;

	CtrlObjType myCtrlObjShapeType;

	CObj* myObj;

	typedef QMap<QString,CtrlObjType> CtrlObjNameTypeMap;
};

#endif // HDOSEEVENT_H
