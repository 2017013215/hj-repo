#include "StdAfx.h"
#include "missileLineWidget.h"
#include "ByyApp.h"
#include "ByyHdose.h"

#include <QPainter>



MissileLineWidget::MissileLineWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//this->resize(1920, this->height());
	connect(&timer,SIGNAL(timeout()),SLOT(onTick()));

	m_maxTime = 0;
}

MissileLineWidget::~MissileLineWidget()
{

}

void MissileLineWidget::setApp(ByyApp* app)
{
	myApp = app;
	connect(&app->hdose(), SIGNAL(signal_misslieInfo(int , double, double, double, QString, int)), this, SLOT(slt_misslieInfo(int , double, double, double, QString, int)));
	
	timer.start(300);
}

void MissileLineWidget::paintEvent( QPaintEvent *event )
{

	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap));

	//!1

	/*static bool first = true;
	if(first)
	{
		

		first = false;
	}*/
	painter.setPen(QPen(QColor(90,100,130), 2, Qt::SolidLine, Qt::RoundCap));
	for(int index = 0; index < this->width() / 20; ++index)
	{
		painter.drawLine(QPoint(index * 20, this->height()), QPoint((index+1) * 20, this->height()));
		painter.drawLine(QPoint(index * 20, this->height()), QPoint(index * 20, this->height()-5));
	}
	painter.drawText(this->width()-80, this->height() - 10, QString::fromLocal8Bit("时间"));
	
	for(int index = 0; index < this->height() / 20; ++index)
	{
		painter.drawLine(QPoint(0, this->height() - index * 20), QPoint(0, this->height() - (index+1) * 20));
		painter.drawLine(QPoint(0, this->height() - index * 20), QPoint(5, this->height() - index * 20));
	}
	painter.drawText(10, 30, QString::fromLocal8Bit("高度"));

	//!2
	QMap<int, QList<MissileInfo> >::Iterator it = m_missileMap.begin();
	while(it != m_missileMap.end())
	{
		QList<MissileInfo> infoList = it.value();
		/*if(infoList.at(infoList.size()).time*4 > this->width())
		{
			this->setFixedWidth(infoList.at(infoList.size()).time*4 + 100);
		}*/
		if(infoList.size() > 0)
		{
			if(infoList.at(0).type == 1) //紅
			{
				painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap));
			}
			else
			{
				painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine, Qt::RoundCap));
			}
		}
		for(int index = 0; index < infoList.size()-1; ++index)
		{
			painter.drawLine(QPoint(infoList.at(index).time*4, this->height()- 20 - infoList.at(index).height /100), QPoint(infoList.at(index+1).time*4, this->height() - 20 - infoList.at(index+1).height/100));
		}

		++it;
	}

}
void MissileLineWidget::slt_misslieInfo(int id, double height, double time, double v, QString name, int type)
{
	QList<MissileInfo> infoList = m_missileMap[id];
	MissileInfo info;
	info.id = id;
	info.height = height;
	info.time = time;
	info.v = v;
	info.name = name;
	info.type = type;
	infoList.append(info);

	m_missileMap[id] = infoList;

	if(time > m_maxTime)
		m_maxTime = time;

}


void MissileLineWidget::clearUi()
{
	m_missileMap.clear();
	update();
}

void MissileLineWidget::onTick()
{
	update();
	if(!myApp)
		return;

	if(dynamic_cast<QWidget*>(this->parent())->width() < dynamic_cast<QWidget*>(this->parent()->parent())->width())
	{
		dynamic_cast<QWidget*>(this->parent())->setFixedWidth(dynamic_cast<QWidget*>(this->parent()->parent())->width());
	}

	if(m_maxTime*4 > dynamic_cast<QWidget*>(this->parent())->width())
		dynamic_cast<QWidget*>(this->parent())->setFixedWidth(m_maxTime*4 + 100);
}