#include "scenarioV2.h"
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
#include "ByyAttackArrow.h"
#include "ByyDoubleArrow.h"
#include "ByyFineArrow.h"
#include "ByyGatheringPlace.h"
#include "ByyStraightArrow.h"
#include "ByyArc.h"

#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <unordered_map>
#include <QStringList>
#include <QDebug>

ByyScenarioV2::ByyScenarioV2( ByyHdose& hdose )
	:myHdose(hdose)
	,myCore(hdose.core())
	,myModified(false)
{

	myScnInfo.myName="Untitled";
	myScnInfo.myStartDateTime=QDateTime(QDate(2023,1,1));
	myScnInfo.myEndDateTime=QDateTime(QDate(2023,1,2));

	myScnInfo.m_ScnRunN=1;
	myScnInfo.m_UseRandomSeed=0;
	myScnInfo.m_RandomSeed=0;
	myScnInfo.m_T=0;
	myScnInfo.m_SimNetworkComm=0;

	Environment environment;

	environment.mySeaStateLevel=2;
	environment.myWindCou=290;
	environment.myWindVel=15;
	environment.myName="default";

	myEnvironmentList.append(environment);
}

ByyScenarioV2::~ByyScenarioV2()
{

}

void ByyScenarioV2::setScnInfo( const ScenarioInfo& scnInfo )
{
	if (myScnInfo!=scnInfo)
	{
		myScnInfo=scnInfo;
		setModified();
	}
}

const ByyScenarioV2::ScenarioInfo& ByyScenarioV2::getScnInfo() const
{
	return myScnInfo;
}
/*

void ByyScenarioV2::setStartDateTime( const QDateTime& dt )
{
	if (myStartDateTime!=dt)
	{
		myStartDateTime=dt;
		setModified();
	}
}

const QDateTime& ByyScenarioV2::startDateTime() const
{
	return myStartDateTime;
}

void ByyScenarioV2::setEndDateTime( const QDateTime& dt )
{
	if (myEndDateTime!=dt)
	{
		myEndDateTime=dt;
		setModified();
	}
}

const QDateTime& ByyScenarioV2::endDateTime() const
{
	return myEndDateTime;
}
*/

QString ByyScenarioV2::filename() const
{
	return myFilename;
}

bool ByyScenarioV2::load( const QString &filename )
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

	QDomElement ScenarioElement =root.firstChildElement("Scenario");

	if (ScenarioElement.isNull())
		return false;

	myEnvironmentList.clear();

	QDomElement TimeElement=ScenarioElement.firstChildElement("Time");

	QDomNodeList items=TimeElement.elementsByTagName("Item");

	for (int i=0;i!=items.size();++i)
	{
		QDomElement item=items.item(i).toElement();

		if (item.isNull())
			continue;

		QString itemClass=item.attribute("ItemClass");

		if (itemClass=="JSScnProfile")
		{
			readProfile(item);
		}
		else if (itemClass=="JSEnvironment" || itemClass=="JSEnvironmentGV")
		{
			myEnvironmentList.append(readEnvironment(item));
		}
		else if (itemClass=="JSFormation")
		{
			myFormationList.append(readFormation(item));
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

			if (itemClass=="JSWayPoint")
			{
				ctrlObj=new ByyWaypoint;
			}
			else if (itemClass=="JSCircle")
			{
				ctrlObj=new ByyCircle;
			}
			else if (itemClass=="JSEllipse")
			{
				ctrlObj=new ByyEllipse;
			}
			else if (itemClass=="JSPhaseline")
			{
				ctrlObj=new ByyPhaseLine;
			}
			else if (itemClass=="JSPolygon")
			{
				ctrlObj=new ByyPolygon;
			}
			else if (itemClass=="JSRectangle")
			{
				ctrlObj=new ByyRectangle;
			}
			else if (itemClass=="JSRoute")
			{
				ctrlObj=new ByyRoute;
			}
			else if(itemClass=="CArc")
			{
				ctrlObj=new ByyArc;
			}
			else if(itemClass=="ByyAttackArrow")
			{
				ctrlObj=new ByyAttackArrow;
			}
			else if(itemClass=="ByyDoubleArrow")
			{
				ctrlObj=new ByyDoubleArrow;
			}
			else if(itemClass=="ByyFineArrow")
			{
				ctrlObj=new ByyFineArrow;
			}
			else if(itemClass=="ByyGatheringPlace")
			{
				ctrlObj=new ByyGatheringPlace;
			}
			else if(itemClass=="ByyStraightArrow")
			{
				ctrlObj=new ByyStraightArrow;
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

	myCore.localObjectManager().updateNameHashAfterScnLoaded();

	myModified=false;

	myFilename=filename;

	return true;
}

bool ByyScenarioV2::save()
{
	QDomDocument doc("");

	QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"GB2312\"");

	doc.appendChild(instruction);

	QDomElement root = doc.createElement("Simulation");
	doc.appendChild(root);

	QDomElement ScenarioElement = doc.createElement("Scenario");
	root.appendChild(ScenarioElement);

	QDomElement TimeElement = doc.createElement("Time");
	TimeElement.setAttribute("Value","0");

	ScenarioElement.appendChild(TimeElement);

	//TimeElement.appendChild(doc.createComment(""));//空白注释

	TimeElement.appendChild(serializeProfile(doc));

	TimeElement.appendChild(doc.createComment(""));//空白注释

	TimeElement.appendChild(doc.createComment(QString::fromLocal8Bit("环境信息")));

	for (int i=0;i!=myEnvironmentList.size();++i)
	{
		TimeElement.appendChild(serializeEnvironment(myEnvironmentList[i],doc));
	}

	std::unordered_map<ByyForceType,QString> entityForceTypes;
	entityForceTypes[ByyForceFriendly]=QString::fromLocal8Bit("红方初始态势");
	entityForceTypes[ByyForceOpposing]=QString::fromLocal8Bit("蓝方初始态势");
	entityForceTypes[ByyForceNeutral]=QString::fromLocal8Bit("中立初始态势");
	entityForceTypes[ByyForceOther]=QString::fromLocal8Bit("其他初始态势");

	std::unordered_map<ByyForceType,QString>::iterator it=entityForceTypes.begin();

	for (;it!=entityForceTypes.end();++it)
	{
		TimeElement.appendChild(doc.createComment(""));//空白注释
		TimeElement.appendChild(doc.createComment(""));//空白注释
		TimeElement.appendChild(doc.createComment(""));//空白注释
		TimeElement.appendChild(doc.createComment(it->second));

		QVector<QDomElement> entElements=serializeEntityObjects(doc,it->first);

		foreach(const QDomElement& element,entElements)
		{
			TimeElement.appendChild(element);
		}
	}

	TimeElement.appendChild(doc.createComment(""));//空白注释
	TimeElement.appendChild(doc.createComment(""));//空白注释
	TimeElement.appendChild(doc.createComment(""));//空白注释
	TimeElement.appendChild(doc.createComment(QString::fromLocal8Bit("控制对象信息")));

	QVector<QDomElement> ctrlObjElements=serializeControlObjects(doc);

	foreach(const QDomElement& element,ctrlObjElements)
	{
		TimeElement.appendChild(element);
	}

	TimeElement.appendChild(doc.createComment(""));//空白注释
	TimeElement.appendChild(doc.createComment(""));//空白注释
	TimeElement.appendChild(doc.createComment(""));//空白注释
	TimeElement.appendChild(doc.createComment(QString::fromLocal8Bit("任务信息")));

	QVector<QDomElement> taskElements=serializeEntityTask(doc);

	foreach(const QDomElement& element,taskElements)
	{
		TimeElement.appendChild(element);
	}

	//编队信息
	TimeElement.appendChild(doc.createComment(""));//空白注释
	TimeElement.appendChild(doc.createComment(""));//空白注释
	TimeElement.appendChild(doc.createComment(""));//空白注释
	TimeElement.appendChild(doc.createComment(QString::fromLocal8Bit("编队信息")));
	QVector<QDomElement> foramtionElements=serializeFormation(doc);
	foreach(const QDomElement& element,foramtionElements)
	{
		TimeElement.appendChild(element);
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

QDomElement ByyScenarioV2::serializeProfile( QDomDocument& doc )
{
	QDomElement scnProfile = doc.createElement( "Item" );
	scnProfile.setAttribute( "ItemClass", "JSScnProfile" );
	scnProfile.setAttribute( "ID", "0" );
	scnProfile.setAttribute( "Name", "" );

	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "ScnName" );
		paraElement.setAttribute( "Value", myScnInfo.myName );
		scnProfile.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "OutLine" );
		paraElement.setAttribute( "Value", myScnInfo.myOutline );
		scnProfile.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "ScnStartTime" );
		paraElement.setAttribute( "Value",  myScnInfo.myStartDateTime.toString("yyyy,M,d,h,m,s") );
		scnProfile.appendChild( paraElement );
	}

	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "ScnEndTime" );
		paraElement.setAttribute( "Value", myScnInfo.myEndDateTime.toString("yyyy,M,d,h,m,s") );
		scnProfile.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "UseRandomSeed" );
		paraElement.setAttribute( "Value", myScnInfo.m_UseRandomSeed );
		scnProfile.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "RandomSeed" );
		paraElement.setAttribute( "Value", myScnInfo.m_RandomSeed );
		scnProfile.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "T" );
		paraElement.setAttribute( "Value", myScnInfo.m_T );
		scnProfile.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "SimNetworkComm" );
		paraElement.setAttribute( "Value", myScnInfo.m_SimNetworkComm );
		scnProfile.appendChild( paraElement );
	}
	//仿真区域:矩形区域
	{
		scnProfile.appendChild(doc.createComment(""));//空白注释

		QDomComment comment=doc.createComment(QString::fromLocal8Bit("仿真区域:矩形区域"));

		scnProfile.appendChild(comment);

		const QVector<osg::Vec3d>& points=myScnInfo.mySimulationRect.points();

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

QDomElement ByyScenarioV2::serializeEnvironment(Environment& environment, QDomDocument& doc )
{
	QDomElement environmentElement = doc.createElement( "Item" );
	environmentElement.setAttribute( "ItemClass", "JSEnvironment" );
	environmentElement.setAttribute( "ID", "0" );
	environmentElement.setAttribute( "Name", environment.myName );

	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "EnvObjName" );
		paraElement.setAttribute( "Value", "Env_All");
		environmentElement.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "Name" );
		paraElement.setAttribute( "Value", environment.myName);
		environmentElement.appendChild( paraElement );
	}
	const QVector<osg::Vec3d>& points=environment.myRectangle.points();

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
		paraElement.setAttribute( "Value", environment.mySeaStateLevel);
		environmentElement.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "WindCou" );
		paraElement.setAttribute( "Value", environment.myWindCou );
		environmentElement.appendChild( paraElement );
	}
	{
		QDomElement paraElement = doc.createElement( "Parameter" );
		paraElement.setAttribute( "Name", "WindVel" );
		paraElement.setAttribute( "Value", environment.myWindVel );
		environmentElement.appendChild( paraElement );
	}

	return environmentElement;
}

QVector<QDomElement> ByyScenarioV2::serializeEntityObjects( QDomDocument& doc,ByyForceType forceType )
{
	ByyLocalObjectManager& myObjManager=myCore.localObjectManager();

	ByyLocalObjectManager::EntityObjectMap& entObjs=myObjManager.entityObjects();

	QMultiMap<int,ByyEntityObject*> filterOrderedEntObjs;


	{
		ByyLocalObjectManager::EntityObjectMap::iterator it=entObjs.begin();
		//筛选，排序
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

				//任务转化为行为树字符串
				QString Name_=(*it)->Name;
				QString Value_=(*it)->getValue();
				qDebug()<<__LINE__<<__FUNCTION__<<Name_<<":"<<Value_;
				if (Name_ == "Plan")
				{
					ByyTaskManager& taskMgr=entObj->taskManager();
					ByyTaskManager::TaskList& taskList=taskMgr.taskList();
					int task_size = taskList.size();
					QString taskStrings;
					for (int i=0;i!=task_size;++i)
					{
						ByyKeyValueList keyValues=taskList[i]->getConfig();
						//for (int i=0;i!=keyValues.size();++i)
						//{
						//qDebug()<<__LINE__<<__FUNCTION__<<keyValues[i].first<<":"<<keyValues[i].second;
						//}
						//判断任务类型
						QString m_TaskType;
						keyValues.get("TaskType",m_TaskType);
						QString m_Executant;
						keyValues.get("Executant",m_Executant);
						QString m_TaskName;
						keyValues.get("TaskName",m_TaskName);
						QString m_StartTime;
						keyValues.get("StartTime",m_StartTime);
						QString m_DeviceName;
						keyValues.get("DeviceName",m_DeviceName);
						if (m_TaskType == "SensorOper")
						{
							QString m_PowerOn;
							keyValues.get("PowerOn",m_PowerOn);
							//QString taskString = QString("BTNTrigger(BTCTime($gt;,%1,SIMTIME),Finish,Finish,-1){BTNTask(CEquipmentOperTsk,%2,%3,1,1,%4,,,,,,,)}")
							//	.arg(m_StartTime,m_TaskName,m_DeviceName,m_PowerOn);
							QString taskString = QString("BTNTask(CEquipmentOperTsk,%1,%2,1,1,%3,,,,,,,)")
								.arg(m_TaskName,m_DeviceName,m_PowerOn);

							QString bTNTrigger = QString("BTNTrigger(BTCTime($gt;,%1,SIMTIME),Finish,Finish,1){ %2 }").arg(m_StartTime.toDouble()).arg(taskString);
							QString taskString_this = QString("BTNCompose(And,SeqAll){ %1 }").arg(bTNTrigger);

							taskStrings += taskString_this;
						} 
						else if (m_TaskType == "MoveToPoint")
						{
							QString m_CtrlObjName;
							keyValues.get("CtrlObjName",m_CtrlObjName);
							//获取点信息
							ByyWaypoint* point = (ByyWaypoint*)myCore.localObjectManager().findEntity_ctrlobj(m_CtrlObjName);
							if (point)
							{
								/*点或路线，格式：lat, lon, alt(m),speed(m/s),bcross;...;lat, lon, alt(m),speed(m/s),bcross;*/
								QString p = QString("%1/%2/%3/0/0;").arg(point->position().y()).arg(point->position().x()).arg(point->position().z());
								QString taskString = QString("BTNTask(CMoveAlongTsk,%1,3,1,,0,%2,1,0,0)")
									.arg(m_TaskName,p);

								QString bTNTrigger = QString("BTNTrigger(BTCTime($gt;,%1,SIMTIME),Finish,Finish,1){ %2 }").arg(m_StartTime.toDouble()).arg(taskString);
								QString taskString_this = QString("BTNCompose(And,SeqAll){ %1 }").arg(bTNTrigger);

								taskStrings += taskString_this;
							}
						}
						else if (m_TaskType == "MoveAlongRoute")
						{
							QString m_CtrlObjName;
							keyValues.get("CtrlObjName",m_CtrlObjName);
							QString taskString = QString("BTNTask(CMoveAlongTsk,%1,1,0,%2,0,,1,0,0)")
								.arg(m_TaskName,m_CtrlObjName);

							QString bTNTrigger = QString("BTNTrigger(BTCTime($gt;,%1,SIMTIME),Finish,Finish,1){ %2 }").arg(m_StartTime.toDouble()).arg(taskString);
							QString taskString_this = QString("BTNCompose(And,SeqAll){ %1 }").arg(bTNTrigger);

							taskStrings += taskString_this;
						}
						else if (m_TaskType == "PatrolAlongRoute")
						{
							QString m_CtrlObjName;
							keyValues.get("CtrlObjName",m_CtrlObjName);
							QString taskString = QString("BTNTask(CPatrolTsk,%1,2,1,,,0,0,0,%2,,3600,0,0)")
								.arg(m_TaskName,m_CtrlObjName);

							QString bTNTrigger = QString("BTNTrigger(BTCTime($gt;,%1,SIMTIME),Finish,Finish,1){ %2 }").arg(m_StartTime.toDouble()).arg(taskString);
							QString taskString_this = QString("BTNCompose(And,SeqAll){ %1 }").arg(bTNTrigger);

							taskStrings += taskString_this;
						}
						else if (m_TaskType == "PatrolInZone")
						{
							QString m_CtrlObjName;
							keyValues.get("CtrlObjName",m_CtrlObjName);
							QString taskString = QString("BTNTask(CPatrolTsk,%1,3,1,,,0,0,0,%2,,3600,0,0)")
								.arg(m_TaskName,m_CtrlObjName);

							QString bTNTrigger = QString("BTNTrigger(BTCTime($gt;,%1,SIMTIME),Finish,Finish,1){ %2 }").arg(m_StartTime.toDouble()).arg(taskString);
							QString taskString_this = QString("BTNCompose(And,SeqAll){ %1 }").arg(bTNTrigger);

							taskStrings += taskString_this;
						}
						else if (m_TaskType == "TakeOff")
						{
							QString m_TakeOff;
							keyValues.get("TakeOff",m_TakeOff);
							QString taskString = QString("BTNTask(CTakeOffTsk,%1,%2)")
								.arg(m_TaskName,m_TakeOff);

							QString bTNTrigger = QString("BTNTrigger(BTCTime($gt;,%1,SIMTIME),Finish,Finish,1){ %2 }").arg(m_StartTime.toDouble()).arg(taskString);
							QString taskString_this = QString("BTNCompose(And,SeqAll){ %1 }").arg(bTNTrigger);

							taskStrings += taskString_this;
						}
						else if (m_TaskType == "ReturnBase")
						{
							QString m_LandingDesLocType;
							keyValues.get("LandingDesLocType",m_LandingDesLocType);
							QString m_Location;
							keyValues.get("Location",m_Location);
							QString m_Landing;
							keyValues.get("Landing",m_Landing);
							QString m_RunWayHead;
							keyValues.get("RunWayHead",m_RunWayHead);
							QString taskString = QString("BTNTask(CReturnBaseTsk,%1,%2,%3,%4,%5,%6,%7,%8,1)")
								.arg(m_TaskName).arg(m_LandingDesLocType).arg("").arg("").arg("")
								.arg(m_Location.replace(",","/")).arg(m_Landing).arg(m_RunWayHead);

							QString bTNTrigger = QString("BTNTrigger(BTCTime($gt;,%1,SIMTIME),Finish,Finish,1){ %2 }").arg(m_StartTime.toDouble()).arg(taskString);
							QString taskString_this = QString("BTNCompose(And,SeqAll){ %1 }").arg(bTNTrigger);

							taskStrings += taskString_this;
						}
						else if (m_TaskType == "LaunchMissile")
						{
							QString m_TargetAssignType;
							keyValues.get("TargetAssignType",m_TargetAssignType);
							QString m_TargetObjectName;
							keyValues.get("TargetObjectName",m_TargetObjectName);
							QString m_TargetPosition;
							keyValues.get("TargetPosition",m_TargetPosition);
							//BTNTask(CAttackAutoTsk,TaskName,1,|ArleighBurkeFlightIIA 2|1|3;)
							QString taskString = QString("BTNTask(CAttackAutoTsk,%1,1,|%2|1|3;)")
								.arg(m_TaskName).arg(m_TargetObjectName);

							QString bTNTrigger = QString("BTNTrigger(BTCTime($gt;,%1,SIMTIME),Finish,Finish,1){ %2 }").arg(m_StartTime.toDouble()).arg(taskString);
							QString taskString_this = QString("BTNCompose(And,SeqAll){ %1 }").arg(bTNTrigger);

							taskStrings += taskString_this;
						}
						//拼接字符串
					}

					/*
					//添加流程行为树脚本Start
					for(int index = 0; index < m_processList.size(); ++index)
					{
						ByyParameter* dd = parameters.findParameter("EntityName");
						QString entityName = dd->getValue();
						if(strcmp(entityName.toStdString().c_str(), m_processList.at(index).m_entityName.toStdString().c_str()) != 0)
							continue;

						QString mesg = QString("%1;%2").arg(m_processList.at(index).m_taskName).arg(m_processList.at(index).m_cmdName);
						double processTime_sec = QTime(m_processList.at(index).m_startHour, m_processList.at(index).m_startMin,  m_processList.at(index).m_startSec)
							.secsTo(QTime(m_processList.at(index).m_endHour, m_processList.at(index).m_endMin,  m_processList.at(index).m_endSec));
						QString taskString = QString("BTNTask(CGenericFlowOrderTsk,%1,1,1,%2,%3,%4,%5,3,%6,,,)").arg("Task")
							.arg(mesg).arg(processTime_sec).arg("5")
							.arg((m_processList.at(index).m_targetName == "None")? 0 : 1)
							.arg(m_processList.at(index).m_targetName);

						double startTime_sec = QTime(0, 0, 0)
							.secsTo(QTime(m_processList.at(index).m_startHour, m_processList.at(index).m_startMin,  m_processList.at(index).m_startSec));
						QString bTNTrigger = QString("BTNTrigger(BTCTime($gt;,%1,SIMTIME),Finish,Finish,1){ %2 }").arg(startTime_sec).arg(taskString);
						QString taskString_this = QString("BTNCompose(And,SeqAll){ %1 }").arg(bTNTrigger);

						taskStrings += taskString_this;
					}
					//添加流程行为树脚本End
					*/

					if(task_size > 0 /*|| m_processList.size() > 0*/)
					{
						QString allTask = QString("BTNCompose(And,Pararell){ %1 }").arg(taskStrings);
						(*it)->setValue(allTask);
					}
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

QVector<QDomElement> ByyScenarioV2::serializeControlObjects( QDomDocument& doc,ByyForceType forceType/*=ByyForceAll*/ )
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
			if(keyValues[i].first == "WayPoint_Temp" || keyValues[i].first == "speed" || keyValues[i].first == "bcross" || 
				keyValues[i].first == "waittime" || keyValues[i].first == "btimectrl" || keyValues[i].first == "arrivetime" || 
				keyValues[i].first == "iround" || keyValues[i].first == "rsurround" ||
				keyValues[i].first == "Point_Temp" || keyValues[i].first == "CenterPos_Temp")
				continue;

			QDomElement paramElement = doc.createElement( "Parameter" );
			itemElement.appendChild(paramElement);

			paramElement.setAttribute("Name",keyValues[i].first);
			paramElement.setAttribute("Value",keyValues[i].second);
		}

	}

	return result;
}

QVector<QDomElement> ByyScenarioV2::serializeEntityTask( QDomDocument& doc )
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

QVector<QDomElement> ByyScenarioV2::serializeFormation(QDomDocument& doc)
{
	QVector<QDomElement> result;
	for (unsigned int i=0;i<myFormationList.size();i++)
	{
		Formation formation = myFormationList.at(i);
		QDomElement itemElement = doc.createElement( "Item" );
		itemElement.setAttribute( "ItemClass", "JSFormation");
		itemElement.setAttribute( "ID", -1);
		itemElement.setAttribute( "Name","");
		//编队名称
		{
			QDomElement paramElement = doc.createElement( "Parameter" );
			itemElement.appendChild(paramElement);
			paramElement.setAttribute("Name","FormationName");
			paramElement.setAttribute("Value",formation.m_FormationName);
		}
		//领队名称
		{
			QDomElement paramElement = doc.createElement( "Parameter" );
			itemElement.appendChild(paramElement);
			paramElement.setAttribute("Name","LeaderName");
			paramElement.setAttribute("Value",formation.m_LeaderName);
		}
		//编队成员数量
		{
			QDomElement paramElement = doc.createElement( "Parameter" );
			itemElement.appendChild(paramElement);
			paramElement.setAttribute("Name","MemberCount");
			paramElement.setAttribute("Value",formation.m_MemberCount);
		}
		//编队成员位置信息，分号隔开
		{
			QDomElement paramElement = doc.createElement( "Parameter" );
			itemElement.appendChild(paramElement);
			paramElement.setAttribute("Name","FormationPara");
			QString formationPara;
			for (unsigned int j=0;j<formation.m_FormationPara.size();j++)
			{
				FormationPara para = formation.m_FormationPara.at(j);
				QString text = QString("%1,%2,%3,%4,%5;").arg(para.m_MemberName,QString::number(para.m_index),QString::number(para.m_x),QString::number(para.m_y),QString::number(para.m_z));
				formationPara += text;
			}
			paramElement.setAttribute("Value",formationPara);
		}

		result.push_back(itemElement);
	}

	return result;
}

void ByyScenarioV2::setFilename( const QString& filename )
{
	myFilename=filename;
}

bool ByyScenarioV2::valid( const QString& filename )
{
	QFile file(filename);
	
	if (!file.open(QFile::ReadOnly))
		return false;

	QDomDocument doc("");

	bool result=doc.setContent(&file);

	file.close();
	return result;
}

bool ByyScenarioV2::modified()
{
	return myModified;
}

void ByyScenarioV2::setModified(bool modified)
{
	myModified=modified;
}
/*

void ByyScenarioV2::setSimulationRect( const ByyRectangle& rect )
{
	mySimulationRect.setPoints(rect.points());
	setModified();
}

const ByyRectangle& ByyScenarioV2::simulationRect()
{
	return mySimulationRect;
}*/

void ByyScenarioV2::setEnvironmentList( const EnvironmentList& e )
{
	if (myEnvironmentList!=e)
	{
		myEnvironmentList=e;
	}

	setModified();
}

const ByyScenarioV2::EnvironmentList& ByyScenarioV2::environmentList() const
{
	return myEnvironmentList;
}

void ByyScenarioV2::setFormationList(const FormationList& e)
{
	if (myFormationList!=e)
	{
		myFormationList=e;
	}
	setModified();
}

const ByyScenarioV2::FormationList& ByyScenarioV2::formationList() const
{
	return myFormationList;
}

void ByyScenarioV2::readProfile( QDomElement& item )
{
	QDomNodeList paramters=item.elementsByTagName("Parameter");

	QDomElement paramter;
	QString _name,_value;

	myScnInfo.mySimulationRect.clear();

	for (int i=0;i!=paramters.size();++i)
	{
		paramter=paramters.item(i).toElement();

		if (paramter.isNull())
			continue;

		_name=paramter.attribute("Name");
		_value=paramter.attribute("Value");

		if (_name=="ScnName")
		{
			myScnInfo.myName=_value;
		}
		else if (_name=="OutLine")
		{
			myScnInfo.myOutline=_value;
		}
		else if (_name=="ScnStartTime")
		{
			myScnInfo.myStartDateTime=QDateTime::fromString(_value,"yyyy,M,d,h,m,s");
		}
		else if (_name=="ScnEndTime")
		{
			myScnInfo.myEndDateTime=QDateTime::fromString(_value,"yyyy,M,d,h,m,s");
		}
		else if (_name=="UseRandomSeed")
		{
			myScnInfo.m_UseRandomSeed=_value.toLong();
		}
		else if (_name=="RandomSeed")
		{
			myScnInfo.m_RandomSeed=_value.toLong();
		}
		else if (_name=="T")
		{
			myScnInfo.m_T=_value.toDouble();
		}
		else if (_name=="SimNetworkComm")
		{
			myScnInfo.m_SimNetworkComm=_value.toLong();
		}
		else if (_name=="WayPoint")
		{
			double x,y,z;
			QTextStream(&_value)>>y>>QChar()>>x>>QChar()>>z;
			myScnInfo.mySimulationRect.append(osg::Vec3d(x,y,z));
		}
	}
}

ByyScenarioV2::Environment ByyScenarioV2::readEnvironment( QDomElement& item )
{
	Environment environment;

	environment.myName=item.attribute("Name");

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
			environment.mySeaStateLevel=_value.toInt();
		}
		else if (_name=="WindCou")
		{
			environment.myWindCou=_value.toInt();
		}
		else if (_name=="WindVel")
		{
			environment.myWindVel=_value.toInt();
		}
		else if (/*_name=="Name" || */_name=="EnvObjName")
		{
			environment.myName=_value;
		}
		else if (_name=="WayPoint")
		{
			double x,y,z;
			QTextStream(&_value)>>y>>QChar()>>x>>QChar()>>z;
			environment.myRectangle.append(osg::Vec3d(x,y,z));
		}
	}

	return environment;
}

ByyScenarioV2::Formation ByyScenarioV2::readFormation(QDomElement& item)
{
	Formation formation;

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

		if (_name=="FormationName")
		{
			formation.m_FormationName=_value;
		}
		else if (_name=="LeaderName")
		{
			formation.m_LeaderName=_value;
		}
		else if (_name=="MemberCount")
		{
			formation.m_MemberCount=_value.toInt();
		}
		else if (_name=="FormationPara")
		{
			QString text = _value;
			QStringList members = text.split(";");
			for (unsigned int i=0;i<members.size();i++)
			{
				QString para = members.at(i);
				if(para.isEmpty()) continue;
				QStringList paras = para.split(",");
				if (paras.size()>=5)
				{
					FormationPara ftPara;
					ftPara.m_MemberName = paras.at(0);
					ftPara.m_index = paras.at(1).toInt();
					ftPara.m_x = paras.at(2).toDouble();
					ftPara.m_y = paras.at(3).toDouble();
					ftPara.m_z = paras.at(4).toDouble();
					formation.m_FormationPara.append(ftPara);
				}
			}
		}
	}

	return formation;
}
