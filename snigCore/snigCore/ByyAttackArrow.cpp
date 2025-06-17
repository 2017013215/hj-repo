#include "ByyAttackArrow.h"

ByyAttackArrow::ByyAttackArrow()
{
	myItemClass="ByyAttackArrow";
}

ByyAttackArrow::~ByyAttackArrow()
{

}

const QString& ByyAttackArrow::type() const
{
	return theType();
}

const QString& ByyAttackArrow::theType()
{
	static QString _type=QString::fromLocal8Bit("½ø¹¥·½Ïò");

	return _type;
}
