#include "localEntity.h"
#include "coreUtils.h"

#include <QStringList>

ByyLocalEntity::ByyLocalEntity()
{

}

ByyLocalEntity::~ByyLocalEntity()
{

}

void ByyLocalEntity::setForceType( ByyForceType side )
{
	myParameters.setValue("Side",QString::number(side));
	__super::setForceType(side);
}

ByyForceType ByyLocalEntity::forceType()
{
	ByyParameter* p=myParameters.findParameter("Side");

	if (p)return (ByyForceType)p->getValue().toInt();
	else return ByyForceOther;
}

void ByyLocalEntity::setName( const QString& name )
{
	myParameters.setValue("EntityName",name);
	__super::setName(name);
}

QString ByyLocalEntity::name()
{
	ByyParameter* p=myParameters.findParameter("EntityName");

	if (p)return p->getValue();
	else return QString();
}

void ByyLocalEntity::setSuperiorName( const QString& superiorName )
{
	myParameters.setValue("Superior",superiorName);
}

QString ByyLocalEntity::superiorName()
{
	ByyParameter* p=myParameters.findParameter("Superior");

	if (p)return p->getValue();
	else return QString();
}

void ByyLocalEntity::setPosition( const osg::Vec3d& pos )
{
	if (pos!=position())
	{
		osg::Vec3d p(pos.y(),pos.x(),pos.z());

		myParameters.setValue("Position",vec3d2Str(p));

		emit positionChanged(pos);
	}
}

osg::Vec3d ByyLocalEntity::position()
{
	ByyParameter* p=myParameters.findParameter("Position");

	if (p)
	{
		osg::Vec3d rel=str2Vec3d(p->getValue());
		return osg::Vec3d(rel.y(),rel.x(),rel.z());
	}
	else return osg::Vec3d();
}

void ByyLocalEntity::setOrientation( const osg::Vec3d& ori )
{
	if (ori!=orientation())
	{
		myParameters.setValue("Heading",QString::number(ori.x()));

		emit orientationChanged(ori);
	}
}

osg::Vec3d ByyLocalEntity::orientation()
{
	ByyParameter* p=myParameters.findParameter("Heading");

	if (p)return osg::Vec3d(p->getValue().toDouble(),0,0);
	else return osg::Vec3d();
}

void ByyLocalEntity::setSpeed( double speed )
{
	myParameters.setValue("Velocity",QString::number(speed));
}

double ByyLocalEntity::speed()
{
	ByyParameter* p=myParameters.findParameter("Velocity");

	if (p)return p->getValue().toDouble();
	else return 0;
}

void ByyLocalEntity::setVelocity( const osg::Vec3d& vel )
{

}

osg::Vec3d ByyLocalEntity::velocity()
{
	return osg::Vec3d();
}

void ByyLocalEntity::setLevel( int l )
{
	myParameters.setValue("Level",QString::number(l));
}

int ByyLocalEntity::level()
{
	ByyParameter* p=myParameters.findParameter("Level");

	if (p)return p->getValue().toInt();
	else return 0;
}

void ByyLocalEntity::setParameters( ParameterList& p )
{
	bool _nameChanged=false;
	bool _posChanged=false;
	bool _oriChanged=false;
	bool _sideChanged=false;

	ByyParameter* newName=p.findParameter("EntityName");
	ByyParameter* nowName=myParameters.findParameter("EntityName");

	_nameChanged=!newName||!nowName||newName->getValue()!=nowName->getValue();

	QString oldNameStr,newNameStr;

	if (_nameChanged&&newName&&nowName)
	{
		oldNameStr=nowName->getValue();
		newNameStr=newName->getValue();

		emit nameChanged1(oldNameStr,newNameStr);
	}


	ByyParameter* newPos=p.findParameter("Position");
	ByyParameter* nowPos=myParameters.findParameter("Position");

	_posChanged=!newPos||!nowPos||newPos->getValue()!=nowPos->getValue();

	ByyParameter* newOri=p.findParameter("Heading");
	ByyParameter* nowOri=myParameters.findParameter("Heading");

	_oriChanged=!newOri||!nowOri||newOri->getValue()!=nowOri->getValue();

	ByyParameter* newSide=p.findParameter("Side");
	ByyParameter* nowSide=myParameters.findParameter("Side");

	_sideChanged=!newSide||!nowSide||newSide->getValue()!=nowSide->getValue();

	myParameters=p;

	if (_nameChanged)
	{
		emit nameChanged(name());
	}

	if (_posChanged)
		emit positionChanged(position());

	if (_oriChanged)
		emit orientationChanged(orientation());

	if (_sideChanged)
		emit forceTypeChanged(forceType());
}

ParameterList& ByyLocalEntity::getParameters()
{
	return myParameters;
}

void ByyLocalEntity::fromConfig( const ByyKeyValueList& keyValues )
{
	__super::fromConfig(keyValues);

	ParameterList::iterator it;

	for (int i=0;i!=keyValues.size();++i)
	{
		it=myParameters.begin();

		for (;it!=myParameters.end();++it)
		{
			if (keyValues.at(i).first==(*it)->Name)
			{
				(*it)->setValue(keyValues.at(i).second);

				break;
			}
		}
	}
}

bool ByyLocalEntity::isLocal() const
{
	return true;
}

QString ByyLocalEntity::getAttributeValueSet()
{
	QStringList attList;

	attList.push_back(QString::number(forceType()));
	attList.push_back(name());

	{
		QString Marking;

		ByyParameter* p=myParameters.findParameter("Marking");
		if (p)Marking= p->getValue();

		attList.push_back(Marking);
	}

	attList.push_back(QString::number(level()));
	attList.push_back(superiorName());
	attList.push_back(vec3d2Str(swapXY(position())));
	attList.push_back(QString::number(speed()));
	attList.push_back(QString::number(orientation().x()));

	{
		QString ReportToSuperior;

		ByyParameter* p=myParameters.findParameter("ReportToSuperior");
		if (p)ReportToSuperior= p->getValue();

		attList.push_back(ReportToSuperior);
	}

	{
		QString InformToSubordinates;

		ByyParameter* p=myParameters.findParameter("InformToSubordinates");
		if (p)InformToSubordinates= p->getValue();

		attList.push_back(InformToSubordinates);
	}

	{
		QString InformationReleaseCycle;

		ByyParameter* p=myParameters.findParameter("InformationReleaseCycle");
		if (p)InformationReleaseCycle= p->getValue();

		attList.push_back(InformationReleaseCycle);
	}

	{
		QString Embarkation;

		ByyParameter* p=myParameters.findParameter("Embarkation");
		if (p)Embarkation= p->getValue();

		attList.push_back(Embarkation);
	}

	{
		QString EmbarkedEntityName;

		ByyParameter* p=myParameters.findParameter("EmbarkedEntityName");
		if (p)EmbarkedEntityName= p->getValue();

		attList.push_back(EmbarkedEntityName);
	}

	{
		QString JoinCommNetwork;

		ByyParameter* p=myParameters.findParameter("JoinCommNetwork");
		if (p)JoinCommNetwork= p->getValue();

		attList.push_back(JoinCommNetwork);
	}

	{
		QString CommNetworkName;

		ByyParameter* p=myParameters.findParameter("CommNetworkName");
		if (p)CommNetworkName= p->getValue();

		attList.push_back(CommNetworkName);
	}

	{
		QString ExEntityId;

		ByyParameter* p=myParameters.findParameter("ExEntityId");
		if (p)ExEntityId= p->getValue();

		attList.push_back(ExEntityId);
	}

	{
		QString ExEntityDomain;

		ByyParameter* p=myParameters.findParameter("ExEntityDomain");
		if (p)ExEntityDomain= p->getValue();

		attList.push_back(ExEntityDomain);
	}

	{
		QString ExPlatFormType;

		ByyParameter* p=myParameters.findParameter("ExPlatFormType");
		if (p)ExPlatFormType= p->getValue();

		attList.push_back(ExPlatFormType);
	}
	return attList.join(";");
}
