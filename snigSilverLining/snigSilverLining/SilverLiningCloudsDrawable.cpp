#include <SilverLining.h>
#include "SilverLiningCloudsDrawable.h"
#include "SilverLiningContext.h"
#include <osgEarth/SpatialReference>
#include <osgEarth/CullingUtils>

#undef  LC
#define LC "[SilverLining:SkyDrawable] "

struct CloudsDrawableCullCallback : public osg::Drawable::CullCallback
{
	CloudsDrawableCullCallback(CloudsDrawable* cd):myCloudsDrawable(cd){}

	bool  cull (osg::NodeVisitor *nv, osg::Drawable *drawable, osg::RenderInfo *renderInfo) const  
	{
		return myCloudsDrawable->cull(nv,drawable,renderInfo);
	}

protected:
	osg::observer_ptr<CloudsDrawable> myCloudsDrawable;

};

CloudsDrawable::CloudsDrawable(ByySilverLiningContext* SL) :
_SL(SL)
{
    // call this to ensure draw() gets called every frame.
    setSupportsDisplayList( false );
    
    // not MT-safe (camera updates, etc)
    this->setDataVariance(osg::Object::DYNAMIC);
    
    setName("SilverLining::CloudsDrawable");

	setCullCallback(new CloudsDrawableCullCallback(this));
}

void
CloudsDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
	if ( _SL->ready())
	{
		osg::State* state = renderInfo.getState();

		// adapt the SL shaders so they can accept OSG uniforms:
		osgEarth::NativeProgramAdapterCollection& adapters = _adapters[ state->getContextID() ]; // thread safe.
		if ( adapters.empty() )
		{
			adapters.push_back( new osgEarth::NativeProgramAdapter(state, _SL->getAtmosphere()->GetSkyShader()) );
			adapters.push_back( new osgEarth::NativeProgramAdapter(state, _SL->getAtmosphere()->GetBillboardShader()) );
			adapters.push_back( new osgEarth::NativeProgramAdapter(state, _SL->getAtmosphere()->GetStarShader()) );
			adapters.push_back( new osgEarth::NativeProgramAdapter(state, _SL->getAtmosphere()->GetPrecipitationShader()) );
			//adapters.push_back(new osgEarth::NativeProgramAdapter(state, _SL->getAtmosphere()->GetAtmosphericLimbShader()) );

			SL_VECTOR(unsigned) handles = _SL->getAtmosphere()->GetActivePlanarCloudShaders();
			for(int i=0; i<handles.size(); ++i)          
				adapters.push_back( new osgEarth::NativeProgramAdapter(state, handles[i]) );
		}
		adapters.apply( state );

		// invoke the user callback if it exists
		if (_SL->getCallback())
			_SL->getCallback()->onDrawClouds();

		renderInfo.getState()->disableAllVertexArrays();
		_SL->getAtmosphere()->DrawObjects( true, true, true );

		// Restore the GL state to where it was before.
		state->dirtyAllVertexArrays();
		state->dirtyAllAttributes();

		state->apply();
	}
}

osg::BoundingBox
#if OSG_VERSION_GREATER_THAN(3,3,1)
CloudsDrawable::computeBoundingBox() const
#else
CloudsDrawable::computeBound() const
#endif
{
    osg::BoundingBox cloudBoundBox;
    if ( !_SL->ready() )
        return cloudBoundBox;
    
    double minX, minY, minZ, maxX, maxY, maxZ;
    _SL->getAtmosphere()->GetCloudBounds( minX, minY, minZ, maxX, maxY, maxZ );
    cloudBoundBox.set( osg::Vec3d(minX, minY, minZ), osg::Vec3d(maxX, maxY, maxZ) );
    return cloudBoundBox;
}

bool CloudsDrawable::cull( osg::NodeVisitor *nv, osg::Drawable *drawable, osg::RenderInfo *renderInfo ) const
{
	osgUtil::CullVisitor* cv = osgEarth::Culling::asCullVisitor(nv);
	double _lastAltitude = _SL->getSRS()->isGeographic() ?
		cv->getEyePoint().length() - _SL->getSRS()->getEllipsoid()->getRadiusEquator() :
	cv->getEyePoint().z();

	return _SL->options().cloudsMaxAltitude()<_lastAltitude;
}
