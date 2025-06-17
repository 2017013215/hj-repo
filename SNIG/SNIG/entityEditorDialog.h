#ifndef ENTITYEDITORDIALOG_H
#define ENTITYEDITORDIALOG_H

#include <QDialog>
#include "ui_entityEditorDialog.h"

class ByyEntityEditorDialog : public QDialog
{
	Q_OBJECT

public:
	ByyEntityEditorDialog(QWidget *parent = 0);
	~ByyEntityEditorDialog();

private:
	void init();

private slots:
	void modelFilter(const QString& text);
	void entityFilter(const QString& text);
	void currentModelDefineChanged(QListWidgetItem * current, QListWidgetItem * previous);
	void modelRename();
	void addModelDefine();
	void removeModelDefine();
	void editModelUrl(QTableWidgetItem * item);
	void addEntityDefine();
	void removeEntityDefine();
	void entityRename();
	void entityDefineChanged(QTableWidgetItem* item);

private:
	Ui::ByyEntityEditorDialog ui;
};

#endif // ENTITYEDITORDIALOG_H
