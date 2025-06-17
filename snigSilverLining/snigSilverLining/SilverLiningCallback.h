#ifndef BYY_SILVERLINING_CALLBACK_H
#define BYY_SILVERLINING_CALLBACK_H 1

#include "SilverLiningExport.h"
#include "SilverLiningAPIWrapper.h"
#include <osg/Referenced>
namespace SilverLining {
	class Atmosphere;
	class CloudLayer;
	class MillisecondTimer;
}

class SNIGSILVERLINING_EXPORT SilverLiningCallback : public osg::Referenced
{
public:
	SilverLiningCallback():myAtmosphere(0){}
    virtual void onInitialize(SilverLining::Atmosphere* atmosphere) {myAtmosphere=atmosphere;}

    virtual void onDrawSky() { }

    virtual void onDrawClouds() { }

    virtual unsigned long getMilliseconds() const { return 0; }

protected:
	SilverLining::Atmosphere* myAtmosphere;
};

#endif // BYY_SILVERLINING_CALLBACK_H
