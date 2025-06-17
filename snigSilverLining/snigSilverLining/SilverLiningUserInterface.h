#ifndef BYY_SILVERLINING_USERINTERFACE_H
#define BYY_SILVERLINING_USERINTERFACE_H 1

#include "SilverLiningCallback.h"
#include <osg/observer_ptr>
#include <osg/Fog>

class SNIGSILVERLINING_EXPORT SilverLiningUserInterface : public SilverLiningCallback
{
public:
	SilverLiningUserInterface(void);
	~SilverLiningUserInterface(void);

	void setCloudLayerType(int type);
	void setPrecipitation(int precipitationType,double precipitationRate,double  nearClip = -1, double  farClip = -1, bool  bUseDepthBuffer = false  );
	void setWind(double speed, double direction);
	void updateFog();
	void setVisibility(double range);
	void setFog(osg::Fog* fog);

protected:
	void onInitialize(SilverLining::Atmosphere* atmosphere);
	void onDrawSky();
	void updateCloudLayer();

protected:
	int myNewCloudLayerType;
	osg::observer_ptr<osg::Fog> myFog;

	double mtVisibility;
};

#endif