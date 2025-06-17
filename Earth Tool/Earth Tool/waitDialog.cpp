#include "stdafx.h"
#include "waitDialog.h"

ByyWaitDialog::ByyWaitDialog(const QString &message, QWidget *parent)
	: QDialog(parent, Qt::CustomizeWindowHint|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
{
	setStyleSheet("background-color: #666; color: #fff; font-size: 18px;");
	QLabel* msgLabel = new QLabel(message);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->setContentsMargins(30, 30, 30, 30);
	mainLayout->addWidget(msgLabel);
	setLayout(mainLayout);
}

ByyWaitDialog::~ByyWaitDialog()
{

}
