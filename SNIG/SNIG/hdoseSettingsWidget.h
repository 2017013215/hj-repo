#ifndef SIMFILESETTINGSWIDGET_H
#define SIMFILESETTINGSWIDGET_H

#include <QWidget>
#include "ui_hdoseSettingsWidget.h"

class ByyHdoseSettingsWidget : public QWidget
{
	Q_OBJECT

public:
	ByyHdoseSettingsWidget(QWidget *parent = 0);
	~ByyHdoseSettingsWidget();

protected slots:
	void	onCreateSelectDir();
	void	onCreateSelectSimFile();

	void onJoinSelectDir();
	void onJoinSelectSimFile();

	void onSingleSlectDir();
	void onSingleSelectSimFile();

	void onOpdSelectFile();
	void onRsmSelectFile();

	void onOkButtonClicked();

private:
	Ui::ByyHdoseSettingsWidget ui;
};

#endif // SIMFILESETTINGSWIDGET_H
