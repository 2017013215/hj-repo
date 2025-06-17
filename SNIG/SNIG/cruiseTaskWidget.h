#ifndef CRUISETASKWIDGET_H
#define CRUISETASKWIDGET_H

#include "taskEditorWidget.h"
#include "ui_cruiseTaskWidget.h"

class ByyCruiseTask;

class ByyCruiseTaskWidget : public ByyTaskEditorWidget
{
	Q_OBJECT

public:
	ByyCruiseTaskWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyCruiseTaskWidget();

	virtual void init(ByyTask* task);
	virtual ByyTask* task();

public slots:
	void accept();
	void reject();

private:
	Ui::ByyCruiseTaskWidget ui;
	ByyCruiseTask* myTask;
};

typedef ByyTaskEditorWidgetCreatorTemplate<ByyCruiseTaskWidget,ByyCruiseTask> ByyCruiseTaskWidgetCreator;

#endif // CRUISETASKWIDGET_H
