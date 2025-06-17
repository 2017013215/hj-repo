//******************************************************************************
// @file unitsGraphicsView.h
// @Description:队形编辑视图
// @Author：LYJ
// @Date：2017.12.20
// @Copyright:军尚科技
//******************************************************************************

#ifndef UNITSGRAPHICSVIEW_H
#define UNITSGRAPHICSVIEW_H

#include <QGraphicsView>

class uinitsGraphicsItem;
class unitsGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	unitsGraphicsView(QWidget *parent);
	~unitsGraphicsView();

private:
	double m_scaleValue;		//缩放系数
	
};

#endif // UNITSGRAPHICSVIEW_H
