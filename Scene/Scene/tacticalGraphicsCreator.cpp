#include "StdAfx.h"
#include "tacticalGraphicsCreator.h"
#include "ByyIG.h"
#include "terrain.h"
#include "ByyRenderer.h"
#include "ByyInputController.h"

ByyTacticalGraphicsCreator::ByyTacticalGraphicsCreator( ByyIG& ig)
	:myIG(ig)
	,myMapNode(myIG.terrain()->mapNode())
	,myRoot(ig.renderer()->getPropRoot())
	/*, myPathColor(osgEarth::Color(osgEarth::Color::White, 0.0))
	, myFillColor(osgEarth::Color(osgEarth::Color::Black, 0.5))*/
	, myPathColor(osgEarth::Symbology::Color::Red)
	, myFillColor(0,1,0,0.5)
	, myEventProcessor(this)
{
	myIG.inputController().addEventProcessor(&myEventProcessor);
}

ByyTacticalGraphicsCreator::~ByyTacticalGraphicsCreator()
{
	myIG.inputController().removeEventProcessor(&myEventProcessor);
}

bool ByyTacticalGraphicsCreator::processEvent( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	switch(ea.getEventType())
	{
	case ea.PUSH:
		myMouseDown=ea.getButton();
		myXDown=ea.getX();
		myYDown=ea.getY();
		return true;
	case ea.RELEASE:
		if (ea.getButton()==myMouseDown&&ea.getX()==myXDown&&ea.getY()==myYDown)
		{
			return mouseClicked(ea.getButton(),ea.getX(),ea.getY(),aa.asView());
		}
		return false;
	case ea.MOVE:
		return mouseMove(ea.getButton(),ea.getX(),ea.getY(),aa.asView());
	}

	return false;
}

bool ByyTacticalGraphicsCreator::mouseClicked( int button,int x,int y,osg::View *view )
{
	return false;
}

bool ByyTacticalGraphicsCreator::mouseMove( int button,int x,int y,osg::View *view )
{
	return false;
}

ByySceneObjCreateVisualizer::ByySceneObjCreateVisualizer( ByyIG& ig )
	:myMapNode(ig.terrain()->mapNode())
	,myRoot(ig.renderer()->getPropRoot())
	/*, myPathColor(osgEarth::Color(osgEarth::Color::White, 0.0))
	, myFillColor(osgEarth::Color(osgEarth::Color::Black, 0.5))*/
	, myPathColor(osgEarth::Symbology::Color::Red)
	, myFillColor(0,1,0,0.5)
{

}
