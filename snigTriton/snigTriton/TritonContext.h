#ifndef OSGEARTH_TRITON_CONTEXT_H
#define OSGEARTH_TRITON_CONTEXT_H

#include "TritonExport.h"
#include "TritonOptions.h"
#include "TritonCallback.h"
#include <osg/Referenced>
#include <osg/Light>
#include <osgEarth/ThreadingUtils>
#include <Triton.h>

namespace osgEarth {
    class SpatialReference;
}

    /**
     * Contains all the Triton SDK handles.
     */
class SNIGRITON_EXPORT ByyTritonContext : public osg::Referenced
{
public:
    ByyTritonContext(const ByyTritonOptions& options);

    /** Sets the spatial reference system of the map */
    void setSRS(const osgEarth::SpatialReference* srs);

    /** Sets the user callback */
    void setCallback(TritonCallback* callback);
    TritonCallback* getCallback() const { return _callback.get(); }

public: // accessors

    bool ready() const { return _initAttempted && !_initFailed; }

    /** Spatial reference of the map */
    const osgEarth::SpatialReference* getSRS() const { return _srs.get(); }

    void initialize(osg::RenderInfo& renderInfo);

    void update(double simTime);

    ::Triton::Environment* getEnvironment() { return _environment; }

    ::Triton::Ocean* getOcean() { return _ocean; }

    bool passHeightMapToTriton() const;

    int getHeightMapSize() const;

protected:

    virtual ~ByyTritonContext();


private:
    ByyTritonOptions    _options;

    bool             _initAttempted;
    bool             _initFailed;
    osgEarth::Threading::Mutex _initMutex;

    osg::ref_ptr<const osgEarth::SpatialReference> _srs;

    ::Triton::ResourceLoader* _resourceLoader;
    ::Triton::Environment*    _environment;
    ::Triton::Ocean*          _ocean;

    osg::ref_ptr<TritonCallback> _callback;
};

#endif // OSGEARTH_TRITON_CONTEXT_H