#include <QPainter>

#include "QtDrawJB.h"
#include "QtDrawJBPainter.h"
#include "LoadJB.h"
#include "JunBiaoPainter.h"

#include <QTransform>
#include <QDebug>
#include <QLabel>
#include <QMouseEvent>


CQtDrawJB::CQtDrawJB(QWidget *parent) :
    QWidget(parent)
		, m_row(0)
		, m_column(0)
		, m_nSelID(-1)
		
{
	this->setMouseTracking (true);
	setMinimumWidth(JB_ITEM_WIDTH + 20);//最少画两列军标	
}

CQtDrawJB::~CQtDrawJB()
{
}

void CQtDrawJB::setCurrentUnIDs(const QVector<QT_GROUP> &vGroups)
{
    myUnIDs.clear();
    m_vReadyGroups.clear();
    myUnIDs = vGroups;    
    update();
}

void CQtDrawJB::setCurrentUnIDs(const QVector<JB_GROUP> &vGroups)
{
    m_vReadyGroups.clear();
    myUnIDs.clear();
    m_vReadyGroups = vGroups;
    update();
}

/// 绘制函数
void CQtDrawJB::paintEvent(QPaintEvent *)
{
    CQtDrawJBPainter painter(this);
	QRect rect = this->rect();

    static MyRect S_myRect;

	int rowitemCount = (rect.width()-20)/JB_ITEM_WIDTH;
	rowitemCount += rowitemCount== 0 ? 1:0;
	int colum = myUnIDs.size() / rowitemCount;
	//设置滚动条随军标个数变动而变动
	setMinimumHeight(colum * JB_ITEM_HEIGHT + JB_ITEM_HEIGHT + 20);
    drawBackground(&painter); /// 绘制网格

    
    painter.setRenderHint(QPainter::Antialiasing);
	QRect rectJB;
	if(m_column == 0)
	{
		return ;
	}


	
	int myunIDsCount = 0;
	m_jbGroup.clear();
    m_mpGroup.clear();

    if(!myUnIDs.isEmpty())
    {
        int rowCount = myUnIDs.size() / m_column;
        rowCount += myUnIDs.size()%m_column != 0 ? 1 : 0;

        for(int i = 0;i < rowCount;++i)
        {
            rectJB.setTop(10 + JB_ITEM_HEIGHT*i);
            rectJB.setBottom(rectJB.top() + JB_ITEM_HEIGHT);
            for(int j = 0;j < m_column;++j)
            {
                rectJB.setLeft(10 + JB_ITEM_WIDTH*j);
                rectJB.setRight(rectJB.left()+JB_ITEM_WIDTH);
                drawJB(&painter,myUnIDs[myunIDsCount],rectJB);
                myunIDsCount ++;

                if(myunIDsCount >= myUnIDs.size())
                    return;
            }
        }
    }
    else
    {
        int rowCount = m_vReadyGroups.size() / m_column;
        rowCount += m_vReadyGroups.size()%m_column != 0 ? 1 : 0;
        QVector<JB_GROUP>::iterator itr = m_vReadyGroups.begin();
        JunBiaoRender  render(painter);
        for(int i = 0;i < rowCount;++i)
        {
            rectJB.setTop(10 + JB_ITEM_HEIGHT*i);
            rectJB.setBottom(rectJB.top() + JB_ITEM_HEIGHT);
            for(int j = 0;j < m_column;++j)
            {
                rectJB.setLeft(10 + JB_ITEM_WIDTH*j);
                rectJB.setRight(rectJB.left()+JB_ITEM_WIDTH);

                ///绘制军标
                render.draw(itr->shapeInfo,rectJB);
                S_myRect.setLeft(rectJB.left());
                S_myRect.setTop(rectJB.top());
                S_myRect.setRight(rectJB.right());
                S_myRect.setBottom(rectJB.bottom());
                m_mpGroup[S_myRect] = *itr;

                itr++;
                if(itr == m_vReadyGroups.end())
                    return;
            }
        }
    }

}

void CQtDrawJB::drawJB(CQtDrawJBPainter *painter, const QT_GROUP &rGroup, const QRect &rect)
{
    //static QT_GROUP rGroup;
	static MyRect myRect;
	myRect.setLeft(rect.left());
	myRect.setRight(rect.right());
	myRect.setTop(rect.top());
	myRect.setBottom(rect.bottom());

	painter->save();
	m_jbGroup[myRect] = rGroup;
	painter->setPen(Qt::red);
	painter->SetJBSize(JB_ITEM_WIDTH,JB_ITEM_HEIGHT);
	if (rGroup.unID == m_nSelID)
	{
		painter->setPen(Qt::green);
		painter->drawRect(myRect);
		painter->setPen(Qt::red);
	}
	painter->translate(rect.center().x(), rect.center().y());
	painter->DrawGroup(rGroup);
	painter->restore();    
}
#include <QToolTip>
void CQtDrawJB::mouseMoveEvent(QMouseEvent * event)
{
	QPoint pt = event->globalPos();
	QPoint pt2 = event->pos();
	for(QMap<MyRect,QT_GROUP>::iterator it = m_jbGroup.begin();
		it != m_jbGroup.end();++it)
	{
		if(it.key().contains(pt2))
		{
			QString strText = QString("%1(%2)").arg(it->qstrName).arg(it->unID);
			QToolTip::showText(pt,strText,this);
            return;
		}
	}
   for(QMap<MyRect,JB_GROUP>::iterator itr = m_mpGroup.begin();
       itr  != m_mpGroup.end();++itr)
   {
       if(itr.key().contains(pt2))
       {
           qDebug() << itr.key();
           QString strText = QString("%1(%2)").arg(itr->strName).arg(itr->unGroupId);
           QToolTip::showText(pt,strText);
           return;
       }
   }
}

//双击该军标设置为当前选中军标
void CQtDrawJB::mouseDoubleClickEvent(QMouseEvent * event)
{
	QPoint pt = event->pos();
	for(QMap<MyRect,QT_GROUP>::iterator it = m_jbGroup.begin();
		it != m_jbGroup.end();++it)
	{
		if(it.key().contains(pt))
		{
			emit currentJBChange(it->unID);
            return;
		}
	}
    for(QMap<MyRect,JB_GROUP>::iterator itr = m_mpGroup.begin();
        itr != m_mpGroup.end();++itr)
    {
        if(itr.key().contains(pt))
        {
            emit currentJBChange(itr->unGroupId);
            return;
        }
    }
}

void CQtDrawJB::drawBackground(QPainter *painter)
{
    painter->save();

    /// 绘制背景色
    painter->fillRect(rect(),QColor(232,227,212));

    painter->setPen(QColor(128, 128, 128));

	QRect rect = this->rect();

	//画行
	int nHorizonal = (rect.height() - 20) / JB_ITEM_HEIGHT;
	int x2 = (width() - 20)%JB_ITEM_HEIGHT;	//取出最后一行不足画一条线的剩余宽度	
	for(int i = 0;i <= nHorizonal; ++i)
	{
		int y = JB_ITEM_HEIGHT * i + 10;
		painter->drawLine(rect.left() + 10, y, rect.right() - x2 - 10, y);
	}
	m_row = nHorizonal;
	
	//画列
	int nVertical = (rect.width() - 20) / JB_ITEM_WIDTH;
	int y2 = (height() - 20) % JB_ITEM_HEIGHT;//去除最后一列不足以话一列的剩余宽度
	for(int i = 0;i <= nVertical;++i)
	{
		int x = JB_ITEM_WIDTH * i + 10;
		painter->drawLine(x, rect.top() + 10, x, rect.bottom() - y2 - 10);
	}
	m_column = nVertical;

    painter->restore();
}

void CQtDrawJB::mousePressEvent(QMouseEvent *event)
{
	QPoint pt = event->pos();
	for (QMap<MyRect, QT_GROUP>::iterator it = m_jbGroup.begin();
		it != m_jbGroup.end(); ++it)
	{
		if (it.key().contains(pt))
		{
			m_nSelID = it->unID;
			//emit currentJBChange(it->unID);
			update();
			return;
		}
	}
	
	for (QMap<MyRect, JB_GROUP>::iterator itr = m_mpGroup.begin();
		itr != m_mpGroup.end(); ++itr)
	{
		if (itr.key().contains(pt))
		{
			//emit currentJBChange(itr->unGroupId);
			m_nSelID = itr->unGroupId;
			update();
			return;
		}
	}
	
	m_nSelID = -1;
	update();
}
