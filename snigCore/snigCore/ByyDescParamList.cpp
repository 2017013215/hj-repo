#include "ByyDescParamList.h"

ByyDescParamList::ByyDescParamList(const QString& name)
	:myName(name)
{

}

ByyDescParamList::~ByyDescParamList()
{

}

void ByyDescParamList::setName( const QString& name )
{
	myName=name;
}

QString ByyDescParamList::getName() const
{
	return myName;
}

void ByyDescParamList::addParameter( const QString& name,const QString& value )
{
	myParameters.insert(name,value);

	myOrder.append(name);
}

int ByyDescParamList::count() const
{
	return myOrder.size();
}

QString ByyDescParamList::value( const QString& name )const
{
	QString rel;

	Key_Value_List::iterator it=myParameters.find(name);

	if (it!=myParameters.end())
	{
		rel=it.value();
	}

	return rel;
}

QString ByyDescParamList::value( int index ) const
{
	QString rel;

	if (index<myOrder.size())
	{
		rel=myParameters[myOrder[index]];
	}
	return rel;
}

QString ByyDescParamList::name( int index ) const
{
	QString rel;

	if (index<myOrder.size())
	{
		rel=myOrder[index];
	}
	return rel;
}

bool ByyDescParamList::hasParameter( const QString& name )
{
	Key_Value_List::iterator it=myParameters.find(name);

	return it==myParameters.end();
}
