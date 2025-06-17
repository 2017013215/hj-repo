#ifndef RETURNBASETASKWIDGET_H
#define RETURNBASETASKWIDGET_H

#include "taskEditorWidget.h"
#include "ui_returnBaseTaskWidget.h"

class ByyReturnBaseTask;

class ByyReturnBaseTaskWidget : public ByyTaskEditorWidget
{
	Q_OBJECT

public:
	ByyReturnBaseTaskWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyReturnBaseTaskWidget();

	virtual void init(ByyTask* task);
	virtual ByyTask* task();

public slots:
	void accept();
	void reject();

protected:
	void mouseClicked(int button,int x,int y,osg::View *view);

	friend class MyEventProcessor;

private:
	Ui::ByyReturnBaseTaskWidget ui;
	ByyReturnBaseTask* myTask;

	class MyEventProcessor : public ByyEventProcessor
	{
	public:
		MyEventProcessor(ByyReturnBaseTaskWidget* w):myWidget(w){}

		virtual bool processEvent(const osgGA::GUIEventAdapter& ea,ByyChannel &channel);

	protected:
		ByyReturnBaseTaskWidget* myWidget;
		float myXdown;
		float myYdown;
	};

	MyEventProcessor myEventProcessor;
};

typedef ByyTaskEditorWidgetCreatorTemplate<ByyReturnBaseTaskWidget,ByyReturnBaseTask> ByyReturnBaseTaskWidgetCreator;

#endif // RETURNBASETASKWIDGET_H
