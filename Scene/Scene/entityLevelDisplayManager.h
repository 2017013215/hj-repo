#ifndef ENTITYLEVELDISPLAYMANAGER_H
#define ENTITYLEVELDISPLAYMANAGER_H

#include <QObject>

class ByyIG;
class ByyEntityObject;
class ByySubjectionVisualizer;

class ByyEntityLevelDisplayManager : public QObject
{
	Q_OBJECT

public:
	ByyEntityLevelDisplayManager(ByyIG& ig);
	~ByyEntityLevelDisplayManager();

protected slots:
	void appendEntObject(ByyEntityObject* entObj);
	void removeEntObject(ByyEntityObject* entObj);

	void tick();

private:
	ByyIG& myIG;

	typedef QPair<ByyEntityObject*,ByyEntityObject*> Subjection;

	typedef QMap<Subjection,ByySubjectionVisualizer*> SubjectionVisualizers;

	typedef QMap<Subjection,ByySubjectionVisualizer*>::iterator Iter;

	SubjectionVisualizers mySubjectionVisualizers;

	QMap<ByyEntityObject*,int> myUnMatchingEntityObjList;
};

#endif // ENTITYLEVELDISPLAYMANAGER_H
