#include <osgEarthUtil/ContourMap>
#include <osgEarthUtil/VerticalScale>
#include "scene_global.h"

#include "TerrainShaderExtension.h"
namespace osgEarth
{
	namespace Util
	{
		class ContourMap;
		class VerticalScale;
	}
}

class ByyTerrain;

class SCENE_EXPORT ByyTerrainEffect
{
public:
	ByyTerrainEffect(ByyTerrain* terrain);
	~ByyTerrainEffect();

	void enableContour(bool enabled);
	void enableVertical(bool enabled);
	void setContourOpacity(float value);
	void setVerticalScale(float value);
	void openGrayEffect(bool val);

	void	setTerrainEffect();

	void readSettings();
	void writeSettings();

	bool	isOpenGrayEffect(){return myOpenGrayEffect;}
	bool	isEnableContour(){return myEnableContour;}
	bool	isEnableVertical(){return myEnableVertical;}
	float	contourVal(){return myContourVal;}
	float	verticalVal(){return myVerticalVal;}

private:
	ByyTerrain* myTerrain;

	bool	myEnableContour;
	bool	myEnableVertical;
	float	myContourVal;
	float	myVerticalVal;
	bool	myOpenGrayEffect;

	osg::ref_ptr<osgEarth::Util::ContourMap> myContourMap;
	osg::ref_ptr<osgEarth::Util::VerticalScale> myVerticalScale;

	//TerrainShaderOptions*	tsOptions;
	//GLSLEffect*				glsEffect;

	//osg::ref_ptr<TerrainEffect>        _effect;
	osgEarth::TerrainShader::TerrainShaderExtension*		tsExtention;
};

