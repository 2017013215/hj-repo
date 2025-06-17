#ifndef OBJECTLISTWIDGET_H
#define OBJECTLISTWIDGET_H

#include <QWidget>

class ByyApp;
class ByyEntityObjectWidget;
class ByyControlObjectWidget;

class ByyObjectListWidget : public QWidget
{
	Q_OBJECT

public:
	ByyObjectListWidget(ByyApp &app,QWidget *parent = 0);
	~ByyObjectListWidget();

private:
	ByyEntityObjectWidget* myEntObjWidget;
	ByyControlObjectWidget* myCtrlObjWidget;
};

#endif // OBJECTLISTWIDGET_H
