#ifndef AUDIOSETTINGSDIALOG_H
#define AUDIOSETTINGSDIALOG_H

#include <QDialog>
#include "ui_audioSettingsDialog.h"

class ByyAudioSettingsDialog : public QDialog
{
	Q_OBJECT

public:
	ByyAudioSettingsDialog(QWidget *parent = 0);
	~ByyAudioSettingsDialog();

private slots:
	void soundVolumeToggled(int value);

private:
	Ui::ByyAudioSettings ui;
};

#endif // AUDIOSETTINGSDIALOG_H
