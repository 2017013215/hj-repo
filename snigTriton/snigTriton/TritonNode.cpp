#include <Triton.h>
#include "TritonNode.h"
#include "TritonContext.h"
#include "TritonDrawable.h"
#include <osgEarth/CullingUtils>

#define LC "[TritonNode] "

ByyTritonNode::ByyTritonNode(osgEarth::MapNode*   mapNode,
                       const ByyTritonOptions& options,
                       TritonCallback*            callback) :
OceanNode( options ),
_options ( options )
{
    const osgEarth::Map* map = mapNode->getMap();
    if ( map )
        setSRS( map->getSRS() );

    _TRITON = new ByyTritonContext( options );

    if ( map )
        _TRITON->setSRS( map->getSRS() );

    if ( callback )
        _TRITON->setCallback( callback );

    _drawable = new ByyTritonDrawable(mapNode, _TRITON);
    _alphaUniform = getOrCreateStateSet()->getOrCreateUniform("oe_ocean_alpha", osg::Uniform::FLOAT);
    _alphaUniform->set(getAlpha());

    osg::Geode* geode = new osg::Geode();
    geode->addDrawable( _drawable );
    geode->setNodeMask( TRITON_OCEAN_MASK );

    this->addChild( geode );

    this->setNumChildrenRequiringUpdateTraversal(1);

    // Place in the depth-sorted bin and set a rendering order.
    // We want Triton to render after the terrain.
    _drawable->getOrCreateStateSet()->setRenderBinDetails( options.renderBinNumber().get(), "DepthSortedBin" );
}

ByyTritonNode::~ByyTritonNode()
{
    //nop
}

void ByyTritonNode::onSetSeaLevel()
{
    if ( _TRITON->ready() )
    {
        _TRITON->getEnvironment()->SetSeaLevel( getSeaLevel() );
    }
    dirtyBound();
}

void ByyTritonNode::onSetAlpha()
{
    _alphaUniform->set(getAlpha());
}

osg::BoundingSphere ByyTritonNode::computeBound() const
{
    return osg::BoundingSphere();
}

void ByyTritonNode::traverse(osg::NodeVisitor& nv)
{
	if (_TRITON->ready())
	{
		if ( nv.getVisitorType() == nv.UPDATE_VISITOR) 
		{
			_TRITON->update(nv.getFrameStamp()->getSimulationTime());
		}
		else if (nv.getVisitorType()==nv.CULL_VISITOR)
		{
			osgUtil::CullVisitor* cv = osgEarth::Culling::asCullVisitor(nv);

			_TRITON->getEnvironment()->SetCameraMatrix( cv->getModelViewMatrix()->ptr() );
			_TRITON->getEnvironment()->SetProjectionMatrix( cv->getProjectionMatrix()->ptr() );
		}
		
	}
	
    
    osgEarth::Util::OceanNode::traverse(nv);
}
