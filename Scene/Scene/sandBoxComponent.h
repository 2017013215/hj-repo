#ifndef SANDBOXCOMPONENT_H
#define SANDBOXCOMPONENT_H

#include <QObject>
#include "scene_global.h"

#include <osgEarthSymbology/Style>
#include <QVector>
#include <QHash>

struct SandboxElement;

struct SandboxInfo
{
	SandboxElement* _element;
	QString name;
};

typedef void (*SANDBOXCLICKCALLBACK)(osg::Node* node,void *user);

class ByySandboxEventCallback : public osg::NodeCallback
{
public:
	ByySandboxEventCallback(SANDBOXCLICKCALLBACK cb,void* user):_callback(cb),_user(user),_enabled(false)
	{

	}

	virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

	void setEnabled(bool enabled){_enabled=enabled;}

protected:
	SANDBOXCLICKCALLBACK _callback;
	void* _user;
	bool _enabled;
};

class ByyIG;

class SCENE_EXPORT ByySandBoxComponent : public QObject
{
	Q_OBJECT

public:
	ByySandBoxComponent(ByyIG& ig);
	~ByySandBoxComponent();

	void addChild(osg::Node* node,const QString& name,int elementId);
	void removeChild(int index);
	
	osg::Node* getChild(int index);

	int getChildIndex(osg::Node* node);

	void setEnabled(int index,bool enabled);

	void rename(int index,const QString& name);

	QVector<SandboxInfo>& getSandboxInfos(){return d_sanboxInfos;}

	virtual void writeToMapNode(osgEarth::MapNode* mapNode);

	void moved();

	QHash<int,SandboxElement>& elementsHash(){return d_elementsHash;}

	void setScale(double value);

	osgEarth::Symbology::Style& getPlaceStyle(){return d_placeStyle;}

	static void sandboxClickCallback(osg::Node* node,void *user);

	void setContextMenuEnabled(bool enabled);

public slots:
	virtual void init(osgEarth::MapNode* mapNode);
	virtual void release();

signals:
	void mouseDown(osg::Node*);
	void initialized();
	void released();

private:
	QVector<SandboxInfo> d_sanboxInfos;

	QHash<int,SandboxElement> d_elementsHash;

	double d_scale;

	osgEarth::Symbology::Style d_placeStyle;

	osgUtil::LineSegmentIntersector::Intersections intersections;

	osg::ref_ptr<ByySandboxEventCallback> _sandboxEventCallback;

	osg::ref_ptr<osg::Group> d_root;

	bool d_changed;
	//osg::Node* d_currentNode;
};

#endif // SANDBOXCOMPONENT_H
