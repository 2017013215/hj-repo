#ifndef AUDIOADDDIALOG_H
#define AUDIOADDDIALOG_H

#include <QDialog>
#include "ui_audioAddDialog.h"

class ByyAudioAddDialog : public QDialog
{
	Q_OBJECT

public:
	ByyAudioAddDialog(const QString& entitytype="-1:-1:-1:-1:-1:-1:-1",const QString& filename="",QWidget *parent = 0);
	~ByyAudioAddDialog();

	QString getEntityType();
	QString getFileName();


private slots:
	void browserFile();
	void typeChanged();

private:
	Ui::ByyAudioAddDialog ui;
};

#endif // AUDIOADDDIALOG_H
