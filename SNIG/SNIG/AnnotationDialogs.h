#ifndef OSGEARTHQT_ANNOTATIONDIALOGS_H
#define OSGEARTHQT_ANNOTATIONDIALOGS_H 1

#include <osgEarthQt/Common>
#include <osgEarthAnnotation/Draggers>
#include <osgEarth/GeoMath>
#include <osgEarthAnnotation/AnnotationEditing>
#include <osgEarthAnnotation/EllipseNode>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/LabelNode>
#include <osgEarthFeatures/Feature>
#include <osgEarthSymbology/Geometry>

#include <QCheckBox>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "geoMath.h"

struct AddPointsMouseHandler;
struct PointDraggerCallback;

class BaseAnnotationDialog : public QDialog
{
Q_OBJECT
      
public:
	BaseAnnotationDialog(osgEarth::MapNode* mapNode, const osgEarth::QtGui::ViewVector& views, QWidget* parent = 0, Qt::WindowFlags f = 0);

	std::string getName() { return _nameEdit ? _nameEdit->text().toUtf8().data() : ""; }
	std::string getDescription() { return _descriptionEdit ? _descriptionEdit->text().toUtf8().data() : ""; }
	virtual osgEarth::Annotation::AnnotationNode* getAnnotation() = 0;

	virtual void mapMouseClick(const osgEarth::GeoPoint& point, int button) { };
	virtual void mapMouseMove(const osgEarth::GeoPoint& point) { };

protected:
	void initDefaultUi();
	void updateButtonColorStyle(QPushButton* button, const QColor& color);

	virtual void movePoint(int index, const osgEarth::GeoPoint& position) { };

	osg::ref_ptr<osgEarth::MapNode> _mapNode;
	osgEarth::QtGui::ViewVector _views;
	QLineEdit* _nameEdit;
	QLineEdit* _descriptionEdit;
	QVBoxLayout* _customLayout;
	QPushButton* _okButton;

	osgEarth::Symbology::Style labelStyle;

	friend struct AddPointsMouseHandler;
	friend struct PointDraggerCallback;

};

//---------------------------------------------------------------------------

class AddMarkerDialog : public BaseAnnotationDialog
{
Q_OBJECT

public:
	AddMarkerDialog(osg::Group* root, osgEarth::MapNode* mapNode, const osgEarth::QtGui::ViewVector& views, osgEarth::Annotation::PlaceNode* marker=0, QWidget* parent = 0, Qt::WindowFlags f = 0);

osgEarth::Annotation::AnnotationNode* getAnnotation() { return _placeNode.get(); }

void mapMouseClick(const osgEarth::GeoPoint& point, int button);

public slots:
void accept();
void reject();

private slots:
void onNameCheckStateChanged(int state);
void onNameTextChanged(const QString& text);

protected:
void closeEvent(QCloseEvent* event);

void initialize();
void clearDisplay();
void resetValues();

osg::ref_ptr<osg::Group> _root;
osg::ref_ptr<osgGA::GUIEventHandler>  _guiHandler;
QCheckBox* _nameCheckbox;
osg::ref_ptr<osg::Image> _markerImage;
osgEarth::Symbology::Style _placeStyle;
osg::ref_ptr<osgEarth::Annotation::PlaceNode> _placeNode;
std::string _altName;
bool _editing;
osgEarth::GeoPoint _inPosition;
std::string _inName;
std::string _inDescription;
bool _inNameSet;
};

//---------------------------------------------------------------------------

class AddPathDialog : public BaseAnnotationDialog
{
Q_OBJECT

public:
	AddPathDialog(osg::Group* root, osgEarth::MapNode* mapNode, const osgEarth::QtGui::ViewVector& views, osgEarth::Annotation::FeatureNode* path=0L, QWidget* parent = 0, Qt::WindowFlags f = 0);
	~AddPathDialog();
osgEarth::Annotation::AnnotationNode* getAnnotation() { return _pathNode.get(); }

void mapMouseClick(const osgEarth::GeoPoint& point, int button);
void addPoint(const osgEarth::GeoPoint& point,bool refresh=true);
virtual void mapMouseMove(const osgEarth::GeoPoint& point);

public slots:
void accept();
void reject();

private slots:
void onDrapeCheckStateChanged(int state);
void onLineColorButtonClicked();

protected:
void closeEvent(QCloseEvent* event);

void initialize();
void refreshFeatureNode();
void createPointDragger(int index, const osgEarth::GeoPoint& point);
void movePoint(int index, const osgEarth::GeoPoint& position);
void clearDisplay();
void resetValues();

osg::ref_ptr<osg::Group> _root;
osg::Group* _draggers;
osg::ref_ptr<AddPointsMouseHandler>  _guiHandler;
QCheckBox* _drapeCheckbox;
QPushButton* _lineColorButton;
osg::Vec4f _pathColor;
osg::ref_ptr<osgEarth::Symbology::LineString> _pathLine;
osg::ref_ptr<osgEarth::Features::Feature> _pathFeature;
osg::ref_ptr<osgEarth::Annotation::FeatureNode> _pathNode;
bool _editing;
osg::ref_ptr<osgEarth::Features::Feature> _inFeature;
std::string _inName;
std::string _inDescription;

osg::ref_ptr<osg::Group> textLabelGroup;
osg::ref_ptr<osgEarth::Annotation::LabelNode> textLabelNode;

};

//---------------------------------------------------------------------------

class AddPolygonDialog : public BaseAnnotationDialog
{
Q_OBJECT

public:
	AddPolygonDialog(osg::Group* root, osgEarth::MapNode* mapNode, const osgEarth::QtGui::ViewVector& views, osgEarth::Annotation::FeatureNode* polygon=0L, QWidget* parent = 0, Qt::WindowFlags f = 0);
	~AddPolygonDialog();
osgEarth::Annotation::AnnotationNode* getAnnotation() { return _polyNode.get(); }

void mapMouseClick(const osgEarth::GeoPoint& point, int button);
void mapMouseMove(const osgEarth::GeoPoint& point);
void addPoint(const osgEarth::GeoPoint& point);

public slots:
void accept();
void reject();

private slots:
void onDrapeCheckStateChanged(int state);
void onLineColorButtonClicked();
void onFillColorButtonClicked();

protected:
void closeEvent(QCloseEvent* event);

void initialize();
void refreshFeatureNode(bool geometryOnly=false);
void createPointDragger(int index, const osgEarth::GeoPoint& point);
void movePoint(int index, const osgEarth::GeoPoint& position);
void clearDisplay();
void resetValues();

osg::ref_ptr<osg::Group> _root;
osg::Group* _draggers;
osg::ref_ptr<AddPointsMouseHandler>  _guiHandler;
QCheckBox* _drapeCheckbox;
QPushButton* _lineColorButton;
QPushButton* _fillColorButton;
osg::Vec4f _pathColor;
osg::Vec4f _fillColor;
osg::Vec3d _mousePoint;
osg::ref_ptr<osgEarth::Symbology::Polygon> _polygon;
osg::ref_ptr<osgEarth::Features::Feature> _polyFeature;
osg::ref_ptr<osgEarth::Annotation::FeatureNode> _polyNode;
bool _editing;
osg::ref_ptr<osgEarth::Features::Feature> _inFeature;
std::string _inName;
std::string _inDescription;

osg::ref_ptr<osgEarth::Annotation::LabelNode> textLabelNode;

};

//---------------------------------------------------------------------------

struct AddEllipseMouseHandler;
class AddEllipseDialog : public BaseAnnotationDialog
{
Q_OBJECT

public:
	AddEllipseDialog(osg::Group* root, osgEarth::MapNode* mapNode, const osgEarth::QtGui::ViewVector& views, osgEarth::Annotation::EllipseNode* ellipse=0, QWidget* parent = 0, Qt::WindowFlags f = 0);
	~AddEllipseDialog();
osgEarth::Annotation::AnnotationNode* getAnnotation() { return _ellipseNode.get(); }

void addEllipse(const osgEarth::GeoPoint& position, const osgEarth::Linear& radiusMajor, const osgEarth::Linear& radiusMinor, const osgEarth::Angular& rotationAngle);

public slots:
void accept();
void reject();

private slots:
void onDrapeCheckStateChanged(int state);
void onLineColorButtonClicked();
void onFillColorButtonClicked();

protected:
void closeEvent(QCloseEvent* event);

void initialize();
void refreshFeatureNode(bool geometryOnly=false);
void clearDisplay();
void resetValues();
void removeGuiHandlers();

osg::ref_ptr<osg::Group> _root;
osg::ref_ptr<AddEllipseMouseHandler>  _guiHandler;
osg::ref_ptr<osgEarth::Annotation::EllipseNodeEditor> _editor;
QCheckBox* _drapeCheckbox;
QPushButton* _lineColorButton;
QPushButton* _fillColorButton;
osg::Vec4f _pathColor;
osg::Vec4f _fillColor;
osg::ref_ptr<osgEarth::Annotation::EllipseNode> _ellipseNode;
osgEarth::Symbology::Style _ellipseStyle;
bool _editing;
osgEarth::Symbology::Style _inStyle;
std::string _inName;
std::string _inDescription;
osgEarth::GeoPoint _inPosition;
osgEarth::Linear _inRadiusMajor;
osgEarth::Linear _inRadiusMinor;
osgEarth::Angular _inRotationAngle;

friend struct AddEllipseMouseHandler;
friend struct EllipseDraggerCallback;

osg::ref_ptr<osgEarth::Annotation::LabelNode> textLabelNode;
}; 

//---------------------------------------------------------------------------

struct AddPointsMouseHandler : public osgGA::GUIEventHandler
{
AddPointsMouseHandler(BaseAnnotationDialog* dialog, osgEarth::MapNode* mapNode, osg::Group* root, bool drawLead=true)
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


BaseAnnotationDialog* _dialog;
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

struct PointDraggerCallback : public osgEarth::Annotation::Dragger::PositionChangedCallback
{
PointDraggerCallback(int index, BaseAnnotationDialog* dialog)
    : _index(index), _dialog(dialog)
{
}

void onPositionChanged(const osgEarth::Annotation::Dragger* sender, const osgEarth::GeoPoint& position)
{
    _dialog->movePoint(_index, position);
}

int _index;
BaseAnnotationDialog* _dialog;
};

struct EllipseDraggerCallback : public osgEarth::Annotation::Dragger::PositionChangedCallback
{
	EllipseDraggerCallback(AddEllipseDialog* dialog)
		: _dialog(dialog)
	{
	}

	void onPositionChanged(const osgEarth::Annotation::Dragger* sender, const osgEarth::GeoPoint& position)
	{
		if (sender==_dialog->_editor->_majorDragger||sender==_dialog->_editor->_minorDragger)
		{
			_dialog->textLabelNode->setText(ByyGeoMath::getAnnotationLabelText(_dialog->_ellipseNode).at(0));
		}
		else
		{
			_dialog->textLabelNode->setPosition(position);
		}
	}

	AddEllipseDialog* _dialog;
};

//---------------------------------------------------------------------------

struct AddEllipseMouseHandler : public osgGA::GUIEventHandler
{
AddEllipseMouseHandler(AddEllipseDialog* dialog, osgEarth::MapNode* mapNode, osg::Group* root)
    : _dialog(dialog), _mapNode(mapNode), _root(root), _mouseDown(false), _capturing(true), _rotationAngle(0, osgEarth::Units::DEGREES)
{
    //Define a style for the ellipse outline
    _ellipseStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->color() = osgEarth::Color(osgEarth::Color::White, 0.75);
    _ellipseStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->stipple() = 0x0F0F;
    _ellipseStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 2.0;
    _ellipseStyle.getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
}

void setCapturing(bool capturing)
{
    _capturing = capturing;
}

bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa )
{
    if (!_capturing)
    return false;

    osgViewer::View* view = static_cast<osgViewer::View*>(aa.asView());

    if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
    {
    _mouseDown = true;
    _xDown = _xCurr = ea.getX();
    _yDown = _yCurr = ea.getY();
    }
    else if (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && _mouseDown)
    {
    _xCurr = ea.getX();
    _yCurr = ea.getY();
        
    updateValues(aa.asView());
    updateDisplay();

    return true;
    }
    else if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON && _mouseDown)
    {
    _xCurr = ea.getX();
    _yCurr = ea.getY();

    updateValues(aa.asView());

    _mouseDown = false;
    updateDisplay();

    if (_xCurr != _xDown && _yCurr != _yDown)  
        _dialog->addEllipse(_position, _radiusMajor, _radiusMinor, _rotationAngle);

    return true;
    }

    return false;
}

void updateValues(osg::View* view)
{
    float xMax = osg::maximum(_xDown, _xCurr);
    float xMin = osg::minimum(_xDown, _xCurr);
    float xMid = (xMax + xMin) / 2.0;
    float yMax = osg::maximum(_yDown, _yCurr);
    float yMin = osg::minimum(_yDown, _yCurr);
    float yMid = (yMax + yMin) / 2.0;

    osg::Vec3d world;
    if (_mapNode->getTerrain()->getWorldCoordsUnderMouse(view, xMid, yMid, world))
    {
        _position.fromWorld( _mapNode->getMapSRS(), world );
    //_mapNode->getMap()->worldPointToMapPoint(world, _position);
    }

    double xRad = 0.0;
    if (_mapNode->getTerrain()->getWorldCoordsUnderMouse(view, xMax, yMid, world))
    {
    osgEarth::GeoPoint mapPoint;
    mapPoint.fromWorld( _mapNode->getMapSRS(), world );
    //_mapNode->getMap()->worldPointToMapPoint(world, mapPoint);

    xRad = osgEarth::GeoMath::distance(_position.vec3d(), mapPoint.vec3d(), _position.getSRS());
    }

    double yRad = 0.0;
    if (_mapNode->getTerrain()->getWorldCoordsUnderMouse(view, xMid, yMax, world))
    {
    osgEarth::GeoPoint mapPoint;
    mapPoint.fromWorld( _mapNode->getMapSRS(), world );
    //_mapNode->getMap()->worldPointToMapPoint(world, mapPoint);

    yRad = osgEarth::GeoMath::distance(_position.vec3d(), mapPoint.vec3d(), _position.getSRS());
    }

    if (xRad > yRad)
    {
    _radiusMajor.set(xRad, osgEarth::Units::METERS);
    _radiusMinor.set(yRad, osgEarth::Units::METERS);
    _rotationAngle.set(90.0, osgEarth::Units::DEGREES);
    }
    else
    {
    _radiusMajor.set(yRad, osgEarth::Units::METERS);
    _radiusMinor.set(xRad, osgEarth::Units::METERS);
    _rotationAngle.set(0.0, osgEarth::Units::DEGREES);
    }
}

void updateDisplay()
{
    if (_ellipseNode.valid())
    _root->removeChild(_ellipseNode);

    if (_root.valid() && _mouseDown)
    {
    _ellipseNode = new osgEarth::Annotation::EllipseNode(_mapNode, _position, _radiusMajor, _radiusMinor, _rotationAngle, _ellipseStyle);
    _root->addChild(_ellipseNode);
    }

	_dialog->textLabelNode->setText(ByyGeoMath::getAnnotationLabelText(_ellipseNode).at(0));
	_dialog->textLabelNode->setPosition(_position);
}


AddEllipseDialog* _dialog;
osg::ref_ptr<osgEarth::MapNode>  _mapNode;
osg::ref_ptr<osg::Group> _root;
osg::ref_ptr<osgEarth::Annotation::EllipseNode> _ellipseNode;
osgEarth::Symbology::Style _ellipseStyle;
osgEarth::GeoPoint _position;
osgEarth::Linear _radiusMajor;
osgEarth::Linear _radiusMinor;
osgEarth::Angular _rotationAngle;

bool _capturing;
bool _mouseDown;
float _xDown, _yDown, _xCurr, _yCurr;
};


#endif // OSGEARTHQT_ANNOTATIONDIALOGS_H
