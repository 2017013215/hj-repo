#include "ByySatelliteSR.h"

ByySatelliteSR::ByySatelliteSR(ByyRemoteEntity* ent)
	:ByyStateRepository(ent)
{

}

ByySatelliteSR::~ByySatelliteSR()
{

}

const QString& ByySatelliteSR::type()
{
	return theType();
}

const QString& ByySatelliteSR::theType()
{
	static QString _type("Satellite");
	return _type;
}

void ByySatelliteSR::setOrbitData( const ByyOrbitData& od )
{
	if (myOrbitData!=od)
	{
		myOrbitData=od;
		emit orbitChanged(myOrbitData);
	}
}
