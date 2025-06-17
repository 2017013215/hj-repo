#ifndef ENTITYDEFINEADDDIALOG_H
#define ENTITYDEFINEADDDIALOG_H

#include <QDialog>
#include "ui_entityDefineAddDialog.h"

class QListWidget;

class ByyEntityDefineAddDialog : public QDialog
{
	Q_OBJECT

public:
	ByyEntityDefineAddDialog(QWidget *parent = 0);
	~ByyEntityDefineAddDialog();

	QString getEntityType();
	QString getModelName();
	int getModelId();

private:
	Ui::ByyEntityDefineAddDialog ui;
};

#endif // ENTITYDEFINEADDDIALOG_H
