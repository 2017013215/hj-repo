#ifndef BYY_TRITON_DRAWABLE_H
#define BYY_TRITON_DRAWABLE_H

#include "TritonExport.h"
#include <osg/Drawable>
#include <osg/RenderInfo>
#include <osg/TextureCubeMap>
#include <osg/Version>
#include <osg/Texture2D>
#include <osg/buffered_value>

#include <osgEarth/MapNode>
#include <osgEarth/Terrain>
#include <osgEarth/NativeProgramAdapter>

const unsigned int TRITON_OCEAN_MASK = 0x4; // 0100

    class ByyTritonContext;

    /**
     * Custom drawable for rendering the Triton ocean effects
     */
    class SNIGRITON_EXPORT ByyTritonDrawable : public osg::Drawable
    {
    public:
        ByyTritonDrawable(osgEarth::MapNode* mapNode=NULL, ByyTritonContext* TRITON =0L);
        META_Object(snigTriton, ByyTritonDrawable);

    public: // osg::Drawable

        // custom draw (called with an active GC)
        void drawImplementation(osg::RenderInfo& ri) const;

        void setupHeightMap(osg::State& state);

        void updateHeightMap(osg::RenderInfo& renderInfo) const;

        void dirtyAllContexts();

#if OSG_VERSION_GREATER_THAN(3,3,1)
        osg::BoundingBox computeBoundingBox() const
#else
        osg::BoundingBox computeBound() const
#endif
            { return osg::BoundingBox(); }

        void SetPlanarReflectionMap(osg::Texture2D* map) {_planarReflectionMap = map;};
        void SetPlanarReflectionProjection(osg::RefMatrix * proj) {_planarReflectionProjection = proj;};

    protected:
        virtual ~ByyTritonDrawable();

        osg::observer_ptr<ByyTritonContext>  _TRITON;
        osg::observer_ptr<osgEarth::MapNode> _mapNode;
        osg::ref_ptr<osg::TextureCubeMap> _cubeMap;
        osg::BoundingBox                  _bbox;
        osg::ref_ptr<osg::Texture2D> _heightMap;
        osg::ref_ptr<osg::Camera> _heightCamera;
        osg::observer_ptr<osgEarth::TerrainCallback> _terrainChangedCallback;

        osg::ref_ptr< osg::Texture2D >       _planarReflectionMap;
        osg::ref_ptr< osg::RefMatrix >       _planarReflectionProjection;

        mutable osg::buffered_object<int>         _contextDirty;
        mutable osg::Matrix                       _viewMatrix;
        mutable osg::Matrix                       _projMatrix;

		mutable double  myLastHeightMapUpdateTime ;

        mutable osg::buffered_object<osgEarth::NativeProgramAdapterCollection> _adapters;

        ByyTritonDrawable(const ByyTritonDrawable& copy, const osg::CopyOp& op=osg::CopyOp::SHALLOW_COPY) { }
    };

#endif // BYY_TRITON_DRAWABLE_H
