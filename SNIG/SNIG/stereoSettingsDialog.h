#ifndef STEREOSETTINGSDIALOG_H
#define STEREOSETTINGSDIALOG_H

#include <QWidget>
#include "ui_stereoSettingsDialog.h"

class ByyStereoSettingsDialog : public QWidget
{
	Q_OBJECT

public:
	ByyStereoSettingsDialog(QWidget *parent = 0);
	~ByyStereoSettingsDialog();

private slots:
	void eyeSeparationChanged(double value);
	void screenDistanceChanged(double value);

private:
	Ui::ByyStereoSettingsDialog ui;
};

#endif // STEREOSETTINGSDIALOG_H
