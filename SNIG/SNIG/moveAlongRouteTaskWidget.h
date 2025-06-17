#ifndef MOVEALONGROUTETASKWIDGET_H
#define MOVEALONGROUTETASKWIDGET_H

#include "taskEditorWidget.h"
#include "ui_moveAlongRouteTaskWidget.h"

class ByyMoveAlongRouteTask;

class ByyMoveAlongRouteTaskWidget : public ByyTaskEditorWidget
{
	Q_OBJECT

public:
	ByyMoveAlongRouteTaskWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyMoveAlongRouteTaskWidget();

	virtual void init(ByyTask* task);
	virtual ByyTask* task();

public slots:
	void accept();
	void reject();

private:
	Ui::ByyMoveAlongRouteTaskWidget ui;
	ByyMoveAlongRouteTask* myTask;
};

typedef ByyTaskEditorWidgetCreatorTemplate<ByyMoveAlongRouteTaskWidget,ByyMoveAlongRouteTask> ByyMoveAlongRouteTaskWidgetCreator;

#endif // MOVEALONGROUTETASKWIDGET_H
