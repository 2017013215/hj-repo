#include "stdafx.h"
#include "maskWidget.h"
#include "sceneController.h"
#include "area.h"

#include <osgEarthFeatures/GeometryUtils>
#include "AnnotationDialogs.h"

ByyMaskWidget::ByyMaskWidget(ByySceneController *scene,QWidget *parent)
	: ByyOperateWidget(scene,parent)
{
	ui.setupUi(this);

	ui.pushButton->setIcon(QIcon("../data/icons/edit.png"));
	ui.pushButton_2->setIcon(QIcon("../data/icons/UtilityMinus.png"));
	ui.pushButton_3->setIcon(QIcon("../data/icons/UtilityRename.png"));
	ui.pushButton_4->setIcon(QIcon("../data/icons/EarthTool/find.png"));


	connect(ui.pushButton,SIGNAL(clicked()),SLOT(edit()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(remove()));
	connect(ui.pushButton_3,SIGNAL(clicked()),SLOT(rename()));
	connect(ui.pushButton_4,SIGNAL(clicked()),SLOT(find()));

	connect(ui.listWidget,SIGNAL(currentRowChanged(int)),SLOT(currentRowChanged(int)));
	connect(ui.checkBox,SIGNAL(clicked(bool)),SLOT(forceAltitudeChanged(bool)));
	connect(ui.doubleSpinBox,SIGNAL(valueChanged(double)),SLOT(altitudeChanged(double)));

	connect(ui.myMaskCheckBox,SIGNAL(stateChanged(int)),SLOT(maskEnableChanged(int)));
}

ByyMaskWidget::~ByyMaskWidget()
{

}

void ByyMaskWidget::init()
{
	release();

	osgEarth::Config& externals=d_scene->getMapNode()->externalConfig();

	osgEarth::Config masks=externals.child("masks");
	const osgEarth::ConfigSet& children = masks.children();

	for(osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i )
	{
		if (!i->empty())
		{
			addMask(*i);
		}
	}
}

void ByyMaskWidget::release()
{
	myMaskInfoList.clear();
	ui.listWidget->clear();
}

void ByyMaskWidget::addMask( const osgEarth::Config& conf )
{
	osgEarth:: Config maskConf("mask");
	maskConf.add("driver","feature");
	maskConf.add("name","mask");
	maskConf.add("min_level",0);
	osgEarth::Config featuresConf("features");
	featuresConf.add("driver","ogr");
	featuresConf.add("geometry",conf.value("geometry"));

	maskConf.add(featuresConf);

	maskConf.add("profile","global-geodetic");


	osgEarth::MaskLayerOptions options(maskConf);
	options.name() = maskConf.value( "name" );
	options.driver() = osgEarth::MaskSourceOptions(options);

	osgEarth::MaskLayer *_maskLayer=new osgEarth::MaskLayer(options);

	MaskInfo maskInfo;
	maskInfo.myMaskLayer=_maskLayer;
	maskInfo.geometry=conf.value("geometry");

	conf.getIfSet("enable",maskInfo.maskEnable);

	if (conf.hasChild("forcealtitude"))
	{
		maskInfo.forceAltitude=true;

		conf.getIfSet("forcealtitude",maskInfo.altitude);
	}

	myMaskInfoList.push_back(maskInfo);

	if (maskInfo.maskEnable)
	{
		d_scene->getMap()->addTerrainMaskLayer( _maskLayer );
	}

	QListWidgetItem *item=new QListWidgetItem(QString::fromUtf8(conf.value("name").c_str()));
	ui.listWidget->addItem(item);
}

void ByyMaskWidget::writeToMapNode()
{
	osgEarth::Config& externals=d_scene->getMapNode()->externalConfig();

	externals.remove("masks");

	osgEarth::Config newConf("masks");

	for (int i=0;i!=myMaskInfoList.size();++i)
	{

		osgEarth::Config _maskConf("mask");

		_maskConf.add("name",ui.listWidget->item(i)->text().toUtf8().data());
		_maskConf.add("geometry",myMaskInfoList.at(i).geometry);
		_maskConf.add("enable",myMaskInfoList.at(i).maskEnable);

		if (myMaskInfoList.at(i).forceAltitude)
		{
			_maskConf.add("forcealtitude",myMaskInfoList.at(i).altitude);
		}

		newConf.add(_maskConf);
	}

	externals.add(newConf);
}

void ByyMaskWidget::addArea()
{
	osgEarth::QtGui::ViewVector viewVector;

	viewVector.push_back(d_scene->getViewer());

	osgEarth::QtGui::AddPolygonDialog *_activeDialog = 
		new osgEarth::QtGui::AddPolygonDialog(d_scene->getRootNode(), d_scene->getMapNode(),viewVector);

	connect(_activeDialog, SIGNAL(finished(int)), this, SLOT(addAreaFinished(int)));

	_activeDialog->setWindowTitle(QString::fromLocal8Bit("挖洞"));
	_activeDialog->setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint| Qt::WindowStaysOnTopHint);
	_activeDialog->setAttribute(Qt::WA_DeleteOnClose);
	_activeDialog->show();
}

void ByyMaskWidget::remove()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
	{
		return;
	}

	int r=QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("确认删除吗？"),
		QMessageBox::Yes|QMessageBox::No);

	if (r!=QMessageBox::Yes)
	{
		return;
	}

	d_scene->getMap()->removeTerrainMaskLayer(myMaskInfoList[row].myMaskLayer);

	myMaskInfoList.remove(row);

	delete ui.listWidget->takeItem(row);

	emit modified();
}

void ByyMaskWidget::addAreaFinished( int result )
{
	osgEarth::QtGui::AddPolygonDialog *_activeDialog=dynamic_cast<osgEarth::QtGui::AddPolygonDialog*>(sender());

	if (_activeDialog&&result == QDialog::Accepted)
	{
		osg::ref_ptr<osgEarth::Annotation::FeatureNode> featureNode=dynamic_cast<osgEarth::Annotation::FeatureNode*>(_activeDialog->getAnnotation());

		if (featureNode)
		{
			osgEarth::Config conf("mask");
			conf.add("name",QString("Area %1").arg(ui.listWidget->count()).toUtf8().data());

			osgEarth::Symbology::Geometry *_geometry=featureNode->getFeature()->getGeometry();

			for (int i=0;i!=_geometry->size();++i)
			{
				(*_geometry)[i].z()=0;
			}

			conf.add("enable",true);
			conf.add("geometry",osgEarth::Features::GeometryUtils::geometryToWKT(_geometry));

			addMask(conf);

			emit modified();

			ui.listWidget->setCurrentRow(ui.listWidget->count()-1);
		}
	}
}

void ByyMaskWidget::edit()
{
// 	int row=ui.listWidget->currentRow();
// 
// 	if (row==-1)
// 	{
// 		return;
// 	}
// 
// 	osgEarth::QtGui::ViewVector viewVector;
// 
// 	viewVector.push_back(d_scene->getViewer());
// 
// 	osgEarth::QtGui::AddPolygonDialog *_activeDialog = 
// 		new osgEarth::QtGui::AddPolygonDialog(d_scene->getRootNode(), d_scene->getMapNode(),viewVector,d_areaList[row]->getArea());
// 
// 	connect(_activeDialog, SIGNAL(finished(int)), this, SLOT(editFinished(int)));
// 
// 	_activeDialog->setWindowTitle(QString::fromLocal8Bit("编辑区域"));
// 	_activeDialog->setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint| Qt::WindowStaysOnTopHint);
// 	_activeDialog->setAttribute(Qt::WA_DeleteOnClose);
// 	_activeDialog->show();
// 
// 	setEnabled(false);
}

void ByyMaskWidget::editFinished( int result )
{
	setEnabled(true);

	osgEarth::QtGui::AddPolygonDialog *_activeDialog=dynamic_cast<osgEarth::QtGui::AddPolygonDialog*>(sender());

	if (_activeDialog&&result == QDialog::Accepted)
	{
		emit modified();
	}
}

void ByyMaskWidget::rename()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
	{
		return;
	}

	bool ok;

	QString text = QInputDialog::getText(this, QString::fromLocal8Bit("重命名"),
		QString::fromLocal8Bit("新名称:"), QLineEdit::Normal,ui.listWidget->item(row)->text(), &ok);

	if (ok && !text.isEmpty())
	{
		ui.listWidget->item(row)->setText(text);
		emit modified();
	}
}

void ByyMaskWidget::find()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
	{
		return;
	}

	osg::ref_ptr<osgEarth::Symbology::Geometry> geomtry=osgEarth::Features::GeometryUtils::geometryFromWKT(myMaskInfoList[row].geometry);

	osgEarth::Bounds bounds=geomtry->getBounds();
	
	d_scene->changeViewpoint(osg::Vec3d(bounds.center2d(),qMax(bounds.width(),bounds.height())*111000));
}

void ByyMaskWidget::currentRowChanged( int currentRow )
{
	ui.checkBox->blockSignals(true);
	ui.doubleSpinBox->blockSignals(true);
	ui.myMaskCheckBox->blockSignals(true);

	ui.checkBox->setChecked(myMaskInfoList[currentRow].forceAltitude);
	ui.doubleSpinBox->setValue(myMaskInfoList[currentRow].altitude);

	ui.myMaskCheckBox->setChecked(myMaskInfoList[currentRow].maskEnable);

	ui.myMaskCheckBox->blockSignals(false);
	ui.checkBox->blockSignals(false);
	ui.doubleSpinBox->blockSignals(false);
}

void ByyMaskWidget::forceAltitudeChanged( bool checked )
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
		return;

	myMaskInfoList[row].forceAltitude=checked;

	emit modified();
}

void ByyMaskWidget::altitudeChanged( double value )
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
		return;

	myMaskInfoList[row].altitude=value;

	emit modified();
}

void ByyMaskWidget::maskEnableChanged( int checkState )
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
		return;

	myMaskInfoList[row].maskEnable=checkState==Qt::Checked;

	if (myMaskInfoList[row].maskEnable)
	{
		d_scene->getMap()->addTerrainMaskLayer( myMaskInfoList[row].myMaskLayer );
	}
	else
	{
		d_scene->getMap()->removeTerrainMaskLayer( myMaskInfoList[row].myMaskLayer );
	}

	emit modified();
}
