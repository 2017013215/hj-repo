#include "stdafx.h"
#include "entityDefineAddDialog.h"

ByyEntityDefineAddDialog::ByyEntityDefineAddDialog( QListWidget *listWidget,QWidget *parent /*= 0*/ ) 
	: QDialog(parent,Qt::WindowSystemMenuHint)
{
	ui.setupUi(this);
	ui.lineEdit->setValidator(new QRegExpValidator(QRegExp("^-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*"),this));
	for (int i=0;i!=listWidget->count();++i)
	{
		ui.comboBox->addItem(listWidget->item(i)->text());
	}
}

ByyEntityDefineAddDialog::~ByyEntityDefineAddDialog()
{

}

QString ByyEntityDefineAddDialog::getEntityType()
{
	return ui.lineEdit->text();
}

QString ByyEntityDefineAddDialog::getEntityDefine()
{
	return ui.comboBox->currentText();
}
