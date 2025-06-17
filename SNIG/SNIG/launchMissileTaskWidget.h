#ifndef LAUNCHMISSILETASKWIDGET_H
#define LAUNCHMISSILETASKWIDGET_H

#include "taskEditorWidget.h"
#include "ui_launchMissileTaskWidget.h"
#include "ByyEventProcessor.h"

class ByyLaunchMissileTask;

class ByyLaunchMissileTaskWidget : public ByyTaskEditorWidget
{
	Q_OBJECT

public:
	ByyLaunchMissileTaskWidget(ByyIG& ig,QWidget *parent = 0);
	~ByyLaunchMissileTaskWidget();

	virtual void init(ByyTask* task);
	virtual ByyTask* task();

public slots:
	void accept();
	void reject();

protected:
	void mouseClicked(int button,int x,int y,osg::View *view);

	friend class MyEventProcessor;

private:
	Ui::ByyLaunchMissileTaskWidget ui;
	ByyLaunchMissileTask* myTask;
	
	class MyEventProcessor : public ByyEventProcessor
	{
	public:
		MyEventProcessor(ByyLaunchMissileTaskWidget* w):myWidget(w){}

		virtual bool processEvent(const osgGA::GUIEventAdapter& ea,ByyChannel &channel);

	protected:
		ByyLaunchMissileTaskWidget* myWidget;
		float myXdown;
		float myYdown;
	};

	MyEventProcessor myEventProcessor;

};


typedef ByyTaskEditorWidgetCreatorTemplate<ByyLaunchMissileTaskWidget,ByyLaunchMissileTask> ByyLaunchMissileTaskWidgetCreator;

#endif // LAUNCHMISSILETASKWIDGET_H
