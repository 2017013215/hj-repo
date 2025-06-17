#ifndef QTDRAWJBWIDGET_H
#define QTDRAWJBWIDGET_H

#include <QDialog>

class QLineEdit;
class CJBTreeView;
class CJBTreeModel;
class CQtDrawJB;
class QScrollArea;
class CJBTreeItem;
class ParseJunBiao;
//************************************************************************
// 类名称：ByyQtDrawJBWidget
// 类说明：军标的停靠窗口，本类中绘制了默认目录data\JB下的所有军标
//			双击树的节点会绘制当前节点的军标
//			双击当前军标为当前选中的军标并发出绘制消息
//			
//------------------------------------------------------------------------
// 消息说明：slotItemClicked				注：双击树节点发出的消息响应
//												如果是组节点会绘制整个组的军标到军标窗口
//------------------------------------------------------------------------
// 消息说明：slotSelectJBChange：			注：用于响应军标窗口的当前军标变化
//												此函数会向所有观察者发送一个选中军标代号
//------------------------------------------------------------------------
// 接口说明：virtual getJBPixMap			注：用于获取到一个以加载的军标图片
//------------------------------------------------------------------------
// 重写说明：
//************************************************************************
namespace XGIS
{
	class XAddJBMapTool;
}
using namespace XGIS;
class ByyQtDrawJBWidget : public QDialog
{
	Q_OBJECT
	enum ItemType
	{
        TREE_NONE = 0,
		TREE_Trunk,
		TREE_Branch,
        TREE_LEAF,
        TREE_Group,
        TREE_DATA
	};

public:
	ByyQtDrawJBWidget(XAddJBMapTool* _pAddJBTool);
	~ByyQtDrawJBWidget();


	virtual void closeEvent(QCloseEvent *event);

	const bool IsCheckedItem();

	const bool GetCheckItem(quint32& _unRefLibID, quint32& _unRefItemID);

	//************************************
	// 函数名称:    getJBPixMap
	// 函数域:  	ByyQtDrawJBWidget::getJBPixMap
	// 权限:    	virtual public 
	// 返回值:   	QPixmap					注：返回的pixmap
	// 函数描述: 	通过库id和组id获取军标
	// 参数: 		int headID				注：库ID
	// 参数: 		int group				注：组ID
	// 参数: 		const QColor & color	注：军标颜色
	// 参数: 		int nwidth				注：军标大小
	// 参数: 		int nheight				
	//************************************
	virtual QPixmap getJBPixMap(int headID,int group, int nwidth, int nheight, const QColor& color, int nLineWidth);

	void clearSel();

	const int GetCurSelJBImg(QPixmap& _refImg, int _nW, int _nH, const QColor& _clr, int _nLineW);
protected slots:

	//************************************
	// 函数名称:    slotItemClicked
	// 函数域:  	ByyQtDrawJBWidget::slotItemClicked
	// 权限:    	protected 
	// 返回值:   	void
	// 函数描述: 	军标点击进行绘制
	// 参数: 		QTreeWidgetItem * pItem
	// 参数: 		int nColumn
	//************************************
	void slotItemClicked(CJBTreeItem *pItem);

	//************************************
	// 函数名称:    slotSelectJBChange
	// 函数域:  	ByyQtDrawJBWidget::slotSelectJBChange
	// 权限:    	protected 
	// 返回值:   	void
	// 函数描述: 	接收来自绘制窗口的选中军标组ID
	// 参数: 		int gUid				注：组ID
	//************************************
	void slotSelectJBChange(int gUid);


    /**
     * @brief slotSreach    搜索军标
     * @param text          军标名称
     */
    void slotSreach(const QString& text);

private:
	/**参数1：库代号  参数2：军标号*/
	void currentJBChange(int,int);

	void createUI();

    void drawGroup(CJBTreeItem* pItem);

    void drawJBByData(CJBTreeItem* pItem);

public:
signals:
	void slgCloseWnd();

private:
    QLineEdit*             m_edSreach;
    CJBTreeItem*        m_currentDisplay;				//当前显示的节点信息
    //QWidget*             m_contens;						//当前的一个父窗口，用于中介作用
    CQtDrawJB*		    m_drawJBWidget;					//绘制军标窗口
    CJBTreeView*       m_treeView;						//军标的树节点
    CJBTreeModel*     m_pModel;							//军标的保存的数字模型
    QScrollArea*        m_scroll;						//滚动条
    QPixmap             m_pixmap;

    ParseJunBiao* m_parseJunBiaoNew;

	XAddJBMapTool* m_pAddJBTool;
};

#endif // QTDRAWJBWIDGET_H
