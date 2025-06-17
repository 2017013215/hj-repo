#ifndef BYYMEASUREITEM_H
#define BYYMEASUREITEM_H

#include <QObject>
#include "ByyEventProcessor.h"

class ByyIG;

class ByyMeasureItem : public QObject
{
	Q_OBJECT
public:
	ByyMeasureItem(ByyIG& ig);
	virtual ~ByyMeasureItem();

	virtual void addPoint(const osgEarth::GeoPoint& point,bool refresh=true);

	virtual void mouseMove(const osgEarth::GeoPoint& point);

protected:
	virtual bool processEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
	virtual bool mouseClicked(int button,int x,int y,osg::View *view);
	virtual bool mouseMove(int button,int x,int y,osg::View *view);

signals:
	void finished();

protected:
	ByyIG& myIG;

	typedef ByyHostEventProcessor<ByyMeasureItem> MyEventProcessor;
	friend class MyEventProcessor;

	MyEventProcessor myEventProcessor;

	int myMouseDown;
	float myXDown;
	float myYDown;

};

class ByyMeasureItemCreator
{
public:
	ByyMeasureItemCreator(){}
	virtual ~ByyMeasureItemCreator(){}

	virtual ByyMeasureItem* create(ByyIG& ig)=0;
};

#endif // BYYMEASUREITEM_H
