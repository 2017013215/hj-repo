#ifndef VIDEOEDITDIALOG_H
#define VIDEOEDITDIALOG_H

#include <QDialog>
#include "ui_videoEditDialog.h"

class ByyModel;
struct VideoInfo;

class ByyVideoEditDialog : public QDialog
{
	Q_OBJECT

public:
	ByyVideoEditDialog(QWidget *parent = 0);
	~ByyVideoEditDialog();

	void setCurrentModel(ByyModel* model);

protected slots:
	void addVideo();
	void removeVideo();

	void itemChanged(QTableWidgetItem * item);

protected:
	void row2Data(int row,VideoInfo& vi);
	void data2Row(int row,VideoInfo& vi);

signals:
	void modified();

private:
	Ui::ByyVideoEditDialog ui;
	ByyModel *myModel;
};

#endif // VIDEOEDITDIALOG_H
