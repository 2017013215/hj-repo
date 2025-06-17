#ifndef PATROLROUTETASKWIDGET_H
#define PATROLROUTETASKWIDGET_H

#include "taskEditorWidget.h"
#include "ui_patrolRouteTaskWidget.h"

class ByyPatrolRouteTask;

class ByyPatrolRouteTaskWidget : public ByyTaskEditorWidget
{
	Q_OBJECT

public:
	ByyPatrolRouteTaskWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyPatrolRouteTaskWidget();

	virtual void init(ByyTask* task);
	virtual ByyTask* task();

public slots:
	void accept();
	void reject();

private:
	Ui::ByyPatrolRouteTaskWidget ui;
	ByyPatrolRouteTask* myTask;
};


typedef ByyTaskEditorWidgetCreatorTemplate<ByyPatrolRouteTaskWidget,ByyPatrolRouteTask> ByyPatrolRouteTaskWidgetCreator;

#endif // PATROLROUTETASKWIDGET_H
