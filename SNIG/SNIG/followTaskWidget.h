#ifndef FOLLOWTASKWIDGET_H
#define FOLLOWTASKWIDGET_H

#include "taskEditorWidget.h"
#include "ui_followTaskWidget.h"

class ByyFollowTask;

class ByyFollowTaskWidget : public ByyTaskEditorWidget
{
	Q_OBJECT

public:
	ByyFollowTaskWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyFollowTaskWidget();

	virtual void init(ByyTask* task);
	virtual ByyTask* task();

public slots:
	void accept();
	void reject();

private:
	Ui::ByyFollowTaskWidget ui;
	ByyFollowTask* myTask;
};

typedef ByyTaskEditorWidgetCreatorTemplate<ByyFollowTaskWidget,ByyFollowTask> ByyFollowTaskWidgetCreator;

#endif // FOLLOWTASKWIDGET_H
