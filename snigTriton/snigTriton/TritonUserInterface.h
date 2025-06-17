#ifndef BYY_TRITON_USERINTERFACE_H
#define BYY_TRITON_USERINTERFACE_H 1

#include "TritonCallback.h"
#include <osg/Vec3>
#include <osg/Fog>
#include <osg/observer_ptr>

class SNIGRITON_EXPORT TritonUserInterface : public TritonCallback
{
public:
	void SetAboveWaterVisibility(double visibility,const osg::Vec3& fogColor);
	void setVisibility(double range);
	void setFog(osg::Fog* fog);
	void setWind(double speed, double direction);

protected:
	void onInitialize(Triton::Environment* env,Triton::Ocean* ocean);
	void onDrawOcean();

	void updateAboveWaterVisibility();

protected:
	osg::observer_ptr<osg::Fog> myFog;

	double myVisibility;
};

#endif