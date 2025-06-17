#pragma once

#include <osg/io_utils>
#include <osg/Stencil>
#include <osg/Depth>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/fstream>
#include <osgGA/GUIEventHandler>
#include <osgGA/StateSetManipulator>
#include <osgUtil/LineSegmentIntersector>
#include <osgViewer/CompositeViewer>
#include <osgViewer/View>
#include <osgViewer/ViewerEventHandlers>

#include <osgEarth/MapNode>
#include <osgEarth/TerrainEngineNode>
#include <osgEarth/VirtualProgram>
#include <osgEarth/Viewpoint>
#include <osgEarth/Registry>
#include <osgEarth/CullingUtils>
#include <osgEarth/ImageUtils>
#include <osgEarth/IntersectionPicker>
#include <osgEarth/Utils>
#include <osgEarthSymbology/Style>
#include <osgEarthSymbology/Geometry>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/AnnotationRegistry>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/LabelNode>
#include <osgEarthAnnotation/EllipseNode>
#include <osgEarthAnnotation/LocalGeometryNode>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/GeodeticGraticule>
#include <osgEarthAnnotation/AnnotationEditing>
#include <osgEarthAnnotation/FeatureEditing>

#include <QtCore>

#include "settings.h"
