#ifndef HUDTITLEDIALOG_H
#define HUDTITLEDIALOG_H

#include <QDialog>
#include <osgEarthUtil/Controls>

#include "sceneController.h"
#include "ui_hudTitleDialog.h"

class ByyHudTitleDialog : public QDialog
{
	Q_OBJECT

public:
	ByyHudTitleDialog(ByySceneController* scene,QWidget *parent = 0);
	~ByyHudTitleDialog();

protected slots:
	void onSelect();
	void textChanged(const QString& text);
	void onAccept();

private:
	Ui::ByyHudTitleDialog ui;
	ByySceneController *d_scene;
	osg::ref_ptr<osgEarth::Util::Controls::ImageControl> imageControl;
	osg::ref_ptr<osgEarth::Util::Controls::ControlCanvas> canvas;
};

#endif // HUDTITLEDIALOG_H
