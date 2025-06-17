#ifndef SCNENVIRONMENTSETTINGSWIDGET_H
#define SCNENVIRONMENTSETTINGSWIDGET_H

#include <QWidget>
#include "ui_scnEnvironmentSettingsWidget.h"
#include "scenarioV2.h"

class ByyApp;
class ByySceneRectangle;

class ByyScnEnvironmentSettingsWidget : public QWidget
{
	Q_OBJECT

public:
	ByyScnEnvironmentSettingsWidget(ByyApp& app,QWidget *parent = 0);
	~ByyScnEnvironmentSettingsWidget();

protected slots:
	void onOkClicked();
	void setValueChanged();

	void onCurrentItemChanged(int index);

	void addItem();
	void removeCurrentItem();
	void renameCurrentItem();

	void updateSeaState(int seaState);
	void updateWindDirection(int windDir);
	void updateWindSpeed(int windSpeed);
	void updateRect();

protected:
	void udateEditWidget();
	void removeCurrentSceneRect();

private:
	Ui::ByyScnEnvironmentSettingsWidget ui;
	ByyApp& myApp;

	ByyScenarioV2::EnvironmentList myEditEnviromentList;

	ByySceneRectangle* mySceneRect;
};

#endif // SCNENVIRONMENTSETTINGSWIDGET_H
