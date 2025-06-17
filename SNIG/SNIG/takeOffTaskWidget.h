#ifndef TAKEOFFTASKWIDGET_H
#define TAKEOFFTASKWIDGET_H

#include "taskEditorWidget.h"
#include "ui_takeOffTaskWidget.h"

class ByyTakeOffTask;

class ByyTakeOffTaskWidget : public ByyTaskEditorWidget
{
	Q_OBJECT

public:
	ByyTakeOffTaskWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyTakeOffTaskWidget();

	void init(ByyTask* task);
	ByyTask* task();

public slots:
	void accept();
	void reject();

private:
	Ui::ByyTakeOffTaskWidget ui;
	ByyTakeOffTask* myTask;
};

typedef ByyTaskEditorWidgetCreatorTemplate<ByyTakeOffTaskWidget,ByyTakeOffTask> ByyTakeOffTaskWidgetCreator;

#endif // TAKEOFFTASKWIDGET_H
