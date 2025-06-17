#ifndef WAITTASKWIDGET_H
#define WAITTASKWIDGET_H

#include "taskEditorWidget.h"
#include "ui_waitTaskWidget.h"

class ByyWaitTask;

class ByyWaitTaskWidget : public ByyTaskEditorWidget
{
	Q_OBJECT

public:
	ByyWaitTaskWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyWaitTaskWidget();

	virtual void init(ByyTask* task);
	virtual ByyTask* task();

public slots:
	void accept();
	void reject();

private:
	Ui::ByyWaitTaskWidget ui;
	ByyWaitTask* myTask;
};

typedef ByyTaskEditorWidgetCreatorTemplate<ByyWaitTaskWidget,ByyWaitTask> ByyWaitTaskWidgetCreator;

#endif // WAITTASKWIDGET_H
