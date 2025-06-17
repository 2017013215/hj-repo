#ifndef SHOWCONTROLWIDGET_H
#define SHOWCONTROLWIDGET_H

#include <QWidget>
#include "ui_showControlWidget.h"

class ByyShowControlWidget : public QWidget
{
	Q_OBJECT

public:
	ByyShowControlWidget(QWidget *parent = 0);
	~ByyShowControlWidget();

private:
	Ui::ByyShowControlWidget ui;
};

#endif // SHOWCONTROLWIDGET_H
