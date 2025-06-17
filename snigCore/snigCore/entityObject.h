#ifndef ENTITYOBJECT_H
#define ENTITYOBJECT_H

#include <QList>
#include "ByyObject.h"
#include "entityType.h"
#include "articulatedPartCollection.h"

class ByyTaskManager;
class ByyParameter;
class ByyStateRepository;
class ByyDescribeEntry;

//查找目标类型
enum EntitySimpleObjectType {
	AirEntityObject  = 0x0001,		//空中目标实体
	ShipEntityObject = 0x0002,		//舰艇实体
	SubmarineObject  = 0x0004,		//潜艇实体
	MissileObject    = 0x0008,		//导弹实体
	TorpedoObject    = 0x0010,		//鱼类实体
	DepthChargeObject= 0x0020,		//深水炸弹
	LandcraftObject	 = 0x0040,		//陆地目标
	InvalidObject	 = 0x0080		//无效目标
};

//阵营方
#define RED_SIDE	QString::fromLocal8Bit("红方")
#define BLUE_SIDE	QString::fromLocal8Bit("蓝方")

class SNIGCORE_EXPORT ByyEntityObject : public ByyObject
{
	Q_OBJECT
public:
	ByyEntityObject();
	virtual ~ByyEntityObject();

	virtual void setSuperiorName(const QString& superiorName)=0;
	virtual QString superiorName()=0;

	virtual void setPosition(const osg::Vec3d& pos)=0;
	virtual osg::Vec3d position()=0;

	virtual void setOrientation(const osg::Vec3d& ori)=0;
	virtual osg::Vec3d orientation()=0;

	virtual void setSpeed(double speed)=0;
	virtual double speed()=0;

	virtual void setVelocity(const osg::Vec3d& vel)=0;
	virtual osg::Vec3d velocity()=0;

	virtual void setEntityType(const ByyEntityType& entType);
	virtual const ByyEntityType& entityType();

	virtual void setDamageState(ByyDamageState ds);
	virtual ByyDamageState damageState()const;
	virtual double getHealthPoint();

	virtual void setLevel(int l)=0;
	virtual int level()=0;

	virtual void setTargetName(const QString& targetName);
	QString getTargetName(void);

	virtual bool complete();

	ByyTaskManager& taskManager();

	virtual QString getAttributeValueSet();

	ByyArticulatedPartCollection* artPartList();

	virtual ByyStateRepository* getExtendEsr(const QString& name){return 0;}

	ByyDescribeEntry* describeEntry();

	void setDescribeEntry(const ByyDescribeEntry& descEntry);

signals:
	void positionChanged(const osg::Vec3d&);
	void orientationChanged(const osg::Vec3d&);
	void damageStateChanged(ByyDamageState);

protected:
	ByyEntityType myEntityType;	
	ByyDamageState myDamageState;
	QString		  myTargetName;
	//QString mySuperiorName;

	//int myLevel;

	//osg::Vec3d myPosition;   // lon lat high
	//osg::Vec3d myOrientation;//  hpr

	//double mySpeed;
	//osg::Vec3d myVelocity;

	//ByyDamageState myDamageState;

	//ByyEntityType myEntityType;

	//

	ByyTaskManager* myTaskManager;

	ByyDescribeEntry* myDescribeEntry;

	ByyArticulatedPartCollection myArtPartList;
};

SNIGCORE_EXPORT bool isRed(ByyEntityObject* entity);
SNIGCORE_EXPORT bool isBlue(ByyEntityObject* entity);
SNIGCORE_EXPORT bool isAircraft(ByyEntityObject* entity);
SNIGCORE_EXPORT bool isLandcraft(ByyEntityObject* entity);
SNIGCORE_EXPORT bool isShip(ByyEntityObject* entity);
SNIGCORE_EXPORT bool isSubmarine(ByyEntityObject* entity);
SNIGCORE_EXPORT bool isMissile(ByyEntityObject* entity);
SNIGCORE_EXPORT bool isTorpedo(ByyEntityObject* entity);
SNIGCORE_EXPORT bool isSameEntity(ByyEntityObject* entity1 ,ByyEntityObject* entity2);
SNIGCORE_EXPORT QString getPartyName(ByyEntityObject* entity);
SNIGCORE_EXPORT QString getAntiPartyName(ByyEntityObject* entity);
SNIGCORE_EXPORT QString getAntiPartyName(const QString& partyname);
SNIGCORE_EXPORT EntitySimpleObjectType typeJudgement(const ByyEntityType& typeInfo,double z);

#endif // ENTITYOBJECT_H
