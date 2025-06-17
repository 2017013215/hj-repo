#ifndef MOUSECOORDSWIDGET_H
#define MOUSECOORDSWIDGET_H

#include <QWidget>

#include "ui_mouseCoordsWidget.h"
#include "ByyEventProcessor.h"

class ByyIG;

class ByyMouseCoordsWidget : public QWidget
{
	Q_OBJECT

public:
	ByyMouseCoordsWidget(ByyIG &ig,QWidget *parent = 0);
	~ByyMouseCoordsWidget();

protected:
	bool processEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

private:
	Ui::ByyMouseCoordsWidget ui;
	ByyIG &myIG;

	typedef ByyHostEventProcessor<ByyMouseCoordsWidget> MouseCoordsEventProcessor;
	friend class MouseCoordsEventProcessor;

	MouseCoordsEventProcessor myEventProcessor;

	osgViewer::View*	_view;
};

#endif // MOUSECOORDSWIDGET_H
