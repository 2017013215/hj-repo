#ifndef BUILDINGINFODIALOG_H
#define BUILDINGINFODIALOG_H

#include <QDialog>
#include "ui_buildingInfoDialog.h"

class ByyModel;

class ByyBuildingInfoDialog : public QDialog
{
	Q_OBJECT

public:
	ByyBuildingInfoDialog(QWidget *parent = 0);
	~ByyBuildingInfoDialog();

	void setCurrentModel(ByyModel* model);

protected:
	bool eventFilter(QObject *obj, QEvent *event);

protected slots:
	void setTitle(const QString& title);
	void setContent(const QString& content);
	void setWindowSize();
	void contextEditFinished();
	void changeBackgroundColor();

private:
	void updateColor();

signals:
	void modified();

private:
	void colorChange(QWidget *widget);

	Ui::ByyBuildingInfoDialog ui;
	ByyModel *d_model;
};

#endif // BUILDINGINFODIALOG_H
