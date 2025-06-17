#include "stdafx.h"
#include "sandBoxComponent.h"

#include "mappingQuery.h"

#include "ByyIG.h"
#include "terrain.h"
#include "ByyRenderer.h"

ByySandBoxComponent::ByySandBoxComponent( ByyIG& ig) 
	: d_scale(1.0)
{
	QVector<SandboxElement> d_sandboxElement=ByyMappingQuery::instance()->getSandboxElements();

	for (int i=0;i!=d_sandboxElement.size();++i)
	{
		d_elementsHash.insert(d_sandboxElement[i].id,d_sandboxElement[i]);
	}

	d_placeStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->halo()->color()=osgEarth::Color::Black;
	d_placeStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding()=osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	d_placeStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->font()="../data/fonts/msyh.ttf";
	d_placeStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->occlusionCull()=true;
	d_placeStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;


	_sandboxEventCallback=new ByySandboxEventCallback(sandboxClickCallback,this);

	d_root=new osg::Group;

	d_root->addEventCallback(_sandboxEventCallback);

	ig.renderer()->addNodeToRoot(d_root,ByyOsgRenderer::Terrain);

	connect(ig.terrain(),SIGNAL(terrainLoaded(osgEarth::MapNode*)),SLOT(init(osgEarth::MapNode*)));
}

ByySandBoxComponent::~ByySandBoxComponent()
{

}

void ByySandBoxComponent::init( osgEarth::MapNode* mapNode )
{
	release();

	osgEarth::Config& externals=mapNode->externalConfig();

	osgEarth::Config sandboxs=externals.child("sandboxs");
	const osgEarth::ConfigSet& children = sandboxs.children();

	for(osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i )
	{
		if (!i->empty())
		{
			int _elementNameId=QString::fromUtf8(i->value("element").c_str()).toInt();

			if (!d_elementsHash.contains(_elementNameId))
				continue;

			SandboxElement& _element=d_elementsHash[_elementNameId];

			d_placeStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->url()->setLiteral(_element.iconFilename.toLocal8Bit().data() );
			d_placeStyle.getOrCreate<osgEarth::Symbology::IconSymbol>()->scale()=d_scale;

			osgEarth::GeoPoint _pos(i->child("position"));
			osgEarth::Annotation::PlaceNode *placeNode=new osgEarth::Annotation::PlaceNode(mapNode,osgEarth::GeoPoint(mapNode->getMapSRS(),_pos.x(),_pos.y(),_pos.alt()),i->value("name"), d_placeStyle);
			placeNode->setDynamic(true);

			d_root->addChild(placeNode);

			//osg::StateSet *stateSet=placeNode->getAttachPoint()->getChild(0)->asGeode()->getOrCreateStateSet();
			
			//osg::Depth *depth=dynamic_cast<osg::Depth*>(stateSet->getAttribute(osg::StateAttribute::DEPTH));

			//depth->setRange(0.5,1.0);

			//stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
			//osgEarth::Decluttering::setEnabled( placeNode->getOrCreateStateSet(), true);

			SandboxInfo s;
			s.name=QString::fromUtf8(i->value("name").c_str());
			s._element=&_element;

			d_sanboxInfos.append(s);
		}
	}

	d_changed=false;

	emit initialized();
}

void ByySandBoxComponent::release()
{
	d_root->removeChildren(0,d_root->getNumChildren());

	d_sanboxInfos.clear();

	emit released();
}

void ByySandBoxComponent::addChild( osg::Node* node,const QString& name,int elementId )
{
	d_root->addChild(node);

	SandboxInfo s;
	s.name=name;
	s._element=&d_elementsHash[elementId];

	d_sanboxInfos.append(s);

	d_changed=true;
}
void ByySandBoxComponent::removeChild( int index )
{
	d_root->removeChild(index);
	d_sanboxInfos.remove(index);

	d_changed=true;
}

void ByySandBoxComponent::rename( int index,const QString& name )
{
	d_sanboxInfos[index].name=name;

	dynamic_cast<osgEarth::Annotation::PlaceNode*>(d_root->getChild(index))->setText(name.toUtf8().data());

	d_changed=true;
}
osg::Node* ByySandBoxComponent::getChild( int index )
{
	return d_root->getChild(index);
}

void ByySandBoxComponent::writeToMapNode( osgEarth::MapNode* mapNode )
{
	osgEarth::Config& externals=mapNode->externalConfig();

	externals.remove("sandboxs");

	osgEarth::Config newConf("sandboxs");

	for (int i=0;i!=d_sanboxInfos.count();++i)
	{
		osgEarth::Config _sangboxConf("sandbox");

		_sangboxConf.add("name",d_sanboxInfos[i].name.toUtf8().data());
		_sangboxConf.add("element",d_sanboxInfos[i]._element->id);

		osgEarth::Annotation::PlaceNode *_place=dynamic_cast<osgEarth::Annotation::PlaceNode*>(getChild(i));

		_sangboxConf.addObj("position",_place->getPosition());

		newConf.add(_sangboxConf);
	}
	externals.add(newConf);
}

void ByySandBoxComponent::moved()
{
	d_changed=true;
}

void ByySandBoxComponent::setEnabled( int index,bool enabled )
{
	d_root->getChild(index)->setNodeMask(enabled);
}

void ByySandBoxComponent::setScale( double value )
{
	d_scale=value;

	for (int i=0;i!=d_root->getNumChildren();++i)
	{
		osgEarth::Annotation::PlaceNode *_placeNode=dynamic_cast<osgEarth::Annotation::PlaceNode*>(d_root->getChild(i));

		if (_placeNode)
		{
			osgEarth::Symbology::Style _style=_placeNode->getStyle();

			_style.getOrCreate<osgEarth::Symbology::IconSymbol>()->scale()=value;

			_placeNode->setStyle(_style);
		}
	}
}

void ByySandBoxComponent::sandboxClickCallback( osg::Node* node,void *user )
{
	ByySandBoxComponent *component=static_cast<ByySandBoxComponent*>(user);

	emit component->mouseDown(node);
}

int ByySandBoxComponent::getChildIndex( osg::Node* node )
{
	return d_root->getChildIndex(node);
}

void ByySandBoxComponent::setContextMenuEnabled( bool enabled )
{
	if (_sandboxEventCallback)
	{
		_sandboxEventCallback->setEnabled(enabled);
	}
}


void ByySandboxEventCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
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
				std::set<osgEarth::Annotation::PlaceNode*> fired; // prevent multiple hits on the same instance

				for( osgEarth::IntersectionPicker::Hits::const_iterator h = hits.begin(); h != hits.end(); ++h )
				{
					osgEarth::Annotation::PlaceNode* anno = picker.getNode<osgEarth::Annotation::PlaceNode>( *h );

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
