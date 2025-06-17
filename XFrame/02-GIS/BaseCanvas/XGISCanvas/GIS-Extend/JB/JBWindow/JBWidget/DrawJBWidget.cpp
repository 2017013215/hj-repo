#include "DrawJBWidget.h"
#include "QPainter"

ByyDrawJBWidget::ByyDrawJBWidget(QWidget *parent) : QWidget(parent)
{

}



ByyDrawJBWidget::~ByyDrawJBWidget()
{

}

void ByyDrawJBWidget::setPixmap(QPixmap& pixmap)
{
	m_pixmap = pixmap;
    update();
}

QPixmap ByyDrawJBWidget::getPixmap() const
{
       return m_pixmap;
}

///ÖØ»æÊÂ¼þ
void ByyDrawJBWidget::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	painter.drawPixmap((rect().width() - m_pixmap.width())/2,
						(rect().height() - m_pixmap.height())/2,
						m_pixmap);
}
