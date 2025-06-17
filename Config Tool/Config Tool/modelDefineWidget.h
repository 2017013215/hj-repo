#ifndef MODELDEFINEWIDGET_H
#define MODELDEFINEWIDGET_H

#include <QWidget>
#include "ui_modelDefineWidget.h"

struct ModelDefine;

class ByyModelDefineWidget : public QWidget
{
	Q_OBJECT

public:
	ByyModelDefineWidget(QWidget *parent = 0);
	~ByyModelDefineWidget();

	void init();

private slots:
	void modelFilter(const QString& text);
	void currentModelDefineChanged(QListWidgetItem * current, QListWidgetItem * previous);
	void modelRename();
	void addModelDefine();
	void removeModelDefine();
	void editFileName(QTableWidgetItem * item);

private:
	QString openModelFile(const QString& path=QString());
	QString openIconFile(const QString& path=QString());

	void addModelDefine(const ModelDefine& md);

signals:
	void modified();

private:
	Ui::ByyModelDefineWidget ui;
	QString myLastModelPath;
	QString myLastIconPath;

	enum Row{ModelRow,IconRow};
};

#endif // MODELDEFINEWIDGET_H
