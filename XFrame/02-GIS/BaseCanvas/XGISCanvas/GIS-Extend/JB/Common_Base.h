#ifndef COMMON_BASE_H
#define COMMON_BASE_H

/***********************************************************
 * 作用: 通用功能接口 头文件
 * 作者: yty
 ***********************************************************/
#define tr8(arg) QString::fromLocal8Bit(arg)

#include <string>

class CByyAppManager;
class CMessageManager;
class CTimerManager;
class QPixmap;

namespace osg
{
    class Image;
}

/// 菜单消息
enum Act_Type
{
    ACT_WORLDMAPPING=0x01030000,  /// 显示平面地图
    ACT_3DMAPPINGMAPPING,                  /// 显示三维地图
    ACT_EARTHMAPPING,                            /// 地球网格
    ACT_NAVIGATIONMAPPING,                  /// 导航图
    ACT_LAYERMAPPING,                             /// 图层管理
    ACT_COMPASSMAPPING,                       /// 指北针

    ACT_ZOOMMAPPING=0x01020000,       /// 地图放大
    ACT_DEFLAMAPPING,                              /// 地图缩小
    ACT_HANDMAPPING,                                /// 地图漫游
    ACT_PRTSCMAPPING,                             /// 截屏


    ACT_FILEOPENMAPPING= 0x01010000, /// 打开文件
    ACT_FILESAVEMAPPING,                          /// 保存文件

    ACT_DISTANCEMAPPING = 0x01000000, /// 距离量算
    ACT_ACREAGEMAPPING,                           /// 面积量算
    ACT_SLOPEMAPPING,                                 /// 坡度量算
        ACT_CALCCLEAR,				      //量算清空

    ACT_DEVATTVIEWMAPPING=0x01040000, /// 设备属性
    ACT_DIRLINEVIEWMAPPING,                        /// 测量线
    ACT_JBVIEWMAPPING,                                 /// 军标面板


	ACT_DRAWLINE=0x01050001,				//画线
	ACT_DRAWPLOY,						//画多边形
	ACT_DRAWSIGNS,						//画标牌
	ACT_DRAWCLEAR,						//标绘清空
	ACT_DRAWICON,						//军标标绘

	ACT_AIGHTLINEANAL=0x01060000,		//直线通视分析
	ACT_ROUNDANAL,						//圆形通视分析
	ACT_SECTORANAL,						//扇形通视分析
	ACT_PTREFANAL,						//基准点分析
	ACT_ANALYSISCLEAR,					//清空分析

        ACT_SREACHPLACE=0x01070000,		       //搜索
	ACT_SREACHNAME,
	ACT_ADDBUTTON=0x01080000
};

/// 鼠标当前的状态
enum MouseState
{
    ZOOM_IN,                                                          /// 放大
    ZOOM_OUT,                                                      /// 缩小
    STATE_FREE,                                                                 /// 漫游

    CAL_DISTANCE,                                              /// 距离量算
    CAL_AREA,                                                       /// 面积量算
    CAL_SLOP,                                                       /// 坡度量算
    CAL_CLEAN,                                                     /// 清空量算

	STATE_PLOT,												///标绘军标

    ANALYSIS_LINEVISIBLE,                              /// 通视分析
    ANALYSIS_CIRCLEVISIBLE,                        /// 圆形通视分析
    ANALYSIS_WATCHVISIBLE,                         /// 基准点通视分析
    ANALYSIS_CLEAR,                                         /// 清空分析结果


    PICK_POINT,                                                  /// 点点选
    PICK_RECT,                                                   /// 矩形框选

    STATE_IDLE                                                                 /// 空闲状态
};

std::string& GetApplicationPath();


/**
 * @brief 获取框架核心管理者
 * @return
 */
CByyAppManager* GetAppManager();

/**
 * @brief 获取消息管理者
 * @return
 */
CMessageManager* GetMsgManager();

/**
 * @brief 获取时间管理者
 * @return
 */
CTimerManager* GetTimerManager();

/**
 * @brief 获取鼠标当前的状态
 * @return
 */
MouseState GetMouseState();

/**
 * @brief 将QT的图片类型转换成 osg的图片类型
 * @param image
 * @return
 */
osg::Image* CovertImage(QPixmap& image);

#endif // COMMON_BASE_H
