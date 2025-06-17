#ifndef BYYQGIS2DAPDATER_H
#define BYYQGIS2DAPDATER_H

#include <QObject>

class ByyApp;
class XTest_GISWnd;
class ByyEntityObject;
class ByyControlObject;
class ByyDetectObject;
class ByyEntityJBUpdater;
class ByyDetectEntityJBUpdater;
class ByyControlObjectUpdater;

namespace XGIS
{
	class XShape;
	class XShapePointJB;
}

class ByyQGis2DApdater : public QObject
{
	Q_OBJECT

public:
	ByyQGis2DApdater(ByyApp& app,XTest_GISWnd* gisWnd,QObject *parent);
	~ByyQGis2DApdater();

public slots:
	void appendEntObject(ByyEntityObject* entObj);
	void removeEntObject(ByyEntityObject* entObj);

	void appendContrlObject(ByyControlObject* obj);
	void removeContrlObject(ByyControlObject* obj);

	//void setControlObjectVisible(bool visible);
	//void setEntityVisible(bool visible);

	void appendDetectObject(ByyDetectObject* obj);
	void removeDetectObject(ByyDetectObject* obj);

	void showRoleChanged(int rule);

private:
	ByyApp& myApp;
	XTest_GISWnd* myGisWnd;

	/*struct JBObject
	{
		ByyEntityJBUpdater* updater;
		XGIS::XShapePointJB* JB;
	};

	struct DetectJBObject
	{
		ByyDetectEntityJBUpdater* updater;
		XGIS::XShapePointJB* JB;
	};

	struct ShapeObject
	{
		ByyControlObjectUpdater* updater;
		XGIS::XShape* shape;
	};*/
	

	typedef QMap<ByyEntityObject*,ByyEntityJBUpdater*> EntityObjectList;
	EntityObjectList myEntityObjectList;

	typedef QMap<ByyDetectObject*,ByyDetectEntityJBUpdater*> DetectEntityObjectList;
	DetectEntityObjectList myDetectEntityObjectList;

	typedef QMap<ByyControlObject*,ByyControlObjectUpdater*> ControlObjectList;
	ControlObjectList myControlObjectList;
};


class ByyObjectUpdater : public QObject
{
	Q_OBJECT
public:
	ByyObjectUpdater(ByyObject* obj,XGIS::XShape* shape,ByyQGis2DApdater* apdater);
	virtual ~ByyObjectUpdater();

public slots:
	void updateVisible(bool v);
	virtual void updateName(const QString& name);
	virtual void updateforceType(ByyForceType ft)=0;
	

protected:
	ByyQGis2DApdater* myApdater;
	ByyObject* myObj;
	XGIS::XShape* myShape;
};

class ByyEntityJBUpdater : public ByyObjectUpdater
{
	Q_OBJECT
public:
	ByyEntityJBUpdater(ByyEntityObject* entObj,XGIS::XShapePointJB* JB,ByyQGis2DApdater* apdater);

	XGIS::XShapePointJB* jbObj(){return myJB;}

public slots:
	void updatePosition();
	void updateforceType(ByyForceType ft);
	void updateDamageState(ByyDamageState ds);

protected:
	ByyEntityObject* myEntObj;
	XGIS::XShapePointJB* myJB;
};

class ByyControlObjectUpdater : public ByyObjectUpdater
{
	Q_OBJECT
public:
	ByyControlObjectUpdater(ByyControlObject* obj,XGIS::XShape* shape,ByyQGis2DApdater* apdater);

	XGIS::XShape* shape(){return myShape;}

public slots:
	void updateforceType(ByyForceType ft);
	void rebuild();

protected:
	ByyControlObject* myObj;

};

class ByyDetectEntityJBUpdater : public ByyObjectUpdater
{
	Q_OBJECT
public:
	ByyDetectEntityJBUpdater(ByyDetectObject* entObj,XGIS::XShapePointJB* JB,ByyQGis2DApdater* apdater);

	XGIS::XShapePointJB* jbObj(){return myJB;}

public slots:
	void updatePosition();
	void updateforceType(ByyForceType ft);

protected:
	ByyDetectObject* myEntObj;
	XGIS::XShapePointJB* myJB;
};

#endif // BYYQGIS2DAPDATER_H
