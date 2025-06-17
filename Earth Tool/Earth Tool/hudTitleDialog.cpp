#include "stdafx.h"
#include "hudTitleDialog.h"

ByyHudTitleDialog::ByyHudTitleDialog( ByySceneController* scene,QWidget *parent /*= 0*/ )
	: QDialog(parent),d_scene(scene)
{
	ui.setupUi(this);
	connect(ui.pushButton,SIGNAL(clicked()),SLOT(onSelect()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(onAccept()));
	connect(ui.lineEdit,SIGNAL(textChanged(const QString&)),SLOT(textChanged(const QString&)));


	imageControl=new osgEarth::Util::Controls::ImageControl;
	imageControl->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_CENTER);
	imageControl->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_CENTER);
	canvas=osgEarth::Util::Controls::ControlCanvas::getOrCreate(d_scene->getViewer());

	canvas->addControl(imageControl.get());

	//scene->addChild(canvas);


	const osgEarth::Config& externals = d_scene->getMapNode()->externalConfig();

	const osgEarth::Config& roadsConf=externals.child("hudtitle");

	ui.lineEdit->setText(roadsConf.value("url").c_str());

	osgEarth::optional<double> altitude,xmin,xmax,ymin,ymax;

	roadsConf.getIfSet("altitude",altitude);
	roadsConf.getIfSet("xmin",xmin);
	roadsConf.getIfSet("xmax",xmax);
	roadsConf.getIfSet("ymin",ymin);
	roadsConf.getIfSet("ymax",ymax);

	ui.doubleSpinBox->setValue(altitude.get());
	ui.doubleSpinBox_2->setValue(xmin.get());
	ui.doubleSpinBox_3->setValue(xmax.get());
	ui.doubleSpinBox_4->setValue(ymin.get());
	ui.doubleSpinBox_5->setValue(ymax.get());
}

ByyHudTitleDialog::~ByyHudTitleDialog()
{
	canvas->removeControl(imageControl);
	//d_scene->removeChild(canvas);
}

void ByyHudTitleDialog::onSelect()
{
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Ñ¡ÔñÍ¼Æ¬"),
		"../data/textures",tr("Images (*.png)"));

	if (fileName.isEmpty())
	{
		return;
	}

	QString imageAbsolutePath=QDir("../data").absolutePath();
	QString a=QDir::fromNativeSeparators(fileName).left(imageAbsolutePath.count()).toLower();
	QString b=imageAbsolutePath.toLower();
	bool inImagePath=QDir::fromNativeSeparators(fileName).left(imageAbsolutePath.count()).toLower()==imageAbsolutePath.toLower();

	if (inImagePath)
	{
		fileName=QDir(".").relativeFilePath(fileName);
		
	}
	ui.lineEdit->setText(fileName);
}

void ByyHudTitleDialog::textChanged( const QString& text )
{
	osg::Image *image=osgDB::readImageFile(text.toStdString());
	imageControl->setImage(image);
}

void ByyHudTitleDialog::onAccept()
{
	osgEarth::Config& externals=d_scene->getMapNode()->externalConfig();
	externals.remove("hudtitle");

	if (!ui.lineEdit->text().isEmpty())
	{
		osgEarth::Config conf("hudtitle");
		conf.add("url",ui.lineEdit->text().toStdString());
		conf.add("altitude",ui.doubleSpinBox->value());
		conf.add("xmin",ui.doubleSpinBox_2->value());
		conf.add("xmax",ui.doubleSpinBox_3->value());
		conf.add("ymin",ui.doubleSpinBox_4->value());
		conf.add("ymax",ui.doubleSpinBox_5->value());
		externals.add(conf);
	}
}
