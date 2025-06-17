#include "StdAfx.h"
#include "tacticalGraphEditor.h"
#include "ByyControlObject.h"

ByyTacticalGraphEditor::ByyTacticalGraphEditor( ByyIG& ig,ByyControlObject* obj )
	:ByyTacticalGraphicsCreator(ig)
	,myObj(obj)
{
	myObj->setVisible(false);
}

ByyTacticalGraphEditor::~ByyTacticalGraphEditor()
{
	myObj->setVisible(true);
}

bool ByyTacticalGraphEditor::mouseClicked( int button,int x,int y,osg::View *view )
{
	if (button==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{
		emit finished(Ok);
		return true;
	}

	return false;
}
