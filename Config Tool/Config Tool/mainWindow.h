#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainWindow.h"

class ByyMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	ByyMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ByyMainWindow();

protected:
	void closeEvent(QCloseEvent *event);

protected slots:
	void save();
	void setModified();

private:
	Ui::ByyMainWindowClass ui;
};

#endif // MAINWINDOW_H
