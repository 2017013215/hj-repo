#include "stdafx.h"
#include "annotationComponent.h"
#include "altitudeCullingCallback.h"

#include <osgEarthAnnotation/AnnotationRegistry>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/LabelNode>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/AnnotationData>

#include "ByyIG.h"
#include "ByyRenderer.h"
#include "terrain.h"
#include "ByyInputController.h"

ByyAnnotationComponent::ByyAnnotationComponent(ByyIG& ig)
	:myIG(ig)
	,myEventProcessor(this)
{
	d_annoGroup=new osg::Group;
	myInfoGroup=new osg::Group;
	myLabelGroup=new osg::Group;

	d_root=new osg::Group;
	ig.renderer()->addNodeToRoot(d_root,ByyOsgRenderer::Terrain);
	d_root->addChild(d_annoGroup);
	d_root->addChild(myInfoGroup);
	d_root->addChild(myLabelGroup);

	_annoEventCallback=new ByyAnnotationEventCallback(annoClickCallback,this);
	d_annoGroup->addEventCallback(_annoEventCallback);

	infoStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->fill()=osgEarth::Color("#ffff00");
	infoStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_CENTER_CENTER;
	infoStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding()=osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	infoStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->font()="../data/fonts/msyh.ttf";
	infoStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;

	connect(ig.terrain(),SIGNAL(terrainLoaded(osgEarth::MapNode*)),SLOT(init(osgEarth::MapNode*)));

	ig.inputController().addEventProcessor(&myEventProcessor);
}

ByyAnnotationComponent::~ByyAnnotationComponent()
{
	myIG.inputController().removeEventProcessor(&myEventProcessor);
}

void ByyAnnotationComponent::init( osgEarth::MapNode *mapNode )
{
	release();

	_enabledPathHeight=false;

	d_mapNode=mapNode;

	_altitudeCullingCallback=new ByyAltitudeCullingCallback(d_mapNode->getMapSRS(),55000);

	osgEarth::Config& externals=mapNode->externalConfig();

	osgEarth::Config snannotations=externals.child("snannotations");

	osg::Group *g=d_annoGroup.get();

	osgEarth::Annotation::AnnotationRegistry::instance()->create(mapNode,snannotations,osgEarth::Registry::instance()->cloneOrCreateOptions(),g);

	const osgEarth::ConfigSet& children = snannotations.children();

	int j=0;

	for(osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i,++j )
	{
		if (!i->empty())
		{
			osgEarth::Annotation::AnnotationNode* annoNode=dynamic_cast<osgEarth::Annotation::AnnotationNode*>(d_annoGroup->getChild(j));

			annoNode->setName(i->value("name"));
			createLabel(annoNode);
		}
	}

	d_changed=false;

	emit initialized();
}

void ByyAnnotationComponent::release()
{
	osgEarth::clearChildren(d_annoGroup);
	osgEarth::clearChildren(myInfoGroup);
	osgEarth::clearChildren(myLabelGroup);

	myLabelMap.clear();

	_altitudeCullingCallback=NULL;

	if (d_pathHeightPlaceNode.valid())
	{
		d_root->removeChild(d_pathHeightPlaceNode);
		d_pathHeightPlaceNode=NULL;
	}

	emit released();
}

void ByyAnnotationComponent::addChild( osgEarth::Annotation::AnnotationNode* node )
{
	if (d_annoGroup->addChild(node))
	{
		createLabel(node);
		d_changed=true;
	}
}

void ByyAnnotationComponent::writeToMapNode( osgEarth::MapNode* mapNode )
{
	osgEarth::Config& externals=mapNode->externalConfig();

	externals.remove("snannotations");

	osgEarth::Config newConf("snannotations");

	for (int i=0;i!=d_annoGroup->getNumChildren();++i)
	{
		osgEarth::Annotation::AnnotationNode* anno=dynamic_cast<osgEarth::Annotation::AnnotationNode*>(d_annoGroup->getChild(i));
		osgEarth::Config conf=anno->getConfig();

		osgEarth::Annotation::FeatureNode* featureNode=dynamic_cast<osgEarth::Annotation::FeatureNode*>(anno);

		if (featureNode)
		{
			osgEarth::Symbology::LineString* lineString=dynamic_cast<osgEarth::Symbology::LineString*>(featureNode->getFeature()->getGeometry());

			if (lineString)
			{
				QStringList _points;

				for (int i=0;i!=lineString->size();++i)
				{
					_points.append(QString("%1 %2").arg(lineString->at(i).x()).arg(lineString->at(i).y()));
				}

				QString _str=QString("LINESTRING(%1)").arg(_points.join(","));

				conf.set("geometry",_str.toStdString());
			}
		}


		conf.add("name",anno->getName().c_str());
		newConf.add(conf);
	}
	externals.add(newConf);
}

void ByyAnnotationComponent::removeChild( osgEarth::Annotation::AnnotationNode* node )
{
	if (d_annoGroup->removeChild(node))
	{
		if(myLabelMap.contains(node))
		{
			myLabelGroup->removeChild(myLabelMap[node]);
			myLabelMap.remove(node);
		}
		d_changed=true;
	}
}

void ByyAnnotationComponent::annoClickCallback( osgEarth::Annotation::AnnotationNode* node,void *user )
{
	ByyAnnotationComponent *annoComponent=static_cast<ByyAnnotationComponent*>(user);

	emit annoComponent->mouseDown(node);
}

void ByyAnnotationComponent::setConextMenuEnabled( bool eanble )
{
	if (_annoEventCallback)
	{
		_annoEventCallback->setEnabled(eanble);
	}
}

void ByyAnnotationComponent::setLabelText( const std::vector<std::string>& textVec )
{
	resizeLabelGroup(textVec.size());

	for (int i=0;i!=textVec.size();++i)
	{
		osgEarth::Annotation::LabelNode* label=dynamic_cast<osgEarth::Annotation::LabelNode*>(myInfoGroup->getChild(i));

		label->setText(textVec[i]);
	}
}

void ByyAnnotationComponent::setLabelPosition( const std::vector<osg::Vec3d>& positionVec )
{
	resizeLabelGroup(positionVec.size());

	for (int i=0;i!=positionVec.size();++i)
	{
		osgEarth::Annotation::LabelNode* label=dynamic_cast<osgEarth::Annotation::LabelNode*>(myInfoGroup->getChild(i));

		label->setPosition(osgEarth::GeoPoint(d_mapNode->getMapSRS(),positionVec[i]));
	}

	getOrCreatePathHeightPlaceNode()->setNodeMask(0);

	_recalcPathHeightRatio=true;
}

void ByyAnnotationComponent::setLabelVisible( bool visible )
{
	if (myInfoGroup.valid())
	{
		myInfoGroup->setNodeMask(visible);
	}
}

void ByyAnnotationComponent::resizeLabelGroup( int size )
{
	while(size>myInfoGroup->getNumChildren())
	{
		infoStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->content()=osgEarth::StringExpression("");
		osgEarth::Annotation::LabelNode *textLabelNode=new osgEarth::Annotation::LabelNode(d_mapNode.get(),infoStyle);
		textLabelNode->setDynamic(true);
		textLabelNode->setCullCallback(_altitudeCullingCallback);

		myInfoGroup->addChild(textLabelNode);
	}

	for (int i=0;i!=myInfoGroup->getNumChildren();++i)
	{
		myInfoGroup->getChild(i)->setNodeMask(i<size);
	}
}

void ByyAnnotationComponent::updatePathPlacePosition( const osg::Vec3d& position )
{
	getOrCreatePathHeightPlaceNode()->setPosition(osgEarth::GeoPoint(d_mapNode->getMapSRS(),position.x(),position.y()));

	double _height=0;

	d_mapNode->getTerrain()->getHeight(d_mapNode->getMapSRS(),position.x(),position.y(),&_height);

	getOrCreatePathHeightPlaceNode()->setText(QString("%1m").arg((int)_height).toStdString());

}

bool ByyAnnotationComponent::processEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	if (_enabledPathHeight&&ea.getEventType()==ea.MOVE&&_currentPathHeightNode.valid())
	{
		osg::ref_ptr<osgEarth::Symbology::Geometry> geom=_currentPathHeightNode->getFeature()->getGeometry();

		if (geom->getType()==geom->TYPE_POLYGON)
		{
			osgEarth::Symbology::Polygon *_polygon=new osgEarth::Symbology::Polygon(&geom->asVector());
			_polygon->close();

			geom=_polygon;
		}

		if (_recalcPathHeightRatio)
		{
			std::vector<osg::Vec3d> points=geom->asVector();

			_currentPathHeightRatioVec.resize(points.size());

			double _totalDistance=osgEarth::GeoMath::distance(points);

			_currentPathHeightRatioVec.front()=0;
			_currentPathHeightRatioVec.back()=1;

			points.pop_back();

			while(points.size()>1)
			{
				double _distance=osgEarth::GeoMath::distance(points);
				_currentPathHeightRatioVec[points.size()-1]=_distance/_totalDistance;

				points.pop_back();
			}

			_recalcPathHeightRatio=false;

			getOrCreatePathHeightPlaceNode()->setNodeMask(1);
		}
		double ratio=(ea.getX()-ea.getXmin())/(ea.getXmax()-ea.getXmin());

		int _index;

		for (_index=1;_index<_currentPathHeightRatioVec.size();++_index)
		{
			if (ratio<=_currentPathHeightRatioVec[_index])
			{
				break;
			}
		}
	 
		if (_index!=_currentPathHeightRatioVec.size())
		{
			double out_latRad,out_lonRad;
			const osg::Vec3d& _start=geom->at(_index-1);
			const osg::Vec3d& _end=geom->at(_index);

			double _subRatio=(ratio-_currentPathHeightRatioVec[_index-1])/(_currentPathHeightRatioVec[_index]-_currentPathHeightRatioVec[_index-1]);

			osgEarth::GeoMath::interpolate(osg::DegreesToRadians(_start.y()),osg::DegreesToRadians(_start.x()),
				osg::DegreesToRadians(_end.y()),osg::DegreesToRadians(_end.x()),_subRatio,out_latRad,out_lonRad);

			updatePathPlacePosition(osg::Vec3d(osg::RadiansToDegrees(out_lonRad),osg::RadiansToDegrees(out_latRad),0));
		}

	}

	return false;
}

void ByyAnnotationComponent::setPathHeightEnabled( bool enabled,osgEarth::Annotation::FeatureNode* node/*=NULL*/ )
{
	getOrCreatePathHeightPlaceNode()->setNodeMask(enabled&&node==_currentPathHeightNode.get());

	_currentPathHeightNode=node;
	
	_enabledPathHeight=enabled;
}

osgEarth::Annotation::PlaceNode* ByyAnnotationComponent::getOrCreatePathHeightPlaceNode()
{
	if (!d_pathHeightPlaceNode.valid())
	{
		osgEarth::Symbology::Style d_placeStyle;

		d_placeStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=osgEarth::Color::Black;
		//d_placeStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding()=osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
		//d_placeStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->font()="../data/fonts/msyh.ttf";
		d_placeStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->occlusionCull()=true;
		d_placeStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;

		osg::Image* image=osgDB::readImageFile("../data/icons/placemark32.png");

		d_pathHeightPlaceNode=new osgEarth::Annotation::PlaceNode(d_mapNode.get(),osgEarth::GeoPoint(),image,"",d_placeStyle);
		d_pathHeightPlaceNode->setDynamic(true);
		d_pathHeightPlaceNode->setCullCallback(_altitudeCullingCallback);
		d_pathHeightPlaceNode->setNodeMask(0);

		d_root->addChild(d_pathHeightPlaceNode);
	}

	return d_pathHeightPlaceNode;
}

void ByyAnnotationComponent::createLabel( osgEarth::Annotation::AnnotationNode* annoNode )
{
	osgEarth::Symbology::Style labelStyle=infoStyle;
	labelStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->pixelOffset()=osg::Vec2s(20,50);

	osgEarth::Annotation::PlaceNode *textLabelNode=new osgEarth::Annotation::PlaceNode(d_mapNode.get(),osgEarth::GeoPoint(),"",labelStyle);
	textLabelNode->setDynamic(true);
	myLabelGroup->addChild(textLabelNode);


	myLabelMap.insert(annoNode,textLabelNode);

	updateLabel(annoNode);
}

void ByyAnnotationComponent::updateLabel( osgEarth::Annotation::AnnotationNode* annoNode )
{
	if (!myLabelMap.contains(annoNode))return;

	myLabelMap[annoNode]->setText(annoNode->getName());

	osgEarth::Annotation::EllipseNode *ellipseNode=dynamic_cast<osgEarth::Annotation::EllipseNode*>(annoNode);

	if (ellipseNode)
	{
		myLabelMap[annoNode]->setPosition(ellipseNode->getPosition());
		return;
	}
	
	osgEarth::Annotation::FeatureNode *featureNode=dynamic_cast<osgEarth::Annotation::FeatureNode*>(annoNode);

	if (!featureNode)return;

	osg::Vec3d firstPoint=featureNode->getFeature()->getGeometry()->at(0);
	myLabelMap[annoNode]->setPosition(osgEarth::GeoPoint(d_mapNode->getMapSRS(),firstPoint));
}

void ByyAnnotationEventCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
	if (!_enabled)
		return;

	osgGA::EventVisitor* ev = static_cast<osgGA::EventVisitor*>(nv);
	osgGA::EventQueue::Events& events = ev->getEvents();

	osgViewer::View* view = static_cast<osgViewer::View*>(ev->getActionAdapter());

	for( osgGA::EventQueue::Events::const_iterator e = events.begin(); e != events.end(); ++e )
	{
		osgGA::GUIEventAdapter* ea = dynamic_cast<osgGA::GUIEventAdapter*>(e->get());

		if (ea&& ea->getEventType() == osgGA::GUIEventAdapter::RELEASE &&ea->getButton()==ea->RIGHT_MOUSE_BUTTON)
		{
			osgEarth::IntersectionPicker picker( view, node );

			osgEarth::IntersectionPicker::Hits hits;
			if ( picker.pick( ea->getX(), ea->getY(), hits ) )
			{
				std::set<osgEarth::Annotation::AnnotationNode*> fired; // prevent multiple hits on the same instance

				for( osgEarth::IntersectionPicker::Hits::const_iterator h = hits.begin(); h != hits.end(); ++h )
				{
					osgEarth::Annotation::AnnotationNode* anno = picker.getNode<osgEarth::Annotation::AnnotationNode>( *h );

					if ( anno && fired.find(anno) == fired.end() )
					{
						_callback(anno,_user);
						fired.insert( anno );
						//break;
					}
				}
			}
		}

	}

	traverse(node,nv);
}