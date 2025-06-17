#pragma once

#include "snigcore_global.h"
#include <QtCore>

#define EVENT_USER QEvent::User

#define EVENT_REF	EVENT_USER

enum RefEventFlag
{
	BeforSerializeIn=0,
	AfterSerializeIn,
	BeforSerializeOut,
	AfterSerializeOut,
	AfterPropReading,
};



#define EVT_FRAME_VIEW	(QEvent::User+1)
#define EVT_FRAME_VIEW_OPEN	1
#define EVT_FRAME_VIEW_CLOSE	2
#define EVT_FRAME_VIEW_REMOVE	3

class SNIGCORE_EXPORT Event :	public QEvent
{
public:
	~Event();
	Event(int type);
	Event(int type,QVariant data);
	Event(int type,QVariant data1,QVariant data2);
	Event(int type,QVariant data1,QVariant data2,QVariant data3);
	Event(int type,QVariantList datas);

	int			dataSize(){return m_datas.size();}
	QVariant	data(int i=0){return m_datas.at(i);}
protected:
	QVariantList	m_datas;
};
