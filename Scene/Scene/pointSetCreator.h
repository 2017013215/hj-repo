#ifndef POINTSETCREATOR_H
#define POINTSETCREATOR_H

#include "tacticalGraphicsCreator.h"

class ByyPointSetCreator : public ByyTacticalGraphicsCreator
{
	Q_OBJECT

public:
	ByyPointSetCreator(ByyIG& ig);
	virtual ~ByyPointSetCreator();

	virtual void addPoint(const osgEarth::GeoPoint& point,bool refresh=true);

	virtual void mouseMove(const osgEarth::GeoPoint& point);
	virtual void movePoint(int index, const osgEarth::GeoPoint& position){}

	//virtual void setPoints(const ByyVec3dArray& points);
	virtual ByyVec3dArray getPoints();

	virtual unsigned int getPointCount();
	virtual unsigned int getMinPointCount()=0;

	virtual void update(const ByyVec3dArray& points)=0;

	struct PointDraggerCallback : public osgEarth::Annotation::Dragger::PositionChangedCallback
	{
		PointDraggerCallback(int index, ByyPointSetCreator* editor)
			: _index(index), myEditor(editor)
		{
		}

		void onPositionChanged(const osgEarth::Annotation::Dragger* sender, const osgEarth::GeoPoint& position)
		{
			myEditor->movePoint(_index, position);
		}

		int _index;
		ByyPointSetCreator* myEditor;
	};

protected:
	virtual bool mouseClicked(int button,int x,int y,osg::View *view);
	virtual bool mouseMove(int button,int x,int y,osg::View *view);

	void createPointDragger(int index, const osgEarth::GeoPoint& point);

protected:
	osg::ref_ptr<osg::Group> myDraggers;
	ByyVec3dArray myPoints;
};

class ByyFixedPointSetCreator : public ByyPointSetCreator
{
public:
	ByyFixedPointSetCreator(ByyIG& ig);
	virtual ~ByyFixedPointSetCreator();

	virtual unsigned int getFixedPointCount()=0;

	virtual unsigned int getMinPointCount(){return getFixedPointCount();}

	virtual void addPoint(const osgEarth::GeoPoint& point,bool refresh=true);

	virtual void mouseMove(const osgEarth::GeoPoint& point);
};

#endif // POINTSETCREATOR_H
