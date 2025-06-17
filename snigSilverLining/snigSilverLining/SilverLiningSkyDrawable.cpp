#include <SilverLining.h>
#include "SilverLiningSkyDrawable.h"
#include "SilverLiningContext.h"
#include <osgEarth/SpatialReference>

#define LC "[SilverLining:SkyDrawable] "


SkyDrawable::SkyDrawable(ByySilverLiningContext* SL) :
_SL(SL)
{
    // call this to ensure draw() gets called every frame.
    setSupportsDisplayList( false );

    // not MT-safe (camera updates, etc)
    this->setDataVariance( osg::Object::DYNAMIC );

    setName("SilverLining::SkyDrawable");
}

void
SkyDrawable::drawImplementation(osg::RenderInfo& renderInfo) const
{
    osg::Camera* camera = renderInfo.getCurrentCamera();
	if ( camera)
	{
		renderInfo.getState()->disableAllVertexArrays();

		_SL->initialize( renderInfo );

		// convey the sky box size (far plane) to SL:
		double fovy, ar, znear, zfar;
		_SL->setCamera(camera);
		camera->getProjectionMatrixAsPerspective(fovy, ar, znear, zfar);
		_SL->setSkyBoxSize( zfar < 100000.0 ? zfar-100 : 100000.0 );

		// invoke the user callback if it exists
		if (_SL->getCallback())
			_SL->getCallback()->onDrawSky();
		// draw the sky.
		_SL->getAtmosphere()->DrawSky(
			true,
			_SL->getSRS()->isGeographic(),
			_SL->getSkyBoxSize(),
			true,
			false );

		// Dirty the state and the program tracking to prevent GL state conflicts.
		renderInfo.getState()->dirtyAllVertexArrays();
		renderInfo.getState()->dirtyAllAttributes();

#if 0
#if OSG_VERSION_GREATER_OR_EQUAL(3,4,0)
		osg::GLExtensions* api = renderInfo.getState()->get<osg::GLExtensions>();
#else
		osg::GL2Extensions* api = osg::GL2Extensions::Get(renderInfo.getState()->getContextID(), true);
#endif
		api->glUseProgram((GLuint)0);
		renderInfo.getState()->setLastAppliedProgramObject(0L);
#endif

		renderInfo.getState()->apply();
	}
}

osg::BoundingBox
#if OSG_VERSION_GREATER_THAN(3,3,1)
SkyDrawable::computeBoundingBox() const
#else
SkyDrawable::computeBound() const
#endif
{
    osg::BoundingBox skyBoundBox;
    if ( !_SL->ready() )
        return skyBoundBox;

    ::SilverLining::Atmosphere* atmosphere = _SL->getAtmosphere();
    double skyboxSize = _SL->getSkyBoxSize();
    if ( skyboxSize == 0.0 )
        skyboxSize = 1000.0;

    osg::Vec3d radiusVec = osg::Vec3d(skyboxSize, skyboxSize, skyboxSize) * 0.5;
    osg::Vec3d camPos = _SL->getCameraPosition();
    /*if (_SL->getCamera())
    {
        osg::Vec3f eye, center, up;
        _SL->getCamera()->getViewMatrixAsLookAt(eye, center, up);
        camPos = osg::Vec3d(eye.x(), eye.y(), eye.z());
    }*/

    skyBoundBox.set( camPos-radiusVec, camPos+radiusVec );

    // this enables the "blue ring" around the earth when viewing from space.
    bool hasLimb = atmosphere->GetConfigOptionBoolean("enable-atmosphere-from-space");
    if ( hasLimb )
    {
        // Compute bounds of atmospheric limb centered at (0,0,0)
        double earthRadius = atmosphere->GetConfigOptionDouble("earth-radius-meters");
        double atmosphereHeight = earthRadius + atmosphere->GetConfigOptionDouble("atmosphere-height");
        double atmosphereThickness = atmosphere->GetConfigOptionDouble("atmosphere-scale-height-meters") + earthRadius;

        osg::BoundingBox atmosphereBox;
        osg::Vec3d atmMin(-atmosphereThickness, -atmosphereThickness, -atmosphereThickness);
        osg::Vec3d atmMax(atmosphereThickness, atmosphereThickness, atmosphereThickness);
        atmosphereBox.set( atmMin, atmMax );
        skyBoundBox.expandBy( atmosphereBox );
    }
    return skyBoundBox;
}
