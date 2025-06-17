#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "localizedCtrlObject.h"

class SNIGCORE_EXPORT ByyEllipse : public ByyLocalizedCtrlObject
{
	Q_OBJECT
public:
	ByyEllipse();
	virtual ~ByyEllipse();

	double majorSemiAxis();
	void setMajorSemiAxis(double val);

	double minorSemiAxis();
	void setMinorSemiAxis(double val);

	double rotationAngle();
	void setRotationAngle(double val);

	virtual const QString& type()const;
	static const QString& theType();

	virtual ByyKeyValueList getConfig();
	virtual void fromConfig(ByyKeyValueList& keyValues);

	virtual QString getAttributeValueSet();

signals:
	void majorSemiAxisChanged(double);
	void minorSemiAxisChanged(double);
	void rotationAngleChanged(double);

private:
	double myMajorSemiAxis;
	double myMinorSemiAxis;
	double myRotationAngle; //DEG
};

#endif // ELLIPSE_H
