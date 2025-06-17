#include "geoMath.h"

#include <QString>
#include <osgEarth/GeoMath>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/EllipseNode>

namespace ByyGeoMath
{

double calcPolygonArea( const std::vector<osg::Vec3d>& polygon )
{
	double totalArea = 0;
	int Count = polygon.size();
	if (Count>2)
	{
		double LowX=0.0;
		double LowY=0.0;
		double MiddleX=0.0;
		double MiddleY=0.0;
		double HighX=0.0;
		double HighY=0.0;
		double AM = 0.0;
		double BM = 0.0;
		double CM = 0.0;
		double AL = 0.0;
		double BL = 0.0;
		double CL = 0.0;    
		double AH = 0.0;
		double BH = 0.0;
		double CH = 0.0;		
		//Coefficient系数
		double CoefficientL = 0.0;
		double CoefficientH = 0.0;
		//tangent切线
		double ALtangent = 0.0;
		double BLtangent = 0.0;
		double CLtangent = 0.0;
		double AHtangent = 0.0;
		double BHtangent = 0.0;
		double CHtangent = 0.0;	
		//NormalLine法线     
		double ANormalLine = 0.0;
		double BNormalLine = 0.0;
		double CNormalLine = 0.0;
		//Orientation  Value方向值  
		double OrientationValue = 0.0; 
		//余弦角
		double AngleCos = 0.0;

		double Sum1 = 0.0;
		double Sum2 = 0.0;
		int Count2 = 0;	
		int Count1 = 0; 

		double Sum = 0.0;

		for( int i = 0; i<Count; i++ )
		{
			if (i==0)
			{
				//换算成弧度;
				LowX = osg::DegreesToRadians( polygon.at(Count-1).x() );
				LowY = osg::DegreesToRadians( polygon.at(Count-1).y() ); 
				MiddleX = osg::DegreesToRadians( polygon.at(0).x() );
				MiddleY = osg::DegreesToRadians( polygon.at(0).y() );
				HighX = osg::DegreesToRadians( polygon.at(1).x() );
				HighY = osg::DegreesToRadians( polygon.at(1).y() );
			}
			else if (i == Count-1 )
			{
				LowX = osg::DegreesToRadians( polygon.at(Count-2).x() );
				LowY = osg::DegreesToRadians( polygon.at(Count-2).y() ); 
				MiddleX = osg::DegreesToRadians( polygon.at(Count-1).x() );
				MiddleY = osg::DegreesToRadians( polygon.at(Count-1).y() );
				HighX = osg::DegreesToRadians( polygon.at(0).x() );
				HighY = osg::DegreesToRadians( polygon.at(0).y() );
			}
			else
			{
				LowX = osg::DegreesToRadians( polygon.at(i-1).x() );
				LowY = osg::DegreesToRadians( polygon.at(i-1).y() ); 
				MiddleX = osg::DegreesToRadians( polygon.at(i).x() );
				MiddleY = osg::DegreesToRadians( polygon.at(i).y() );
				HighX = osg::DegreesToRadians( polygon.at(i+1).x() );
				HighY = osg::DegreesToRadians( polygon.at(i+1).y() );
			}
			AM = cos(MiddleY) * cos(MiddleX);
			BM = cos(MiddleY) * sin(MiddleX);
			CM = sin(MiddleY);
			AL = cos(LowY) * cos(LowX);
			BL = cos(LowY) * sin(LowX);
			CL = sin(LowY);
			AH = cos(HighY) * cos(HighX);
			BH = cos(HighY) * sin(HighX);
			CH = sin(HighY); 

			CoefficientL = (AM*AM + BM*BM + CM*CM)/(AM*AL + BM*BL + CM*CL);
			CoefficientH = (AM*AM + BM*BM + CM*CM)/(AM*AH + BM*BH + CM*CH);

			ALtangent =  CoefficientL * AL - AM;
			BLtangent =  CoefficientL * BL - BM;
			CLtangent =  CoefficientL * CL - CM;
			AHtangent = CoefficientH * AH - AM;
			BHtangent = CoefficientH * BH - BM;
			CHtangent = CoefficientH * CH - CM;

			AngleCos = (AHtangent * ALtangent + BHtangent * BLtangent + CHtangent * CLtangent)/
				(sqrt(AHtangent * AHtangent + BHtangent * BHtangent +CHtangent * CHtangent) * 
				sqrt(ALtangent * ALtangent + BLtangent * BLtangent +CLtangent * CLtangent));

			AngleCos = acos(AngleCos);

			ANormalLine = BHtangent * CLtangent - CHtangent * BLtangent;
			BNormalLine = 0 - (AHtangent * CLtangent - CHtangent * ALtangent); 
			CNormalLine = AHtangent * BLtangent - BHtangent * ALtangent;

			if(AM!=0)
			{
				OrientationValue = ANormalLine/AM;
			}
			else if(BM!=0)
			{
				OrientationValue = BNormalLine/BM;
			}
			else
			{
				OrientationValue = CNormalLine/CM;
			}
			if(OrientationValue>0)
			{
				Sum1 += AngleCos;
				Count1 ++;
			}
			else
			{
				Sum2 += AngleCos;
				Count2 ++;
			}
		}
		if(Sum1>Sum2)
		{
			Sum = Sum1+(2*osg::PI*Count2-Sum2);
		}
		else
		{
			Sum = (2*osg::PI*Count1-Sum1)+Sum2;
		}

		totalArea = (Sum-(Count-2)*osg::PI)* osg::WGS_84_RADIUS_EQUATOR * 
			osg::WGS_84_RADIUS_EQUATOR;
	}

	return totalArea;
}

std::vector<std::string> ByyGeoMath::getAnnotationLabelText( osgEarth::Annotation::AnnotationNode* node )
{
	std::vector<std::string> labelTextVec;

	osgEarth::Annotation::EllipseNode* ellipse=dynamic_cast<osgEarth::Annotation::EllipseNode*>(node);
	osgEarth::Annotation::FeatureNode *_featureNode=dynamic_cast<osgEarth::Annotation::FeatureNode*>(node);

	if (ellipse)
	{
		double area=ellipse->getRadiusMajor().as(osgEarth::Units::METERS)*ellipse->getRadiusMinor().as(osgEarth::Units::METERS)*osg::PI;

		bool km2=false;

		if (area>1000000)
		{
			area=area/1000000;

			km2=true;
		}

		QString _text=QString::fromLocal8Bit("%1 %2").arg(area,0,'f',2).arg(QString::fromLocal8Bit(km2?"km2":"m2"));

		labelTextVec.push_back(_text.toUtf8().data());
	}
	else if(_featureNode)
	{
		if (_featureNode->getFeature()->getGeometry()->getType()==osgEarth::Symbology::Geometry::TYPE_LINESTRING)
		{
			std::vector<osg::Vec3d> geoVec=_featureNode->getFeature()->getGeometry()->asVector();

			labelTextVec.resize(geoVec.size()-1);

			double _distance;
			QString _text;
			bool _km;

			while(geoVec.size()>1)
			{
				_distance=osgEarth::GeoMath::distance(geoVec);

				_km=false;

				if (_distance>1000)
				{
					_distance=_distance/1000;
					_km=true;
				}

				_text=QString::fromLocal8Bit("%1 %2").arg(_distance,0,'f',2).arg(QString::fromLocal8Bit(_km?"km":"m"));

				labelTextVec[geoVec.size()-2]=_text.toUtf8().data();

				geoVec.pop_back();
			}
		}
		else if (_featureNode->getFeature()->getGeometry()->getType()==osgEarth::Symbology::Geometry::TYPE_POLYGON)
		{
			const std::vector< osg::Vec3d > &points=_featureNode->getFeature()->getGeometry()->asVector();

			double distance=osgEarth::GeoMath::distance(points);

			distance+=osgEarth::GeoMath::distance(points[0],points[points.size()-1],node->getMapNode()->getMapSRS());

			bool km=false;

			if (distance>1000)
			{
				distance=distance/1000;
				km=true;
			}

			double area=calcPolygonArea(_featureNode->getFeature()->getGeometry()->asVector());

			bool km2=false;

			if (area>1000000)
			{
				area=area/1000000;

				km2=true;
			}

			QString _text=QString::fromLocal8Bit("%1 %2,%3 %4").arg(distance,0,'f',2).arg(km?"km":"m").arg(area,0,'f',2).arg(QString::fromLocal8Bit(km2?"km2":"m2"));

			labelTextVec.push_back(_text.toUtf8().data());
		}
	}

	return labelTextVec;
}

}