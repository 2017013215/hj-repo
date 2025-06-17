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
#include <osg/Referenced>
#include <osg/Light>
#include <osg/Camera>
#include <osgEarth/ThreadingUtils>

namespace SilverLining {
    class Atmosphere;
    class CloudLayer;
}
namespace osgEarth {
    class SpatialReference;
}

namespace SNIG
{
    using namespace osgEarth;

    /**
     * Contains all the SilverLining SDK pointers.
     */
    class SilverLiningContext : public osg::Referenced
    {
    public:
        SilverLiningContext(const SilverLiningOptions& options);
        virtual ~SilverLiningContext();
        /** Sets the light source that will represent the sun */
        void setLight(osg::Light* light);

        /** Sets the spatial reference system of the map */
        void setSRS(const SpatialReference* srs);

    public: // accessors

        bool ready() const { return _initAttempted && !_initFailed; }

        ::SilverLining::Atmosphere* getAtmosphere() { return _atmosphere; }

        /** Spatial reference of the map */
        const SpatialReference* getSRS() const { return _srs.get(); }

        void setSkyBoxSize(double size) { _skyBoxSize = size; }
        double getSkyBoxSize() const { return _skyBoxSize; }

        void initialize(osg::RenderInfo& renderInfo);

        void updateLocation();

        void updateLight();
        /** Set/get the cached camers. NOT THREAD/MULTI-CAM SAFE. */
        /** TODO */
        void setCamera(osg::Camera* camera) { _camera = camera; }
        osg::Camera* getCamera() { return _camera.get(); }

        void setCameraPosition(const osg::Vec3d& pos) { _cameraPos = pos; }
        const osg::Vec3d& getCameraPosition() const { return _cameraPos; }

		void addCloudLayer(int type);
		void removeCloudLayer(int index);

		void setWind(double speed, double direction);

    private:
		::SilverLining::CloudLayer* createCIRROCUMULUSClouds();
		::SilverLining::CloudLayer* createCIRRUS_FIBRATUSClouds();
		::SilverLining::CloudLayer* createSTRATUSClouds();
		::SilverLining::CloudLayer* createCUMULUS_MEDIOCRISClouds();
		::SilverLining::CloudLayer* createCUMULUS_CONGESTUSClouds();
		::SilverLining::CloudLayer* createCUMULUS_CONGESTUS_HI_RESClouds();
		::SilverLining::CloudLayer* createCUMULONIMBUS_CAPPILATUSClouds();
		::SilverLining::CloudLayer* createSTRATOCUMULUSClouds();
		::SilverLining::CloudLayer* createTOWERING_CUMULUSClouds();

    private:
        ::SilverLining::Atmosphere* _atmosphere;

		std::list<int> cloundLayerHandles;

        double _skyBoxSize;

        osg::observer_ptr<osg::Light>        _light;
        osg::ref_ptr<const SpatialReference> _srs;

        bool             _initAttempted;
        bool             _initFailed;
        Threading::Mutex _initMutex;

        double _maxAmbientLightingAlt;

        osg::observer_ptr<osg::Camera> _camera;
        osg::Vec3d                     _cameraPos; // eye point

        SilverLiningOptions _options;

		int windHandle;
    };

}  // namespace osgEarth::Drivers::SilverLining
