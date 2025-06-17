#include "XTest_GISWnd.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	XTest_GISWnd w;
	//w.setMinimumSize(1200,960);
	w.showMaximized();
	return a.exec();
}
