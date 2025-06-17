#ifndef GEOMATH_H
#define GEOMATH_H

#include "snigcore_global.h"

#include <osg/Array>

namespace osgEarth
{
	namespace Annotation
	{
		class AnnotationNode;
	}
}

namespace ByyGeoMath
{
	SNIGCORE_EXPORT double calcPolygonArea( const std::vector<osg::Vec3d>& polygon );

	SNIGCORE_EXPORT std::vector<std::string> getAnnotationLabelText( osgEarth::Annotation::AnnotationNode* node );
}

#endif // GEOMATH_H
