#ifndef TACTICALGRAPHICSPROCESSER_H
#define TACTICALGRAPHICSPROCESSER_H

#include <QObject>
#include "scene_global.h"

class ByyIG;
class ByyControlObject;
class ByyTacticalGraphicsCreator;
class ByyEventProcessor;

typedef ByyTacticalGraphicsCreator* (*ByyGraphicsCreator)(ByyIG& ig);
typedef ByyTacticalGraphicsCreator* (*ByyGraphicsEditorCreator)(ByyIG& ig,ByyControlObject* obj);

class SCENE_EXPORT ByyTacticalGraphicsProcesser : public QObject
{
	Q_OBJECT

public:
	ByyTacticalGraphicsProcesser(ByyIG& ig,QObject *parent=0);
	~ByyTacticalGraphicsProcesser();

	void addCreatorCreator(const QString& type,ByyGraphicsCreator func);

	void addEditorCreator(const QString& type,ByyGraphicsEditorCreator func);

	void edit(ByyControlObject* obj);

	void createCreator(const QString& type);

	void createEditor(const QString& type,ByyControlObject* obj);

protected:
	void release();

protected slots:
	void onCreateFinished(int status);
	void onEditFinished(int status);

signals:
	void graphicsCreated(ByyControlObject*);

protected:
	ByyIG& myIG;
	ByyTacticalGraphicsCreator *myCreator;
	
	typedef QMap<QString,ByyGraphicsCreator> ByyGraphicsCreatorList;

	ByyGraphicsCreatorList myGraphicsCreators;

	typedef QMap<QString,ByyGraphicsEditorCreator> ByyGraphicsEditorCreatorList;

	ByyGraphicsEditorCreatorList myGraphicsEditorCreators;
};

#endif // TACTICALGRAPHICSPROCESSER_H
