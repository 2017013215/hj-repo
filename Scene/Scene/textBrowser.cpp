#include "stdafx.h"
#include "textBrowser.h"
#include "settings.h"

#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollArea>
#include <QHBoxLayout>
ByyTextBrowser::ByyTextBrowser(QWidget *parent)
	:QWidget(parent,Qt::Window|Qt::WindowSystemMenuHint|Qt::FramelessWindowHint),
	d_color(255,0,0,125)
{
 	setStyleSheet("background-color:transparent");
	setAttribute(Qt::WA_TranslucentBackground, true); 
	setAttribute(Qt::WA_OpaquePaintEvent,true); 

	label=new QLabel;
	label->setWordWrap(true);
	label->setScaledContents(true);
	label->setAlignment(Qt::AlignTop);

	setLayout(new QHBoxLayout);
	layout()->addWidget(label);

	layout()->setContentsMargins(0,0,0,0);
}

ByyTextBrowser::~ByyTextBrowser()
{

}

void ByyTextBrowser::paintEvent( QPaintEvent *event )
{
	QPainter p(this);
	p.setBrush(Qt::NoBrush);
	p.fillRect(rect(),d_color);
	//p.setPen(QPen(Qt::black,1));
	//p.drawRect(rect().adjusted(0,0,-1,-1));

	QWidget::paintEvent(event);
}

void ByyTextBrowser::setText( const QString &text )
{
	label->setText(text);

	resize(label->width(),label->height());
}

void ByyTextBrowser::setColor( const QColor &color )
{
	d_color=color;
}
