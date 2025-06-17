//******************************************************************************
// @Copyright:军尚科技
// @file uinitsGraphicsItem.h
// @Description:队形中实体对应图形项
// @Author：LYJ
// @Date：2017.12.20
//******************************************************************************

#ifndef UINITSGRAPHICSITEM_H
#define UINITSGRAPHICSITEM_H

#include <QGraphicsObject>
#include <QObject>
#include <QGraphicsItem>
#include <QRectF>

#include "commonStruct.h"

class ByyEntityObject;
class uinitsGraphicsItem : public QGraphicsObject
{
	Q_OBJECT

public:
	uinitsGraphicsItem(QGraphicsObject *parent = NULL);
	uinitsGraphicsItem(const QPointF &point, UNITSTYPE type = WARSHIP, bool isCenter = false,QGraphicsObject *parent = NULL);
	uinitsGraphicsItem(const double &x, const double &y,UNITSTYPE type = WARSHIP, bool isCenter = false,QGraphicsObject *parent = NULL);
	~uinitsGraphicsItem();

protected:
	//重绘
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);

	//鼠标按下
	void mousePressEvent(QGraphicsSceneMouseEvent *event);

	//鼠标移动
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

	//鼠标释放
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
	//将item的外边界作为矩形返回
	QRectF boundingRect() const;

	//设置图形项中心点
	void setCenterPos(const QPointF &point);

	//获取图形项中心点
	QPointF& getCenterPos();

	//设置图形项中心点真实坐标
	void setCenterPosReality(const QPointF &point);

	//获取图形项中心点真实坐标
	QPointF& getCenterPosReality();

	//设置缩放系数
	void setScaleValue(double value);

	//设为中心项
	void setIsCenterItem(bool flag);

	//是否中心项
	bool isCenterItem();

	//设置实体类型
	void setEntityType(UNITSTYPE type);

	//设置实体
	void setCurrentEntity(ByyEntityObject* entity);

	//移除实体
	void removeCurrentEntity();

	//获取实体
	ByyEntityObject* getCurrentEntity();

signals:
	//界面显示距离和角度
	void ShowDistanceAndAngle(uinitsGraphicsItem* item);

private:
	//坐标转换成真实坐标
	void posToReality();

	//真实坐标转换成坐标
	void posToCoordinate();

private:
	QPointF m_itemCenterPos;	//图形项中心点坐标
	QPointF m_itemCenterPosReality;	//图形项中心点实际
	double m_scaleValue;		//缩放系数
	bool isMoved;
	bool isCenter;				//是否是中心项
	UNITSTYPE m_entityType;
	ByyEntityObject* m_currentEntity;
};

#endif // UINITSGRAPHICSITEM_H
