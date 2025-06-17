#include "DrawObserver.h"
#include "./../../Common_Base.h"
//#include "../../../ByyAppManager.h"
//#include "../../RibbonWindow.h"
#include "../QtDrawJBWidget.h"

ByyDrawObserver::ByyDrawObserver()
	//: IMapObserver()
{
	//m_pMainWindow = GetAppManager()->GetMainWindow();
}

ByyDrawObserver::~ByyDrawObserver()
{

}

void ByyDrawObserver::TouchOnToolBarBtn_Msg(int index)
{
#if 0
	switch(index)
	{
	case ACT_DRAWICON:
		if(!m_pMainWindow->getJBWidget()->isVisible())
		{
			if(!m_pMainWindow->getJBWidget()->hasFocus())
			{
				m_pMainWindow->getJBWidget()->setFocus();
			}
			m_pMainWindow->getJBWidget()->show();
		}
		else
		{
			m_pMainWindow->getJBWidget()->hide();
		}
		break;
	}
#endif
}
