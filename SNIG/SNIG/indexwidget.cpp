#include "StdAfx.h"
#include "indexwidget.h"
#include "indexsubwidget.h"

#include <QHBoxLayout>

#define Ret_Width 50
#define Ret_Height 20
#define Left_Padding 100
#define Top_Padding 100

IndexWidget::IndexWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_QuitOnClose,false);

	m_pIndexSubWidget = new IndexSubWidget;
	QHBoxLayout* hl = new QHBoxLayout;
	hl->addWidget(m_pIndexSubWidget);
	//this->setLayout(hl);
	ui.scrollAreaWidgetContents->setLayout(hl);
	//widget->setFixedWidth(1500);
	m_pIndexSubWidget->setFixedHeight(1000);

}

// 主窗口关闭时，此窗口如果没有关闭，则此析构函数有问题
// 20231102
IndexWidget::~IndexWidget()
{

}

IndexWidget* IndexWidget::instance()
{
	static IndexWidget t_instance;
	return &t_instance;
}

void IndexWidget::initIndexTree()
{

}
void IndexWidget::calcAndShow()
{
	m_pIndexSubWidget->calcAndShow();
}