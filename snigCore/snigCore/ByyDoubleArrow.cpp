#include "ByyDoubleArrow.h"

ByyDoubleArrow::ByyDoubleArrow()
{
	myItemClass="ByyDoubleArrow";
}

ByyDoubleArrow::~ByyDoubleArrow()
{

}

const QString& ByyDoubleArrow::type() const
{
	return theType();
}

const QString& ByyDoubleArrow::theType()
{
	static QString _type=QString::fromLocal8Bit("Ç¯»÷");

	return _type;
}
