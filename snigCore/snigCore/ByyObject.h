#ifndef OBJECT_H
#define OBJECT_H

#include <QObject>
#include <QString>
#include <QVector>
#include <osg\Vec3d>

#include "snigcore_global.h"
#include "enums.h"

#include "keyValueList.h"

typedef QVector<osg::Vec3d> ByyVec3dArray;

class SNIGCORE_EXPORT ByyObject : public QObject
{
	Q_OBJECT
public:
	ByyObject();
	virtual ~ByyObject();

	virtual void setId(int id);
	virtual int id()const;

	virtual unsigned int objectId()const;

	virtual void setForceType(ByyForceType side);
	virtual ByyForceType forceType();

	virtual void setName(const QString& name);
	virtual QString name();

	virtual void setLocal(bool b);
	virtual bool isLocal()const;

	virtual void setVisible(bool b);
	virtual bool visible()const;

	virtual void setItemClass(const QString& cls);
	virtual QString itemClass()const;

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(const ByyKeyValueList& keyValues);

	virtual QString getAttributeValueSet()=0;

	void setDriverName(const QString& n);
	QString driverName();

signals:
	void forceTypeChanged(ByyForceType);
	void nameChanged(const QString&);
	void visibleChanged(bool);
	void nameChangedWithOld(QString curname,QString oldname);

private:
	friend class ByyObjectIdManager;

	void setObjectId(unsigned int id);

protected:
	int				myId;
	unsigned int	myObjectId;
	ByyForceType	myForceType;
	QString			myName;
	bool			myVisible;

	QString			myItemClass;

	bool			myLocalObj;
	QString         myDriverName;
};

#endif // OBJECT_H
