#include "StdAfx.h"
#include "objectIconManager.h"

ByyObjectIconManager::ByyObjectIconManager()
{
	myIcons.insert(ByyForceOther,QIcon("../data/icons/sym2525a_landingshipF_green.png"));
	myIcons.insert(ByyForceFriendly,QIcon("../data/icons/sym2525a_landingshipF_red.png"));
	myIcons.insert(ByyForceOpposing,QIcon("../data/icons/sym2525a_landingshipF_blue.png"));
	myIcons.insert(ByyForceNeutral,QIcon("../data/icons/sym2525a_landingshipF_green.png"));
}

ByyObjectIconManager::~ByyObjectIconManager()
{

}

ByyObjectIconManager& ByyObjectIconManager::instance()
{
	static ByyObjectIconManager objManager;
	return objManager;
}

QIcon ByyObjectIconManager::getIcon( ByyForceType type )
{
	return myIcons[type];
}
