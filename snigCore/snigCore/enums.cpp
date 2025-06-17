#include "enums.h"

const QString& forceTypeString( const ByyForceType& ft )
{
	switch(ft)
	{
	case ByyForceOpposing:
		{
			static QString opposingString=QString::fromLocal8Bit("蓝方");
			return opposingString;
		}
	case ByyForceFriendly:
		{
			static QString friendlyString=QString::fromLocal8Bit("红方");
			return friendlyString;
		}
	case ByyForceNeutral:
		{
			static QString neutralString=QString::fromLocal8Bit("中立");
			return neutralString;
		}
	}

	static QString otherString=QString::fromLocal8Bit("其他");
	return otherString;
}