#include "area.h"

#include <osgEarthFeatures/GeometryUtils>

ByyArea::ByyArea( osgEarth::MapNode* mapNode,const osgEarth::Config& conf )
{

	osgEarth::Symbology::Geometry *d_geometry=osgEarth::Features::GeometryUtils::geometryFromWKT(conf.value("geometry"));

	osgEarth::Symbology::Style polyStyle;
	polyStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Color(osgEarth::Color::Black, 0.5);
	polyStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 0;
	polyStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation() = 20;
	polyStyle.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = osgEarth::Color(osgEarth::Color::Black, 0.5);
	polyStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	polyStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	osgEarth::Features::Feature *_polyFeature = new osgEarth::Features::Feature(d_geometry, mapNode->getMapSRS(), polyStyle);

	d_featureNode=new osgEarth::Annotation::FeatureNode(mapNode, _polyFeature,true);


	d_name=QString::fromUtf8(conf.value("name").c_str());

	osgEarth::optional<bool> _mask;

	conf.getIfSet("mask",_mask);

	setMask(_mask.get());

	d_isTexture=conf.hasChild("texture");

	if (d_isTexture)
	{
		d_imageUrl=QString::fromUtf8(conf.child("texture").value("url").c_str());
	}

	d_isCustomHeight=conf.hasChild("customheight");

	if (d_isCustomHeight)
	{
		d_height=QString::fromUtf8(conf.child("customheight").value("height").c_str()).toDouble();
	}
}

ByyArea::ByyArea( osgEarth::Annotation::FeatureNode *featrueNode )
	:d_isMask(false),d_isTexture(false),d_isCustomHeight(false)
{
	d_featureNode=featrueNode;
}

ByyArea::~ByyArea()
{

}

osgEarth::Config ByyArea::getConfig()
{
	osgEarth::Config areaConf("area");
	areaConf.add("name",d_name.toUtf8().data());
	areaConf.add("geometry",osgEarth::Features::GeometryUtils::geometryToWKT( d_featureNode->getFeature()->getGeometry() ));

	if (d_isMask)
	{
		areaConf.add("mask",true);
	}

	if (d_isCustomHeight)
	{
		osgEarth::Config entHeightConf("customheight");
		entHeightConf.add("height",d_height);

		areaConf.add(entHeightConf);
	}

	if (d_isTexture)
	{
		osgEarth::Config entHeightConf("texture");
		entHeightConf.add("url",d_imageUrl.toUtf8().data());

		areaConf.add(entHeightConf);
	}

	return areaConf;
}

void ByyArea::setMask( bool b )
{
	if (d_isMask!=b)
	{
		d_isMask=b;


		if (d_isMask)
		{
			osgEarth:: Config maskConf("mask");
			maskConf.add("driver","feature");
			maskConf.add("name","mask");
			maskConf.add("min_level",13);
			osgEarth::Config featuresConf("features");
			featuresConf.add("driver","ogr");
			featuresConf.add("geometry",osgEarth::Features::GeometryUtils::geometryToWKT(d_featureNode->getFeature()->getGeometry()));

			maskConf.add(featuresConf);

			maskConf.add("profile","global-geodetic");


			osgEarth::MaskLayerOptions options(maskConf);
			options.name() = maskConf.value(d_name.toStdString());
			options.driver() = osgEarth::MaskSourceOptions(options);
			d_maskLayer=new osgEarth::MaskLayer(options);
		}
		else
		{
			d_maskLayer=NULL;
		}
	}
}
