#ifndef REMOTEENTITY_H
#define REMOTEENTITY_H

#include "entityObject.h"

class SNIGCORE_EXPORT ByyRemoteEntity : public ByyEntityObject
{
	Q_OBJECT
public:
	ByyRemoteEntity();
	virtual ~ByyRemoteEntity();

	void setSuperiorName(const QString& superiorName);
	QString superiorName();

	void setHealthPoint(double healthPoint);
	double getHealthPoint(void);

	void setPosition(const osg::Vec3d& pos);
	osg::Vec3d position();

	void setOrientation(const osg::Vec3d& ori);
	osg::Vec3d orientation();

	void setSpeed(double speed);
	double speed();

	void setVelocity(const osg::Vec3d& vel);
	osg::Vec3d velocity();

	void setLevel(int l);
	int level();

	bool isLocal()const;

	void addExtendEsr(ByyStateRepository* esr);
	ByyStateRepository* getExtendEsr(const QString& name);


	template<typename T>
	T* getSR()
	{
		ByyExtendEsrMap::iterator it=myExtendEsrMap.find(T::theType());

		if (it!=myExtendEsrMap.end())
		{
			T* s=dynamic_cast<T*>(it.value());

			if (s)
				return s;
		}

		return 0;
	}

	template<typename T>
	T* getOrCreateSR()
	{
		T* sr=getSR<T>();

		if (!sr)
		{
			sr=new T(this);
			addExtendEsr(sr);
		}

		return sr;
	}

protected:
	QString mySuperiorName;

	int myLevel;
	int m_healthPoint;

	osg::Vec3d myPosition;   // lon lat high
	osg::Vec3d myOrientation;//  hpr

	double mySpeed;
	osg::Vec3d myVelocity;

	ByyDamageState myDamageState;

	ByyEntityType myEntityType;	

	typedef QMap<QString,ByyStateRepository*> ByyExtendEsrMap;

	ByyExtendEsrMap myExtendEsrMap;
};

#endif // REMOTEENTITY_H
