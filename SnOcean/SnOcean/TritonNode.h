/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
 * Copyright 2008-2013 Pelican Mapping
 * http://osgearth.org
 *
 * osgEarth is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */
#ifndef OSGEARTH_DRIVER_TRITON_TRITON_NODE
#define OSGEARTH_DRIVER_TRITON_TRITON_NODE 1

#include "TritonOptions.h"
#include <osgEarthUtil/Ocean>
#include <osgEarth/MapNode>
#include <osg/Drawable>

#include "snocean_global.h"

namespace SNIG
{
    class TritonContext;

    using namespace osgEarth;
    using namespace osgEarth::Util;

    /**
     * Node that roots the Triton adapter.
     */
    class SNOCEAN_EXPORT TritonNode : public OceanNode
    {
    public:
        TritonNode(
            MapNode*           mapNode,
            const TritonOptions& options );

		void setWind(double speed, double direction);

		virtual ~TritonNode();

    protected: // OceanNode

        void onSetSeaLevel();

    public: // osg::Node

        osg::BoundingSphere computeBound() const;

        void traverse(osg::NodeVisitor&);

    protected:
        

        osg::ref_ptr<TritonContext> _TRITON;
        TritonOptions               _options;
        osg::Drawable*              _drawable;
    };

} // namespace osgEarth::Drivers::Triton

#endif // OSGEARTH_DRIVER_TRITON_TRITON_NODE
