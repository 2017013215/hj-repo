#include "entityDefineAddDialog.h"
#include "mappingQuery.h"

ByyEntityDefineAddDialog::ByyEntityDefineAddDialog(QWidget *parent /*= 0*/ ) 
	: QDialog(parent,Qt::WindowSystemMenuHint)
{
	ui.setupUi(this);
	ui.lineEdit->setValidator(new QRegExpValidator(QRegExp("^-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*"),this));

	QVector<ModelDefine> defines=ByyMappingQuery::instance()->getModelDefines();

	for (int i=0;i!=defines.size();++i)
	{
		ui.comboBox->addItem(defines[i].name,defines[i].id);
	}

	ui.comboBox->model()->sort(0);
}

ByyEntityDefineAddDialog::~ByyEntityDefineAddDialog()
{

}

QString ByyEntityDefineAddDialog::getEntityType()
{
	return ui.lineEdit->text();
}

QString ByyEntityDefineAddDialog::getModelName()
{
	return ui.comboBox->currentText();
}

int ByyEntityDefineAddDialog::getModelId()
{
	return ui.comboBox->itemData(ui.comboBox->currentIndex()).toInt();
}
