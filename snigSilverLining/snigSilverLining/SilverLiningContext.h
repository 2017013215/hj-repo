#ifndef BYY_SILVERLINING_CONTEXT_H
#define BYY_SILVERLINING_CONTEXT_H 1

#include "SilverLiningExport.h"
#include "SilverLiningOptions.h"
//#include "SilverLiningAPIWrapper.h"
#include "SilverLiningCallback.h"
#include <osg/Referenced>
#include <osg/Light>
#include <osg/Camera>
#include <osgEarth/ThreadingUtils>

namespace SilverLining {
    class Atmosphere;
    class CloudLayer;
    class MillisecondTimer;
}
namespace osgEarth {
    class SpatialReference;
}

/**
    * Contains all the SilverLining SDK pointers.
    */
class ByySilverLiningContext : public osg::Referenced
{
public:
    ByySilverLiningContext(const ByySilverLiningOptions& options);

    /** Sets the light source that will represent the sun */
    void setLight(osg::Light* light);

    /** Sets the spatial reference system of the map */
    void setSRS(const osgEarth::SpatialReference* srs);

    /** Sets the minimum ambient lighting value */
    void setMinimumAmbient(const osg::Vec4f& value);

    /** Installs a user callback for SL initialization */
    void setCallback(SilverLiningCallback*);
    SilverLiningCallback* getCallback() const { return _callback.get(); }

	const ByySilverLiningOptions& options()const{return _options;}

public: // accessors

    bool ready() const { return _initAttempted && !_initFailed; }

    ::SilverLining::Atmosphere* getAtmosphere() { return _atmosphere; }

  //  Atmosphere& getAtmosphereWrapper() { return *_atmosphereWrapper; }

    /** Spatial reference of the map */
    const osgEarth::SpatialReference* getSRS() const { return _srs.get(); }

    void setSkyBoxSize(double size) { _skyBoxSize = size; }
    double getSkyBoxSize() const { return _skyBoxSize; }

    void initialize(osg::RenderInfo& renderInfo);

    void updateLocation();

    void updateLight();

    /** Set/get the cached camers. NOT THREAD/MULTI-CAM SAFE. */
    /** TODO */
    void setCamera(osg::Camera* camera) { _camera = camera; }
    osg::Camera* getCamera() { return _camera.get(); }

    void setCameraPosition(const osg::Vec3d& pos) { _cameraPos = pos; }
    const osg::Vec3d& getCameraPosition() const { return _cameraPos; }

protected:

    virtual ~ByySilverLiningContext();

public:

    void setupClouds();

private:
    ::SilverLining::Atmosphere* _atmosphere;

    double _skyBoxSize;

    osg::observer_ptr<osg::Light>                  _light;
    osg::ref_ptr<const osgEarth::SpatialReference> _srs;

    bool                       _initAttempted;
    bool                       _initFailed;
    osgEarth::Threading::Mutex _initMutex;

    double _maxAmbientLightingAlt;

    osg::observer_ptr<osg::Camera> _camera;
    osg::Vec3d                     _cameraPos; // eye point
    osg::Vec4f                     _minAmbient;

    ByySilverLiningOptions _options;

    osg::ref_ptr<SilverLiningCallback> _callback;

 //   Atmosphere* _atmosphereWrapper;

    ::SilverLining::MillisecondTimer* _msTimer;
};

#endif // BYY_SILVERLINING_CONTEXT_H
