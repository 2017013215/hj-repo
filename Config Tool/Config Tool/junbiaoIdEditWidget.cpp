#include "junbiaoIdEditWidget.h"

ByyJunbiaoIdEditWidget::ByyJunbiaoIdEditWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

ByyJunbiaoIdEditWidget::~ByyJunbiaoIdEditWidget()
{

}

void ByyJunbiaoIdEditWidget::setLibId( int id )
{
	ui.spinBox->setValue(id);
}

void ByyJunbiaoIdEditWidget::setMarkId( int id )
{
	ui.spinBox_2->setValue(id);
}

void ByyJunbiaoIdEditWidget::setId( int lib,int mark )
{
	setLibId(lib);
	setMarkId(mark);
}

int ByyJunbiaoIdEditWidget::libId()
{
	return ui.spinBox->value();
}

int ByyJunbiaoIdEditWidget::markId()
{
	return ui.spinBox_2->value();
}

QString ByyJunbiaoIdEditWidget::string()
{
	JunBiaoId id;
	id.libId=libId();
	id.markId=markId();
	return id.toString();
}
