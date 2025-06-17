#include "StdAfx.h"
#include "uinitsGraphicsItem.h"
#include "formationImage.h"

uinitsGraphicsItem::uinitsGraphicsItem(QGraphicsObject *parent)
	: QGraphicsObject(parent)
	,m_scaleValue(1.0)
	,isMoved(false)
	,isCenter(false)
	,m_entityType(WARSHIP)
	,m_currentEntity(NULL)
{

	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

uinitsGraphicsItem::uinitsGraphicsItem(const QPointF &point, UNITSTYPE type, bool isCenter, QGraphicsObject *parent /*= NULL*/)
	: QGraphicsObject(parent)
	,m_scaleValue(1.0)
	,isMoved(false)
	,isCenter(isCenter)
	,m_entityType(type)
	,m_currentEntity(NULL)
{
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	setCenterPosReality(point);
	posToCoordinate();

}

uinitsGraphicsItem::uinitsGraphicsItem(const double &x, const double &y, UNITSTYPE type, bool isCenter,QGraphicsObject *parent /*= NULL*/)
	: QGraphicsObject(parent)
	,m_scaleValue(1.0)
	,isMoved(false)
	,isCenter(isCenter)
	,m_entityType(type)
	,m_currentEntity(NULL)
{
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	setCenterPosReality(QPointF(x,y));
	posToCoordinate();
}

uinitsGraphicsItem::~uinitsGraphicsItem()
{

}

//将item的外边界作为矩形返回
QRectF uinitsGraphicsItem::boundingRect() const
{
	switch(m_entityType)
	{
	case WARSHIP:
		return QRectF(-formationImage::getInstance()->getPixmap(SHIP_YES)->width()/2, -formationImage::getInstance()->getPixmap(SHIP_YES)->height()/2,
			formationImage::getInstance()->getPixmap(SHIP_YES)->width(), formationImage::getInstance()->getPixmap(SHIP_YES)->height());
	case AIRPLANE:
		return QRectF(-formationImage::getInstance()->getPixmap(AIRPLANE_YES)->width()/2, -formationImage::getInstance()->getPixmap(AIRPLANE_YES)->height()/2,
			formationImage::getInstance()->getPixmap(AIRPLANE_YES)->width(), formationImage::getInstance()->getPixmap(AIRPLANE_YES)->height());
	case PANZER:
		break;
	default:;
	}
	return QRectF();
}

//重绘
void uinitsGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */)
{
	switch(m_entityType)
	{
	case WARSHIP:
		if(m_currentEntity)
		{
			painter->drawPixmap(-formationImage::getInstance()->getPixmap(SHIP_YES)->width()/2, -formationImage::getInstance()->getPixmap(SHIP_YES)->height()/2, *(formationImage::getInstance()->getPixmap(SHIP_YES)));
		}
		else
		{
			painter->drawPixmap(-formationImage::getInstance()->getPixmap(SHIP_NO)->width()/2, -formationImage::getInstance()->getPixmap(SHIP_NO)->height()/2, *(formationImage::getInstance()->getPixmap(SHIP_NO)));
		}
		break;
	case AIRPLANE:
		if(m_currentEntity)
		{
			painter->drawPixmap(-formationImage::getInstance()->getPixmap(AIRPLANE_YES)->width()/2, -formationImage::getInstance()->getPixmap(AIRPLANE_YES)->height()/2, *(formationImage::getInstance()->getPixmap(AIRPLANE_YES)));
		}
		else
		{
			painter->drawPixmap(-formationImage::getInstance()->getPixmap(AIRPLANE_NO)->width()/2, -formationImage::getInstance()->getPixmap(AIRPLANE_NO)->height()/2, *(formationImage::getInstance()->getPixmap(AIRPLANE_NO)));
		}
		break;
	case PANZER:
		break;
	default:;
	}

	if(isCenter)
	{
		painter->drawPixmap(-formationImage::getInstance()->getPixmap(CENTERITEM)->width()/2, -formationImage::getInstance()->getPixmap(CENTERITEM)->height()/2, *(formationImage::getInstance()->getPixmap(CENTERITEM)));
	}

// 	if(option->state & QStyle::State_Selected)
// 	{
// 		painter->setPen(QPen(QColor(255,255,255,100), 1, Qt::DashLine));
// 		painter->setBrush(Qt::NoBrush);
// 		painter->drawRect(boundingRect().adjusted(-3,-3,3,3));
// 	}
}

//设置中心点
void uinitsGraphicsItem::setCenterPos(const QPointF &point)
{
	if(m_itemCenterPos == point)
	{
		return;	
	}
	m_itemCenterPos = point;
	this->setPos(m_itemCenterPos);
}

//获取图形项中心点
QPointF& uinitsGraphicsItem::getCenterPos()
{
	return m_itemCenterPos;
}

//设置缩放系数
void uinitsGraphicsItem::setScaleValue(double value)
{
	m_scaleValue = value;
	posToCoordinate();
}

void uinitsGraphicsItem::posToReality()
{
	double x = m_itemCenterPos.x() * 1000 / 10.0 * m_scaleValue;
	double y = m_itemCenterPos.y() * 1000 / 10.0 * m_scaleValue;
	setCenterPosReality(QPointF(x, y));
}

void uinitsGraphicsItem::posToCoordinate()
{
	double x = m_itemCenterPosReality.x() * 10 / 1000.0 / m_scaleValue;
	double y = m_itemCenterPosReality.y() * 10 / 1000.0 / m_scaleValue;
	setCenterPos(QPointF(x, y));
}

//设置图形项中心点
void uinitsGraphicsItem::setCenterPosReality(const QPointF &point)
{
	m_itemCenterPosReality = point;
}

void uinitsGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	isMoved = true;
	setSelected(true);
	setCursor(QCursor(Qt::OpenHandCursor));
	//qDebug()<<"Set Pos"<<" "<<__FILE__<<__FUNCTION__;
}

void uinitsGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if(isMoved && !isCenter)
	{
		QPointF pos = event->scenePos();
		setCenterPos(pos);
		posToReality();
		this->setPos(pos);
		this->scene()->update();
		//qDebug()<<"Set Pos"<<" "<<pos<<__FILE__<<__FUNCTION__;
	}
}

void uinitsGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	isMoved = false;
	emit ShowDistanceAndAngle(this);
	setSelected(false);
	setCursor(QCursor(Qt::ArrowCursor));
	//qDebug()<<"Set Pos"<<" "<<__FILE__<<__FUNCTION__;
}

//设为中心项
void uinitsGraphicsItem::setIsCenterItem(bool flag)
{
	isCenter = flag;
}

//获取图形项中心点真实坐标
QPointF& uinitsGraphicsItem::getCenterPosReality()
{
	return m_itemCenterPosReality;
}

//设置实体类型
void uinitsGraphicsItem::setEntityType(UNITSTYPE type)
{
	m_entityType = type;
}

//设置实体
void uinitsGraphicsItem::setCurrentEntity(ByyEntityObject* entity)
{
	m_currentEntity = entity;
	this->setToolTip(m_currentEntity->name());
	update();
	//this->scene()->update();
}

//获取实体
ByyEntityObject* uinitsGraphicsItem::getCurrentEntity()
{
	return m_currentEntity;
}

//移除实体
void uinitsGraphicsItem::removeCurrentEntity()
{
	m_currentEntity = NULL;

	this->setToolTip("");
	update();
	this->scene()->update();
}

//是否中心项
bool uinitsGraphicsItem::isCenterItem()
{
	return isCenter;
}


