#ifndef ENUMS_H
#define ENUMS_H

#include "snigcore_global.h"
#include <QString>

enum ByyDamageState
{
	ByyDamageNone,   
	ByyDamageSlight,  
	//ByyDamageMiddle,  
	ByyDamageModerate,  
	ByyDamageDestroyed 
};

enum ByyForceType
{
	ByyForceOther, 
	ByyForceFriendly,
	ByyForceOpposing,
	ByyForceNeutral,
	ByyForceAll
};

SNIGCORE_EXPORT const QString& forceTypeString( const ByyForceType& ft );

#endif // ENUMS_H