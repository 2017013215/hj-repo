//******************************************************************************
// @file unitsGraphicsScene.h
// @Description:队形编辑场景
// @Author：LYJ
// @Date：2017.12.20
// @Copyright:军尚科技
//******************************************************************************

#ifndef UNITSGRAPHICSSCENE_H
#define UNITSGRAPHICSSCENE_H

#include <QGraphicsScene>

class uinitsGraphicsItem;
class unitsGraphicsScene : public QGraphicsScene
{
	Q_OBJECT

public:
	unitsGraphicsScene(QObject *parent = 0);
	~unitsGraphicsScene();

	// 绘制磁吸线 水平
	void MagneticHLine();

	// 绘制磁吸线 垂直
	void MagneticVLine();

protected:
	virtual void wheelEvent(QGraphicsSceneWheelEvent *event);

	virtual void keyPressEvent(QKeyEvent *event);

	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
	QList<uinitsGraphicsItem*> m_listItem;
	double m_scaleValue;	
	bool isMoved;
	//item磁吸/吸附/停靠附近（或指定为编队长）item中心效果
	QPointF mouse_move_point_, mouse_pressed_point_; //鼠标按下与鼠标移动位置
	QGraphicsLineItem *item_hline_,*item_vline_; //水平磁吸线，垂直磁吸线
	uinitsGraphicsItem *item_temp_;//鼠标当前拖动对象，磁吸线会用到该对象
signals:
	//比例信息
	void showPlottingscale(int value);
	
};

#endif // UNITSGRAPHICSSCENE_H
