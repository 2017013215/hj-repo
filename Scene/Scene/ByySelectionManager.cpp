#include "StdAfx.h"
#include "ByySelectionManager.h"

ByySelectionManager::ByySelectionManager(QObject *parent)
	: QObject(parent)
	,mySelectionType(NoValidSelection)
	,mySelectionId(0)
{

}

ByySelectionManager::~ByySelectionManager()
{

}

ByySelectionManager& ByySelectionManager::instance()
{
	static ByySelectionManager manager;
	return manager;
}

ByySelectionManager::SelectionType ByySelectionManager::selectionType() const
{
	return mySelectionType;
}

unsigned int ByySelectionManager::selectionId() const
{
	return mySelectionId;
}

void ByySelectionManager::setCurrentSelection( SelectionType type,unsigned int id )
{
	if (id!=mySelectionId)
	{
		mySelectionId=id;
		mySelectionType=type;
		emit currentSelectionChanged(type,id);
	}
}
