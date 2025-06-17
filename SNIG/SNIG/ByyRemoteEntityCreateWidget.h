#ifndef BYYREMOTEENTITYCREATEWIDGET_H
#define BYYREMOTEENTITYCREATEWIDGET_H

#include <QWidget>
#include "ui_ByyRemoteEntityCreateWidget.h"

class ByyApp;

class ByyRemoteEntityCreateWidget : public QWidget
{
	Q_OBJECT

public:
	ByyRemoteEntityCreateWidget(ByyApp& app,const QString& assemble,ByyForceType ft,const osg::Vec3d& position,QWidget *parent = 0);
	~ByyRemoteEntityCreateWidget();

public slots:
	void accept();

private:
	Ui::ByyRemoteEntityCreateWidget ui;
	ByyApp& myApp;

	QString myAssemble;
	ByyForceType myForceType;
	osg::Vec3d myPos;
};

#endif // BYYREMOTEENTITYCREATEWIDGET_H
