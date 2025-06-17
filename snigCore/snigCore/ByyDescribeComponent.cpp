#include "ByyDescribeComponent.h"

ByyDescribeComponent::ByyDescribeComponent()
{

}

ByyDescribeComponent::~ByyDescribeComponent()
{

}

void ByyDescribeComponent::setParamList( const ByyDescParamList& paramList )
{
	myParamList=paramList;
}

const ByyDescParamList& ByyDescribeComponent::getParamList()const
{
	return myParamList;
}
