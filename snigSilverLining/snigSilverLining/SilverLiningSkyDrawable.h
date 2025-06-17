#ifndef BYY_SILVERLINING_SKYDRAWABLE_H
#define BYY_SILVERLINING_SKYDRAWABLE_H 1

#include <osg/Drawable>
#include <osg/RenderInfo>
#include <osg/Version>


class ByySilverLiningContext;
/**
    * Custom drawable for rendering the SilverLining effects
    */
class SkyDrawable : public osg::Drawable
{
public:
    SkyDrawable(ByySilverLiningContext* SL =0L);
    META_Object(snigSilverLining, SkyDrawable);
     
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
    virtual ~SkyDrawable() { }

    osg::observer_ptr<ByySilverLiningContext> _SL;
        
    SkyDrawable(const SkyDrawable& copy, const osg::CopyOp& op=osg::CopyOp::SHALLOW_COPY) { }
};

#endif // BYY_SILVERLINING_SKYDRAWABLE_H
