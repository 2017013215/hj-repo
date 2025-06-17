#ifndef SCENESETTINGS_H
#define SCENESETTINGS_H

#include <QWidget>
#include <QDateTime>
#include <QTimer>
#include "ui_environmentWidget.h"

class ByyEnvironment;

class ByyEnvironmentWidget : public QWidget
{
	Q_OBJECT

public:
	ByyEnvironmentWidget(ByyEnvironment& environment,QWidget *parent = 0);
	~ByyEnvironmentWidget();

private slots:
	void useSystemSettings();
	void timeScaleChanged(double scale);
	void timeEditToggled(const QTime & time);
	void calendarToggled(const QDate& date);
	void timeSliderChanged();
	void ambientValueChanged(int value);

	void visibilitySliderChanged(int value);
	void updateDateTime();

	void onCloudLayerTypeChanged(int index);

	void updateWind();
	void updatePrecipitation();

private:
	void init();
	void initCmpass();

private:
	Ui::ByyEnvironmentWidget ui;

	QTimer timer;
	int tickInterval;

	ByyEnvironment& myEnvironment;
};

#endif // SCENESETTINGS_H
