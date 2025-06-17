#include "StdAfx.h"
#include "ByyMenu.h"

ByyMenu::ByyMenu( const QString& objectName ,QWidget * parent)
	:QMenu(parent)
{
	setObjectName(objectName);
}

ByyMenu::~ByyMenu()
{

}