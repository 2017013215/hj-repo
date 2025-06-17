#ifndef SENSOROPERTASKWIDGET_H
#define SENSOROPERTASKWIDGET_H

#include "taskEditorWidget.h"
#include "ui_sensorOperTaskWidget.h"


class ByySensorOperTask;

class ByySensorOperTaskWidget : public ByyTaskEditorWidget
{
	Q_OBJECT

public:
	ByySensorOperTaskWidget(ByyIG& ig,QWidget *parent = 0);
	~ByySensorOperTaskWidget();

	virtual void init(ByyTask* task);
	virtual ByyTask* task();

public slots:
	void accept();
	void reject();

private:
	Ui::ByySensorOperTaskWidget ui;
	ByySensorOperTask* myTask;
};

typedef ByyTaskEditorWidgetCreatorTemplate<ByySensorOperTaskWidget,ByySensorOperTask> ByySensorOperTaskWidgetCreator;

#endif // SENSOROPERTASKWIDGET_H
