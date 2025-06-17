#include "taskFactory.h"
#include "task.h"

ByyTaskFactory::ByyTaskFactory()
{

}

ByyTaskFactory::~ByyTaskFactory()
{
	qDeleteAll(myCreators);
}

void ByyTaskFactory::appenCreator( ByyTaskCreator* creator )
{
	myCreators.append(creator);
}

ByyTaskFactory::TaskCreatorList& ByyTaskFactory::creators()
{
	return myCreators;
}

ByyTaskCreator* ByyTaskFactory::findCreator( const QString& type )
{
	for(int i=0;i!=myCreators.size();++i)
	{
		if (myCreators[i]->typeInfo().type==type)
		{
			return myCreators[i];
		}
	}
	return 0;
}
