
#include "Event.h"

Event::Event(int type) : QEvent((Type)type)
{
}


Event::Event(int type,QVariant data): QEvent((Type)type)
{
	m_datas<<data;
}
Event::Event(int type,QVariant data1,QVariant data2): QEvent((Type)type)
{
	m_datas<<data1<<data2;
}
Event::Event(int type,QVariant data1,QVariant data2,QVariant data3): QEvent((Type)type)
{
	m_datas<<data1<<data2<<data3;
}
Event::Event(int type,QVariantList datas): QEvent((Type)type)
{
	m_datas=datas;
}

Event::~Event(void)
{
}
