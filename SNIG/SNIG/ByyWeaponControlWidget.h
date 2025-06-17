#ifndef BYYWEAPONCONTROLWIDGET_H
#define BYYWEAPONCONTROLWIDGET_H

#include "ByyEntityObserver.h"
#include "ui_ByyWeaponControlWidget.h"

class ByyWeaponSysSR;

class ByyWeaponControlWidget : public ByyEntityObserver
{
	Q_OBJECT

public:
	ByyWeaponControlWidget(ByyCore& core,ByyEntityObject* entity=0,QWidget* parent=0,Qt::WindowFlags f = 0);
	~ByyWeaponControlWidget();

	void setEntity(ByyEntityObject *entity);

protected slots:
	void updateFromSR();
	void onSRDataChanged();

private:
	Ui::ByyWeaponControlWidget ui;
	ByyWeaponSysSR *myWeponSysSR;

	QMap<QString,QTreeWidgetItem*> mySystemItems;
	QMultiMap<QString,QTreeWidgetItem*> myWeaponItems;
};

#endif // BYYWEAPONCONTROLWIDGET_H
