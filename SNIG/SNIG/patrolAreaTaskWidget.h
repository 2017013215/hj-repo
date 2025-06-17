#ifndef PATROLAREATASKWIDGET_H
#define PATROLAREATASKWIDGET_H

#include "taskEditorWidget.h"
#include "ui_patrolAreaTaskWidget.h"

class ByyPatrolAreaTask;

class ByyPatrolAreaTaskWidget : public ByyTaskEditorWidget
{
	Q_OBJECT

public:
	ByyPatrolAreaTaskWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyPatrolAreaTaskWidget();

	virtual void init(ByyTask* task);
	virtual ByyTask* task();

public slots:
	void accept();
	void reject();

private:
	Ui::ByyPatrolAreaTaskWidget ui;
	ByyPatrolAreaTask* myTask;
};

typedef ByyTaskEditorWidgetCreatorTemplate<ByyPatrolAreaTaskWidget,ByyPatrolAreaTask> ByyPatrolAreaTaskWidgetCreator;

#endif // PATROLAREATASKWIDGET_H
