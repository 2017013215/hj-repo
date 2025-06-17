#include "ByyAbsDescribe.h"

ByyAbsDescribe::ByyAbsDescribe()
{

}

ByyAbsDescribe::~ByyAbsDescribe()
{

}

QString ByyAbsDescribe::getItemClass() const
{
	return myItemClass;
}

QString ByyAbsDescribe::getType() const
{
	return myType;
}

QString ByyAbsDescribe::getTag() const
{
	return myTag;
}

void ByyAbsDescribe::setItemClass( const QString& val )
{
	myItemClass=val;
}

void ByyAbsDescribe::setType( const QString& val )
{
	myType=val;
}

void ByyAbsDescribe::setTag( const QString& val )
{
	myTag=val;
}
