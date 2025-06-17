#ifndef BYY_SILVERLINING_NODE_H
#define BYY_SILVERLINING_NODE_H 1

#include "SilverLiningExport.h"
#include "SilverLiningOptions.h"
#include "SilverLiningCallback.h"
#include "SilverLiningAPIWrapper.h"
#include <osgEarthUtil/Sky>
#include <osgEarth/Map>
#include <osgEarth/Containers>
#include <osgEarth/PhongLightingEffect>
#include <osg/Light>
#include <osg/LightSource>

class ByySilverLiningContext;

/**
    * Node that roots the silverlining adapter.
    */
class SNIGSILVERLINING_EXPORT ByySilverLiningNode : public osgEarth::Util::SkyNode
{
public:
    ByySilverLiningNode(
        const osgEarth::SpatialReference* mapSRS,
        const ByySilverLiningOptions& options,
        SilverLiningCallback*                  callback =0L);

public: // SkyNode

    /** The (sun) light that this node controls */
    osg::Light* getSunLight() { return _light.get(); }

    /** Attach to a view so that this node controls its light. */
    void attach(osg::View* view, int lightNum);

	osg::StateSet* getCloudsStateSet() const { return _cloudsDrawable.valid() ? _cloudsDrawable->getStateSet() : 0L; }
	osg::StateSet* getSkyStateSet() const { return _skyDrawable.valid() ? _skyDrawable->getStateSet() : 0L; }

public:
    // callbacks from base class.
    void onSetDateTime();
    void onSetMinimumAmbient();

public: // osg::Node

    void traverse(osg::NodeVisitor&);

protected:
    virtual ~ByySilverLiningNode();

	osg::ref_ptr<ByySilverLiningContext> _SL;
	osg::ref_ptr<osg::Geode> _geode;
	osg::ref_ptr<osg::Drawable> _skyDrawable;
	osg::ref_ptr<osg::Drawable> _cloudsDrawable;

    osg::ref_ptr<osg::LightSource> _lightSource;
    osg::ref_ptr<osg::Light> _light;
    const ByySilverLiningOptions _options;
    osg::ref_ptr<osgEarth::PhongLightingEffect> _lighting;
    const osgEarth::SpatialReference* _mapSRS;
    osg::ref_ptr<SilverLiningCallback> _callback;

	double _lastAltitude;
};

#endif // BYY_SILVERLINING_NODE_H
