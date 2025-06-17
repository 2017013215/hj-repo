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

#include "SilverLiningOptions.h"
#include <osgEarthUtil/Sky>
#include <osgEarth/Map>
#include <osgEarth/PhongLightingEffect>
#include <osg/Light>
#include "snsilverlining_global.h"

namespace SNIG
{
    class SilverLiningContext;

    using namespace osgEarth;
    using namespace osgEarth::Util;

    /**
     * Node that roots the silverlining adapter.
     */
    class SNSILVERLINING_EXPORT SilverLiningNode : public SkyNode
    {
    public:
        SilverLiningNode(
            const Map*                 map,
            const SilverLiningOptions& options );
		virtual ~SilverLiningNode();
    public: // SkyNode

		void setDateTime(int year,int month,int day,int hour,int min,int sec);
        osg::Light* getSunLight() { return _light.get(); }

        void attach(osg::View* view, int lightNum);

        void onSetDateTime();

		void setWind(double speed, double direction);
		void SetPrecipitation  ( int  precipitationType,  
			double  precipitationRate,  
			double  nearClip = -1,  
			double  farClip = -1,  
			bool  bUseDepthBuffer = false  
			) ;
		void SetVisibility (double range);

		void addCloudLayer(int type);
		void removeCloudLayer(int index);

    public: // osg::Node

        void traverse(osg::NodeVisitor&);

    protected:
        

        osg::ref_ptr<SilverLiningContext> _SL;
		osg::ref_ptr<osg::Geode> _geode;
        osg::Drawable* _skyDrawable;
		osg::ref_ptr<osg::Drawable> _cloudsDrawable;
        osg::ref_ptr<osg::Light> _light;
		double _lastAltitude;
		const SilverLiningOptions _options;
        osg::ref_ptr<PhongLightingEffect> _lighting;
    };

} // namespace osgEarth::Drivers::SilverLining
