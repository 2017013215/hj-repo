#pragma once

#include <QString>
#include <QList>
#include <XCoordinateGeo.h>

using namespace XSpace_Common;

// 动态测试-使用的实体
class XDynamicEntity
{
public:
	XDynamicEntity(void);
	~XDynamicEntity(void);

	// 初始化
	const int InitEntity( quint32 _unMarkLibID, quint32 _unMarkItemID, const QString& _strName);

	// 设置推进所有数据点
	const bool SetActionPoints( const QList<XCoordinateGeo>& _xActionPoints);

	// 推进
	const bool Step();

	// 设置名称
	void SetName(const QString& _strName);
	
	// 获取当前位置
	const XCoordinateGeo& GetPos() const;

private:
	bool m_bIsInit;
	QString m_strName;
	QList<XCoordinateGeo> m_xActionPoints;
	XCoordinateGeo m_xCurPos;
	quint32 m_unStepIdx;
};

