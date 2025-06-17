#ifndef MOVETOTASKWIDGET_H
#define MOVETOTASKWIDGET_H

#include "taskEditorWidget.h"
#include "ui_moveToTaskWidget.h"

class ByyMoveToTask;

class ByyMoveToTaskWidget : public ByyTaskEditorWidget
{
	Q_OBJECT

public:
	ByyMoveToTaskWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyMoveToTaskWidget();

	virtual void init(ByyTask* task);
	virtual ByyTask* task();

public slots:
	void accept();
	void reject();

private:
	Ui::ByyMoveToTaskWidget ui;
	ByyMoveToTask* myTask;

};

typedef ByyTaskEditorWidgetCreatorTemplate<ByyMoveToTaskWidget,ByyMoveToTask> ByyMoveToTaskWidgetCreator;

#endif // MOVETOTASKWIDGET_H
