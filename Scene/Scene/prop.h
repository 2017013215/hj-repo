#ifndef MODELCOMPONENT_H
#define MODELCOMPONENT_H

#include <QObject>
#include "scene_global.h"

class ByyIG;
class ByyModel;

class SCENE_EXPORT ByyProp : public QObject
{
	Q_OBJECT
public:
	ByyProp(ByyIG* ig);
	~ByyProp();

	friend class ModelLoadTask;

	QList<ByyModel*>& models();

public slots:
	void init(osgEarth::MapNode *mapNode);
	void release();

protected slots:
	void processModelLoadFinish(ByyModel* model);

signals:
	void modelLoadFinish(ByyModel*);

private:
	ByyIG* myIG;

	QList<ByyModel*> myModels;

	QThreadPool myLoadThreadPool;

	volatile bool _threadNeedRelease;
};

#endif // MODELCOMPONENT_H
