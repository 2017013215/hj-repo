#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "ByyPolygon.h"

class SNIGCORE_EXPORT ByyRectangle : public ByyPolygon
{
	Q_OBJECT
public:
	ByyRectangle();
	virtual ~ByyRectangle();

	virtual const QString& type()const;
	static const QString& theType();

	virtual QString getAttributeValueSet();

	bool operator!=(const ByyRectangle& e);

	bool operator==(const ByyRectangle& e);

	ByyRectangle& operator=(const ByyRectangle& e);
};

#endif // RECTANGLE_H
