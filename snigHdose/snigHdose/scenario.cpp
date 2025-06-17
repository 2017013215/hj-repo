#include "scenario.h"
#include "ByyCore.h"
#include "localObjectManager.h"
#include "localEntity.h"
#include "taskManager.h"
#include "task.h"
#include "ByyHdose.h"
#include "taskFactory.h"

#include "ByyWaypoint.h"
#include "ByyCircle.h"
#include "ByyEllipse.h"
#include "ByyPhaseLine.h"
#include "ByyPolygon.h"
#include "ByyRectangle.h"
#include "ByyRoute.h"
#include "parameter.h"

#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <unordered_map>
#include <QStringList>

ByyScenario::ByyScenario( ByyHdose& hdose )
	:myHdose(hdose)
	,myCore(hdose.core())
	,myModified(false)
	,myStartDateTime(QDate(2016,1,1))
	,myEndDateTime(QDate(2016,1,2))
	,myName("Attack TaiWan NorthEast...")
{
	myEnvironment.mySeaStateLevel=2;
	myEnvironment.myWindCou=290;
	myEnvironment.myWindVel=15;
}

ByyScenario::~ByyScenario()
{

}

void ByyScenario::setName( const QString& n )
{
	if (myName!=n)
	{
		myName=n;
		setModified();
	}
}

const QString& ByyScenario::name() const
{
	return myName;
}

void ByyScenario::setStartDateTime( const QDateTime& dt )
{
	if (myStartDateTime!=dt)
	{
		myStartDateTime=dt;
		setModified();
	}
}

const QDateTime& ByyScenario::startDateTime() const
{
	return myStartDateTime;
}

void ByyScenario::setEndDateTime( const QDateTime& dt )
{
	if (myEndDateTime!=dt)
	{
		myEndDateTime=dt;
		setModified();
	}
}

const QDateTime& ByyScenario::endDateTime() const
{
	return myEndDateTime;
}

QString ByyScenario::filename() const
{
	return myFilename;
}

bool ByyScenario::load( const QString &filename )
{
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly))
		return false;

	QDomDocument doc("");

	bool ok=doc.setContent(&file);

	file.close();

	if (!ok||doc.isNull())
		return false;

	QDomElement root = doc.documentElement();

	if (root.isNull())
		return false;

	QDomElement itemsElement =root.firstChildElement("Items");

	if (itemsElement.isNull())
		return false;

	QDomElement schedulerElement;
	bool find=false;

	for(QDomNode n = itemsElement.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		schedulerElement=n.toElement();

		if (schedulerElement.isNull())
			continue;
		if (schedulerElement.tagName()!="Item")
			continue;
		if (schedulerElement.attribute("ItemClass")!="CScheduler")
			continue;

		find=true;
		break;
	}

	if (!find)
		return false;

	QDomNodeList items=schedulerElement.elementsByTagName("Item");

	for (int i=0;i!=items.size();++i)
	{
		QDomElement item=items.item(i).toElement();

		if (item.isNull())
			continue;

		QString itemClass=item.attribute("ItemClass");

		if (itemClass=="CScnProfile")
		{
			readProfile(item);
		}
		else if (itemClass=="CEnvironment")
		{
			readEnvironment(item);
		}
		else
		{
			ByyKeyValueList keyValues;

			QDomNodeList parameters=item.elementsByTagName("Parameter");

			for (int j=0;j!=parameters.size();++j)
			{
				QDomElement p=parameters.item(j).toElement();

				keyValues.add(p.attribute("Name"),p.attribute("Value"));
			}


			ByyEntityObject* ent=myHdose.createEntity(itemClass,keyValues);

			if (ent)
			{
				continue;
			}

			ByyControlObject* ctrlObj=0;

			if (itemClass=="CWayPoint")
			{
				ctrlObj=new ByyWaypoint;
			}
			else if (itemClass=="CCircle")
			{
				ctrlObj=new ByyCircle;
			}
			else if (itemClass=="CEllipse")
			{
				ctrlObj=new ByyEllipse;
			}
			else if (itemClass=="CPhaseline")
			{
				ctrlObj=new ByyPhaseLine;
			}
			else if (itemClass=="CPolygon")
			{
				ctrlObj=new ByyPolygon;
			}
			else if (itemClass=="CRectangle")
			{
				ctrlObj=new ByyRectangle;
			}
			else if (itemClass=="CRoute")
			{
				ctrlObj=new ByyRoute;
			}
			else if (itemClass=="CSimTask")
			{
				QString Executant;
				QString type;
				keyValues.get("Executant",Executant);
				keyValues.get("TaskType",type);

				ByyEntityObject* entObj=myCore.localObjectManager().findEntity(Executant);

				if (!Executant.isEmpty()&&!type.isEmpty()&&entObj)
				{
					ByyTaskCreator* taskCreator=myCore.taskFactory().findCreator(type);

					if (taskCreator)
					{
						ByyTask *task=taskCreator->createTask(*entObj);
						task->fromConfig(keyValues);
						entObj->taskManager().appendTask(task);
					}
				}
			}

			if (ctrlObj)
			{
				ctrlObj->fromConfig(keyValues);
				myCore.localObjectManager().appendControlObject(ctrlObj,ctrlObj->forceType());
			}

		}
	}

	myModified=false;

	myFilename=filename;

	return true;
}

bool ByyScenario::save()
{
	QDomDocument doc("");

	QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"GB2312\"");

	doc.appendChild(instruction);

	QDomElement root = doc.createElement("Simulation");
	doc.appendChild(root);

	QDomElement itemsElement = doc.createElement("Items");
	root.appendChild(itemsElement);

	QDomElement schedulerElement =serializeCscheduler(doc);

	itemsElement.appendChild(schedulerElement);

	schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ
	schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ

	schedulerElement.appendChild(serializeProfile(doc));

	schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ
	schedulerElement.appendChild(doc.createComment(QString::fromLocal8Bit("»·¾³ÐÅÏ¢")));
	schedulerElement.appendChild(serializeEnvironment(doc));

	std::unordered_map<ByyForceType,QString> entityForceTypes;
	entityForceTypes[ByyForceFriendly]=QString::fromLocal8Bit("ºì·½³õÊ¼Ì¬ÊÆ");
	entityForceTypes[ByyForceOpposing]=QString::fromLocal8Bit("À¶·½³õÊ¼Ì¬ÊÆ");
	entityForceTypes[ByyForceNeutral]=QString::fromLocal8Bit("ÖÐÁ¢³õÊ¼Ì¬ÊÆ");
	entityForceTypes[ByyForceOther]=QString::fromLocal8Bit("ÆäËû³õÊ¼Ì¬ÊÆ");

	std::unordered_map<ByyForceType,QString>::iterator it=entityForceTypes.begin();

	for (;it!=entityForceTypes.end();++it)
	{
		schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ
		schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ
		schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ
		schedulerElement.appendChild(doc.createComment(it->second));

		QVector<QDomElement> entElements=serializeEntityObjects(doc,it->first);

		foreach(const QDomElement& element,entElements)
		{
			schedulerElement.appendChild(element);
		}
	}

	schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ
	schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ
	schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ
	schedulerElement.appendChild(doc.createComment(QString::fromLocal8Bit("¿ØÖÆ¶ÔÏóÐÅÏ¢")));

	QVector<QDomElement> ctrlObjElements=serializeControlObjects(doc);

	foreach(const QDomElement& element,ctrlObjElements)
	{
		schedulerElement.appendChild(element);
	}

	schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ
	schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ
	schedulerElement.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ
	schedulerElement.appendChild(doc.createComment(QString::fromLocal8Bit("ÈÎÎñÐÅÏ¢")));

	QVector<QDomElement> taskElements=serializeEntityTask(doc);

	foreach(const QDomElement& element,taskElements)
	{
		schedulerElement.appendChild(element);
	}

	QString xml = doc.toString();

	QFile file(myFilename);

	if (!file.open(QFile::WriteOnly|QFile::Truncate))
		return false;

	QTextStream out(&file);

	out.setCodec("GB2312");

	doc.save(out,2);

	file.close();

	myModified=false;

	return true;
}

QDomElement ByyScenario::serializeCscheduler( QDomDocument& doc )
{
	QDomElement schedulerElement = doc.createElement("Item");

	schedulerElement.setAttribute( "ItemClass", "CScheduler" );
	schedulerElement.setAttribute( "ID", "1" );
	schedulerElement.setAttribute( "Name", "PlanMgr" );
	//schedulerElement.setAttribute( "CreateTime", "0.0" );

	// <Parameter  Name="Policy" Value="4"/>
	{
		QDomElement parameterElement = doc.createElement("Parameter");

		parameterElement.setAttribute( "Name", "Policy" );
		parameterElement.setAttribute( "Value", "4" );
		schedulerElement.appendChild( parameterElement );
	}

	// <Parameter  Name="Sample" Value="1"/>
	{
		QDomElement parameterElement = doc.createElement( "Parameter" );
		parameterElement.setAttribute( "Name", "Sample" );
		parameterElement.setAttribute( "Value", "1" );
		schedulerElement.appendChild( parameterElement );
	}

	// <Parameter  Name="AutoUpdate" Value="0"/>
	{
		QDomElement parameterElement = doc.createElement( "Parameter" );
		parameterElement.setAttribute( "Name", "AutoUpdate" );
		parameterElement.setAttribute( "Value", "10" );
		schedulerElement.appendChild( parameterElement );
	}

	/*// <Parameter  Name="Dispatcher" Value="CParDisp"/>
	{
		QDomElement parameterElement = doc.createElement( "Parameter" );
		parameterElement.setAttribute( "Name", "Dispatcher" );
		parameterElement.setAttribute( "Value", "CParDisp" );
		schedulerElement.appendChild( parameterElement );
	}*/
	schedulerElement.appendChild(doc.createComment("<Parameter  Name=\"Dispatcher\" Value=\"CParDisp\"/>"));

	//<Item ItemClass="CConsoleEvtConductor" ID="-1" Name="ConsEvtCdtor"/>
	{
		QDomElement itemElement = doc.createElement( "Item" );
		itemElement.setAttribute( "ItemClass", "CConsoleEvtConductor" );
		itemElement.setAttribute( "ID", "-1" );
		itemElement.setAttribute( "Name", "ConsEvtCdtor" );
		schedulerElement.appendChild( itemElement );
	}

	/*//<Item ItemClass="CBackStation" ID="-1" Name="BackSt"/>
	{
		QDomElement itemElement = doc.createElement( "Item" );
		/temElement.setAttribute( "ItemClass", "CBackStation" );
		itemElement.setAttribute( "ID", "-1" );
		itemElement.setAttribute( "Name", "BackSt" );
		schedulerElement.appendChild( itemElement );
	}

	//<Item ItemClass="CInteractionMgr" ID="-1" Name="InteractMgr"/>
	{
		QDomElement itemElement = doc.createElement( "Item" );
		itemElement.setAttribute( "ItemClass", "CInteractionMgr" );
		itemElement.setAttribute( "ID", "-1" );
		itemElement.setAttribute( "Name", "InteractMgr" );
		schedulerElement.appendChild( itemElement );
	}*/

	return schedulerElement;
}

QDomElement ByyScenario::serializeProfile( QDomDocument& doc )
{
	QDomElement scnProfile = doc.createElement( "Item" );
	scnProfile.setAttribute( "ItemClass", "CScnProfile" );
	scnProfile.setAttribute( "ID", "0" );
	scnProfile.setAttribute( "Name", "" );

	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "OutLine" );
		paraElement.setAttribute( "Value", myStartDateTime.date().toString("yyyy.MM.dd")+","+myName );
		scnProfile.appendChild( paraElement );
	}

	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "ScnStartTime" );
		paraElement.setAttribute( "Value",  myStartDateTime.toString("yyyy,M,d,h,m,s") );
		scnProfile.appendChild( paraElement );
	}

	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "ScnEndTime" );
		paraElement.setAttribute( "Value", myEndDateTime.toString("yyyy,M,d,h,m,s") );
		scnProfile.appendChild( paraElement );
	}

	//·ÂÕæÇøÓò:¾ØÐÎÇøÓò
	{
		scnProfile.appendChild(doc.createComment(""));//¿Õ°××¢ÊÍ

		QDomComment comment=doc.createComment(QString::fromLocal8Bit("·ÂÕæÇøÓò:¾ØÐÎÇøÓò"));

		scnProfile.appendChild(comment);

		const QVector<osg::Vec3d>& points=mySimulationRect.points();

		for (int i=0;i!=4;++i)
		{
			osg::Vec3d p=points.value(i);

			QDomElement paraElement = doc.createElement( "Parameter" );
			paraElement.setAttribute( "Name", "WayPoint" );
			paraElement.setAttribute( "Value", QString("%1,%2,%3").arg(p.y()).arg(p.x()).arg(p.z()) );
			scnProfile.appendChild( paraElement );
		}
	}

	return scnProfile;
}

QDomElement ByyScenario::serializeEnvironment( QDomDocument& doc )
{
	QDomElement environmentElement = doc.createElement( "Item" );
	environmentElement.setAttribute( "ItemClass", "CEnvironment" );
	environmentElement.setAttribute( "ID", "0" );
	environmentElement.setAttribute( "Name", "" );

	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "EnvObjName" );
		paraElement.setAttribute( "Value", "Env_All");
		environmentElement.appendChild( paraElement );
	}
	const QVector<osg::Vec3d>& points=myEnvironment.myRectangle.points();

	for (int i=0;i!=4;++i)
	{
		osg::Vec3d p=points.value(i);

		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "WayPoint" );
		paraElement.setAttribute( "Value", QString("%1,%2,%3").arg(p.y()).arg(p.x()).arg(p.z()) );
		environmentElement.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "SeaStateLevel" );
		paraElement.setAttribute( "Value", myEnvironment.mySeaStateLevel);
		environmentElement.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "WindCou" );
		paraElement.setAttribute( "Value", myEnvironment.myWindCou );
		environmentElement.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "WindVel" );
		paraElement.setAttribute( "Value", myEnvironment.myWindVel );
		environmentElement.appendChild( paraElement );
	}

	return environmentElement;
}

QVector<QDomElement> ByyScenario::serializeEntityObjects( QDomDocument& doc,ByyForceType forceType )
{
	ByyLocalObjectManager& myObjManager=myCore.localObjectManager();

	ByyLocalObjectManager::EntityObjectMap& entObjs=myObjManager.entityObjects();

	QMultiMap<int,ByyEntityObject*> filterOrderedEntObjs;


	{
		ByyLocalObjectManager::EntityObjectMap::iterator it=entObjs.begin();
		//É¸Ñ¡£¬ÅÅÐò
		for (;it!=entObjs.end();++it)
		{
			ByyEntityObject* entObj=*it;

			if (forceType!=ByyForceAll&&forceType!=entObj->forceType())
			{
				continue;
			}

			filterOrderedEntObjs.insert(entObj->level(),entObj);
		}
	}

	QVector<QDomElement> result;

	{
		QMultiMap<int,ByyEntityObject*>::iterator it=filterOrderedEntObjs.begin();

		for (;it!=filterOrderedEntObjs.end();++it)
		{
			ByyLocalEntity* entObj=dynamic_cast<ByyLocalEntity*>(*it);

			QDomElement itemElement = doc.createElement( "Item" );
			result.append(itemElement);

			itemElement.setAttribute( "ItemClass", entObj->itemClass() );
			itemElement.setAttribute( "ID", QString::number(/*entObj->id()*/-1) );
			itemElement.setAttribute( "Name", /*entObj->name()*/"" );

			ByyKeyValueList keyValues=entObj->getConfig();

			ParameterList& parameters=entObj->getParameters();

			ParameterList::iterator it=parameters.begin();

			for (;it!=parameters.end();++it)
			{
				if ((*it)->getValue().isEmpty())
				{
					continue;
				}

				QDomElement paramElement = doc.createElement( "Parameter" );
				itemElement.appendChild(paramElement);

				paramElement.setAttribute("Name",(*it)->Name);
				paramElement.setAttribute("Value",(*it)->getValue());
			}
		}
	}

	return result;
}

QVector<QDomElement> ByyScenario::serializeControlObjects( QDomDocument& doc,ByyForceType forceType/*=ByyForceAll*/ )
{
	ByyLocalObjectManager& myObjManager=myCore.localObjectManager();

	ByyLocalObjectManager::ControlObjectMap& ctrlObjs=myObjManager.controlObjects();

	ByyLocalObjectManager::ControlObjectMap::iterator it=ctrlObjs.begin();

	QVector<QDomElement> result;

	for (;it!=ctrlObjs.end();++it)
	{
		ByyControlObject* ctrlObj=*it;

		if (ctrlObj->itemClass().isEmpty())continue;

		QDomElement itemElement = doc.createElement( "Item" );
		result.append(itemElement);

		itemElement.setAttribute( "ItemClass", ctrlObj->itemClass() );
		itemElement.setAttribute( "ID", QString::number(/*entObj->id()*/0) );
		itemElement.setAttribute( "Name", /*entObj->name()*/"" );

		ByyKeyValueList keyValues=ctrlObj->getConfig();

		for (int i=0;i!=keyValues.size();++i)
		{
			QDomElement paramElement = doc.createElement( "Parameter" );
			itemElement.appendChild(paramElement);

			paramElement.setAttribute("Name",keyValues[i].first);
			paramElement.setAttribute("Value",keyValues[i].second);
		}

	}

	return result;
}

QVector<QDomElement> ByyScenario::serializeEntityTask( QDomDocument& doc )
{
	ByyLocalObjectManager& myObjManager=myCore.localObjectManager();

	ByyLocalObjectManager::EntityObjectMap& entOBjs=myObjManager.entityObjects();

	ByyLocalObjectManager::EntityObjectMap::iterator it=entOBjs.begin();

	QVector<QDomElement> result;

	for (;it!=entOBjs.end();++it)
	{
		ByyEntityObject* entObj=*it;

		ByyTaskManager& taskMgr=entObj->taskManager();

		ByyTaskManager::TaskList& taskList=taskMgr.taskList();

		for (int i=0;i!=taskList.size();++i)
		{
			QDomElement itemElement = doc.createElement( "Item" );

			itemElement.setAttribute( "ItemClass", "CSimTask");
			itemElement.setAttribute( "ID", -1);
			itemElement.setAttribute( "Name","");

			ByyKeyValueList keyValues=taskList[i]->getConfig();

			for (int i=0;i!=keyValues.size();++i)
			{
				QDomElement paramElement = doc.createElement( "Parameter" );
				itemElement.appendChild(paramElement);

				paramElement.setAttribute("Name",keyValues[i].first);
				paramElement.setAttribute("Value",keyValues[i].second);
			}

			result.push_back(itemElement);
		}

	}

	return result;
}

void ByyScenario::setFilename( const QString& filename )
{
	myFilename=filename;
}

bool ByyScenario::valid( const QString& filename )
{
	QFile file(filename);
	
	if (!file.open(QFile::ReadOnly))
		return false;

	QDomDocument doc("");

	bool result=doc.setContent(&file);

	file.close();
	return result;
}

bool ByyScenario::modified()
{
	return myModified;
}

void ByyScenario::setModified(bool modified)
{
	myModified=modified;
}

void ByyScenario::setSimulationRect( const ByyRectangle& rect )
{
	mySimulationRect.setPoints(rect.points());
	setModified();
}

const ByyRectangle& ByyScenario::simulationRect()
{
	return mySimulationRect;
}

void ByyScenario::setEnvironment( const Environment& e )
{
	if (myEnvironment!=e)
	{
		myEnvironment=e;
	}

	setModified();
}

const ByyScenario::Environment& ByyScenario::environment() const
{
	return myEnvironment;
}

void ByyScenario::readProfile( QDomElement& item )
{
	QDomNodeList paramters=item.elementsByTagName("Parameter");

	QDomElement paramter;
	QString _name,_value;

	mySimulationRect.clear();

	for (int i=0;i!=paramters.size();++i)
	{
		paramter=paramters.item(i).toElement();

		if (paramter.isNull())
			continue;

		_name=paramter.attribute("Name");
		_value=paramter.attribute("Value");

		if (_name=="OutLine")
		{
			myName=_value;

			QStringList valuesplit=paramter.attribute("Value").split(",");

			if (valuesplit.size()>1)
			{
				valuesplit.removeFirst();

				myName=valuesplit.join(",");
			}
		}
		else if (_name=="ScnStartTime")
		{
			myStartDateTime=QDateTime::fromString(_value,"yyyy,M,d,h,m,s");
		}
		else if (_name=="ScnEndTime")
		{
			myEndDateTime=QDateTime::fromString(_value,"yyyy,M,d,h,m,s");
		}
		else if (_name=="WayPoint")
		{
			double x,y,z;
			QTextStream(&_value)>>y>>QChar()>>x>>QChar()>>z;
			mySimulationRect.append(osg::Vec3d(x,y,z));
		}
	}
}

void ByyScenario::readEnvironment( QDomElement& item )
{
	myEnvironment.myRectangle.clear();

	QDomNodeList paramters=item.elementsByTagName("Parameter");

	QDomElement paramter;
	QString _name,_value;

	for (int i=0;i!=paramters.size();++i)
	{
		paramter=paramters.item(i).toElement();

		if (paramter.isNull())
			continue;

		_name=paramter.attribute("Name");
		_value=paramter.attribute("Value");

		if (_name=="SeaStateLevel")
		{
			myEnvironment.mySeaStateLevel=_value.toInt();
		}
		else if (_name=="WindCou")
		{
			myEnvironment.myWindCou=_value.toInt();
		}
		else if (_name=="WindVel")
		{
			myEnvironment.myWindVel=_value.toInt();
		}
		else if (_name=="WayPoint")
		{
			double x,y,z;
			QTextStream(&_value)>>y>>QChar()>>x>>QChar()>>z;
			myEnvironment.myRectangle.append(osg::Vec3d(x,y,z));
		}
	}
}
