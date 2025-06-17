#include "StdAfx.h"
#include "indexNode.h"

IndexNode::IndexNode(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

IndexNode::~IndexNode()
{

}

void IndexNode::setTag(QString tag)
{
	ui.label_tag->setText(tag);
	ui.label_tag->setToolTip(tag);
}

void IndexNode::setValue(QString value)
{
	ui.label_valueDF->setText(value);
	ui.label_valueDF->setToolTip(value);
}

void IndexNode::setTitle(QString title)
{
	ui.label_titleDF->setText(title);
}

void IndexNode::setQZ(QString qz)
{
	ui.label_valueQZ->setText(qz);
}

void IndexNode::setQZTitle(QString qzt)
{
	ui.label_titleQZ->setText(qzt);
	
	if(qzt.isEmpty())
	{
		ui.label_valueQZ->setVisible(false);
		ui.label_titleQZ->setVisible(false);
	}
}


