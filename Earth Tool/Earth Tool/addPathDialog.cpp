#include "stdafx.h"
#include "addPathDialog.h"

ByyAddPathDialog::ByyAddPathDialog(osg::Group* root, osgEarth::MapNode* mapNode,osgViewer::View *view,osgEarth::Annotation::FeatureNode* path,QWidget *parent,Qt::WindowFlags f)
	: QDialog(parent,f),
	_mapNode(mapNode),
	_root(root),
	_view(view),
	 _draggers(0L), 
	 _pathColor(osgEarth::Color::White),
	 _editing(path ? true : false)
{
	initDefaultUi();

	if (path)
	{
		_pathNode = path;

		const osgEarth::Features::Feature* feat = path->getFeature();
		if (feat)
		{
			_inFeature = const_cast<osgEarth::Features::Feature*>(feat);

			//Get path color
			const osgEarth::Symbology::LineSymbol* lineSymbol = feat->style()->get<osgEarth::Symbology::LineSymbol>();
			if (lineSymbol)
			{
				_pathColor = lineSymbol->stroke()->color();
				updateButtonColorStyle(_lineColorButton, QColor::fromRgbF(_pathColor.r(), _pathColor.g(), _pathColor.b(), _pathColor.a()));
			}

			//Get path clamping
			const osgEarth::Symbology::AltitudeSymbol* altSymbol = feat->style()->get<osgEarth::Symbology::AltitudeSymbol>();
			if (altSymbol)
				_drapeCheckbox->setChecked(altSymbol->clamping() == osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN);

			//Get path points
			const osgEarth::Symbology::LineString* pathLine = dynamic_cast<const osgEarth::Symbology::LineString*>(feat->getGeometry());
			if (pathLine)
			{
				for (osgEarth::Symbology::LineString::const_iterator it = pathLine->begin(); it != pathLine->end(); ++it)
					addPoint(osgEarth::GeoPoint(_mapNode->getMapSRS(), (*it).x(), (*it).y(), (*it).z(), osgEarth::ALTMODE_RELATIVE));
			}
		}
	}
}

ByyAddPathDialog::~ByyAddPathDialog()
{

}

void ByyAddPathDialog::initDefaultUi()
{
	setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::CustomizeWindowHint| Qt::WindowStaysOnTopHint);
	// main layout
	QVBoxLayout* vLayout = new QVBoxLayout;
	setLayout(vLayout);

	// empty layout for custom content
	QVBoxLayout *_customLayout = new QVBoxLayout;
	//vLayout->addLayout(_customLayout);

	// ok/cancel buttons
	//vLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

	_lineColorButton = new QPushButton(tr("Path Color"));
	_lineColorButton->setStyleSheet("QPushButton { color: black; background-color: white }");
	//_customLayout->addWidget(_lineColorButton);

	// add name display checkbox to the dialog
	_drapeCheckbox = new QCheckBox(tr("Clamp to terrain"));
	_drapeCheckbox->setCheckState(Qt::Checked);
	//_customLayout->addWidget(_drapeCheckbox);

	QLabel* textureLabel=new QLabel(QString::fromLocal8Bit("贴图"));
	textureImageLabel=new QLabel;
	textureImageLabel->setScaledContents(true);
	texturePath="../data/Textures/roads/road.png";
	textureImageLabel->setPixmap((QPixmap(texturePath).scaled(100,25,Qt::KeepAspectRatio)));
	QToolButton *changeTextureButton=new QToolButton;
	connect(changeTextureButton,SIGNAL(clicked()),SLOT(changeTexture()));

	QHBoxLayout *textureLayout=new QHBoxLayout;
	textureLayout->addWidget(textureLabel);
	textureLayout->addWidget(textureImageLabel);
	textureLayout->addWidget(changeTextureButton);

	QLabel* widthLabel=new QLabel(QString::fromLocal8Bit("宽度(米):"));
	widthSpinBox=new QDoubleSpinBox;
	widthSpinBox->setRange(1,100);
	widthSpinBox->setValue(10);

	QHBoxLayout *widthLayout=new QHBoxLayout;
	widthLayout->addWidget(widthLabel);
	widthLayout->addWidget(widthSpinBox);

	vLayout->addLayout(textureLayout);
	vLayout->addLayout(widthLayout);

	QHBoxLayout* hb2 = new QHBoxLayout;

	hb2->addStretch();

	_okButton = new QPushButton(QString::fromLocal8Bit("确认"));
	_okButton->setEnabled(false);
	hb2->addWidget(_okButton);

	QPushButton* cancelButton = new QPushButton(QString::fromLocal8Bit("取消"));
	hb2->addWidget(cancelButton);

	vLayout->addLayout(hb2);

	// wire up ui events
	connect(_okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(_drapeCheckbox, SIGNAL(stateChanged(int)), this, SLOT(onDrapeCheckStateChanged(int)));
	connect(_lineColorButton, SIGNAL(clicked()), this, SLOT(onLineColorButtonClicked()));

	

	if (_mapNode.valid() && _view.valid())
	{
		_guiHandler = new AddPointsMouseHandler(this, _mapNode, _root);
			_view->addEventHandler(_guiHandler);
	}
}

void ByyAddPathDialog::mapMouseClick( const osgEarth::GeoPoint& point, int button )
{
	if (button == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
	{
		addPoint(point);
	}
}

void ByyAddPathDialog::addPoint( const osgEarth::GeoPoint& point )
{
	if (!_pathLine.valid())
		_pathLine = new osgEarth::Symbology::LineString();

	_pathLine->push_back(point.vec3d());

	if (!_pathFeature.valid() && _pathLine->size() > 1)
	{
		osgEarth::Symbology::Style pathStyle;
		pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Color::White;
		pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 2.0f;
		pathStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->tessellation() = 20;
		pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
		pathStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;

		_pathFeature = new osgEarth::Features::Feature(_pathLine, _mapNode->getMapSRS(), pathStyle);
		//_pathFeature->geoInterp() = GEOINTERP_GREAT_CIRCLE;

		if (!_pathNode.valid())
		{
			_pathNode = new osgEarth::Annotation::FeatureNode(_mapNode, _pathFeature);
			_root->addChild(_pathNode);
		}

		_okButton->setEnabled(true);
	}

	refreshFeatureNode();
	createPointDragger(_pathLine->size() - 1, point);
}

void ByyAddPathDialog::refreshFeatureNode()
{
	if (_pathNode.valid() && _pathFeature.valid())  
	{
		_pathFeature->style()->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = _pathColor;
		_pathFeature->style()->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = _drapeCheckbox->checkState() == Qt::Checked ? osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN : osgEarth::Symbology::AltitudeSymbol::CLAMP_ABSOLUTE;
		_pathNode->setFeature(_pathFeature);
	}
}

void ByyAddPathDialog::createPointDragger( int index, const osgEarth::GeoPoint& point )
{
	osgEarth::Annotation::SphereDragger* sd = new osgEarth::Annotation::SphereDragger(_mapNode);
	sd->setSize(4.0f);
	sd->setColor(osgEarth::Color::Magenta);
	sd->setPickColor(osgEarth::Color::Green);
	sd->setPosition(point);
	PointDraggerCallback* callback = new PointDraggerCallback(index, this);
	sd->addPositionChangedCallback(callback);

	if (!_draggers)
	{
		_draggers = new osg::Group();
		_root->addChild(_draggers);
	}

	_draggers->addChild(sd);
}

void ByyAddPathDialog::movePoint( int index, const osgEarth::GeoPoint& position )
{
	(*_pathLine.get())[index] = position.vec3d();
	refreshFeatureNode();
}

void ByyAddPathDialog::accept()
{
	clearDisplay();

	if (_pathNode.valid())
	{
		//osgEarth::Annotation::AnnotationData* annoData = new osgEarth::Annotation::AnnotationData();
		//annoData->setViewpoint(osgEarth::Viewpoint(_pathNode->getPosition().vec3d(), 0.0, -90.0, 1e5, _pathNode->getPosition().getSRS()));

		//_pathNode->setAnnotationData(annoData);
	}

	QDialog::accept();
}

void ByyAddPathDialog::reject()
{
	resetValues();
	clearDisplay();
	QDialog::reject();
}

void ByyAddPathDialog::onDrapeCheckStateChanged( int state )
{
	refreshFeatureNode();
}

void ByyAddPathDialog::onLineColorButtonClicked()
{
	QColor color = QColorDialog::getColor(QColor::fromRgbF(_pathColor.r(), _pathColor.g(), _pathColor.b(), _pathColor.a()), this);
	if (color.isValid())
	{
		_pathColor = osgEarth::Symbology::Color(color.redF(), color.greenF(), color.blueF());
		refreshFeatureNode();

		updateButtonColorStyle(_lineColorButton, color);
	}
}

void ByyAddPathDialog::clearDisplay()
{
	if (_root.valid())
	{
		if (!_editing)
		_root->removeChild(_pathNode);

		_root->removeChild(_draggers);
	}

	if (_guiHandler.valid())
	{
		_view->removeEventHandler(_guiHandler);

		_guiHandler->clearDisplay();
	}
}

void ByyAddPathDialog::resetValues()
{
	if (_inFeature.valid())
	{
		_pathNode->setFeature(_inFeature);
	}
	else
	{
		if (_root.valid())
		{
			_root->removeChild(_pathNode);
			_pathNode = 0L;
			_pathLine = 0L;
		}
	}
}

void ByyAddPathDialog::updateButtonColorStyle( QPushButton* button, const QColor& color )
{
	if (button)
	{
		if (color.alpha() == 0)
		{
			button->setStyleSheet("QPushButton { color: black; }");
		}
		else
		{
			int invR = 255 - color.red();
			int invG = 255 - color.green();
			int invB = 255 - color.blue();
			QColor invColor(invR, invG, invB);

			button->setStyleSheet("QPushButton { color: " + invColor.name() + "; background-color: " + color.name() + " }");
		}
	}
}

void ByyAddPathDialog::closeEvent( QCloseEvent* event )
{
	clearDisplay();
	QDialog::closeEvent(event);
}

void ByyAddPathDialog::changeTexture()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
		"../data/Textures/roads/",
		tr("Images (*.png  *.jpg)"));

	if (!fileName.isEmpty())
	{
		textureImageLabel->setPixmap(QPixmap(fileName).scaled(100,25,Qt::KeepAspectRatio));
		texturePath=QDir().relativeFilePath(fileName);
	}
}
