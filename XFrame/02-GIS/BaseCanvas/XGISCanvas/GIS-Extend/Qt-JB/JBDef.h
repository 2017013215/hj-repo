#pragma once
#include <QtCore/QtGlobal>
#include <QRect>
#include <QMap>
#include <QStringList>
#include <QVector>

namespace XGIS
{
	// 读取库时使用的结构体
	struct QJB_POINT
	{
		qint32 x;
		qint32 y;
	};

	struct QJB_RECT
	{
		qint32 nLeft;
		qint32 nTop;
		qint32 nRight;
		qint32 nBottom;
	};

	// JB-Head
	struct QJB_HEAD
	{
		char		csFileID[8];            // 文件标识
		char		csFileDesc[80];			// 文件描述
		qint32		nID;                    // 库代号
		quint32		unLayerNum;             // 图层数
		quint32		unGrpNum;               // 元组数
		quint32		unDataAddr;             // 数据起始地址
	};

	// JB-Layer
	struct QJB_LAYER
	{
		qint32      nLen;					 // 表项长度
		quint32     unID;      				 // 图层码
		qint32      nGrpNum;				 // 元组数 (0空类别；>0含有Num个标号；<0含有-Num个子类别
		char        csName[80];				 // 层名
		quint32     unGrpOffset;			 // 第一个军标元组地址
	};

	// JB-ItemInfo
	struct QJB_ITEM_HEAD
	{
		quint32	    unLen;					// 元组长度   所有图元+元组头
		quint32	    unGrpID;				// 元组码
		quint32	    unNextOff;				// 后一元组
		char        csName[80];				// 标号名
		QJB_POINT   pointHotSpot;			// 定位点
		qint16      snHotSpotAttr;			// 定位点属性
		qint16      snHaveTextBorder;		// 有文本框(个数)
		qint16      snAngle;				// 军标正方向 0-360度，0度y轴正向，逆时针
		qint16      snFlag;					// 标志值：0-点状军标，1-线状军标
		QJB_RECT    rectBound;				// 周界
	};

	// JB-ItemUnit_Head -图元头
	struct QJB_ITEM_UNIT_HEAD
	{
		qint16  snType;						// 图元类型
		qint16  snColorGrpNo;				// 图元属性长度
		quint32 unLen;						// 图元长度，包括图元头
	};

	// 图元类型
	enum ITEM_UNIT_TYPE
	{
		U_NONE=0,							// 无效
		U_BLINE,							// 曲/直线
		U_RECT,								// 矩形
		U_ELLI,								// 圆
		U_ARC,								// 弧
		U_PIE,								// 扇
		U_CHORD,							// 弦
		U_TEXT,								// 文字
		U_BMP,								// 位图
		U_PLINE,							// 多点连线
		U_PGON,								// 折线填充
		U_POINT,							// 点
		U_LINE,								// 直线
		U_TEXTBORDER,						// 文本框
		U_BITMAP,							// 位图
		U_BBLINE 							// 内含曲/直线
	};

	// 图元属性-线
	struct LINE_ATTR
	{
		qint16 snAttrSize;					// 结构长度
		qint16 snLineType;  				// 线型
		qint16 snLineWidth;     			// 线宽
		qint16 snClose;						// 图形闭合
		qint16 snOutLineWidth;  			// 衬线宽  0 无衬色
		qint16 snOutLineDir;				// 衬线方向 0，右，1左，2双
		qint16 snOutLineTran;				// 衬线透明度
		qint16 snFillType;  				// 填充类型
		qint16 snFillTran;					// 填充透明度
		qint16 snFadeMode;					// 渐变模式
		qint16 snFadeDir;					// 渐变方向
		qint16 snFadeCenter;				// 渐变中点
		qint16 snFadeRange;					// 渐变范围
		qint16 snSoftEdge;					// 边缘柔化
		qint32 nLineC;  					// 线色
		quint32 unOutLineC;					// 衬线色
		quint32 unFillC;					// 填充色
		quint32 unFadeC1;					// 渐变色1
		quint32 unFadeC2;					// 渐变色2
		char  cRev[4];						// 保留
	};

	// 字体效果属性
	union FONT_EFFECT
	{
		struct Bit
		{
			unsigned unItalic:1;			// 倾斜
			unsigned unBold:1;				// 加重
			unsigned unUnderLine:1;			// 下划线
			unsigned unBorder:1;			// 镶边
			unsigned unShadow:1;			// 阴影
			unsigned unBack:1;				// 背景框
			unsigned unAlign:2;				// 对齐方式0:居中,1:居左,2:居右
		};
		quint32 unByte;
	};

	// 文本属性
	struct TEXT_ATTR
	{
		qint16 snAttrSize;					// 结构长度
		qint16 snAngle;						// 转角:单位0.1度
		char  cFontName[32];				// 字体名
		qint16 snFontWidth;					// 字宽
		qint16 snFontHight;					// 字高
		quint32 unTextColor;				// 字色
		qint16 snColSpan;					// 字距
		qint16 snRowSpan;					// 行距
		FONT_EFFECT cEffect;				// 字体效果
	};

	/// 图像属性
	struct IMG_ATTR
	{
		qint32 nX;
		qint32 nY;
		qint32 nWidth;
		qint32 nHight;
		qint32 bTran;        // 是否透明
		quint32 unTranColor; // 透明色
	};
//-------------------------------------------------------------------------------------------------//

	// 特殊绘制
	struct ExceptiveInfo
	{
		enum EDrawType
		{
			edwType_None = 0,			// 无效
			edwType_DoubleArrow = 1,	// 双箭头
			edwType_FineArrow = 2,		// 细直箭标
			edwType_GatheringPlace = 3,	// 腰子形区域
			edwType_AttackArrow = 4,	// 攻击箭头
			edwType_StraightArrow = 5,	// >>
		};
		quint32 unLibID;
		quint32 unItemID;
		QString strName;
		quint32 unMinPoint;
		quint32 unMaxPoint;
		EDrawType eDwType;
	};
	typedef QMap<quint32,QMap<quint32,ExceptiveInfo*>> XExceptiveInfoMap;

	// 内存中存储时使用的数据结构
	struct Q_JBItem;	 // JB项
	// JB库分类
	struct Q_JBCategory
	{
		~Q_JBCategory()
		{
			foreach(Q_JBItem* pItem,cJBItems)
			{
				delete pItem;
				pItem = nullptr;
			}
			cJBItems.clear();
			cJBItemMap.clear();

			foreach(Q_JBCategory* pSubCate,cSubCateList)
			{
				delete pSubCate;
				pSubCate = nullptr;
			}
			cSubCateList.clear();
			cSubCateMap.clear();
		}
		quint32   unID;      	    // 类别码
		bool      bHaveInfo;        // 是否有JB信息 如果有JB信息那么将不含有子类别，如果含有子类别，将不含有JB信息
		QString	  strName;	        // 类别名称

		const Q_JBItem* FindItemByID(quint32 _unItemID) const
		{
			if(true == cJBItemMap.contains(_unItemID))
			{
				return cJBItemMap[_unItemID];
			}

			// 查找子类别
			const Q_JBItem* pRetItem = nullptr;
			foreach (Q_JBCategory* pSubCate, cSubCateList)
			{
				if(nullptr != pSubCate)
				{
					pRetItem = pSubCate->FindItemByID(_unItemID);
					if(nullptr != pRetItem)
					{
						return pRetItem;
					}
				}
			}

			return nullptr;
		}
		
		quint32 unLibID;

		// 子类别信息
		QList<Q_JBCategory*> cSubCateList;
		QMap<quint32,Q_JBCategory*> cSubCateMap;

		QList<Q_JBItem*> cJBItems;
		QMap<quint32,Q_JBItem*> cJBItemMap;
	};

	// JB库文件-存储的信息对应到某个库文件
	struct Q_JBLib
	{
		~Q_JBLib()
		{
			cCategoryMap.clear();
			foreach(Q_JBCategory* pCate, cCategoryList)
			{
				if(nullptr != pCate)
				{
					delete pCate;
					pCate = nullptr;
				}
			}
			cCategoryList.clear();
		}

		QString strFilePath;

		quint32 unID;		// ID
		QString strDesc;	// 描述

		const Q_JBItem* FindJBItem(quint32 _unItemID) const
		{
			const Q_JBItem* pRetItem = nullptr;
			foreach(Q_JBCategory* pCate, cCategoryList)
			{
				if(nullptr != pCate)
				{
					pRetItem = pCate->FindItemByID(_unItemID);
					if(nullptr != pRetItem)
					{
						return pRetItem;
					}
				}
			}

			return nullptr;
		}

		QMap<quint32,Q_JBCategory*> cCategoryMap;	// 无序分类集合-用于查询
		QList<Q_JBCategory*> cCategoryList;		// 有序分类列表-用于存储
	};


	struct Q_JBUnitSegm
	{
		bool bIsCurve;
		QVector<QPointF> cControlPnts;
		QVector<QPointF> cPoints;
		QList<Q_JBUnitSegm> cSubSegms;
		QString strTxt;
	};

	struct Q_JBItemUnitInfo
	{
		quint16 unType;	// 图元类型 对应：ITEM_UNIT_TYPE 枚举
		LINE_ATTR cLineAttr;	// 图元Line属性
		TEXT_ATTR cTxtAttr;		// 图元文本属性
		//QList<QPointF> cPoints;	// 图元基本点位置
		QList<Q_JBUnitSegm> cSegms;	// 图元分段信息
	};

	// JB库Item项
	struct Q_JBItem
	{
		quint32 unID;				// ID
		QString strName;			// 名称
		QPoint  cHotSpot;			// 定位点
		qint16  snHotSpotAttr;		// 定位点属性
		qint16  snHaveTextBorder;	// 有文本框(个数)
		qint16  snAngle;			// 正方向 0-360度，0度y轴正向，逆时针
		qint16  snFlag;				// 标志值：0-点状军标，1-线状军标
		QRect   rectBound;			// 周界
		QByteArray byteInfo;		// 内存数据

		QList<Q_JBItemUnitInfo> cDrawUnitInfo;
	};
}