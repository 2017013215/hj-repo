#include "ByyFineArrow.h"

ByyFineArrow::ByyFineArrow()
{
	myItemClass="ByyFineArrow";
}

ByyFineArrow::~ByyFineArrow()
{

}

const QString& ByyFineArrow::type() const
{
	return theType();
}

const QString& ByyFineArrow::theType()
{
	static QString _type=QString::fromLocal8Bit("¸¨¹¥Ïß");

	return _type;
}
