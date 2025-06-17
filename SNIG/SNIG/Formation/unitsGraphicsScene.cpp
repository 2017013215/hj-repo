#include "StdAfx.h"
#include "unitsGraphicsScene.h"
#include "uinitsGraphicsItem.h"

unitsGraphicsScene::unitsGraphicsScene(QObject *parent)
	: QGraphicsScene(parent)
	,m_scaleValue(1.0)
	,isMoved(false)
	,item_temp_(nullptr)
	,item_hline_(nullptr)
	,item_vline_(nullptr)
{

}

unitsGraphicsScene::~unitsGraphicsScene()
{

}

void unitsGraphicsScene::MagneticHLine()
{
	// 删除上次磁吸线 水平
	if (item_hline_) { removeItem(item_hline_); delete item_hline_; item_hline_ = nullptr; }
	// 水平磁吸线
	// 磁吸线判断区域 判断区域越小越好，最好所有在区域内的图元都在同一水平线上，这样相近的两个图元不会互相干扰
	QRectF magarea(QPointF(this->sceneRect().left(), mouse_move_point_.y() - 1), QSizeF(this->width(), 1));
	QList<QGraphicsItem*> area_items = items(magarea);
	// 去除掉自己
	if (area_items.contains(item_temp_)) {
		area_items.removeOne(item_temp_);
	}
	// 去除垂直磁吸线
	if (area_items.contains(item_vline_)) {
		area_items.removeOne(item_vline_);
	}
	// 判断区域内有无图元
	if (!area_items.isEmpty())
	{
		foreach (QGraphicsItem* item , area_items)
		{
			// 根据鼠标y坐标和另一个图元判断，水平对齐
			float y = mouse_move_point_.y();
			float ny = item->sceneBoundingRect().center().y();
			if (y > ny - 25 && y < ny + 25)
			{
				if (item_hline_) { removeItem(item_hline_); delete item_hline_; item_hline_ = nullptr; }
				QPointF temp_point = QPointF(item_temp_->sceneBoundingRect().center().x(), ny) - item_temp_->boundingRect().center();
				item_temp_->setPos(temp_point);
				//qDebug()<<"Set Pos"<<" "<<temp_point<<__FILE__<<__FUNCTION__;
				//画线
				item_hline_ = new QGraphicsLineItem(QLineF(item_temp_->sceneBoundingRect().center(), item->sceneBoundingRect().center()));
				QPen pen(Qt::red, 1,  Qt::DashLine/*, Qt::FlatCap, Qt::RoundJoin*/);
				item_hline_->setPen(pen);
				addItem(item_hline_);
			}
		}
	}
}

void unitsGraphicsScene::MagneticVLine()
{
	// 删除上次磁吸线 垂直
	if (item_vline_) { removeItem(item_vline_); delete item_vline_; item_vline_ = nullptr; }

	// 水平磁吸线
	// 磁吸线判断区域 判断区域越小越好，最好所有在区域内的图元都在同一水平线上，这样相近的两个图元不会互相干扰
	QRectF magarea(QPointF(mouse_move_point_.x() - 1, this->sceneRect().top()), QSizeF(1, this->height()));
	QList<QGraphicsItem*> area_items = items(magarea);
	// 去除掉自己
	if (area_items.contains(item_temp_)) {
		area_items.removeOne(item_temp_);
	}
	// 去除水平磁吸线
	if (area_items.contains(item_hline_)) {
		area_items.removeOne(item_hline_);
	}
	// 判断区域内有无图元
	if (!area_items.isEmpty())
	{
		foreach (QGraphicsItem* item , area_items)
		{
			// 根据鼠标y坐标和另一个图元判断，水平对齐
			float x = mouse_move_point_.x();
			float nx = item->sceneBoundingRect().center().x();
			if (x > nx - 25 && x < nx + 25)
			{
				if (item_vline_) { removeItem(item_vline_); delete item_vline_; item_vline_ = nullptr; }
				QPointF temp_point = QPointF(nx, item_temp_->sceneBoundingRect().center().y()) - item_temp_->boundingRect().center();
				item_temp_->setPos(temp_point);
				//画线
				item_vline_ = new QGraphicsLineItem(QLineF(item_temp_->sceneBoundingRect().center(), item->sceneBoundingRect().center()));
				QPen pen(Qt::red, 1,  Qt::DashLine/*, Qt::FlatCap, Qt::RoundJoin*/);
				item_vline_->setPen(pen);
				addItem(item_vline_);
			}
		}
	}
}

void unitsGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	if(event->delta() < 0)
	{
		if(m_scaleValue >= 1)
		{
			if(m_scaleValue >= 10)
				return;
			m_scaleValue++;
		}
		else
		{
			m_scaleValue += 0.1;
		}
	}
	else
	{
		if(m_scaleValue > 1)
		{
			m_scaleValue--;
		}
		else
		{
			if(m_scaleValue < 0.2)
				return;
			m_scaleValue -= 0.1;
		}
	}

	emit showPlottingscale(1000*m_scaleValue);
	
	foreach( QGraphicsItem* t_item, this->items())
	{
		dynamic_cast<uinitsGraphicsItem*>(t_item)->setScaleValue(m_scaleValue);
	}
	update();
}

void unitsGraphicsScene::keyPressEvent(QKeyEvent *event)
{
	return QGraphicsScene::keyPressEvent(event);
}

void unitsGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	// ! [1] 获取鼠标点击位置
	mouse_pressed_point_ = event->scenePos();
	if (event->button() == Qt::LeftButton)
	{
		// 降序获取，第一个为顶层图元
		QList<QGraphicsItem*> select_items = this->items(event->scenePos(), Qt::IntersectsItemShape, Qt::DescendingOrder);
		if (select_items.isEmpty())
			return QGraphicsScene::mousePressEvent(event);
		// 获取当前点击图元
		item_temp_ = dynamic_cast<uinitsGraphicsItem*>(select_items.at(0));
		if (item_temp_->isCenterItem())
		{
			item_temp_ = nullptr;
		}
	}
	else if (event->button() == Qt::RightButton) 
	{
		
	}
	return QGraphicsScene::mousePressEvent(event);
}

void unitsGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	// ! [1] 实时获取鼠标位置
	mouse_move_point_ = event->scenePos();

	QList<QGraphicsItem*> area_items = items();
	// 去除垂直磁吸线
	if (area_items.contains(item_vline_)) {
		area_items.removeOne(item_vline_);
	}
	// 去除水平磁吸线
	if (area_items.contains(item_hline_)) {
		area_items.removeOne(item_hline_);
	}

	QGraphicsScene::mouseMoveEvent(event);

	// ! [5] 绘制磁吸线
	// 判断选中多个不走磁吸线逻辑
	if (selectedItems().size() > 1) {
		return;
	}
	if (item_temp_ == nullptr) {
		return;
	}
	// 移动图元时，保证鼠标位于图元中心点
	item_temp_->setPos(mouse_move_point_ - item_temp_->boundingRect().center());
	// 水平磁吸线
	MagneticHLine();
	// 垂直磁吸线
	MagneticVLine();
}

void unitsGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	// ! [1]  临时图元置空
	item_temp_ = nullptr;

	// ! [2] 磁吸线删除
	if (item_hline_) { removeItem(item_hline_); delete item_hline_; item_hline_ = nullptr; }
	if (item_vline_) { removeItem(item_vline_); delete item_vline_; item_vline_ = nullptr; }
	return QGraphicsScene::mouseReleaseEvent(event);
}

