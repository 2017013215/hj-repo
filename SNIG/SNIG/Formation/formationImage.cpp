#include "StdAfx.h"
#include "formationImage.h"

formationImage::formationImage(QObject *parent)
	: QObject(parent)
{
	m_EnPixmap.insert(AIRPLANE_YES, new QPixmap("../data/icons/Formation/airplaneYes.png"));
	m_EnPixmap.insert(AIRPLANE_NO, new QPixmap("../data/icons/Formation/airplaneNo.png"));
	m_EnPixmap.insert(SHIP_YES, new QPixmap("../data/icons/Formation/shipYes.png"));
	m_EnPixmap.insert(SHIP_NO, new QPixmap("../data/icons/Formation/shipNo.png"));
	m_EnPixmap.insert(CENTERITEM, new QPixmap("../data/icons/Formation/centeritem.png"));
}

formationImage::~formationImage()
{

}

formationImage* formationImage::getInstance()
{
	static formationImage instance;
	return &instance;
}

QPixmap* formationImage::getPixmap(EntityImageType type)
{
	return m_EnPixmap.value(type);
}
