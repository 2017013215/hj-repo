#ifndef BYYSELECTIONMANAGER_H
#define BYYSELECTIONMANAGER_H

#include <QObject>
#include "scene_global.h"

class SCENE_EXPORT ByySelectionManager : public QObject
{
	Q_OBJECT

public:
	ByySelectionManager(QObject *parent=0);
	~ByySelectionManager();

	enum SelectionType
	{
		NoValidSelection,
		Entity,
		ControlObject
	};

	void setCurrentSelection(SelectionType type,unsigned int id);

	SelectionType selectionType()const;
	unsigned int  selectionId()const;

	static ByySelectionManager& instance();

signals:
	void currentSelectionChanged(ByySelectionManager::SelectionType type,unsigned int id);

protected:
	SelectionType  mySelectionType;	
	unsigned int   mySelectionId;
};

#endif // BYYSELECTIONMANAGER_H
