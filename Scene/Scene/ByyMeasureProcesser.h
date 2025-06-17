#ifndef BYYMEASUREPROCESSER_H
#define BYYMEASUREPROCESSER_H

#include <QObject>
#include "scene_global.h"

class ByyIG;
class ByyMeasureItemCreator;
class ByyMeasureItem;

class SCENE_EXPORT ByyMeasureProcesser : public QObject
{
	Q_OBJECT

public:
	ByyMeasureProcesser(ByyIG& ig,QObject *parent=0);
	~ByyMeasureProcesser();

	void addCreator(const QString& type,ByyMeasureItemCreator* creator);

	void measure(const QString& type);

public slots:
	void release();

protected:
	ByyIG& myIG;

	ByyMeasureItem *myMeasureItem;

	typedef QMap<QString,ByyMeasureItemCreator*> CreatorMap;
	CreatorMap myCreators;
};

#endif // BYYMEASUREPROCESSER_H
