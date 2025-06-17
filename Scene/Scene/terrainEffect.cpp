#include "stdafx.h"
#include "terrainEffect.h"
#include "terrain.h"

#define TE_SCRIPT "../data/config/terrainEffect.txt"

ByyTerrainEffect::ByyTerrainEffect( ByyTerrain* terrain )
	:myTerrain(terrain)
{
	myContourMap=new osgEarth::Util::ContourMap;
	myContourMap->opacity()=0.5;
	myVerticalScale=new osgEarth::Util::VerticalScale;
	readSettings();	
	osgEarth::TerrainShader::TerrainShaderOptions tsOptions;//=new osgEarth::TerrainShader::TerrainShaderOptions();
	myVerticalScale->setScale(1.0);

	osgEarth::TerrainShader::TerrainShaderOptions::Code codeExa;
	QFile file(TE_SCRIPT);
	file.open(QIODevice::ReadOnly);
	QByteArray str=file.readAll();
	codeExa._source=str;//"#version 110";
	//codeExa._source="#version 110 #pragma vp_entryPoint invert #pragma vp_location fragment_coloring void invert(input vec4 color){vec3 W=vec3(0.2125,0.7154,0.0721);float luminance=dot(color.rgb,W);color.rgb=vec3(luminance);}";
	//osgEarth::TerrainShader::TerrainShaderOptions* tsOptions=new TerrainShaderOptions;
	tsOptions.code().push_back(codeExa);

	//_effect=new GLSLEffect(*tsOptions,new osgDB::Options);

	tsExtention=new osgEarth::TerrainShader::TerrainShaderExtension(tsOptions);
}


ByyTerrainEffect::~ByyTerrainEffect(void)
{
	writeSettings();
}

void ByyTerrainEffect::setTerrainEffect()
{
	enableContour(myEnableContour);
	enableVertical(myEnableVertical);
	setContourOpacity(myContourVal);
	setVerticalScale(myVerticalVal);
	openGrayEffect(myOpenGrayEffect);
}

void ByyTerrainEffect::enableContour( bool enabled )
{
	myEnableContour=enabled;
	if(enabled)
	{
		myTerrain->mapNode()->getTerrainEngine()->addEffect(myContourMap);
	}
	else
		myTerrain->mapNode()->getTerrainEngine()->removeEffect(myContourMap);
}

void ByyTerrainEffect::enableVertical( bool enabled )
{
	myEnableVertical=enabled;
	if(enabled)
		myTerrain->mapNode()->getTerrainEngine()->addEffect(myVerticalScale);
	else
		myTerrain->mapNode()->getTerrainEngine()->removeEffect(myVerticalScale);
}

void ByyTerrainEffect::setContourOpacity( float value )
{
	myContourVal=value;
	myContourMap->opacity()=value;
}

void ByyTerrainEffect::setVerticalScale( float value )
{
	myVerticalVal=value;
	myVerticalScale->setScale(value);
}


void ByyTerrainEffect::writeSettings()
{
	QSettings s("../data/config/TerrainEffect.ini",QSettings::IniFormat);
	s.beginGroup("BZZZY");

	s.setValue("EnableContour",myEnableContour);
	s.setValue("EnableVertical",myEnableContour);
	s.setValue("ContourVal",myContourVal);
	s.setValue("VerticalVal",myVerticalVal);
	s.setValue("OpenGrayEffect",myOpenGrayEffect);
}

void ByyTerrainEffect::readSettings()
{
	QSettings s("../data/config/TerrainEffect.ini",QSettings::IniFormat);
	s.beginGroup("BZZZY");
	myEnableContour=s.value("EnableContour").toBool();
	myEnableVertical=s.value("EnableVertical").toBool();
	myContourVal=s.value("ContourVal").toDouble();
	myVerticalVal=s.value("VerticalVal").toDouble();
	myOpenGrayEffect=s.value("OpenGrayEffect").toBool();
}

void ByyTerrainEffect::openGrayEffect(bool val)
{
	myOpenGrayEffect=val;
	if(myOpenGrayEffect)
	{
		//glsEffect->onInstall(myTerrain->mapNode()->getTerrainEngine());
		//myTerrain->mapNode()->getTerrainEngine()->addEffect(_effect.get());
		tsExtention->connect(myTerrain->mapNode());
	}
	else
		tsExtention->disconnect(myTerrain->mapNode());
		//myTerrain->mapNode()->getTerrainEngine()->removeEffect(_effect.get());
}
