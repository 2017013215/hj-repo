#ifndef ADDPATHDIALOG_H
#define ADDPATHDIALOG_H

#include <QDialog>
#include <osgEarth/MapNode>
#include <osgViewer/View>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthSymbology/Style>
#include <osgEarth/Draggers>

class QCheckBox;
class QPushButton;
struct AddPointsMouseHandler;

class ByyAddPathDialog : public QDialog
{
	Q_OBJECT

public:
	ByyAddPathDialog(osg::Group* root, osgEarth::MapNode* mapNode,osgViewer::View *view,osgEarth::Annotation::FeatureNode* path=0L,QWidget *parent=0,Qt::WindowFlags f = 0);
	~ByyAddPathDialog();

	osgEarth::Annotation::FeatureNode* getAnnotation() { return _pathNode.get(); }
	void mapMouseClick(const osgEarth::GeoPoint& point, int button);
	void addPoint(const osgEarth::GeoPoint& point);
	void mapMouseMove(const osgEarth::GeoPoint& point) { };

public slots:
	void accept();
	void reject();

private slots:
	void onDrapeCheckStateChanged(int state);
	void onLineColorButtonClicked();

protected:
	void initDefaultUi();
	void refreshFeatureNode();
	void createPointDragger(int index, const osgEarth::GeoPoint& point);
	void movePoint(int index, const osgEarth::GeoPoint& position);
	void clearDisplay();
	void resetValues();
	void updateButtonColorStyle(QPushButton* button, const QColor& color);
	void closeEvent(QCloseEvent* event);

private:
	QPushButton* _okButton;
	QCheckBox* _drapeCheckbox;
	QPushButton* _lineColorButton;

	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osg::ref_ptr<osg::Group> _root;
	osg::ref_ptr<osgViewer::View> _view;

	osg::ref_ptr<AddPointsMouseHandler>  _guiHandler;

	osg::ref_ptr<osgEarth::Symbology::LineString> _pathLine;
	osg::ref_ptr<osgEarth::Features::Feature> _pathFeature;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> _pathNode;
	osg::Vec4f _pathColor;
	osg::Group* _draggers;
	bool _editing;
	osg::ref_ptr<osgEarth::Features::Feature> _inFeature;

	friend struct AddPointsMouseHandler;
	friend struct PointDraggerCallback;
};


struct AddPointsMouseHandler : public osgGA::GUIEventHandler
{
	AddPointsMouseHandler(ByyAddPathDialog* dialog, osgEarth::MapNode* mapNode, osg::Group* root, bool drawLead=true)
		: _dialog(dialog), _mapNode(mapNode), _root(root), _drawLead(drawLead), _mouseDown(-1), _lastPoint(-500.0, 0.0, 0.0)
	{
		if (_drawLead)
		{
			//Define a style for the lead line
			osgEarth::Symbology::LineSymbol* ls = _lineStyle.getOrCreateSymbol<osgEarth::Symbology::LineSymbol>();
			ls->stroke()->color() = osgEarth::Symbology::Color::White;
			ls->stroke()->width() = 2.0f;
			ls->stroke()->stipple() = 0x0F0F;
			ls->tessellation() = 20;
			_lineStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
			_lineStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;
		}
	}

	bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
	{
		osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());

		if ( ea.getEventType() == osgGA::GUIEventAdapter::PUSH  && _mouseDown == -1)
		{
			_mouseDown = ea.getButton();
			_xDown = ea.getX();
			_yDown = ea.getY();
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE && _lastPoint.x() != -500.0)
		{
			osg::Vec3d world;
			if (_mapNode->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), world))
			{
				osgEarth::GeoPoint mapPoint;
				mapPoint.fromWorld( _mapNode->getMapSRS(), world );
				//_mapNode->getMap()->worldPointToMapPoint(world, mapPoint);

				_dialog->mapMouseMove(mapPoint);

				_currentPoint = mapPoint.vec3d();
				updateDisplay();
			}
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)
		{
			if (ea.getButton() == _mouseDown)
			{
				if (_xDown == ea.getX() && _yDown == ea.getY())
				{
					osg::Vec3d world;
					if (_mapNode->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), world))
					{
						osgEarth::GeoPoint mapPoint;
						mapPoint.fromWorld( _mapNode->getMapSRS(), world );
						//_mapNode->getMap()->worldPointToMapPoint( world, mapPoint );
						_currentPoint = mapPoint.vec3d();
						_lastPoint = mapPoint.vec3d();

						_dialog->mapMouseClick(mapPoint, _mouseDown);

						updateDisplay();
					}
				}

				_mouseDown = -1;
			}
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK)
		{
			QMetaObject::invokeMethod(_dialog, "accept", Qt::QueuedConnection);
			return true;
		}

		return false;
	}

	void clearDisplay()
	{
		_lastPoint.x() = -500.0;

		if (_featureNode.valid() && _root.valid())
		{
			_root->removeChild( _featureNode.get() );
			_featureNode = 0L;
		}
	}

	void updateDisplay()
	{
		if (_drawLead && _root.valid() && _lastPoint.x() != -500.0)
		{
			osgEarth::Symbology::LineString* line = new osgEarth::Symbology::LineString();
			line->push_back( _lastPoint );
			line->push_back( _currentPoint );

			osgEarth::Features::Feature* feature = new osgEarth::Features::Feature(line, _mapNode->getMapSRS());
			feature->geoInterp() = osgEarth::GEOINTERP_GREAT_CIRCLE;    
			feature->style() = _lineStyle;

			if (!_featureNode.valid())
			{
				_featureNode = new osgEarth::Annotation::FeatureNode( _mapNode, feature );
				_featureNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
				_root->addChild( _featureNode.get() );
			}
			else
			{
				_featureNode->setFeature(feature);
			}
		}
	}


	ByyAddPathDialog* _dialog;
	osg::ref_ptr<osgEarth::MapNode>  _mapNode;
	osg::ref_ptr<osg::Group> _root;
	osg::ref_ptr<osgEarth::Annotation::FeatureNode> _featureNode;
	osgEarth::Symbology::Style _lineStyle;

	bool _drawLead;
	int _mouseDown;
	float _xDown, _yDown;
	osg::Vec3d _lastPoint;
	osg::Vec3d _currentPoint;
};


struct PointDraggerCallback : public osgEarth::Dragger::PositionChangedCallback
{
	PointDraggerCallback(int index, ByyAddPathDialog* dialog)
		: _index(index), _dialog(dialog)
	{
	}

	void onPositionChanged(const osgEarth::Dragger* sender, const osgEarth::GeoPoint& position)
	{
		_dialog->movePoint(_index, position);
	}

	int _index;
	ByyAddPathDialog* _dialog;
};

#endif // ADDPATHDIALOG_H
