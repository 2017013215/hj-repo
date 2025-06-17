#include "StdAfx.h"
#include "indexsubwidget.h"

#include <QFile>
#include <QDomDocument>
#include <QDebug>

#define Ret_Width 180
#define Ret_Height 50
#define Left_Padding 20
#define Top_Padding 40
#define L0_X (Left_Padding)
#define L1_X (Left_Padding + Ret_Width*2)
#define L2_X (Left_Padding + Ret_Width*4)
#define L3_X (Left_Padding + Ret_Width*6)


IndexSubWidget::IndexSubWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_isUpdata = false;

}

IndexSubWidget::~IndexSubWidget()
{

}

void IndexSubWidget::paintEvent( QPaintEvent *event )
{

	QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(QPen(QBrush(QColor(70,166,250)), 1, Qt::SolidLine, Qt::RoundCap));

	//!1

	if(!m_isUpdata)
		return;

	if(m_nodeInfo.list.size() < 1)
		return;
	
	painter.drawLine(QPoint(m_nodeInfo.x + Ret_Width, m_nodeInfo.y + Ret_Height/2), QPoint(m_nodeInfo.x + Ret_Width + Ret_Width/2, m_nodeInfo.y + Ret_Height/2));
	painter.drawLine(QPoint(m_nodeInfo.x + Ret_Width + Ret_Width/2, m_nodeInfo.list.at(0).y+Ret_Height/2), QPoint(m_nodeInfo.x + Ret_Width + Ret_Width/2, m_nodeInfo.list.at(m_nodeInfo.list.size()-1).y+Ret_Height/2));

	for(int index = 0; index < m_nodeInfo.list.size(); ++index) //L1
	{
		painter.drawLine(QPoint(m_nodeInfo.x + Ret_Width + Ret_Width/2, m_nodeInfo.list.at(index).y + Ret_Height/2), QPoint(m_nodeInfo.x + Ret_Width + Ret_Width, m_nodeInfo.list.at(index).y + Ret_Height/2));

		painter.drawLine(QPoint(m_nodeInfo.list.at(index).x + Ret_Width, m_nodeInfo.list.at(index).y + Ret_Height/2), QPoint(m_nodeInfo.list.at(index).x + Ret_Width + Ret_Width/2, m_nodeInfo.list.at(index).y + Ret_Height/2));
		painter.drawLine(QPoint(m_nodeInfo.list.at(index).x + Ret_Width + Ret_Width/2, m_nodeInfo.list.at(index).list.at(0).y+Ret_Height/2), QPoint(m_nodeInfo.list.at(index).x + Ret_Width + Ret_Width/2, m_nodeInfo.list.at(index).list.at(m_nodeInfo.list.at(index).list.size()-1).y+Ret_Height/2));
		QList<Node_Info> list_L2 = m_nodeInfo.list.at(index).list;
		for(int i = 0; i < list_L2.size(); ++i) //L2
		{
			painter.drawLine(QPoint(m_nodeInfo.list.at(index).x + Ret_Width + Ret_Width/2, list_L2.at(i).y + Ret_Height/2), QPoint(m_nodeInfo.list.at(index).x + Ret_Width + Ret_Width, list_L2.at(i).y + Ret_Height/2));

			painter.drawLine(QPoint(list_L2.at(i).x + Ret_Width, list_L2.at(i).y + Ret_Height/2), QPoint(list_L2.at(i).x + Ret_Width + Ret_Width/2, list_L2.at(i).y + Ret_Height/2));
			painter.drawLine(QPoint(list_L2.at(i).x + Ret_Width + Ret_Width/2, list_L2.at(i).list.at(0).y+Ret_Height/2), QPoint(list_L2.at(i).x + Ret_Width + Ret_Width/2, list_L2.at(i).list.at(list_L2.at(i).list.size()-1).y+Ret_Height/2));
			QList<Node_Info> list_L3 = list_L2.at(i).list;
			for(int j = 0; j < list_L3.size(); ++j) //L3
			{
				painter.drawLine(QPoint(list_L2.at(i).x + Ret_Width + Ret_Width/2, list_L3.at(j).y + Ret_Height/2), QPoint(list_L2.at(i).x + Ret_Width + Ret_Width, list_L3.at(j).y + Ret_Height/2));
				qDebug()<<list_L3.at(j).x<<list_L3.at(j).y<<list_L3.at(j).x + Ret_Width<<list_L3.at(j).y;

				list_L3.at(j).nodeWidget->setGeometry(list_L3.at(j).x,list_L3.at(j).y,Ret_Width,Ret_Height);
			}
			qDebug()<<list_L2.at(i).x<<list_L2.at(i).y<<list_L2.at(i).x + Ret_Width<<list_L2.at(i).y;
			list_L2.at(i).nodeWidget->setGeometry(list_L2.at(i).x,list_L2.at(i).y,Ret_Width,Ret_Height);
		}
		m_nodeInfo.list.at(index).nodeWidget->setGeometry(m_nodeInfo.list.at(index).x,m_nodeInfo.list.at(index).y,Ret_Width,Ret_Height);
	}
	m_nodeInfo.nodeWidget->setGeometry(m_nodeInfo.x,m_nodeInfo.y,Ret_Width,Ret_Height);
}

bool IndexSubWidget::readIndexXml()
{
	m_nodeInfo.list.clear();

	QFile file("Record/uvEffConf.xml");

	if (!file.open(QIODevice::ReadOnly))
		return false;

	QDomDocument doc("");

	bool ok=doc.setContent(&file);

	file.close();

	if (!ok||doc.isNull())
		return false;

	QDomElement root = doc.documentElement();

	if (root.isNull())
		return false;
	
	m_nodeInfo.tag = QString::fromLocal8Bit("无人机作战效能评估");
	m_nodeInfo.nodeWidget = new IndexNode(this);
	m_nodeInfo.nodeWidget->setTag(m_nodeInfo.tag);

	QDomNodeList items=root.childNodes();
	for (int i=0;i<items.size();++i)
	{
		if (items.at(i).isNull())
			continue;

		if(items.at(i).nodeName() != "FirstIndex")
			continue;

		qDebug()<<items.at(i).nodeName();

		Node_Info infoL1;
		infoL1.max = items.at(i).toElement().attribute("max").toDouble();
		infoL1.min = items.at(i).toElement().attribute("min").toDouble();
		infoL1.name = items.at(i).toElement().attribute("name");
		infoL1.tag = items.at(i).toElement().attribute("tag");
		infoL1.weigth = items.at(i).toElement().attribute("weigth").toDouble();
		infoL1.nodeWidget = new IndexNode(this);
		infoL1.nodeWidget->setTag(infoL1.tag);
		qDebug()<<"L1-tag"<<infoL1.tag;

		QDomNodeList itemsL2=items.at(i).childNodes();
		for (int j=0;j<itemsL2.size();++j)
		{
			if(itemsL2.at(j).nodeName() != "SecondIndex")
				continue;

			Node_Info infoL2;
			infoL2.max = itemsL2.at(j).toElement().attribute("max").toDouble();
			infoL2.min = itemsL2.at(j).toElement().attribute("min").toDouble();
			infoL2.name = itemsL2.at(j).toElement().attribute("name");
			infoL2.tag = itemsL2.at(j).toElement().attribute("tag");
			infoL2.weigth = itemsL2.at(j).toElement().attribute("weigth").toDouble();
			infoL2.nodeWidget = new IndexNode(this);
			infoL2.nodeWidget->setTag(infoL2.tag);
			qDebug()<<"L2-tag"<<infoL2.tag;

			QDomNodeList itemsL3=items.at(i).childNodes().at(j).childNodes();
			for (int k=0;k<itemsL3.size();++k)
			{
				if(itemsL3.at(k).nodeName() != "ThirdIndex")
					continue;

				Node_Info infoL3;
				infoL3.max = itemsL3.at(k).toElement().attribute("max").toDouble();
				infoL3.min = itemsL3.at(k).toElement().attribute("min").toDouble();
				infoL3.name = itemsL3.at(k).toElement().attribute("name");
				infoL3.tag = itemsL3.at(k).toElement().attribute("tag");
				infoL3.weigth = itemsL3.at(k).toElement().attribute("weigth").toDouble();
				infoL3.nodeWidget = new IndexNode(this);
				infoL3.nodeWidget->setTag(infoL3.tag);
				infoL3.nodeWidget->setTitle(QString::fromLocal8Bit("结果"));
				qDebug()<<"L3-tag"<<infoL3.tag;

				infoL2.list.append(infoL3);
			}
			infoL1.list.append(infoL2);
		}
		m_nodeInfo.list.append(infoL1);
	}
	
	return true;
}

QString IndexSubWidget::readResultFileName()
{
	QString fileName;

	QFile file("Record/FileIndex.txt");
	if (!file.open(QIODevice::ReadOnly))
		return fileName;

	fileName = file.readAll();
	file.close();

	return fileName;
}

void IndexSubWidget::readResultData(QString fileName)
{
	QFile file("Record/" + fileName);
	if (!file.open(QIODevice::ReadOnly))
		return ;

	QStringList t_listTitle = QString(file.readLine()).split(',');
	QStringList t_listValue = QString(file.readLine()).split(',');

	for(int index = 0; index < t_listTitle.size(); ++index)
	{
		m_resultMap.insert(t_listTitle.at(index), t_listValue.at(index) );
	}

	file.close();
}

void IndexSubWidget::calcValue()
{
	if(m_nodeInfo.list.size() == 0)
		return;

	int t_currY = Top_Padding;
	double dfVlueL0 = 0;
	for(int index = 0; index < m_nodeInfo.list.size(); ++index) //L1
	{
		QList<Node_Info> list_L2 = m_nodeInfo.list.at(index).list;
		double dfVlueL1 = 0;
		for(int i = 0; i < list_L2.size(); ++i) //L2
		{
			QList<Node_Info> list_L3 = list_L2.at(i).list;

			double dfVlueL2 = 0;
			for(int j = 0; j < list_L3.size(); ++j) //L3
			{
				list_L3[j].x = L3_X;
				list_L3[j].y = t_currY;
				t_currY += 1.3*Ret_Height;
				list_L3[j].value = m_resultMap.value(list_L3[j].name).toDouble();
				list_L3[j].nodeWidget->setValue(m_resultMap.value(list_L3[j].name));
				list_L3[j].nodeWidget->setQZ(QString::number(list_L3[j].weigth));

				double df_L3 = list_L3[j].weigth;
				if(list_L3[j].name == "detectTgtCount")
				{
					df_L3 = list_L3[j].value / list_L3[j].max * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "detectTgtDmax")
				{
					df_L3 = list_L3[j].value / list_L3[j].max * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "detectTgtT0")
				{
					df_L3 = (1 - list_L3[j].value / (list_L3[j].max - list_L3[j].min)) * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "detectTgtRatio")
				{
					df_L3 = list_L3[j].value * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "trackTgtCount")
				{
					df_L3 = list_L3[j].value / list_L3[j].max * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "trackTgtDuration")
				{
					df_L3 = list_L3[j].value / list_L3[j].max * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "trackTgtRatio")
				{
					df_L3 = list_L3[j].value * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "detectedPb")
				{
					df_L3 = (1 - list_L3[j].value) * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "detectedT0")
				{
					df_L3 = list_L3[j].value / list_L3[j].max * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "detectedDmax")
				{
					df_L3 = list_L3[j].value / list_L3[j].max * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "trackedPb")
				{
					df_L3 = (1 - list_L3[j].value) * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "trackedDuration")
				{
					df_L3 = (1 - list_L3[j].value / (list_L3[j].max - list_L3[j].min)) * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "hitPb")
				{
					df_L3 = (1 - list_L3[j].value / (list_L3[j].max - list_L3[j].min)) * list_L3[j].weigth;
				}
				else if(list_L3[j].name == "killedPb")
				{
					df_L3 = (1 - list_L3[j].value) * list_L3[j].weigth;
				}


				if(df_L3 > 1)
					df_L3 = 1;
				if(df_L3 < 0)
					df_L3 = 0;

				dfVlueL2 += df_L3;
			}
			list_L2[i].value = dfVlueL2;
			list_L2[i].nodeWidget->setValue(QString::number(dfVlueL2*100));
			list_L2[i].nodeWidget->setQZ(QString::number(list_L2[i].weigth));
			dfVlueL1 += dfVlueL2*list_L2[i].weigth;

			list_L2[i].list = list_L3;
			list_L2[i].x = L2_X;
			list_L2[i].y = (list_L2[i].list[list_L2.at(i).list.size()-1].y - list_L2[i].list[0].y)/2 + list_L2[i].list[0].y;
			qDebug()<<"L2_X:"<<L2_X;

		}
		
		m_nodeInfo.list[index].value = dfVlueL1;
		m_nodeInfo.list[index].nodeWidget->setValue(QString::number(dfVlueL1*100));
		m_nodeInfo.list[index].nodeWidget->setQZ(QString::number(m_nodeInfo.list[index].weigth));
		dfVlueL0 += dfVlueL1*m_nodeInfo.list[index].weigth;

		m_nodeInfo.list[index].list = list_L2;
		m_nodeInfo.list[index].x = L1_X;
		m_nodeInfo.list[index].y = (m_nodeInfo.list[index].list[m_nodeInfo.list.at(index).list.size()-1].y - m_nodeInfo.list[index].list[0].y)/2 + m_nodeInfo.list[index].list[0].y;
		qDebug()<<"L1_X:"<<L1_X;
	}
	m_nodeInfo.value = dfVlueL0;
	m_nodeInfo.nodeWidget->setValue(QString::number(dfVlueL0*100));
	m_nodeInfo.nodeWidget->setQZ("");
	m_nodeInfo.nodeWidget->setQZTitle("");
	m_nodeInfo.x = L0_X;
	m_nodeInfo.y = (m_nodeInfo.list[m_nodeInfo.list.size()-1].y - m_nodeInfo.list[0].y) / 2 + m_nodeInfo.list[0].y;
}


void IndexSubWidget::calcAndShow()
{
	m_nodeInfo.list.clear();
	m_resultMap.clear();

	if(!readIndexXml())
		return;


	QString fileName = readResultFileName();

	readResultData(fileName);

	calcValue();

	m_isUpdata = true;
}