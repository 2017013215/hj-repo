#ifndef DATABASE_H
#define DATABASE_H

#include <osgEarthSymbology/Style>
#include <osgEarthAnnotation/PlaceNode>
#include <osgViewer/Viewer>
#include <osg/Group>

#include "ui_sandboxWidget.h"
#include "mappingQuery.h"

class ByySandBoxPlacer;
class ByyIG;
class ByySandBoxComponent;

class ElementToolbutton :public QToolButton
{
public:
	ElementToolbutton(SandboxElement& e,int size=64,QWidget* p=NULL)
		:QToolButton(p),_ele(e)
	{
		setIconSize(QSize(size,size));
		setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		setElement(e);
	}

	void setElement(SandboxElement& e)
	{
		_ele=e;

		setText(e.name);
		setIcon(QIcon(e.iconFilename));
	}

	SandboxElement& getElement(){return _ele;}

	int forceType(){return _ele.category.forceType;}
	int category(){return _ele.category.id;}

	int kindType(){return _ele.category.kindType;}

private:
	SandboxElement& _ele;
};

class ByySandboxWidget : public QWidget
{
	Q_OBJECT

public:
	ByySandboxWidget(ByyIG& ig,QWidget *parent = 0, Qt::WFlags flags = 0);
	~ByySandboxWidget();

public slots:
	void init();
	void release();

protected slots:
	void update1();
	void updateKind();

	void renameItem();
	void deleteItem();
	void add2D();
	void placeFinished(int result);
	void move();
	void moveFinish(int result);
	void scaleIcon(double spinValue);

	void listCurrentRowChanged(int row);

	void itemClickedFrom3D(osg::Node* node);

	void itemDoubleClicked(QListWidgetItem *item);

protected:
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *e);

private:
	void initTree(QVector<ElementForceType> &forceTypes,QVector<SandboxCategory>& categories);
	void initHiddenWidget();

	bool needShow(SandboxElement& element);

signals:
	void placeStarted();

private:
	Ui::ByySandboxWidget ui;

	QList<ElementToolbutton*> d_eleButtons;

	SandboxElement *d_currentElement;

	ByyIG& myIG;
	ByySandBoxComponent *d_sandbox;


	QAction* moveItemAction;
	QAction* removeItemAction;
	QAction* renameItemAction;
};

#include "ui_sandboxForceDisplay.h"

class ByySandboxForceDisplay:public QWidget
{
	Q_OBJECT
public:
	ByySandboxForceDisplay(ElementForceType& forceType,QVector<SandboxCategory>& categories,ByySandBoxComponent *sandbox);

private slots:
	void forceTypeClicked(bool checked);

	void categoryClicked(bool checked);

private:
	Ui::ByySandboxForceDisplay ui;
	ByySandBoxComponent *d_sandbox;

	QMap<QCheckBox*,int> categoryCheckBoxMap;

	int forceTypeId;
};

class ByySandboxDisplayControlWidget :public QWidget
{
public:
	ByySandboxDisplayControlWidget(QVector<ElementForceType> &forceTypes,QVector<SandboxCategory>& categories,ByySandBoxComponent *sandbox,QWidget* parent=NULL);

};

#endif // DATABASE_H
