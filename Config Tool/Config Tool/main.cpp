#include "mainWindow.h"
#include <QtGui/QApplication>
#include <QDir>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QDir::setCurrent(a.applicationDirPath());
	ByyMainWindow w;
	w.showMaximized();
	return a.exec();
}
