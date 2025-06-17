#ifndef JBSETWIDGET_H
#define JBSETWIDGET_H

#include <QDialog>

#include "../JunBiaoInfo.hpp"

class QHBoxLayout;
class QWidget;
class QLabel;
class QSpinBox;
class QHBoxLayout;
class QPushButton;
class QSpacerItem;
class QVBoxLayout;
class QToolButton;
class ByyDrawJBWidget;

struct QT_GROUP;
//************************************************************************
// 类名称：ByyJBSetDialog
// 类说明：军标属性调整窗口
//------------------------------------------------------------------------
// 消息说明：slotSizeChange				注：军标的大小发生变化后，控制绘制窗口跟随改变
//											依赖于CJBPixmapHelper::getPixMap
//------------------------------------------------------------------------
// 消息说明：slotColorChange			注：军标的颜色发生变化后，控制绘制窗口和选色按钮跟随改变
//											依赖于CJBPixmapHelper::getPixMap
//------------------------------------------------------------------------
// 消息说明：slotLineWidthChange		注：军标属性的线宽发生变化后，控制绘制窗口跟随
//											依赖于CJBPixmapHelper::getPixMap
//------------------------------------------------------------------------
// 接口说明：setJBGroup					注：要绘制的军标Group
//------------------------------------------------------------------------
// 重写说明：
//************************************************************************
class ByyJBSetDialog : public QDialog
{
	Q_OBJECT

public:
	ByyJBSetDialog(QT_GROUP* jbGroup, QWidget *parent = 0);
	~ByyJBSetDialog();

    int getSize()  const     { return m_nSize; }
    int  getLineWidth() const { return m_nLineWidth; }
    QColor getColor() const { return m_color; }

	
	//************************************
	// 函数名称:    setJBGroup
	// 函数域:  	ByyJBSetDialog::setJBGroup
	// 权限:    	public 
	// 返回值:   	void
	// 函数描述: 	设置一个军标到当前显示窗口
	// 参数: 		QT_GROUP * jbGroup		注：绘制当前军标
	//************************************
	void setJBGroup(QT_GROUP* jbGroup);

    void setJBGroup(JB_GROUP* jbGroup);
private slots:
	//void slotWidthChange(int i);
	//************************************
	// 函数名称:    slotSizeChange
	// 函数域:  	ByyJBSetDialog::slotSizeChange
	// 权限:    	private 
	// 返回值:   	void
	// 函数描述: 	军标大小改变
	// 参数: 		int						改变后的大小
	//************************************
	void slotSizeChange(int);

	//************************************
	// 函数名称:    slotColorChange
	// 函数域:  	ByyJBSetDialog::slotColorChange
	// 权限:    	private 
	// 返回值:   	void
	// 函数描述: 	军标颜色改变
	//************************************
	void slotColorChange();

	//************************************
	// 函数名称:    slotLineWidthChange
	// 函数域:  	ByyJBSetDialog::slotLineWidthChange
	// 权限:    	private 
	// 返回值:   	void
	// 函数描述: 	军标线宽改变
	// 参数: 		int
	//************************************
    void slotLineWidthChange(int);
private:
    void createUI();
    void updateData();
private:
	QHBoxLayout *horizontalLayout_5;
	ByyDrawJBWidget *widget;			//显示军标的widget
	QVBoxLayout *verticalLayout;
	QHBoxLayout *horizontalLayout;
	QLabel *label;
	QSpinBox *m_spiSize;				//军标大小调整数字框
	QHBoxLayout *horizontalLayout_2;
	QLabel *label_3;
	QSpinBox *m_spiLineWidth;			//军标线宽调整数字框
	QHBoxLayout *horizontalLayout_3;
	QLabel *label_4;
	QToolButton *m_btnColor;			//军标颜色改变颜色
	QHBoxLayout *horizontalLayout_4;
	QPushButton *m_btnFinish;			//点击完成后操作
	QSpacerItem *horizontalSpacer;

	QT_GROUP* m_jbGroup;
    JB_GROUP* m_pGroup;

    int m_nSize;
    int m_nLineWidth;
    QColor m_color;

};

#endif // JBSETWIDGET_H
