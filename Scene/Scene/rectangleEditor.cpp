#include "StdAfx.h"
#include "rectangleEditor.h"
#include "ByyRectangle.h"

ByyRectangleEditor::ByyRectangleEditor( ByyIG& ig,ByyRectangle *rectangle )
	:ByyTacticalGraphEditor(ig,rectangle)
	,myRectangle(rectangle)
{
	osgEarth::Symbology::Style myRectStyle;

	myRectStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = myFillColor;
	myRectStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 0.0;
	myRectStyle.getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = myFillColor;
	myRectStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
	myRectStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;

	osgEarth::Bounds bd;

	ByyVec3dArray pnts=myRectangle->points();

	QList<double> lons,lats;

	if (pnts.size()>=4)
	{
		for (int i=0;i!=4;++i)
		{
			lons.push_back(pnts[i].x());
			lats.push_back(pnts[i].y());

			bd.expandBy(pnts[i].x(),pnts[i].y());
		}

		qSort(lons.begin(),lons.end());
		qSort(lats.begin(),lats.end());


		myRectNode=new osgEarth::Annotation::RectangleNode(myMapNode.get(),osgEarth::GeoPoint(myMapNode->getMapSRS(),bd.center2d().x(),bd.center2d().y()),
			osgEarth::Distance(),osgEarth::Distance(),myRectStyle);

		myRectNode->setUpperLeft(osgEarth::GeoPoint(myMapNode->getMapSRS(),lons.first(),lats.last()));
		myRectNode->setLowerLeft(osgEarth::GeoPoint(myMapNode->getMapSRS(),lons.first(),lats.first()));
		myRectNode->setLowerRight(osgEarth::GeoPoint(myMapNode->getMapSRS(),lons.last(),lats.first()));
		myRectNode->setUpperRight(osgEarth::GeoPoint(myMapNode->getMapSRS(),lons.last(),lats.last()));

		myRoot->addChild(myRectNode);

		myEditor=new osgEarth::Annotation::RectangleNodeEditor(myRectNode);

		myRoot->addChild(myEditor);
	}
}

ByyRectangleEditor::~ByyRectangleEditor()
{
	myRoot->removeChild(myRectNode);
	myRoot->removeChild(myEditor);
}

ByyControlObject* ByyRectangleEditor::accept()
{
	QVector<osg::Vec3d> result;

	result.append(myRectNode->getUpperLeft().vec3d());
	result.append(myRectNode->getLowerLeft().vec3d());
	result.append(myRectNode->getLowerRight().vec3d());
	result.append(myRectNode->getUpperRight().vec3d());

	//myRectangle->setPoints(result);
	myRectangle->setPoints_xy(result);  // 20240708, ÷ª…Ë÷√xy÷µ

	return myRectangle;
}

ByyTacticalGraphicsCreator* ByyRectangleEditor::create( ByyIG& ig,ByyControlObject* obj )
{
	ByyRectangle *rectangle=dynamic_cast<ByyRectangle*>(obj);

	if (rectangle)
	{
		return new ByyRectangleEditor(ig,rectangle);
	}

	return 0;
}
