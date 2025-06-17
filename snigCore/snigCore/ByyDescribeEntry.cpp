#include "ByyDescribeEntry.h"

ByyDescribeEntry::ByyDescribeEntry()
{

}

ByyDescribeEntry::~ByyDescribeEntry()
{

}

void ByyDescribeEntry::addComponent( const ByyDescribeComponent& component )
{
	myComponents.push_back(component);
}

ByyDescribeEntry::ComponentList ByyDescribeEntry::getComponent( const QString& type )
{
	ComponentList rel;

	for (int i=0;i!=myComponents.size();++i)
	{
		if (myComponents[i].getType().toLower()==type.toLower())
		{
			rel.append(myComponents[i]);
		}
	}

	return rel;
}

const ByyDescribeEntry::ComponentList& ByyDescribeEntry::getComponent()
{
	return myComponents;
}
