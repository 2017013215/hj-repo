#ifndef ASSEMBLEWIDGET_H
#define ASSEMBLEWIDGET_H

#include <QWidget>
#include "ui_assembleWidget.h"

class ByyApp;
class ByyAssemble;

class ByyAssembleWidget : public QWidget
{
	Q_OBJECT

public:
	ByyAssembleWidget(ByyApp& app,QWidget *parent = 0);
	~ByyAssembleWidget();

public slots:
	void checkEnabled();

protected:
	void load();

protected slots:
	void itemClicked(QTreeWidgetItem * item, int column);

	void pushModel(const osgEarth::GeoPoint& geoPoint, const osg::Vec3d& orientation, int carryID);

	void pushRelease();

	void filter();

	void onEntityEditFinished(ByyLocalEntity* entObj);
	void setdeploy(bool);
	void setterraincheck(bool);
	void userChangeSize(const QString&);

protected:
	bool processEvent(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);

private:
	Ui::ByyAssembleWidget ui;
	ByyApp& myApp;

	ByyAssemble* myPlaceAssemble;


	bool		_opeDeploy;
	bool		_opeCheck;
	QLabel*		_entityStr;
	QLineEdit*	_length;
	QLineEdit*	_width;
	QRadioButton *_radioDeploy;
	QRadioButton *_radioCheck;
	QLabel*	_posLon;
	QLabel*	_posLat;
	bool	_enabled;
	QLabel*	_canDeploy;
	
	typedef ByyHostEventProcessor<ByyAssembleWidget> MouseCoordsEventProcessor;
	friend class MouseCoordsEventProcessor;

	MouseCoordsEventProcessor myEventProcessor;

};

#endif // ASSEMBLEWIDGET_H
