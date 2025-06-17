#ifndef ANNOTATIONCOMPONENT_H
#define ANNOTATIONCOMPONENT_H

#include <QObject>
#include "scene_global.h"
#include "ByyEventProcessor.h"

#include <osgEarthSymbology/Style>

class ByyAltitudeCullingCallback;

namespace osgEarth
{
	namespace Annotation
	{
		class AnnotationNode;
		class PlaceNode;
		class FeatureNode;
	}
}

typedef void (*ANNOCLICKCALLBACK)(osgEarth::Annotation::AnnotationNode* node,void *user);

class ByyAnnotationEventCallback : public osg::NodeCallback
{
public:
	ByyAnnotationEventCallback(ANNOCLICKCALLBACK cb,void* user):_callback(cb),_user(user),_enabled(false)
	{

	}

	virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

	void setEnabled(bool enable){_enabled=enable;}

protected:
	ANNOCLICKCALLBACK _callback;
	void* _user;

	bool _enabled;
};

class ByyIG;

class SCENE_EXPORT ByyAnnotationComponent : public QObject
{
	Q_OBJECT

public:
	ByyAnnotationComponent(ByyIG& ig);
	~ByyAnnotationComponent();

	void addChild(osgEarth::Annotation::AnnotationNode* node);
	void removeChild(osgEarth::Annotation::AnnotationNode* node);

	virtual void writeToMapNode(osgEarth::MapNode* mapNode);

	void setChanged(){d_changed=true;}

	static void annoClickCallback(osgEarth::Annotation::AnnotationNode* node,void *user);

	void setConextMenuEnabled(bool eanble);

	osg::Group* annoGroup(){return d_annoGroup;}

	void setLabelText(const std::vector<std::string>& textVec);
	void setLabelPosition(const std::vector<osg::Vec3d>& positionVec);
	void setLabelVisible(bool visible);

	void updatePathPlacePosition(const osg::Vec3d& position);

	void setPathHeightEnabled(bool enabled,osgEarth::Annotation::FeatureNode* node=NULL);

	void updateLabel(osgEarth::Annotation::AnnotationNode* annoNode);

public slots:
	void init(osgEarth::MapNode *mapNode);
	void release();

signals:
	void mouseDown(osg::Node*);
	void initialized();
	void released();

private:
	void resizeLabelGroup(int size);
	bool processEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

	osgEarth::Annotation::PlaceNode* getOrCreatePathHeightPlaceNode();

	void createLabel(osgEarth::Annotation::AnnotationNode* annoNode);
	

	typedef ByyHostEventProcessor<ByyAnnotationComponent> AnnoEventProcessor;
	friend class AnnoEventProcessor;

private:
	ByyIG &myIG;
	osg::ref_ptr<ByyAnnotationEventCallback> _annoEventCallback;
	osg::ref_ptr<ByyAltitudeCullingCallback> _altitudeCullingCallback;

	osg::ref_ptr<osg::Group> d_annoGroup;
	osg::ref_ptr<osg::Group> myInfoGroup;
	osg::ref_ptr<osg::Group> myLabelGroup;
	osg::observer_ptr<osgEarth::MapNode> d_mapNode;
	QMap<osgEarth::Annotation::AnnotationNode*,osgEarth::Annotation::PlaceNode*> myLabelMap;
	osg::ref_ptr<osgEarth::Annotation::PlaceNode> d_pathHeightPlaceNode;

	osgEarth::Symbology::Style infoStyle;

	bool _enabledPathHeight;

	osg::observer_ptr<osgEarth::Annotation::FeatureNode> _currentPathHeightNode;

	std::vector<double> _currentPathHeightRatioVec;

	bool _recalcPathHeightRatio;

	bool d_changed;

	osg::ref_ptr<osg::Group> d_root;

	AnnoEventProcessor myEventProcessor;
};

#endif // ANNOTATIONCOMPONENT_H
