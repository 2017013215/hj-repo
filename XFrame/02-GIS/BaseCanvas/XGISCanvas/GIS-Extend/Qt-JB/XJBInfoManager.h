#pragma once
// 读取并解析JB库

#include <QString>
#include <QMap>
#include <QFile>

namespace XGIS
{
	class Q_JBLib;
	class QJB_LAYER;
	class Q_JBCategory;
	class Q_JBItem;
	struct QJB_ITEM_UNIT_HEAD;
	struct QJB_POINT;
	struct ExceptiveInfo;
	class XJBInfoManager
	{
		friend class XJBWidget;
	private:
		XJBInfoManager(void);
		~XJBInfoManager(void);

	public:
		static XJBInfoManager* GetInstance();

		// 读取指定目录下的JB库文件
		bool ReadJBInfoByDir(const QString& _strFilePath);

		// 查找指定节点Item
		const Q_JBItem* FindJBItem(quint32 _unLibID, quint32 _unItemID) const;

		// 克隆指定节点Item信息
		Q_JBItem* CloneItem(quint32 _unLibID, quint32 _unItemID) const;

		// 判断指定Item是否是点JB
		const bool ItemIsPointType(quint32 _unLibID, quint32 _unItemID);

		// 获取指定Item额外扩展信息
		ExceptiveInfo* GetItemExceptiveInfo(quint32 _unLibID, quint32 _unItemID);

	private:
		// 读取指定库文件
		bool readJBLibPath(const QString& _strFilePath);
		// 读取特别控制XML
		bool readExceptive(const QString& _strXMLPath);
		// 读取指定类别JB信息
		bool readCateJBInfo(Q_JBCategory* _pCate,const QJB_LAYER* _pLayer, QFile& _refFile );

		// 清空所有数据并返回
		void clearAllData();
		// 根据ID获取类别层级
		int getLayerLevel(quint32 _unID);
		// 判断是否是改分类的子类别--返回NULL说明不是改类别下的任何节点，否则返回节点指针
		Q_JBCategory* isSubCate(Q_JBCategory* _pParCate, quint32 _unSubID);
		// 解析Item图元信息
		const bool parseItemUnit(Q_JBItem* _pItem);

		// 解析曲/直线图元  ---U_BLINE
		const bool parseBLine(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo);
		// 解析内含曲/直线 --U_BBLINE
		const bool parseBBLine(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo);
		// 解析多边形/直线 --U_PLINE
		const bool parsePLine(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo);
		// 解析矩形
		const bool parseRect(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo);
		// 解析椭圆
		const bool parseEllipse(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo);
		// 解析饼
		const bool parsePie(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo);
		// 解析弧
		const bool parseArc(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo);
		// 解析多边形
		const bool parsePolygon(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo);
		// 文本
		const bool parseText(Q_JBItem* _pJBItem, QJB_ITEM_UNIT_HEAD* _pUnitInfo);


		// 计算顶点数量
		int browseLine(int *_pData, int flag);
		void checkPointCache(quint32 _unNewCount);

	private:
		static XJBInfoManager ms_Instance;

		QString m_strCurJBDir;
		bool m_bIsLoad;

		QMap<quint32,Q_JBLib*> m_cJBLibMap;
		QList<Q_JBLib*> m_cJBLibList;

		QMap<quint32,QMap<quint32,ExceptiveInfo*>> m_cExceptiveInfos;


		const static char ms_strCheckTag[];


		const int ms_nConstCurveFlag;
		QJB_POINT* m_pPointCache;
		quint32 m_unPointCacheCount;
	};
}