#include "stdafx.h"
#include "hla13ConnectWidget.h"
#include "connectManager.h"

ByyHLA13ConnectWidget::ByyHLA13ConnectWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.pushButton_3->setIcon(QIcon("../data/icons/UtilityRename.png"));
	ui.pushButton_4->setIcon(QIcon("../data/icons/UtilityAdd.png"));
	ui.pushButton_5->setIcon(QIcon("../data/icons/UtilityMinus.png"));

	connect(ui.pushButton_3,SIGNAL(clicked()),SLOT(renameClicked()));
	connect(ui.pushButton_4,SIGNAL(clicked()),SLOT(addClicked()));
	connect(ui.pushButton_5,SIGNAL(clicked()),SLOT(removeClicked()));
	connect(ui.pushButton_6,SIGNAL(clicked()),SLOT(ConnectClicked()));

	connect(ui.comboBox,SIGNAL(currentIndexChanged(int)),SLOT(updateWidget()));

	connect(ui.lineEdit,SIGNAL(textChanged(const QString&)),SLOT(infoChanged()));
	connect(ui.lineEdit_2,SIGNAL(textChanged(const QString&)),SLOT(infoChanged()));
	connect(ui.lineEdit_3,SIGNAL(textChanged(const QString&)),SLOT(infoChanged()));
}

ByyHLA13ConnectWidget::~ByyHLA13ConnectWidget()
{

}

void ByyHLA13ConnectWidget::showEvent( QShowEvent *event )
{
	ui.comboBox->clear();
	QList<ByyConnect*>& connects=manager->getConnects();
	for (int i=0;i!=connects.count();++i)
	{
		ByyHLA13Connect* conn=dynamic_cast<ByyHLA13Connect*>(connects[i]);

		if (conn)
		{
			addConnect(conn);
		}
	}

	QWidget::showEvent(event);
}

void ByyHLA13ConnectWidget::addConnect( ByyHLA13Connect* conn )
{
	ui.comboBox->addItem(conn->name(),(int)conn);
}

void ByyHLA13ConnectWidget::renameClicked()
{
	int index=ui.comboBox->currentIndex();
	ByyHLA13Connect* conn=(ByyHLA13Connect*)ui.comboBox->itemData(index).toInt();
	bool ok;
	QString text = QInputDialog::getText(this,QString::fromLocal8Bit("重命名"),
		QString::fromLocal8Bit("连接名称:"), QLineEdit::Normal,
		conn->name(), &ok);
	if (ok && !text.isEmpty())
	{
		ui.comboBox->setItemText(index,text);
		conn->setName(text);
	}
}

void ByyHLA13ConnectWidget::addClicked()
{
	ByyHLA13Connect *hla13=new ByyHLA13Connect;

	hla13->ExecName("VR-Link");
	hla13->FederateType("SN-Sim");
	hla13->FedFileName("VR-link.fed");
	hla13->setName(QString("HLA13 Connect %1").arg(ui.comboBox->count()));

	manager->addConnect(hla13);

	addConnect(hla13);

	ui.comboBox->setCurrentIndex(ui.comboBox->count()-1);
}

void ByyHLA13ConnectWidget::removeClicked()
{
	int r=QMessageBox::warning(this,
		QString::fromLocal8Bit("提示"),
		QString::fromLocal8Bit("确认删除吗？"),
		QMessageBox::Yes|QMessageBox::No);

	if (r!=QMessageBox::Yes)
	{
		return;
	}

	int index=ui.comboBox->currentIndex();
	ByyHLA13Connect* conn=(ByyHLA13Connect*)ui.comboBox->itemData(index).toInt();

	manager->removeConnect(conn);

	ui.comboBox->removeItem(index);
}

void ByyHLA13ConnectWidget::ConnectClicked()
{
	int index=ui.comboBox->currentIndex();
	ByyHLA13Connect* conn=(ByyHLA13Connect*)ui.comboBox->itemData(index).toInt();

	conn->isConnect()?conn->release():conn->init();
	ui.pushButton_6->setText(QString::fromLocal8Bit(conn->isConnect()?"断开":"连接"));
}

void ByyHLA13ConnectWidget::infoChanged()
{
	int index=ui.comboBox->currentIndex();

	if (index==-1)
	{
		return;
	}

	ByyHLA13Connect* conn=(ByyHLA13Connect*)ui.comboBox->itemData(index).toInt();

	if (conn->ExecName()!=ui.lineEdit->text())
	{
		conn->ExecName(ui.lineEdit->text());
	}
	if (conn->FedFileName()!=ui.lineEdit_2->text())
	{
		conn->FedFileName(ui.lineEdit_2->text());
	}
	if (conn->FederateType()!=ui.lineEdit_3->text())
	{
		conn->FederateType(ui.lineEdit_3->text());
	}
}

void ByyHLA13ConnectWidget::updateWidget()
{
	int index=ui.comboBox->currentIndex();

	ui.pushButton_3->setEnabled(index!=-1);
	ui.pushButton_5->setEnabled(index!=-1);
	ui.pushButton_6->setEnabled(index!=-1);

	ui.lineEdit->blockSignals(true);
	ui.lineEdit_2->blockSignals(true);
	ui.lineEdit_3->blockSignals(true);

	ui.lineEdit->clear();
	ui.lineEdit_2->clear();
	ui.lineEdit_3->clear();

	if (index==-1)
	{
		ui.lineEdit->blockSignals(false);
		ui.lineEdit_2->blockSignals(false);
		ui.lineEdit_3->blockSignals(false);
		return;
	}

	ByyHLA13Connect* conn=(ByyHLA13Connect*)ui.comboBox->itemData(index).toInt();

	ui.pushButton_6->setText(QString::fromLocal8Bit(conn->isConnect()?"断开":"连接"));

	ui.lineEdit->setText(conn->ExecName());
	ui.lineEdit_2->setText(conn->FedFileName());
	ui.lineEdit_3->setText(conn->FederateType());

	ui.lineEdit->blockSignals(false);
	ui.lineEdit_2->blockSignals(false);
	ui.lineEdit_3->blockSignals(false);
}

void ByyHLA13ConnectWidget::setManager( ByyConnectManager *cm )
{
	manager=cm;
}
