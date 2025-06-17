#ifndef JBTYPE_H
#define JBTYPE_H

#include <QList>
#include <QString>

//////////////////////////////////////// 文件中 军标固有信息 //////////////////////////////////

/// 军标定位点
struct JB_POINT
{
    qint32 nX;
    qint32 nY;
};

struct JB_RECT
{
    qint32 nLeft;
    qint32 nTop;
    qint32 nRight;
    qint32 nBottom;
};

/// 图元类型
enum TYPE_UNIT
{
    U_NONE=0,
    U_BLINE,	  /// 曲/直线
    U_RECT,       /// 矩形
    U_ELLI,       /// 圆
    U_ARC,        /// 弧
    U_PIE,        /// 扇
    U_CHORD,      /// 弦
    U_TEXT,       /// 文字
    U_BMP,        /// 位图
    U_PLINE,      /// 多点连线
    U_PGON,       /// 折线填充
    U_POINT,      /// 点
    U_LINE,       /// 直线
    U_TEXTBORDER, /// 文本框
    U_BITMAP,     /// 位图
    U_BBLINE 	  /// 内含曲/直线
};

/**
 * @brief 军标头
 */
struct  JB_HEAD
{
    char		csFileID[8];            /// 文件标识
    char		csFileDeclaring[80];    /// 文件描述
    qint32		nID;                    /// 库代号
    quint32		unLayerNum;             /// 图层数
    quint32		unGrpNum;               /// 元组数
    quint32		unDataAddr;             /// 数据起始地址
};

/// 军标图层结构
struct JB_LAYER_TABLE
{
    qint32      nLen;			    /// 表项长度
    quint32     unID;      	        /// 图层码
    qint32      nGrpNum;            /// 元组数 (0空类别；>0含有Num个标号；<0含有-Num个子类别
    char        csName[80];         /// 层名
    quint32     unGrpOffset;        /// 第一个军标元组地址
};

/// 军标元组头
struct JB_GROUP_HEAD
{
    quint32	    unLen;              /// 元组长度   所有图元+元组头
    quint32	    unGrpID;            /// 元组码
    quint32	    unNextOff;          /// 后一元组
    char        csName[80];         /// 标号名
    JB_POINT    pointHotSpot;       /// 定位点
    qint16      sHotSpotAttr;       /// 定位点属性
    qint16      sHaveTextBorder;	/// 有文本框(个数)
    qint16      sAngle;             /// 军标正方向 0-360度，0度y轴正向，逆时针
    qint16      sFlag;              /// 标志值：0-点状军标，1-线状军标
    JB_RECT     rectBound;	        /// 周界
};

/// 军标图元头信息
struct JB_UNIT_HEAD
{
    qint16  sType;       /// 图元类型
    qint16  sColorGrpNo; /// 图元属性长度
    quint32 unLen;       /// 图元长度，包括图元头
};

//////////////////////////////////////// 文件中 军标固有信息 end//////////////////////////////////

///////////////////////////////////////////// 图元属性 ////////////////////////////////////////////////////////
/// 线状军标属性
struct LINE_ATTR
{
    qint16 sAttrSize;     // 结构长度
    qint16 sLineType;  	// 线型
    qint16 sLineWidth;     	// 线宽
    qint16 sClose;		// 图形闭合
    qint16 sOutLineWidth;  	// 衬线宽  0 无衬色
    qint16 sOutLineDir;	// 衬线方向 0，右，1左，2双
    qint16 sOutLineTran; // 衬线透明度
    qint16 sFillType;  	// 填充类型
    qint16 sFillTran;	// 填充透明度
    qint16 sFadeMode;   // 渐变模式
    qint16 sFadeDir;    // 渐变方向
    qint16 sFadeCenter; // 渐变中点
    qint16 sFadeRange;  // 渐变范围
    qint16 sSoftEdge;	// 边缘柔化
    qint32 sLineC;  	// 线色
    quint32 nOutLineC; // 衬线色
    quint32 nFillC;     // 填充色
    quint32 nFadeC1;	// 渐变色1
    quint32 nFadeC2;	// 渐变色2
    char  cRev[4];	   // 保留
};

/// 修饰属性
union EFFECT
{
    struct Bit
    {
        unsigned uItalic:1;	// 倾斜
        unsigned uBold:1;		// 加重
        unsigned uUnderLine:1;// 下划线
        unsigned uBorder:1;	// 镶边
        unsigned uShadow:1;	// 阴影
        unsigned uBack:1;		// 背景框
        unsigned uAlign:2;	// 对齐方式0:居中,1:居左,2:居右
    };
    quint32 unByte;
};

/// 字体属性
struct TEXT_ATTR
{
    qint16 sAttrSize;      // 结构长度
    qint16 sAngle;       // 转角:单位0.1度
    char  csFontName[32];// 字体名
    qint16 sFontWidth;		 // 字宽
    qint16 sFontHight;		 // 字高
    quint32 sTextColor;	// 字色
    qint16 sColSpan;		// 字距
    qint16 sRowSpan;		// 行距
    EFFECT stEffect;		// 修饰
};

/// 图像属性
struct BMP_ATTR
{
    qint32 nX;
    qint32 nY;
    qint32 nWidth;
    qint32 nHight;
    qint32 bTran;             // 是否透明
    quint32 ucTranColor; // 透明色
};
///////////////////////////////////////////// 图元属性 end /////////////////////////////////////////////////

//////////////////////////////////////// 用于QT处理的信息   //////////////////////////////////////

struct QT_GROUP
{
    quint32 unID;               /// 元组码
    QString qstrName;           /// 军标名称
    qint16  sHotSpotAttr;       /// 定位点属性
    qint16  sHaveTextBorder;	/// 有文本框(个数)
    qint16  sAngle;             /// 军标正方向 0-360度，0度y轴正向，逆时针
    qint16  sFlag;              /// 标志值：0-点状军标，1-线状军标
    JB_RECT rectBound;	        /// 周界
    QByteArray byteInfo;        /// 内存数据
};

struct QT_LAYER
{
    quint32   unID;      	    /// 图层码
    bool      bHaveInfo;        /// 是否有Group信息 子图层和Group信息 互斥
    QString	  qstrName;	        /// 层名
    QList<QT_LAYER> listChild;  /// 子图层信息
    QList<QT_GROUP> listGropu;  /// 真实的图层信息
};

struct QT_JB
{
    qint32		    nID;          /// 库代号
    QString		    qstrName;     /// 文件描述
    QList<QT_LAYER> listLayer;    /// 第一级图层列表
};

//////////////////////////////////////// 用于QT处理的信息   //////////////////////////////////////
#endif // JBTYPE_H
