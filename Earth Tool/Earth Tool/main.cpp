#include "stdafx.h"
#include "mainWindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QDir::setCurrent(a.applicationDirPath());
	ByyMainWindow w;
	w.showMaximized();
	return a.exec();
}
