#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "localizedCtrlObject.h"

class SNIGCORE_EXPORT ByyWaypoint : public ByyLocalizedCtrlObject
{
	Q_OBJECT
public:
	ByyWaypoint();
	virtual ~ByyWaypoint();

	virtual const QString& type()const;
	static const QString& theType();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(ByyKeyValueList& keyValues);

	virtual QString getAttributeValueSet();
};

#endif // WAYPOINT_H
