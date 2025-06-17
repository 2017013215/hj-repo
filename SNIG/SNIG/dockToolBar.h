#ifndef DOCKTOOLBAR_H
#define DOCKTOOLBAR_H

#include <QDockWidget>
#include "ui_dockToolBar.h"
#include <osgEarth/MaskLayer>

class ByyMainWindow;

class ByyDockToolBar : public QDockWidget
{
	Q_OBJECT

public:
	ByyDockToolBar(ByyMainWindow* mainWindow,QWidget *parent = 0);
	~ByyDockToolBar();

	void toggleFullScreen();

protected:
	void closeEvent(QCloseEvent *event);

private:
	void setupIcons();
	void init();

private slots:
	void snapshoot();
	void onCaveTransparency(bool enable);
	void setFullScreen(bool fullscreen);
	void setAudio(bool audio);

signals:
	void fullScreenChanged(bool);
	void quadBufferClicked(bool);
	void closed();

public:
	Ui::ByyDockToolBar ui;

	 osgEarth::MaskLayerVector maskLayerVector;

	 ByyMainWindow* myMainWindow;
};

#endif // DOCKTOOLBAR_H
