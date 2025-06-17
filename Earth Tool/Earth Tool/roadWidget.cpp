#include "stdafx.h"
#include "roadWidget.h"
#include "sceneController.h"
#include "roadPick.h"

#include <osgEarth/Registry>
#include <osgEarthAnnotation/AnnotationRegistry>
#include <osgEarthAnnotation/ImageOverlay>
#include <osgEarth/Registry>
#include <osgEarthFeatures/GeometryUtils>

ByyRoadWidget::ByyRoadWidget( ByySceneController* sceneController,QWidget *parent /*= 0*/ ) 
	: ByyOperateWidget(sceneController,parent)
{
	ui.setupUi(this);

	ui.pushButton->setIcon(QIcon(QIcon("../data/icons/UtilityMinus.png")));
	ui.pushButton_2->setIcon(QIcon("../data/icons/EarthTool/UtilityRename.png"));
	ui.pushButton_3->setIcon(QIcon("../data/icons/EarthTool/find.png"));

	connect(ui.pushButton,SIGNAL(clicked()),SLOT(removeRoad()));
	connect(ui.pushButton_2,SIGNAL(clicked()),SLOT(rename()));
	connect(ui.pushButton_3,SIGNAL(clicked()),SLOT(zoomTo()));
	connect(ui.listWidget,SIGNAL(currentRowChanged(int)),SLOT(zoomTo()));

	roadPick=new ByyRoadPick(d_root);
	connect(roadPick,SIGNAL(currentRoadChanged(int)),SLOT(picking(int)));
}

ByyRoadWidget::~ByyRoadWidget()
{

}

void ByyRoadWidget::addRoad( osg::ref_ptr<osgEarth::Annotation::FeatureNode> roadLine,double width,const QString& texture )
{
	double halfWidth=width/2;

	osgEarth::Symbology::Geometry *geometry=roadLine->getFeature()->getGeometry();
	std::vector<osg::Vec3d>& vec3dArray=geometry->asVector();

	double out_latRad,out_lonRad;

	osg::Vec2d start_left,start_right,end_left,end_right;

	osg::Group *road=0;

	for (int i=0;i!=vec3dArray.size()-1;++i)
	{
		osg::Vec3d& start=vec3dArray.at(i);
		osg::Vec3d& end=vec3dArray.at(i+1);

		double bearing=osgEarth::GeoMath::rhumbBearing(osg::inDegrees(start.y()),
			osg::inDegrees(start.x()),osg::inDegrees(end.y()),osg::inDegrees(end.x()));


		double reverseBearing=osgEarth::GeoMath::rhumbBearing(osg::inDegrees(end.y()),
			osg::inDegrees(end.x()),osg::inDegrees(start.y()),osg::inDegrees(start.x()));

		if (i==0)
		{
			osgEarth::GeoMath::rhumbDestination(osg::inDegrees(start.y()),osg::inDegrees(start.x()),bearing-osg::inDegrees(90.0),halfWidth,out_latRad,out_lonRad);
			start_left.x()=osg::RadiansToDegrees(out_lonRad);
			start_left.y()=osg::RadiansToDegrees(out_latRad);

			osgEarth::GeoMath::rhumbDestination(osg::inDegrees(start.y()),osg::inDegrees(start.x()),bearing+osg::inDegrees(90.0),halfWidth,out_latRad,out_lonRad);
			start_right.x()=osg::RadiansToDegrees(out_lonRad);
			start_right.y()=osg::RadiansToDegrees(out_latRad);
		}
		else
		{
			start_left=end_right;
			start_right=end_left;
		}

		if (i==vec3dArray.size()-2)
		{
			osgEarth::GeoMath::rhumbDestination(osg::inDegrees(end.y()),osg::inDegrees(end.x()),reverseBearing-osg::inDegrees(90.0),halfWidth,out_latRad,out_lonRad);
			end_left.x()=osg::RadiansToDegrees(out_lonRad);
			end_left.y()=osg::RadiansToDegrees(out_latRad);

			osgEarth::GeoMath::rhumbDestination(osg::inDegrees(end.y()),osg::inDegrees(end.x()),reverseBearing+osg::inDegrees(90.0),halfWidth,out_latRad,out_lonRad);
			end_right.x()=osg::RadiansToDegrees(out_lonRad);
			end_right.y()=osg::RadiansToDegrees(out_latRad);
		}
		else
		{
			double a=osgEarth::GeoMath::distance(start,end,d_scene->getMapNode()->getMapSRS());

			osg::Vec3d& nextEnd=vec3dArray.at(i+2);
			double b=osgEarth::GeoMath::distance(end,nextEnd,d_scene->getMapNode()->getMapSRS());

			double c=osgEarth::GeoMath::distance(start,nextEnd,d_scene->getMapNode()->getMapSRS());

			double B=osg::RadiansToDegrees(acos((a*a+c*c-b*b)/(2*a*c)));
			double C=osg::RadiansToDegrees(acos((a*a+b*b-c*c)/(2*a*b)));
			
			double _dis=halfWidth/cos(osg::inDegrees(90-C/2));

			double leftbear,rightbear;
			double nextBearing=osgEarth::GeoMath::rhumbBearing(osg::inDegrees(start.y()),osg::inDegrees(start.x()),osg::inDegrees(nextEnd.y()),osg::inDegrees(nextEnd.x()));

			bool leftMin=true;

			if (bearing>=osg::inDegrees(0.0)&&bearing<osg::inDegrees(90.0))
			{
				leftMin=!(nextBearing>=reverseBearing||nextBearing<=bearing);
			}
			else if (bearing>=osg::inDegrees(90.0)&&bearing<osg::inDegrees(180.0))
			{
				leftMin=nextBearing>=bearing&&nextBearing<reverseBearing;
			}
			else if (bearing>=osg::inDegrees(180.0)&&bearing<osg::inDegrees(270.0))
			{
				leftMin=(nextBearing>=bearing||nextBearing<=reverseBearing);
			}
			else
			{
				leftMin=nextBearing>=bearing||nextBearing<=reverseBearing;
			}

			if (leftMin)
			{
				leftbear=reverseBearing-osg::inDegrees(C/2);
				rightbear=reverseBearing+osg::inDegrees(180-C/2);
			}
			else
			{
				leftbear=reverseBearing-osg::inDegrees(180-C/2);
				rightbear=reverseBearing+osg::inDegrees(C/2);
			}

			osgEarth::GeoMath::rhumbDestination(osg::inDegrees(end.y()),osg::inDegrees(end.x()),leftbear,_dis,out_latRad,out_lonRad);

			end_left.x()=osg::RadiansToDegrees(out_lonRad);
			end_left.y()=osg::RadiansToDegrees(out_latRad);


			osgEarth::GeoMath::rhumbDestination(osg::inDegrees(end.y()),osg::inDegrees(end.x()),rightbear,_dis,out_latRad,out_lonRad);
			end_right.x()=osg::RadiansToDegrees(out_lonRad);
			end_right.y()=osg::RadiansToDegrees(out_latRad);
		}

		osgEarth::Config conf("imageoverlay");
		conf.set("url",QDir("../data/earthFile/").relativeFilePath(QFileInfo(texture).absoluteFilePath()).toStdString());

		std::string _referrer=d_scene->getMap()->getMapOptions().getConfig().referrer();

		conf.setReferrer(_referrer);

		/*osg::ref_ptr<osgEarth::Symbology::Geometry> g = new osgEarth::Symbology::Polygon();
		g->push_back( osg::Vec3d(start_right.x(),  start_right.y(), 0) );
		g->push_back( osg::Vec3d(end_left.x(), end_left.y(), 0) );
		g->push_back( osg::Vec3d(start_left.x(), start_left.y(), 0) );
		g->push_back( osg::Vec3d(end_right.x(),  end_right.y(),  0) );

		osgEarth::Config geomConf("geometry");
		geomConf.value() = osgEarth::Features::GeometryUtils::geometryToWKT( g.get() );
		conf.add( geomConf );*/

		osgEarth::Annotation::ImageOverlay *imageOverlay=new osgEarth::Annotation::ImageOverlay(d_scene->getMapNode(),conf,0);
		
		imageOverlay->setCorners(start_right,end_left,start_left,end_right);

		if (!road)
		{
			road=new osg::Group;
		}
		road->addChild(imageOverlay);
	}

	if (road)
	{
		ui.listWidget->addItem(QString("Road %1").arg(ui.listWidget->count()));
		d_root->addChild(road);
	}
	emit modified();
}

void ByyRoadWidget::init()
{
	release();

	const osgEarth::Config& externals = d_scene->getMapNode()->externalConfig();

	const osgEarth::Config& roadsConf=externals.child("roads");

	const osgEarth::ConfigSet& roadConfs=roadsConf.children("road");

	osg::ref_ptr<osgDB::Options> dbOptions = osgEarth::Registry::instance()->cloneOrCreateOptions();

	for(osgEarth::ConfigSet::const_iterator i = roadConfs.begin(); i != roadConfs.end(); ++i )
	{
		if (!i->empty())
		{
			osg::Group* _road = 0L;
			osgEarth::Annotation::AnnotationRegistry::instance()->create( d_scene->getMapNode(), *i, dbOptions.get(), _road );

			if (_road)
			{
				osgEarth::Symbology::Style style;
				style.getOrCreateSymbol<osgEarth::Symbology::AltitudeSymbol>()->clamping()=osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
				style.getOrCreateSymbol<osgEarth::Symbology::AltitudeSymbol>()->technique()=osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
				for (int ii=0;ii!=_road->getNumChildren();++ii)
				{
					dynamic_cast<osgEarth::Annotation::AnnotationNode*>(_road->getChild(ii))->setStyle(style);
				}
				ui.listWidget->addItem(QString::fromUtf8(i->value("name").c_str()));

				d_root->addChild(_road);
			}
		}
	}
	d_scene->getViewer()->addEventHandler(roadPick);
}

void ByyRoadWidget::release()
{
	d_scene->getViewer()->removeEventHandler(roadPick);
	d_root->removeChildren(0,d_root->getNumChildren());
	ui.listWidget->clear();
}

void ByyRoadWidget::writeToMapNode()
{
	osgEarth::Config& externals=d_scene->getMapNode()->externalConfig();
	externals.remove("roads");
	osgEarth::Config newConf("roads");

	for (int i=0;i!=d_root->getNumChildren();++i)
	{
		osg::Group *road=d_root->getChild(i)->asGroup();

		osgEarth::Config conf("road");
		conf.add("name",ui.listWidget->item(i)->text().toUtf8().data());


		for (int j=0;j!=road->getNumChildren();++j)
		{
			osgEarth::Annotation::ImageOverlay *imageOverlay=dynamic_cast<osgEarth::Annotation::ImageOverlay*>(road->getChild(j));

			if (imageOverlay)
			{
				conf.add(imageOverlay->getConfig());
			}
		}
		newConf.add(conf);
	}
	externals.add(newConf);
}

void ByyRoadWidget::removeRoad()
{
	int row=ui.listWidget->currentRow();
	if (row==-1)
	{
		return;
	}

	int r=QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("确认删除吗？"),QMessageBox::Yes|QMessageBox::No);

	if (r==QMessageBox::Yes)
	{
		d_root->removeChild(row);
		delete ui.listWidget->takeItem(row);

		emit modified();
	}
}

void ByyRoadWidget::rename()
{
	int row=ui.listWidget->currentRow();
	if (row==-1)
	{
		return;
	}

	bool ok;
	QString text = QInputDialog::getText(this, QString::fromLocal8Bit("重命名"),
		QString::fromLocal8Bit("名称:"), QLineEdit::Normal,
		ui.listWidget->item(row)->text(), &ok);
	if (ok && !text.isEmpty())
	{
		ui.listWidget->item(row)->setText(text);
		d_root->getChild(row)->asGroup()->setName(text.toUtf8().data());
		emit modified();
	}
}

void ByyRoadWidget::zoomTo()
{
	int row=ui.listWidget->currentRow();

	if (row==-1)
	{
		return;
	}
	int imgCount=d_root->getChild(row)->asGroup()->getNumChildren();
	osgEarth::Annotation::ImageOverlay *imgOly=dynamic_cast<osgEarth::Annotation::ImageOverlay*>(d_root->getChild(row)->asGroup()->getChild(imgCount/2));

	if (imgOly)
	{
		osg::Vec2 center=imgOly->getCenter();
#if OSGEARTH_MINOR_VERSION==6
	osgEarth::Viewpoint vp(osg::Vec3(center,300),0,-89,300);
#else if OSGEARTH_MINOR_VERSION==7
	osgEarth::Viewpoint vp;
	vp.setRange(300);
	vp.setPitch(-89);
	vp.focalPoint()->x()=center.x();
	vp.focalPoint()->y()=center.y();
#endif
		
		d_scene->changeViewpoint(vp);
	}
}

void ByyRoadWidget::picking( int index )
{
	ui.listWidget->setCurrentRow(index);
}
