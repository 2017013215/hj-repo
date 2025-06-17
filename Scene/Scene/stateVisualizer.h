#ifndef STATEVISUALIZER_H
#define STATEVISUALIZER_H

#include <QObject>

class ByyEntityObject;
class ByyIG;

class ByyStateVisualizer : public QObject
{
	Q_OBJECT
public:
	ByyStateVisualizer(ByyEntityObject& entObj,ByyIG& ig);
	virtual ~ByyStateVisualizer();

	virtual const QString& type()=0;

	virtual osg::Group* root();

	osgEarth::MapNode* mapNode();

	virtual bool contributesToBound()const;  

	virtual bool isLocalized()const;

	ByyEntityObject& getEntObj();

public slots:
	virtual void setVisible(bool v);

protected:
	ByyEntityObject& myEntObj;
	ByyIG& myIG;

	osg::ref_ptr<osg::Group> myRoot;

	osg::observer_ptr<osgEarth::MapNode> myMapNode;
};


class ByyStateVisualizerCreator
{
public:
	virtual ~ByyStateVisualizerCreator()
	{

	}

	virtual ByyStateVisualizer* createVisualizer(ByyEntityObject& entObj,ByyIG& ig)=0;
	virtual const QString& type()=0;
};

template<class T>
class ByyStateVisualizerCreatorTemplate : public ByyStateVisualizerCreator
{
public:
	virtual ByyStateVisualizer* createVisualizer(ByyEntityObject& entObj,ByyIG& ig)
	{
		return new T(entObj,ig);
	}

	virtual const QString& type()
	{
		return T::theType();
	}
};

#endif // STATEVISUALIZER_H
