#include "hdoseEvent.h"

#include "JSCGFSimDS/JSCGFSimDS.h"
#include "JSCGFSimDS/BaseDataType.h"
#include "JSCGFSimEvt/JSCGFSimEvt.h"
#include "JSCGFSimTsk/JSCGFSimTsk.h"
#include "JSCGFSimMF/JSCGFSimMF.h"
#include "JSCoreAppDS/JSCoreAppDS.h"

#include "JSCGFSimMF/JSEntitySR.h" //"StateRepository.h"
#include "JSCGFSimMF/JSControlObject.h"

#include <qdebug.h>

ByyHdoseEvent::ByyHdoseEvent(Type type)
	:myType(type)
	,myObj(0)
{
	
}

ByyHdoseEvent::ByyHdoseEvent()
	:myType(End)
	,myId(-1)
	,mySide(-1)
{

}

ByyHdoseEvent::~ByyHdoseEvent()
{

}

QSharedPointer<ByyHdoseEvent> ByyHdoseEvent::create( CEngine * pEng, DWORD type, LPVOID pObj )
{
	CObj * p = (CObj*) pObj;

	QSharedPointer<ByyHdoseEvent> t_event(new ByyHdoseEvent);

	t_event->setObj(p);

	if( type == '+' )
	{
		p->RequestUpdate();

		if( p->IsDrivedFrom( "JSEntitySR" ))
		{
			JSEntitySR * psr = (JSEntitySR*)p;
			t_event->setType(EntityAdd);
			t_event->setId(psr->GetID());
			t_event->setSide(psr->GetSide());
			
			//qDebug()<<QString::fromLocal8Bit("------实体名称GetName：--------")<<QString::fromLocal8Bit(p->GetName());
			//qDebug()<<QString::fromLocal8Bit("------实体名称className：--------")<<p->GetClsInfo()->className;
			
			//qDebug()<<QString::fromLocal8Bit("------实体名称：--------")<<QString::fromLocal8Bit(psr->GetEntityName());
			//qDebug()<<QString::fromLocal8Bit("------实体ID：--------")<<psr->GetID();
			//qDebug()<<QString::fromLocal8Bit("------实体名称：--------")<<QString::fromLocal8Bit(psr->m_name);
			//qDebug()<<QString::fromLocal8Bit("------实体类型：--------")<<psr->m_EntityType.kind<<","<<psr->m_EntityType.domain<<","
				//<<psr->m_EntityType.country<<","<<psr->m_EntityType.category<<","<<psr->m_EntityType.subCategory<<","<<psr->m_EntityType.specific<<","
				//<<psr->m_EntityType.extra;

			//if(psr->m_EntityType.kind == 2 && psr->m_EntityType.domain == 1 && psr->m_EntityType.country == 222 && psr->m_EntityType.category == 1 && psr->m_EntityType.subCategory == 18)
			//{
				//qDebug()<<QString::fromLocal8Bit("------实体名称：--------")<<QString::fromLocal8Bit(psr->GetEntityName());
			//}
			
		}
		else if( p->IsDrivedFrom( "JSControlObject" ))
		{
			JSControlObject * psr = (JSControlObject*)p;

			t_event->setType(CtrlObjAdd);
			t_event->setId(psr->GetID());
			t_event->setSide(psr->GetSide());

			static CtrlObjNameTypeMap myCtrlObjNameTypeMapper;

			if (myCtrlObjNameTypeMapper.isEmpty())
			{
				myCtrlObjNameTypeMapper.insert("JSWayPoint",WayPoint);
				myCtrlObjNameTypeMapper.insert("JSPhaseline",Phaseline);
				myCtrlObjNameTypeMapper.insert("JSRoute",Route);
				myCtrlObjNameTypeMapper.insert("JSCircle",Circle);
				myCtrlObjNameTypeMapper.insert("JSPolygon",Polygon);
				myCtrlObjNameTypeMapper.insert("JSEllipse",Ellipse);
				myCtrlObjNameTypeMapper.insert("JSRectangle",Rectangle);
			}

			CtrlObjNameTypeMap::Iterator it=myCtrlObjNameTypeMapper.begin();

			for (;it!=myCtrlObjNameTypeMapper.end();++it)
			{
				if (p->IsDrivedFrom(it.key().toAscii().data()))
				{
					t_event->setShapeType(it.value());
					break;
				}
			}
		}

	}
	else if( type == '-' )
	{
		//printf("%s\n",p->GetClsInfo()->className);
		if( p->IsDrivedFrom( "JSEntitySR" ))
		{
			t_event->setType(EntityRemoved);
			t_event->setId(p->GetID());
		}
		else if (p->IsDrivedFrom( "JSControlObject" ))
		{
			t_event->setType(CtrlObjRemoved);
			t_event->setId(p->GetID());
		}
	}

	return t_event;
}

void ByyHdoseEvent::setType( Type type )
{
	myType=type;
}

ByyHdoseEvent::Type ByyHdoseEvent::type() const
{
	return myType;
}

void ByyHdoseEvent::setId( int id )
{
	myId=id;
}

int ByyHdoseEvent::id() const
{
	return myId;
}

void ByyHdoseEvent::setSide( int side )
{
	mySide=side;
}

int ByyHdoseEvent::side() const
{
	return mySide;
}

void ByyHdoseEvent::setShapeType( CtrlObjType st )
{
	myCtrlObjShapeType=st;
}

ByyHdoseEvent::CtrlObjType ByyHdoseEvent::shapeType() const
{
	return myCtrlObjShapeType;
}

void ByyHdoseEvent::setObj( CObj* obj )
{
	myObj=obj;
}

CObj* ByyHdoseEvent::getObj()
{
	return myObj;
}
