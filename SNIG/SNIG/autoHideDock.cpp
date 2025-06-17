#include "StdAfx.h"
#include "autoHideDock.h"

ByyAutoHideDock::ByyAutoHideDock( const QString & title, QWidget * parent /*= 0*/, Qt::WindowFlags flags /*= 0 */ ) 
	: QDockWidget(title,parent,flags)
{
	//connect(&myTimer,SIGNAL(timeout()),SLOT(hidden()));

	setTitleBarWidget(0);
	setAllowedAreas(Qt::NoDockWidgetArea);
	setFloating(true);

	layout()->setContentsMargins(0,0,0,0);

	setVisible(false);

	setAutoFillBackground(true);

	resize(330,800);
}

ByyAutoHideDock::~ByyAutoHideDock()
{

}

void ByyAutoHideDock::enterEvent( QEvent *event )
{
	myTimer.stop();
	return __super::enterEvent(event);
}

void ByyAutoHideDock::leaveEvent( QEvent * event )
{
	if (!rect().contains(mapFromGlobal(QCursor::pos())))
	{
		//myTimer.start(300);
	}
	return __super::leaveEvent(event);
}

void ByyAutoHideDock::hidden()
{
	myTimer.stop();
	close();
}

void ByyAutoHideDock::closeEvent( QCloseEvent *event )
{
	emit closed();
	__super::closeEvent(event);
}
