#ifndef QT_DRAWJB_H
#define QT_DRAWJB_H

#include <QWidget>
#include "LoadJB.h"
#include <QMap>
#include "MyRect.h"
#include "JBWindow/JunBiaoInfo.hpp"
#define JB_GRID_WIDTH   1600 /// 网格宽度
#define JB_GRID_HEIGHT  800 /// 网格高度
#define JB_GRID_BORDER  40  /// 左上边界
#define JB_GRID_MARGIN  160  /// 网格大小
#define JB_ITEM_WIDTH 30
#define JB_ITEM_HEIGHT 30

class CQtDrawJBPainter;
class QLabel;

class CQtDrawJB : public QWidget
{
    Q_OBJECT

public:
    explicit CQtDrawJB(QWidget *parent = 0);
    ~CQtDrawJB();


    void setCurrentUnIDs(const QVector<QT_GROUP> &vGroups);

    void setCurrentUnIDs(const QVector<JB_GROUP> &vGroups);

	void SetSelID(int _nSelId = -1){ m_nSelID = _nSelId; update(); };

	int GetSelID(){ return m_nSelID; };
protected:
    /// 重写绘制事件
    virtual void paintEvent(QPaintEvent *);

    void drawBackground(QPainter *painter);

	void drawJB(CQtDrawJBPainter *painter, const QT_GROUP &rGroup, const QRect &rect);

	virtual void mouseMoveEvent ( QMouseEvent * event );

	void mouseDoubleClickEvent(QMouseEvent * event);
	// LXQ Add
	void mousePressEvent(QMouseEvent *event);

signals:
	void currentJBChange(int);
private:
    QVector<QT_GROUP>              myUnIDs;
    QVector<JB_GROUP>              m_vReadyGroups;


    QMap<MyRect,QT_GROUP>		m_jbGroup;              ///绘制军标的rect位置对应军标
    QMap<MyRect,JB_GROUP>     m_mpGroup;            ///绘制军标的rect位置对应军标

    int                                            m_row;
    int                                            m_column;

	// LXQ Add
	int m_nSelID;
};

#endif // QT_DRAWJB_H
