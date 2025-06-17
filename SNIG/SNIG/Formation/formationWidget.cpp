#include "StdAfx.h"
#include "formationWidget.h"
#include "ByyApp.h"
//#include "IconSet.h"
#include "mainWindow.h"

#include <QApplication>
#include <QPalette>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QPen>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QDomDocument>
#include <QDomNodeList>
#include <QTextStream>
#include <QDir>
#include <qmath.h>
#include <QMessageBox.h>

//#include "taskButton.h"
#include "uinitsGraphicsItem.h"
#include "unitsGraphicsScene.h"
//#include "entityData.h"
#include "entityObject.h"
#include "scenarioManager.h"
#include "formationImage.h"

#include "JGeoCal/JGeoCal.h"
#include "JGeoCal/JGeoCoordinates.h"

#include "formationAddDialog.h"

formationWidget::formationWidget( QWidget *parent)
	: QWidget(parent)
	,m_gridWidth(20)
	,myApp(NULL)
	,m_centerItem(NULL)
	,m_formationType(AIRPLANE)
	,m_currentItem(NULL)
	,m_pix_viewBackground(NULL)
{
	ui.setupUi(this);

	ui.label_widgetTitle->setText(QString::fromLocal8Bit("任务编队"));

	setWindowFlags(Qt::Window);
	
	//setAttribute(Qt::WA_DeleteOnClose);

	//setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
	//setAttribute(Qt::WA_TranslucentBackground, true);

	//IconSet::Instance()->SetIcon(ui.pushButton_mp_close, QChar(0xf00d), 10);
	//ui.pushButton_mp_close->setToolTip(QString::fromLocal8Bit("关闭"));

	//int t_width = QApplication::desktop()->screenGeometry(0).width();
	//int t_height = QApplication::desktop()->screenGeometry(0).height();
	//this->setGeometry(t_width - t_width/3, t_height - t_height*3/5, t_width/3, t_height*3/5 - t_height/4.3);

// 	QImage* image = new QImage;
// 	image->load("../data/icons/angleShader.png");
// 	ui.label_angle->setPixmap(QPixmap::fromImage(*image));
// 	ui.widget_touming->setStyleSheet("QWidget#widget_touming{max-width:50px;}");

	initUi();
	createFormationListWidget();
}

formationWidget::formationWidget(ByyApp* app,QWidget *parent /*= 0*/)
	: QWidget(parent)
	,myApp(app)
	,m_gridWidth(20)
	,m_centerItem(NULL)
	,m_formationType(AIRPLANE)
	,m_currentItem(NULL)
	,m_pix_viewBackground(NULL)
{
	ui.setupUi(this);

	ui.label_widgetTitle->setText(QString::fromLocal8Bit("任务编队"));

	setWindowFlags(Qt::Window);

	initUi();
	createFormationListWidget();

	closeFlag=0;
}

formationWidget::~formationWidget()
{

}

void formationWidget::debugFormation(ByyScenarioV2::Formation& f)
{
	qDebug()<<"FormationName = "<<f.m_FormationName;
	qDebug()<<"LeaderName = "<<f.m_LeaderName;
	qDebug()<<"MemberCount = "<<f.m_MemberCount;
	for (unsigned int i=0;i<f.m_FormationPara.size();i++)
	{
		qDebug()<<"MemberName = "<<f.m_FormationPara.at(i).m_MemberName;
		qDebug()<<"index = "<<f.m_FormationPara.at(i).m_index;
		qDebug()<<"x = "<<f.m_FormationPara.at(i).m_x;
		qDebug()<<"y = "<<f.m_FormationPara.at(i).m_y;
		qDebug()<<"z = "<<f.m_FormationPara.at(i).m_z;
	}
}

//单例接口
formationWidget* formationWidget::getInstance()
{
	static formationWidget instance;
	return &instance;
}

//初始化界面
void formationWidget::initUi()
{
	connect(ui.pushButton_cancel, SIGNAL(clicked()), this, SLOT(on_pushButton_mp_close_clicked()));
	{
		QImage* image = new QImage;
		image->load("../data/icons/Formation/plottingscale.png");
		ui.label_bilichi->setPixmap(QPixmap::fromImage(image->scaled(18,15)));
		ui.label_bilichi->setAlignment(Qt::AlignCenter);
	}
	m_scene = new unitsGraphicsScene;
	m_scene->setObjectName("graphicsScene_test");
	connect(m_scene, SIGNAL(showPlottingscale(int)), this, SLOT(onShowPlottingscale(int)));
	//paintGridBackground();
	onShowPlottingscale(1000);

	ui.listWidget_formationList->setFocusPolicy(Qt::NoFocus);

	connect(ui.pushButton_cancel, SIGNAL(clicked()), this, SLOT(on_pushButton_mp_close_clicked()));

	connect(ui.listWidget_formationList, SIGNAL(itemClicked ( QListWidgetItem * )), this, SLOT(on_ListWidget_clicked(QListWidgetItem *)));

	connect(ui.comboBox_formationTroops, SIGNAL(activated ( const QString & )), this, SLOT(on_combobox_clicked( const QString & )));
	ui.comboBox_formationTroops->addItem("NONE");

	ui.pushButton_mp_close->setVisible(false);

	m_FormationMenu = new QMenu;
	m_CreateFormationAct = new QAction(QString::fromLocal8Bit("新建编队"),this);
	m_DisbandFormationAct = new QAction(QString::fromLocal8Bit("解散编队"),this);
	m_SpecifyLeaderAct = new QAction(QString::fromLocal8Bit("指定编队长"),this);
	m_AddMemberAct = new QAction(QString::fromLocal8Bit("新增成员"),this);
	m_DeleteMemberAct = new QAction(QString::fromLocal8Bit("删除成员"),this);
	connect(m_CreateFormationAct,SIGNAL(triggered()),SLOT(on_CreateFormationAct()));
	connect(m_DisbandFormationAct,SIGNAL(triggered()),SLOT(on_DisbandFormationAct()));
	connect(m_SpecifyLeaderAct,SIGNAL(triggered()),SLOT(on_SpecifyLeaderAct()));
	connect(m_AddMemberAct,SIGNAL(triggered()),SLOT(on_AddMemberAct()));
	connect(m_DeleteMemberAct,SIGNAL(triggered()),SLOT(on_DeleteMemberAct()));
	m_FormationMenu->addAction(m_CreateFormationAct);
	m_FormationMenu->addAction(m_DisbandFormationAct);
	m_FormationMenu->addSeparator();
	m_FormationMenu->addAction(m_SpecifyLeaderAct);
	m_FormationMenu->addAction(m_AddMemberAct);
	m_FormationMenu->addAction(m_DeleteMemberAct);
	m_customContextMenu = NULL;

}

void formationWidget::setByyApp(ByyApp* app)
{
	myApp = app;
}

//关闭
void formationWidget::on_pushButton_mp_close_clicked()
{
	clearSceneItem();
	this->close();
}

void formationWidget::paintEvent(QPaintEvent *)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//应用
void formationWidget::on_pushButton_ok_clicked()
{
	if(!ui.treeWidget->currentItem()) return;
	//保存当前编队阵型信息，更新兵力实体位置
	QString m_FormationName = ui.treeWidget->currentItem()->text(0);
	if (m_FormationName.isEmpty())
	{
		QMessageBox::warning(0,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("请选择要编辑的编队"),QMessageBox::No, QMessageBox::Yes);
		return;
	} 
	else
	{
		int res = QMessageBox::warning(0,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("是否更新%1编队信息？").arg(m_FormationName),QMessageBox::No, QMessageBox::Yes);
		if(res == QMessageBox::Yes)
		{
			for (unsigned int i=0;i<myFormationList.size();i++)
			{
				ByyScenarioV2::Formation formation = myFormationList.at(i);
				if (formation.m_FormationName == m_FormationName)
				{
					//编队长实体
					ByyEntityObject* m_LeaderEntity = myApp->core().localObjectManager().findEntity(formation.m_LeaderName);
					if(!m_LeaderEntity) break;
					//编队长朝向
					double m_Head = m_LeaderEntity->orientation().x();
					for (unsigned int j=1;j<formation.m_FormationPara.size();j++)
					{
						ByyScenarioV2::FormationPara para = formation.m_FormationPara.at(j);
						//成员名称
						QString m_MemberName = formation.m_FormationPara.at(j).m_MemberName;
						QList<QGraphicsItem*> items = m_scene->items();
						for (unsigned int k=0;k<items.size();k++)
						{
							uinitsGraphicsItem* unitItem = (uinitsGraphicsItem*)items.at(k);
							if(!unitItem) continue;
							if(!unitItem->getCurrentEntity()) continue;
							ByyEntityObject* obj = unitItem->getCurrentEntity();
							if (obj->name() == m_MemberName)
							{
								QPointF thePos = unitItem->getCenterPos();
								QPointF theRealityPos = unitItem->getCenterPosReality();
								para.m_x = -unitItem->getCenterPosReality().y();
								para.m_z = unitItem->getCenterPosReality().x();
								formation.m_FormationPara.replace(j, para);
								//先计算与编队长夹角
								int distance = 0;
								int angle = 0;
								distance = qSqrt(qAbs(theRealityPos.x()) * qAbs(theRealityPos.x()) + qAbs(theRealityPos.y()) * qAbs(theRealityPos.y()));
								if(thePos.x() >= 0 && -thePos.y() >= 0)
								{
									angle = 90 - qAtan2( -thePos.y(),thePos.x())*180/PI;
								}
								else if(thePos.x() > 0 && -thePos.y() < 0)
								{
									angle = 90 + qAbs(qAtan2( -thePos.y(),thePos.x())*180/PI);
								}
								else if(thePos.x() <= 0 && -thePos.y() <= 0)
								{
									angle = 90 + qAbs(qAtan2( -thePos.y(),thePos.x())*180/PI);
								}
								else if(thePos.x() < 0 && -thePos.y() > 0)
								{
									angle = 360 - qAtan2( -thePos.y(),thePos.x())*180/PI + 90;
								}
								//更新实体位置
								double lon,lat;
								JGeoCal::destinationOnSphere(m_LeaderEntity->position().y(),m_LeaderEntity->position().x(),m_Head+angle,distance,lat,lon);
								obj->setPosition(osg::Vec3d(lon,lat,obj->position().z()));
								break;
							}
						}
					}
					//更新本类myFormationList
					myFormationList.replace(i, formation);
					//更新Hdose想定类myFormationList
					ByyScenarioV2 *scn=myApp->hdose().scenarioManager().scenario();
					if (scn)
					{
						scn->setFormationList(myFormationList);
					}
					break;
				}
			}
		}
	}
}

//为场景绘制网格背景
void formationWidget::paintGridBackground()
{
	int widgetWidht = ui.graphicsView_formation->width();
	int widgetheight = ui.graphicsView_formation->height();

	if(m_pix_viewBackground)
	{
		delete m_pix_viewBackground;
		m_pix_viewBackground = NULL;
	}

	m_pix_viewBackground = new QPixmap(widgetWidht, widgetheight);
	m_pix_viewBackground->fill(Qt::transparent);

	QPen pen(Qt::green, 1,  Qt::DotLine, Qt::FlatCap, Qt::RoundJoin);
	QPainter painter;
	painter.setPen(pen);

	int t_numcoloumns = widgetWidht/m_gridWidth;
	int t_numRows = widgetheight/m_gridWidth;

	painter.begin(m_pix_viewBackground);
	for(int index = 0; index < t_numRows; ++index)
	{
		painter.drawLine(QPoint(0, index * m_gridWidth), QPoint(widgetWidht, index * m_gridWidth));
	}

	for(int num = 0; num < t_numcoloumns; ++num)
	{
		//painter.drawLine(QPoint(num * m_gridWidth, 0), QPoint(num * m_gridWidth, num * widgetheight));
		painter.drawLine(QPoint(num * m_gridWidth, 0), QPoint(num * m_gridWidth, widgetheight));
		//qDebug()<<QPoint(num * m_gridWidth, 0)<<";"<<QPoint(num * m_gridWidth, widgetheight);
	}

	painter.end();

	ui.graphicsView_formation->setScene(m_scene);
	m_scene->setBackgroundBrush(*m_pix_viewBackground);
	m_scene->setSceneRect(QRectF(-widgetWidht/2, -widgetheight/2, widgetWidht, widgetheight));

	ui.graphicsView_formation->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.graphicsView_formation->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

void formationWidget::showEvent(QShowEvent *)
{
	paintGridBackground();
}

void formationWidget::closeEvent(QCloseEvent *e)
{
	clearSceneItem();
	QWidget::closeEvent(e);
	closeFlag = 1;
}

//自定义兵力个数确定
void formationWidget::on_pushButton_zdy_ok_clicked()
{
	clearSceneItem();

	int num = ui.lineEdit_formationNum->text().toInt();

	if(num > 0)
	{
		m_centerItem = NULL;

		m_centerItem = new uinitsGraphicsItem(QPointF(0, 0), WARSHIP,true);
		connect(m_centerItem, SIGNAL(ShowDistanceAndAngle(uinitsGraphicsItem*)), this, SLOT(onShowDistanceAndAngle(uinitsGraphicsItem*)));
		m_scene->addItem(m_centerItem);

		for(int index = 0; index < num-1; ++index)
		{
			uinitsGraphicsItem* item = new uinitsGraphicsItem(QPointF(1000*(index+1),0));
			connect(item, SIGNAL(ShowDistanceAndAngle(uinitsGraphicsItem*)), this, SLOT(onShowDistanceAndAngle(uinitsGraphicsItem*)));
			m_scene->addItem(item);
		}
	}

}

//比例信息
void formationWidget::onShowPlottingscale(int value)
{
	QString str;
	if(value < 1000)
	{
		str = QString("%1m").arg(value);
	}
	else
	{
		str = QString("%1km").arg(value/1000.0);
	}
	ui.lineEdit_showPlottingscale->setText(str);
}

//显示距离中心距离和与之角度
void formationWidget::onShowDistanceAndAngle(uinitsGraphicsItem* item)
{
	m_currentItem = item;

	QPointF thePos = item->getCenterPos();
	QPointF theRealityPos = item->getCenterPosReality();
	int distance = 0;
	int angle = 0;
	distance = qSqrt(qAbs(theRealityPos.x()) * qAbs(theRealityPos.x()) + qAbs(theRealityPos.y()) * qAbs(theRealityPos.y()));

	if(thePos.x() >= 0 && -thePos.y() >= 0)
	{
		angle = 90 - qAtan2( -thePos.y(),thePos.x())*180/PI;
	}
	else if(thePos.x() > 0 && -thePos.y() < 0)
	{
		angle = 90 + qAbs(qAtan2( -thePos.y(),thePos.x())*180/PI);
	}
	else if(thePos.x() <= 0 && -thePos.y() <= 0)
	{
		angle = 90 + qAbs(qAtan2( -thePos.y(),thePos.x())*180/PI);
	}
	else if(thePos.x() < 0 && -thePos.y() > 0)
	{
		angle = 360 - qAtan2( -thePos.y(),thePos.x())*180/PI + 90;
	}

	if(distance > 1000)
	{
		ui.lineEdit_formation_distance->setText(QString("%1km").arg(distance/1000.0));
	}
	else
	{
		ui.lineEdit_formation_distance->setText(QString("%1m").arg(distance));
	}
	ui.lineEdit_formation_angle->setText(QString("%1").arg(angle));
}

//清除场景中实体
void formationWidget::clearSceneItem()
{
	foreach( QGraphicsItem* t_item, m_scene->items())
	{
		if(t_item)
		{
			//if(dynamic_cast<uinitsGraphicsItem*>(t_item)->getCurrentEntity())
			//{
			//	ui.comboBox_formationTroops->addItem(dynamic_cast<uinitsGraphicsItem*>(t_item)->getCurrentEntity()->name());
			//	dynamic_cast<uinitsGraphicsItem*>(t_item)->removeCurrentEntity();
			//}

			m_scene->removeItem(t_item);
			delete t_item;
			t_item = NULL;
		}
	}
	m_currentItem = NULL;
	m_centerItem = NULL;
	m_scene->update();
}

//保存编队
bool formationWidget::saveFormation(const QString &fileName)
{
	QDomDocument doc("");
	QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"GB2312\"");
	doc.appendChild(instruction);

	QDomElement root = doc.createElement("Formation");
	doc.appendChild(root);

	for(int index = 0; index < m_scene->items().size(); ++index)
	{
		QPointF theRealityPos = dynamic_cast<uinitsGraphicsItem*>(m_scene->items().at(index))->getCenterPosReality();

		QDomElement element = doc.createElement("Item");
		element.setAttribute("isCenter", dynamic_cast<uinitsGraphicsItem*>(m_scene->items().at(index))->isCenterItem());
		element.setAttribute("axis_x", theRealityPos.x());
		element.setAttribute("axis_y", theRealityPos.y());
		root.appendChild(element);
	}

	QFile file(fileName);
	if(!file.open(QFile::WriteOnly | QFile::Truncate))
	{
		return false;
	}

	QTextStream out(&file);
	out.setCodec("GB2312");
	doc.save(out, 2);

	file.close();
	return true;
}

//响应保存编队按钮
void formationWidget::on_pushButton_leaveFormation_clicked()
{
	QString t_currentPath = QDir::currentPath();
	QString fileName = QFileDialog::getSaveFileName(&(myApp->mainWindow()), QString::fromLocal8Bit("保存编队为经典队形"),
		"../data/formation/",QString::fromLocal8Bit("formation (*.xml)"));

	if (fileName.isEmpty())
		return;

	QString type;
	switch(m_formationType)
	{
	case WARSHIP:
		type = "S";
	case AIRPLANE:
		type = "A";
	case PANZER:
		type = "P";
		break;
	default:;
	}

	QFileInfo info = fileName;
	fileName = info.path() + "/" + QString("%1-").arg(m_scene->items().size()) + info.baseName() + "-" + type + ".xml";

	if(saveFormation(fileName))
	{
		createFormationListWidget();
	}

	QDir::setCurrent(t_currentPath);
}

//加载编队
bool formationWidget::loadFormation(const QString &fileName)
{
	m_formationElementList.clear();

	QFile file(fileName);
	if(!file.open(QFile::ReadOnly))
		return false;

	QDomDocument doc("");
	
	if(!doc.setContent(&file))
		return false;

	if(doc.isNull())
		return false;

	QDomElement root = doc.documentElement();
	if(root.isNull())
		return false;

	QDomNodeList nodeList = root.elementsByTagName("Item");
	
	for(int index = 0; index < nodeList.size(); ++index)
	{
		QDomElement item = nodeList.item(index).toElement();

		if (item.isNull())
			continue;

		FormationElement forment;
		forment.isCenter = item.attribute("isCenter").toInt();
		forment.axis_x = item.attribute("axis_x").toInt();
		forment.axis_y = item.attribute("axis_y").toInt();

		m_formationElementList.append(forment);
	}
	return true;
}

//排序比较
bool compareFileInfo(QFileInfo &info1, QFileInfo &info2)
{
	return info1.baseName().left(info1.baseName().indexOf('-')) < info2.baseName().left(info2.baseName().indexOf('-'));
}

//创建编队列表
void formationWidget::createFormationListWidget()
{
	ui.listWidget_formationList->clear();

	QDir dir("../data/formation");
	QFileInfoList infoList = dir.entryInfoList(QDir::Files);

	//排序
	qSort(infoList.begin(), infoList.end(), compareFileInfo);

	QStringList list;

	for(int index = 0; index < infoList.size(); ++index)
	{
		list.append(infoList.at(index).baseName());
		QListWidgetItem* item = new QListWidgetItem(QString("%1").arg(infoList.at(index).baseName().left(infoList.at(index).baseName().lastIndexOf('-'))), ui.listWidget_formationList);
		item->setData(Qt::UserRole, QString("%1").arg(infoList.at(index).baseName()));
	}
}

//点击队形列表
void formationWidget::on_ListWidget_clicked(QListWidgetItem * item)
{
	if(!loadFormation(QString::fromLocal8Bit("../data/formation/%1.xml").arg(item->data(Qt::UserRole).toString())))
	{
		return;
	}

	clearSceneItem();
	for(int index = 0; index < m_formationElementList.size(); ++index)
	{
		uinitsGraphicsItem* item = new uinitsGraphicsItem(QPointF(m_formationElementList.at(index).axis_x, m_formationElementList.at(index).axis_y));
		item->setIsCenterItem(m_formationElementList.at(index).isCenter);
		connect(item, SIGNAL(ShowDistanceAndAngle(uinitsGraphicsItem*)), this, SLOT(onShowDistanceAndAngle(uinitsGraphicsItem*)));
		m_scene->addItem(item);
	}
}

//添加实体
void formationWidget::appendEntityToList(ByyEntityObject* entObj)
{
	if(!entObj)
		return;

	m_entityList.append(entObj);

	ui.comboBox_formationTroops->addItem(entObj->name());
}

void formationWidget::on_combobox_clicked(const QString& text)
{
 	if(!m_currentItem)
		return;

	if(text == "NONE")
	{
		if(m_currentItem->getCurrentEntity())
		{
			ui.comboBox_formationTroops->addItem(m_currentItem->getCurrentEntity()->name());
		}

		m_currentItem->removeCurrentEntity();
	}
	else
	{
		ByyEntityObject* entity = myApp->core().localObjectManager().findEntity(text);
		if(!entity)
			return;

		ui.comboBox_formationTroops->removeItem(ui.comboBox_formationTroops->currentIndex());

		if(m_currentItem->getCurrentEntity())
		{
			ui.comboBox_formationTroops->addItem(m_currentItem->getCurrentEntity()->name());
		}

		m_currentItem->setCurrentEntity(entity);
	}
}

void formationWidget::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
	QString formationName;
	if (item && !item->parent())
	{
		//topItem
		formationName = item->text(0);
	}
	else if(item && item->parent())
	{
		//childItem
		formationName = item->parent()->text(0);
	}
	for (unsigned int i=0;i<myFormationList.size();i++)
	{
		ByyScenarioV2::Formation formation = myFormationList.at(i);
		QString m_FormationName = formation.m_FormationName;
		if (formationName == m_FormationName)
		{
			showFormation(formation);
			break;
		}
	}
}

void formationWidget::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
	m_CreateFormationAct->setVisible(false);
	m_DisbandFormationAct->setVisible(false);
	m_SpecifyLeaderAct->setVisible(false);
	m_AddMemberAct->setVisible(false);
	m_DeleteMemberAct->setVisible(false);
	QTreeWidgetItem* item = ui.treeWidget->itemAt(pos);
	if (item)
	{
		//按钮
		if (item->parent())
		{
			//舰船，判断实体是否为编队长，如果不是，显示指定编队长和删除按钮，是则隐藏
			QString m_MemberName = item->text(0);
			QList<QGraphicsItem*> graphItems = m_scene->items();
			for (unsigned int i=0;i<graphItems.size();i++)
			{
				uinitsGraphicsItem* unitItem = (uinitsGraphicsItem*)graphItems.at(i);
				if (unitItem && unitItem->getCurrentEntity() && unitItem->getCurrentEntity()->name() == m_MemberName)
				{
					if (!unitItem->isCenterItem())
					{
						m_SpecifyLeaderAct->setVisible(true);
						m_DeleteMemberAct->setVisible(true);
					}
					break;
				}
			}
		}
		else
		{
			//编队
			m_DisbandFormationAct->setVisible(true);
			m_AddMemberAct->setVisible(true);
		}
		m_customContextMenu = item;
	}
	else
	{
		//空白
		m_CreateFormationAct->setVisible(true);
		m_customContextMenu = NULL;
	}
	m_FormationMenu->exec(QCursor::pos());//弹出右键菜单，菜单位置为光标位置
}

void formationWidget::on_CreateFormationAct()
{
	formationAddDialog m_formationAddDialog(myApp, myFormationList, ADD_FORMATION, this);
	if (m_formationAddDialog.exec() == QDialog::Accepted)
	{
		ByyScenarioV2::Formation formation = m_formationAddDialog.getFormation();
		myFormationList.append(formation);
		// 更新想定
		ByyScenarioV2 *scn=myApp->hdose().scenarioManager().scenario();
		if (scn)
		{
			scn->setFormationList(myFormationList);
		}
		// 更新树状结构
		QString m_FormationName = formation.m_FormationName;
		QTreeWidgetItem* topItem = new QTreeWidgetItem();
		topItem->setText(0, m_FormationName);
		ui.treeWidget->addTopLevelItem(topItem);
		for (unsigned int j=0;j<formation.m_FormationPara.size();j++)
		{
			ByyScenarioV2::FormationPara formationPara = formation.m_FormationPara.at(j);
			QTreeWidgetItem* item = new QTreeWidgetItem();
			item->setText(0, formationPara.m_MemberName);
			if (j == 0)
			{
				item->setIcon(0, QIcon(*formationImage::getInstance()->getPixmap(CENTERITEM)));
			}
			topItem->addChild(item);
		}
	}
}

void formationWidget::on_DisbandFormationAct()
{
	if (m_customContextMenu)
	{
		QString m_FormationName = m_customContextMenu->text(0);
		//qDebug()<<m_FormationName;
		for (unsigned int i=0;i<myFormationList.size();i++)
		{
			ByyScenarioV2::Formation formation = myFormationList.at(i);
			QString formationName = formation.m_FormationName;
			if (formationName == m_FormationName)
			{
				myFormationList.removeAt(i);
				ByyScenarioV2 *scn=myApp->hdose().scenarioManager().scenario();
				if (scn)
				{
					scn->setFormationList(myFormationList);
				}
				for (unsigned int j=0;j<ui.treeWidget->topLevelItemCount();j++)
				{
					QTreeWidgetItem* top = ui.treeWidget->topLevelItem(j);
					if (top == m_customContextMenu)
					{
						ui.treeWidget->takeTopLevelItem(j);
						m_customContextMenu = NULL;
						break;
					}
				}
				break;
			}
		}
	}
}

void formationWidget::on_SpecifyLeaderAct()
{
	//切换编队长
	if (m_customContextMenu)
	{
		QString m_MemberName = m_customContextMenu->text(0);
		//qDebug()<<m_MamberName;
		QTreeWidgetItem* top = m_customContextMenu->parent();
		if (top)
		{
			QString m_FormationName = top->text(0);
			//qDebug()<<m_FormationName;
			for (unsigned int i=0;i<myFormationList.size();i++)
			{
				ByyScenarioV2::Formation formation = myFormationList.at(i);
				QString formationName = formation.m_FormationName;
				if (formationName == m_FormationName)
				{
					//////////////////////////////////////////////////////////////////////////
					// 想定内容
					// 1.修改编队长名称
					formation.m_LeaderName = m_MemberName;
					ByyEntityObject* m_LeaderEntity = myApp->core().localObjectManager().findEntity(m_MemberName);
					if(!m_LeaderEntity) continue;
					JGeoCal::GeoPt m_LeaderPt(m_LeaderEntity->position().y(), m_LeaderEntity->position().x(), m_LeaderEntity->position().z());
					// 2.修改编队数量
					formation.m_MemberCount = formation.m_FormationPara.size();
					// 3.修改编队成员信息
					QList<ByyScenarioV2::FormationPara> m_FormationPara = formation.m_FormationPara;
					formation.m_FormationPara.clear();
					ByyScenarioV2::FormationPara firstPara;
					firstPara.m_MemberName = m_MemberName;
					formation.m_FormationPara.append(firstPara);
					for (unsigned int j=0;j<m_FormationPara.size();j++)
					{
						ByyScenarioV2::FormationPara para = m_FormationPara.at(j);
						if (para.m_MemberName != m_MemberName)
						{
							ByyEntityObject* obj = myApp->core().localObjectManager().findEntity(para.m_MemberName);
							if(!obj) continue;
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
							para.m_index = formation.m_FormationPara.size();
							formation.m_FormationPara.append(para);
						}
					}

					myFormationList.replace(i, formation);
					ByyScenarioV2 *scn=myApp->hdose().scenarioManager().scenario();
					if (scn)
					{
						scn->setFormationList(myFormationList);
					}
					//////////////////////////////////////////////////////////////////////////
					// 树状结构
					top->child(0)->setIcon(0, QIcon());
					top->takeChild(top->indexOfChild(m_customContextMenu))->setIcon(0, QIcon(*formationImage::getInstance()->getPixmap(CENTERITEM)));
					top->insertChild(0, m_customContextMenu);
					//////////////////////////////////////////////////////////////////////////
					// graphicsItem
					showFormation(formation);
					//debugFormation(formation);
					break;
				}
			}
		}
	}
}

void formationWidget::on_AddMemberAct()
{
	if (m_customContextMenu)
	{
		QString m_FormationName = m_customContextMenu->text(0);
		//qDebug()<<m_FormationName;
		for (unsigned int i=0;i<myFormationList.size();i++)
		{
			ByyScenarioV2::Formation formation = myFormationList.at(i);
			QString formationName = formation.m_FormationName;
			if (formationName == m_FormationName)
			{
				formationAddDialog m_formationAddDialog(myApp, myFormationList, ADD_MEMBER, this);
				if (m_formationAddDialog.exec() == QDialog::Accepted)
				{
					QList<ByyEntityObject*> objs = m_formationAddDialog.getSelectEntityObjs();
					//////////////////////////////////////////////////////////////////////////
					// 想定内容
					ByyEntityObject* m_LeaderEntity = myApp->core().localObjectManager().findEntity(formation.m_LeaderName);
					if(!m_LeaderEntity) continue;
					JGeoCal::GeoPt m_LeaderPt(m_LeaderEntity->position().y(), m_LeaderEntity->position().x(), m_LeaderEntity->position().z());
					// 修改编队数量
					formation.m_MemberCount += objs.size();
					// 修改编队成员信息
					for (unsigned int j=0;j<objs.size();j++)
					{
						ByyScenarioV2::FormationPara para;
						ByyEntityObject* obj = objs.at(j);
						if(!obj) continue;
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
						para.m_MemberName = obj->name();
						para.m_x = -y;
						para.m_z = x;
						para.m_index = formation.m_FormationPara.size();
						formation.m_FormationPara.append(para);
						//////////////////////////////////////////////////////////////////////////
						// 树状结构
						QTreeWidgetItem* newItem = new QTreeWidgetItem;
						newItem->setText(0, obj->name());
						m_customContextMenu->addChild(newItem);
					}

					myFormationList.replace(i, formation);
					//////////////////////////////////////////////////////////////////////////
					// 想定
					ByyScenarioV2 *scn=myApp->hdose().scenarioManager().scenario();
					if (scn)
					{
						scn->setFormationList(myFormationList);
					}
					//////////////////////////////////////////////////////////////////////////
					showFormation(formation);
				}
				break;
			}
		}
	}
}

void formationWidget::on_DeleteMemberAct()
{
	if (m_customContextMenu)
	{
		QString m_MemberName = m_customContextMenu->text(0);
		//qDebug()<<m_MamberName;
		QTreeWidgetItem* top = m_customContextMenu->parent();
		if (top)
		{
			QString m_FormationName = top->text(0);
			//qDebug()<<m_FormationName;
			for (unsigned int i=0;i<myFormationList.size();i++)
			{
				ByyScenarioV2::Formation formation = myFormationList.at(i);
				QString formationName = formation.m_FormationName;
				if (formationName == m_FormationName)
				{
					//////////////////////////////////////////////////////////////////////////
					// 想定内容
					// 1.修改编队数量
					formation.m_MemberCount = formation.m_FormationPara.size()-1;
					// 2.修改编队成员信息
					QList<ByyScenarioV2::FormationPara> m_FormationPara = formation.m_FormationPara;
					formation.m_FormationPara.clear();
					for (unsigned int j=0;j<m_FormationPara.size();j++)
					{
						ByyScenarioV2::FormationPara para = m_FormationPara.at(j);
						if (para.m_MemberName != m_MemberName)
						{
							para.m_index = formation.m_FormationPara.size();
							formation.m_FormationPara.append(para);
						}
					}

					myFormationList.replace(i, formation);
					ByyScenarioV2 *scn=myApp->hdose().scenarioManager().scenario();
					if (scn)
					{
						scn->setFormationList(myFormationList);
					}
					//////////////////////////////////////////////////////////////////////////
					// 树状结构
					top->takeChild(top->indexOfChild(m_customContextMenu));
					//////////////////////////////////////////////////////////////////////////
					// graphicsItem
					showFormation(formation);
					break;
				}
			}
		}
	}
}

//设置实体列表
void formationWidget::setEntityList(QList<ByyEntityObject*> list)
{
	m_entityList.clear();
	m_entityList = list;

	ui.comboBox_formationTroops->clear();
	ui.comboBox_formationTroops->addItem("NONE");
	for(int index = 0; index < m_entityList.size(); ++index)
	{
		if(m_entityList.at(index))
		{
			ui.comboBox_formationTroops->addItem(m_entityList.at(index)->name());
		}
	}
}

void formationWidget::getScnFormation()
{
	//想定中原有编队
	ByyScenarioV2 *scn=myApp->hdose().scenarioManager().scenario();
	if (scn)
	{
		myFormationList = scn->formationList();
		ui.treeWidget->clear();
		for (unsigned int i=0;i<myFormationList.size();i++)
		{
			ByyScenarioV2::Formation formation = myFormationList.at(i);
			QString m_FormationName = formation.m_FormationName;
			QTreeWidgetItem* topItem = new QTreeWidgetItem();
			topItem->setText(0, m_FormationName);
			ui.treeWidget->addTopLevelItem(topItem);
			for (unsigned int j=0;j<formation.m_FormationPara.size();j++)
			{
				ByyScenarioV2::FormationPara formationPara = formation.m_FormationPara.at(j);
				QTreeWidgetItem* item = new QTreeWidgetItem();
				item->setText(0, formationPara.m_MemberName);
				if (j == 0)
				{
					item->setIcon(0, QIcon(*formationImage::getInstance()->getPixmap(CENTERITEM)));
				}
				topItem->addChild(item);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}

void formationWidget::showFormation(ByyScenarioV2::Formation& f)
{
	clearSceneItem();
	int num = f.m_FormationPara.size();
	if(num > 0)
	{
		ByyEntityObject* m_LeaderEntity = myApp->core().localObjectManager().findEntity(f.m_LeaderName);
		m_centerItem = NULL;
		m_centerItem = new uinitsGraphicsItem(QPointF(0, 0), getEntityUNITTYPE(m_LeaderEntity),true);
		m_centerItem->setCurrentEntity(m_LeaderEntity);
		connect(m_centerItem, SIGNAL(ShowDistanceAndAngle(uinitsGraphicsItem*)), this, SLOT(onShowDistanceAndAngle(uinitsGraphicsItem*)));
		m_scene->addItem(m_centerItem);

		for(unsigned int index = 1; index < num; index++)
		{
			ByyScenarioV2::FormationPara formationPara = f.m_FormationPara.at(index);
			ByyEntityObject* obj = myApp->core().localObjectManager().findEntity(formationPara.m_MemberName);
			if(!obj) continue;
			//如果是其他成员，计算相对位置
			JGeoCal::GeoPt m_LeaderPt(m_LeaderEntity->position().y(), m_LeaderEntity->position().x(), m_LeaderEntity->position().z());
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

			uinitsGraphicsItem* item = new uinitsGraphicsItem(QPointF(x,y), getEntityUNITTYPE(obj));
			connect(item, SIGNAL(ShowDistanceAndAngle(uinitsGraphicsItem*)), this, SLOT(onShowDistanceAndAngle(uinitsGraphicsItem*)));
			item->setCurrentEntity(obj);

			m_scene->addItem(item);
		}
	}
}

UNITSTYPE formationWidget::getEntityUNITTYPE(ByyEntityObject* entObj)
{
	UNITSTYPE type = WARSHIP;
	if (entObj)
	{
		ByyEntityType entityType = entObj->entityType();
		if (entityType.kind == 1 && entityType.domain == 1)
		{
			type = PANZER;
		}
		else if (entityType.kind == 1 && entityType.domain == 2)
		{
			type = AIRPLANE;
		}
		else if (entityType.kind == 1 && entityType.domain == 3)
		{
			type = WARSHIP;
		}
	}
	return type;
}

