#include "JBPixmapHelper.h"

#include <QApplication>
#include <QWidget>
#include <QMutex>

#include "QtDrawJBPainter.h"
#include "JBType.h"
#include "JunBiaoPainter.h"

/*
class CJBPixmapWidget : public QWidget
{
public:
    CJBPixmapWidget(QWidget *parent = nullptr):m_pPixmap(0){}
    ~CJBPixmapWidget(){}

public:
    void drawPixmap(QPixmap *pPixmap)
    {
        m_pPixmap = pPixmap;
        repaint();
    }

protected:
    void paintEvent(QPaintEvent *pEvent)
    {
        Q_UNUSED(pEvent);

        if (m_pPixmap == nullptr)
            return;

        CQtDrawJBPainter painter(m_pPixmap);
        painter.fillRect(m_pPixmap->rect(),Qt::red);

        m_pPixmap = nullptr;
      // if (CLoadJB::GetInstance()->GetGroup(uId, rGroup))
      // {
      //     CQtDrawJBPainter painter(&pixmap);
      //     painter.fillRe
	  ct(pixmap.rect(),Qt::red);
      //     //painter.SetJBSize(width,height);
      //     //painter.setPen(Qt::red);
      //     //painter.translate(width/2.0, height/2.0);
      //     //painter.DrawGroup(rGroup);
      // }
    }


private:
    QPixmap *m_pPixmap;

};
*/

CJBPixmapHelper *CJBPixmapHelper::m_pInstance = NULL;

CJBPixmapHelper *CJBPixmapHelper::Instance()
{
    static QMutex mutex;
    if (NULL == m_pInstance)
    {
        QMutexLocker locker(&mutex);
        if (NULL ==m_pInstance)
        {
            m_pInstance = new CJBPixmapHelper();
        }
    }
    return m_pInstance;
}

void CJBPixmapHelper::setBackColor(QColor color)
{
    m_color = color;
}

QPixmap CJBPixmapHelper::getPixmap(const QT_GROUP &vGroup, int width, int height ,const QColor& color,int nLineWidth)
{
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);	// 透明化
	if(color.isValid())
	{
        //JBPainterSingleColor painter(&pixmap);
        CQtDrawJBPainter painter(&pixmap);
        painter.setUseUserColor(true);
		painter.SetUserColor(color);
		if(-1 != nLineWidth)
		{
			QPen pen = painter.pen();
			pen.setWidth(nLineWidth);
			painter.setPen(pen);
        }
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(pixmap.rect(),m_color);
        //painter.setPen(color);
        painter.translate(width/2.0, height/2.0);
        painter.SetJBSize(width,height);
        painter.DrawGroup(vGroup);
	}
	else 
	{
        CQtDrawJBPainter painter(&pixmap);
		if(-1 != nLineWidth)
		{
            QPen pen = painter.pen();
			pen.setWidth(nLineWidth);
			painter.setPen(pen);
        }
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(pixmap.rect(),m_color);
        painter.translate(width/2.0, height/2.0);
        painter.SetJBSize(width,height);
        painter.DrawGroup(vGroup);
	}

    pixmap.save("E:/ypb.png");
    return pixmap;
}

QPixmap CJBPixmapHelper::getPixmap(const QT_GROUP &vGroup,  QSize size,const QColor& color,int nLineWidth)
{
    return getPixmap(vGroup,size.width(),size.height(),color,nLineWidth);
}

QPixmap CJBPixmapHelper::getPixmap(const JB_GROUP &vGroup, QSize size, const QColor &color, int nLinewidth)
{
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    JunBiaoRender drawJB(painter);
//    QPen oldpen = painter.pen();
//    QPen pen;
//    pen.setWidth(nLinewidth);
//    pen.setColor(color);
//    painter.setPen(pen);
    if(color.isValid())
        drawJB.setColor(color);
    if(nLinewidth != -1)
        drawJB.setLineWidth(nLinewidth);
    drawJB.draw(vGroup.shapeInfo,QRect(pixmap.rect()));

//    painter.setPen(oldpen);
    return pixmap;
}

CJBPixmapHelper::CJBPixmapHelper(QObject *)
    : QObject(qApp)
{
	m_color = QColor(0,0,0,0);	///< 默认全透明
}

CJBPixmapHelper::~CJBPixmapHelper()
{

}
