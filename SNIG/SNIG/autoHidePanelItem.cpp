#include "StdAfx.h"
#include "autoHidePanelItem.h"

ByyAutoHidePanelItem::ByyAutoHidePanelItem(QWidget *parent)
	: QWidget(parent,Qt::SubWindow)
	,myChecked(false)
{
	setFixedSize(30,0);

	setAutoFillBackground(true);
}

ByyAutoHidePanelItem::~ByyAutoHidePanelItem()
{

}

void ByyAutoHidePanelItem::setIcon( const QIcon& icon )
{
	myIcon=icon;
}

void ByyAutoHidePanelItem::setText( const QString& text )
{
	myText=text;
	refreshSize();
}

void ByyAutoHidePanelItem::paintEvent( QPaintEvent *e )
{
	__super::paintEvent(e);

	QPainter p(this);

	if (myChecked)
	{
		p.fillRect(rect(),QBrush(myCheckedGradient));
	}
	
	p.setPen(QPen(Qt::gray));

	p.drawRect(rect());

	if (!myIcon.isNull())
	{
		p.drawPixmap(4,4,22,22,myIcon.pixmap(22,22));
	}

	p.drawText(10,width(),13,height()-10,Qt::TextWordWrap,myText);
}

void ByyAutoHidePanelItem::refreshSize()
{
	setFixedSize(width(),fontMetrics().width(myText)+width()+12);
	myCheckedGradient.setStart(width(),height()/2);
	myCheckedGradient.setFinalStop(0,height()/2);
	myCheckedGradient.setColorAt(0,QColor(111, 111, 111));
	myCheckedGradient.setColorAt(1,QColor(200, 200, 200));
}

void ByyAutoHidePanelItem::mouseReleaseEvent( QMouseEvent *e )
{
	setChecked(!myChecked);

	emit clicked(myChecked);

	__super::mouseReleaseEvent(e);
}

void ByyAutoHidePanelItem::setChecked( bool checked )
{
	if (checked!=myChecked)
	{
		myChecked=checked;
		update();
	}
}
