#ifndef LOCALENTITY_H
#define LOCALENTITY_H

#include "entityObject.h"
#include "parameter.h"

class SNIGCORE_EXPORT ByyLocalEntity : public ByyEntityObject
{
	Q_OBJECT
public:
	ByyLocalEntity();
	~ByyLocalEntity();
	
	void setForceType( ByyForceType side );
	ByyForceType forceType();

	void setName( const QString& name );
	QString name();

	void setSuperiorName( const QString& superiorName );
	QString superiorName();

	void setPosition( const osg::Vec3d& pos );
	osg::Vec3d position();

	void setOrientation(const osg::Vec3d& ori);
	osg::Vec3d orientation();

	void setSpeed(double speed);
	double speed();

	void setVelocity(const osg::Vec3d& vel);
	osg::Vec3d velocity();

	void setLevel(int l);
	int level();

	void setParameters(ParameterList& p);
	ParameterList& getParameters();

	void fromConfig(const ByyKeyValueList& keyValues);

	bool isLocal()const;

	QString getAttributeValueSet();

signals:
	void nameChanged1(const QString&,const QString&);

protected:
	ParameterList myParameters;
};

#endif // LOCALENTITY_H
