#include "StdAfx.h"
#include "formationAddDialog.h"
#include "ByyApp.h"
#include "entityObject.h"
#include "JGeoCal/JGeoCalExp.h"
#include "JGeoCal/JGeoCoordinates.h"
#include <QMessageBox>

formationAddDialog::formationAddDialog(ByyApp* app, ByyScenarioV2::FormationList& list, ADDTYPE type, QWidget *parent)
	: QDialog(parent)
	,myApp(app)
	,myFormationList(list)
	,myType(type)
{
	ui.setupUi(this);
	this->setWindowFlags(Qt::Window/* | Qt::WindowTitleHint | Qt::CustomizeWindowHint*/);
	this->setWindowFlags(this->windowFlags()&~Qt::WindowMinMaxButtonsHint);
	init();
}

formationAddDialog::~formationAddDialog()
{

}


void formationAddDialog::init()
{
	if (myType == ADD_FORMATION)
	{

	} 
	else if (myType == ADD_MEMBER)
	{
		ui.label_formationName->setVisible(false);
		ui.lineEdit_formationName->setVisible(false);
	}
	// 1.获取全部舰船实体
	QList<ByyEntityObject*> objs = myApp->core().localObjectManager().entityObjects().values();
	// 2.获取剩余舰船实体
	remainder_objs.clear();
	for (unsigned int i=0;i<objs.size();i++)
	{
		ByyEntityObject* obj = objs.at(i);
		if(!obj) continue;
		bool isFind = false;
		for (unsigned int j=0;j<myFormationList.size();j++)
		{
			bool isFindPara = false;
			ByyScenarioV2::Formation formation = myFormationList.at(j);
			for (unsigned int k=0;k<formation.m_FormationPara.size();k++)
			{
				ByyScenarioV2::FormationPara para = formation.m_FormationPara.at(k);
				if (para.m_MemberName == obj->name())
				{
					isFindPara = true;
					break;
				}
			}
			if (isFindPara)
			{
				isFind = true;
				break;
			}
		}
		if (!isFind)
		{
			remainder_objs.append(obj);
		}
	}
	// 3.创建listWidgetItem
	ui.listWidget->clear();
	for (unsigned int i=0;i<remainder_objs.size();i++)
	{
		QListWidgetItem* item = new QListWidgetItem(remainder_objs.at(i)->name());
		item->setCheckState(Qt::Unchecked);
		ui.listWidget->addItem(item);
	}
}

void formationAddDialog::setFormationList(ByyScenarioV2::FormationList& list)
{
	myFormationList = list;
}

ByyScenarioV2::FormationList formationAddDialog::getFormationList()
{
	return myFormationList;
}

QList<ByyEntityObject*> formationAddDialog::getSelectEntityObjs()
{
	QList<ByyEntityObject*> select_objs;
	for (unsigned int i=0;i<ui.listWidget->count();i++)
	{
		QListWidgetItem* item = ui.listWidget->item(i);
		if (item->checkState() == Qt::Checked)
		{
			select_objs.append(remainder_objs.at(i));
		}
	}
	return select_objs;
}

ByyScenarioV2::Formation formationAddDialog::getFormation()
{
	return myFormation;
}

void formationAddDialog::on_pushButton_ok_clicked()
{
	QList<ByyEntityObject*> select_objs = getSelectEntityObjs();
	if (select_objs.size()<=0)
	{
		QMessageBox::warning(0,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("请勾选成员名称"),QMessageBox::No, QMessageBox::Yes);
		return;
	}
	if (myType == ADD_FORMATION)
	{
		QString fName = ui.lineEdit_formationName->text();
		if(fName.isEmpty())
		{
			QMessageBox::warning(0,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("请输入编队名称"),QMessageBox::No, QMessageBox::Yes);
			return;
		}
		for (unsigned int i=0;i<myFormationList.size();i++)
		{
			ByyScenarioV2::Formation formation = myFormationList.at(i);
			if (fName == formation.m_FormationName)
			{
				QMessageBox::warning(0,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("编队名称重复，请重新输入"),QMessageBox::No, QMessageBox::Yes);
				return;
			}
		}
		ByyEntityObject* m_LeaderEntity = select_objs.first();
		JGeoCal::GeoPt m_LeaderPt(m_LeaderEntity->position().y(), m_LeaderEntity->position().x(), m_LeaderEntity->position().z());
		myFormation.m_FormationName = fName;
		myFormation.m_LeaderName = m_LeaderEntity->name();
		myFormation.m_MemberCount = select_objs.size();
		myFormation.m_FormationPara.clear();
		ByyScenarioV2::FormationPara para0;
		para0.m_MemberName = myFormation.m_LeaderName;
		myFormation.m_FormationPara.append(para0);
		for (unsigned int i=1;i<select_objs.size();i++)
		{
			ByyEntityObject* obj = select_objs.at(i);
			if(!obj) continue;
			ByyScenarioV2::FormationPara para;
			para.m_MemberName = obj->name();
			para.m_index = myFormation.m_FormationPara.size();
			//如果是其他成员，计算相对位置
			JGeoCal::GeoPt m_MemberPt(obj->position().y(), obj->position().x(), obj->position().z());
			JGeoCal::TopocentricPolar tpcp;	// 计算目标的站心极坐标
			JGeoCal::GeodeticInDegreeToTopocentricPolar(tpcp, m_MemberPt, m_LeaderPt);
			double angle = tpcp.Az;
			angle -= m_LeaderEntity->orientation().x();
			//if(angle<0) angle = angle + 360.0;
			while(angle<0)
			{
				angle = angle + 360.0;
			}
			double x = 0.0;
			double y = 0.0;
			if(angle>=0.0 && angle<90.0)
			{
				x = fabs(tpcp.r * sin(angle*Deg2Rad));
				y = -fabs(tpcp.r * cos(angle*Deg2Rad));
			}
			else if(angle>=90.0 && angle<180.0)
			{
				x = fabs(tpcp.r * sin((180-angle)*Deg2Rad));
				y = fabs(tpcp.r * cos((180-angle)*Deg2Rad));
			}
			else if(angle>=180.0 && angle<270.0)
			{
				x = -fabs(tpcp.r * cos((270-angle)*Deg2Rad));
				y = fabs(tpcp.r * sin((270-angle)*Deg2Rad));
			}
			else if(angle>=270.0 && angle<360.0)
			{
				x = -fabs(tpcp.r * sin((360-angle)*Deg2Rad));
				y = -fabs(tpcp.r * cos((360-angle)*Deg2Rad));
			}
			para.m_x = -y;
			para.m_z = x;
			myFormation.m_FormationPara.append(para);
		}
	} 
	else if (myType == ADD_MEMBER)
	{

	}
	accept();
}

void formationAddDialog::on_pushButton_cancel_clicked()
{
	reject();
}