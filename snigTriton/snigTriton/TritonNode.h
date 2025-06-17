#ifndef BYY_TRITON_NODE
#define BYY_TRITON_NODE 1

#include "TritonExport.h"
#include "TritonOptions.h"
#include "TritonAPIWrapper.h"
#include "TritonCallback.h"
#include <osgEarthUtil/Ocean>
#include <osgEarth/MapNode>
#include <osg/Drawable>

class ByyTritonContext;
class ByyTritonDrawable;

class SNIGRITON_EXPORT ByyTritonNode : public osgEarth::Util::OceanNode
{
public:
    ByyTritonNode(
        osgEarth::MapNode*   mapNode,
        const ByyTritonOptions& options,
        TritonCallback*            userCallback =0L);

protected: // OceanNode

    void onSetSeaLevel();

    void onSetAlpha();

public: // osg::Node

    osg::BoundingSphere computeBound() const;

    void traverse(osg::NodeVisitor&);

protected:
    virtual ~ByyTritonNode();

    osg::ref_ptr<ByyTritonContext> _TRITON;
    ByyTritonOptions               _options;
    ByyTritonDrawable*              _drawable;
    osg::ref_ptr<osg::Uniform>  _alphaUniform;

    //Environment* _environmentWrapper;
};

#endif // BYY_TRITON_NODE
