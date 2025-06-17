#ifndef DRAWJBWIDGET_H
#define DRAWJBWIDGET_H

#include <QWidget>
//************************************************************************
// 类名称：ByyDrawJBWidget
// 类说明：指定一个pixmap 绘制到窗口中
//------------------------------------------------------------------------
// 消息说明：
//------------------------------------------------------------------------
// 接口说明：
//------------------------------------------------------------------------
// 重写说明：paintEvent				注：重写窗口重绘事件，进行绘制选中的图
//************************************************************************
class ByyDrawJBWidget : public QWidget
{
	Q_OBJECT

public:
	ByyDrawJBWidget(QWidget *parent);
	~ByyDrawJBWidget();

	//************************************
	// 函数名称:    setPixmap
	// 函数域:  	ByyDrawJBWidget::setPixmap
	// 权限:    	public 
	// 返回值:   	void
	// 函数描述: 	绘制一个Pixmap
	// 参数: 		QPixmap & pixmap	注：指定的Pixmap
	//************************************
	void setPixmap(QPixmap& pixmap);
    QPixmap getPixmap() const;
protected:
	void paintEvent(QPaintEvent *e);
private:
	QPixmap m_pixmap;
	
};

#endif // DRAWJBWIDGET_H
