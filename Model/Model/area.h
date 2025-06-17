#ifndef AREA_H
#define AREA_H

#include <osgEarth/MapNode>
#include <QString>
#include <osgEarthAnnotation/FeatureNode>
#include "model_global.h"

class MODEL_EXPORT ByyArea
{
public:
	ByyArea(osgEarth::MapNode* mapNode,const osgEarth::Config& conf);
	ByyArea(osgEarth::Annotation::FeatureNode *featrueNode);
	~ByyArea();

	osgEarth::Config getConfig();

	QString name(){return d_name;}
	void setName(const QString& name){d_name=name;}

	osgEarth::Annotation::FeatureNode* getArea(){return d_featureNode;}
	osgEarth::MaskLayer* getMaskLayer(){return d_maskLayer;}

	bool isMask(){return d_isMask;}
	bool isTexture(){return d_isTexture;}
	bool isCustonHeight(){return d_isCustomHeight;}
	double height(){return d_height;}
	QString imageUrl(){return d_imageUrl;}

	void setCustomHeight(bool b){d_isCustomHeight=b;}
	void setHeight(double d){d_height=d;}

	void setMask(bool b);

	void setTexture(bool b){d_isTexture=b;}
	void setImageUrl(const QString& url){d_imageUrl=url;}

private:
	QString d_name;
	bool d_isTexture;
	QString d_imageUrl;
	bool d_isMask;
	bool d_isCustomHeight;
	double d_height;

	osg::ref_ptr<osgEarth::Annotation::FeatureNode> d_featureNode;
	osg::ref_ptr<osgEarth::MaskLayer> d_maskLayer;
};

#endif // AREA_H
