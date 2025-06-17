#ifndef BYY_SILVERLINING_CLOUDSDRAWABLE_H
#define BYY_SILVERLINING_CLOUDSDRAWABLE_H 1

#include <osg/Drawable>
#include <osg/RenderInfo>
#include <osg/Version>
#include <osgEarth/NativeProgramAdapter>
#include <vector>
#include <map>

class ByySilverLiningContext;


/**
    * Custom drawable for rendering the SilverLining clouds
    */
class CloudsDrawable : public osg::Drawable
{
public:
    CloudsDrawable(ByySilverLiningContext* SL =0L);
    META_Object(snigSilverLining, CloudsDrawable);

	/* Sets whether to draw this item */
	void setDraw(bool draw);

	bool  cull (osg::NodeVisitor *nv, osg::Drawable *drawable, osg::RenderInfo *renderInfo) const  ;

     
public: // osg::Drawable

    // custom draw (called with an active GC)
    void drawImplementation(osg::RenderInfo& ri) const;
        
    // custom bounds computation
#if OSG_VERSION_GREATER_THAN(3,3,1)
    osg::BoundingBox computeBoundingBox() const;
#else
    osg::BoundingBox computeBound() const;
#endif

protected:
    virtual ~CloudsDrawable() { }

    osg::observer_ptr<ByySilverLiningContext> _SL;

    mutable osg::buffered_object<osgEarth::NativeProgramAdapterCollection> _adapters;
        
    CloudsDrawable(const CloudsDrawable& copy, const osg::CopyOp& op=osg::CopyOp::SHALLOW_COPY) { }
};

#endif // BYY_SILVERLINING_CLOUDSDRAWABLE_H
