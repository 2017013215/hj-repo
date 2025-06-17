#ifndef TERRAINEFFECTSWIDGET_H
#define TERRAINEFFECTSWIDGET_H

#include <QWidget>
#include "ui_terrainEffectsWidget.h"

class ByyTerrainEffect;

class ByyTerrainEffectWidget : public QWidget
{
	Q_OBJECT

public:
	ByyTerrainEffectWidget(ByyTerrainEffect *effect,QWidget *parent=0,Qt::WindowFlags f=0);
	~ByyTerrainEffectWidget();

private slots:
	void contourCheckChanged(int state);
	void contourSliderChanged(int slider_value);
	void verticalCheckeChanged(int state);
	void verticalSliderChanged(int slider_value);
	void contourSpinChanged(double spin_value);
	void verticalSpinChanged(double spin_value);
	void openGrayEffectChanged(int state);
private:
	void changeSliderValue(QSlider *slider,float slider_value);

private:
	Ui::ByyTerrainEffectsWidget ui;
	ByyTerrainEffect *myEffect;

	QCheckBox*	openGrayEffect;
};

#endif // TERRAINEFFECTSWIDGET_H
