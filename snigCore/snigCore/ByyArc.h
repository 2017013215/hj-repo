#pragma once
#include "byycircle.h"
class SNIGCORE_EXPORT ByyArc :	public ByyCircle
{
	Q_OBJECT
public:
	ByyArc(void);
	~ByyArc(void);

	virtual const QString& type()const;
	static const QString& theType();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(ByyKeyValueList& keyValues);

	virtual QString getAttributeValueSet();

	void setStartAngle(double angle);
	double startAngle(){return myStartAngle;}
	void setEndAngle(double angle);
	double endAngle(){return myEndAngle;}
signals:
	void startAngleChanged(double);
	void endAngleChanged(double);
private:
	double	myStartAngle;
	double	myEndAngle;
};

