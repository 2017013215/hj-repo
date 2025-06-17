#ifndef CIRCLE_H
#define CIRCLE_H

#include "localizedCtrlObject.h"

class SNIGCORE_EXPORT ByyCircle : public ByyLocalizedCtrlObject
{
	Q_OBJECT
public:
	ByyCircle();
	virtual ~ByyCircle();

	void setRadius(double rad);
	double radius();

	virtual const QString& type()const;
	static const QString& theType();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(ByyKeyValueList& keyValues);

	virtual QString getAttributeValueSet();

signals:
	void radiusChanged(double);

private:
	double myRadius;
};

#endif // CIRCLE_H
