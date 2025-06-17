#include "ByyGatheringPlace.h"

ByyGatheringPlace::ByyGatheringPlace()
{
	myItemClass="ByyGatheringPlace";
}

ByyGatheringPlace::~ByyGatheringPlace()
{

}

const QString& ByyGatheringPlace::type() const
{
	return theType();
}

const QString& ByyGatheringPlace::theType()
{
	static QString _type=QString::fromLocal8Bit("¾Û¼¯µØ");

	return _type;
}
