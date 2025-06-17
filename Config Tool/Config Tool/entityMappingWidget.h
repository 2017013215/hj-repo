#ifndef ENTITYMAPPINGWIDGET_H
#define ENTITYMAPPINGWIDGET_H

#include <QWidget>
#include "ui_entityMappingWidget.h"
#include "mappingQuery.h"

class ByyEntityMappingWidget : public QWidget
{
	Q_OBJECT

public:
	ByyEntityMappingWidget(QWidget *parent = 0);
	~ByyEntityMappingWidget();

protected:
	void showEvent(QShowEvent *e);

private:
	void init();

private slots:
	void entityFilter(const QString& text);
	void addEntityDefine();
	void removeEntityDefine();
	void entityRename();
	void entityDefineChanged(QTableWidgetItem* item);

signals:
	void modified();

private:
	Ui::ByyEntityMappingWidget ui;
};

#endif // ENTITYMAPPINGWIDGET_H
