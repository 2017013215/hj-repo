#ifndef DRAWOBSERVER_H
#define DRAWOBSERVER_H

//#include "IMapObserver.h"
//class ByyRibbonWindow;

//************************************************************************
// 类名称：ByyDrawObserver
// 类说明：菜单栏的观察者，用于操作军标窗口监视标绘按钮的状态
//------------------------------------------------------------------------
// 消息说明：
//------------------------------------------------------------------------
// 接口说明：
//------------------------------------------------------------------------
// 重写说明：TouchOnToolBarBtn_Msg				注：重写了标绘按钮的消息，会对军标窗口显隐状态进行绑定
//************************************************************************
class ByyDrawObserver //: public IMapObserver
{
public:
	ByyDrawObserver();
	~ByyDrawObserver();

	//************************************
	// 函数名称:    TouchOnToolBarBtn_Msg
	// 函数域:  	ByyDrawObserver::TouchOnToolBarBtn_Msg
	// 权限:    	virtual public 
	// 返回值:   	void
	// 函数描述: 	菜单按钮消息，点击后传入按钮索引index
	// 参数: 		int index				注：按钮的索引
	//************************************
	virtual void TouchOnToolBarBtn_Msg(int index);
protected:
	//ByyRibbonWindow* m_pMainWindow;
};

#endif // DRAWOBSERVER_H
