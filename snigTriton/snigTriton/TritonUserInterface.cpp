#include "TritonUserInterface.h"
#include <Environment.h>

#define TOVEC3(X)   ::Triton::Vector3(X.x(),X.y(),X.z())
#define FROMVEC3(X) osg::Vec3(X.x,X.y,X.z)

void TritonUserInterface::onInitialize( Triton::Environment* env,Triton::Ocean* ocean )
{
	__super::onInitialize(env,ocean);
	::Triton::Vector3 vec;

	env->GetAboveWaterVisibility(myVisibility,vec);
}

void TritonUserInterface::SetAboveWaterVisibility( double visibility,const osg::Vec3& fogColor )
{
	if (myEnvironment)
	{
		myEnvironment->SetAboveWaterVisibility(visibility,TOVEC3(fogColor));
	}
}

void TritonUserInterface::setVisibility( double range )
{
	myVisibility=range;
}

void TritonUserInterface::setFog( osg::Fog* fog )
{
	myFog=fog;
}

void TritonUserInterface::onDrawOcean()
{
	updateAboveWaterVisibility();
}

void TritonUserInterface::updateAboveWaterVisibility()
{
	if (!myFog.valid())
		return;

	if (myFog->getDensity()>0)
	{
		myEnvironment->SetAboveWaterVisibility(3.912f/myFog->getDensity(),TOVEC3(myFog->getColor()));
	}
	else
	{
		myEnvironment->SetAboveWaterVisibility(myVisibility,TOVEC3(myFog->getColor()));
	}
}

void TritonUserInterface::setWind( double speed, double direction )
{
	myEnvironment->ClearWindFetches();

	Triton::WindFetch wind_fetch;
	wind_fetch.SetWind(speed,direction);

	myEnvironment->AddWindFetch(wind_fetch);
}
