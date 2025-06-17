#ifndef SANDBOXWIDGET_H
#define SANDBOXWIDGET_H

#include "sandboxAbsWidget.h"

class QPushButton;
class ByySceneController;
struct SandboxElement;

class ByySandboxWidget : public ByySandboxAbsWidget
{
	Q_OBJECT

public:
	ByySandboxWidget(ByySceneController *scene,QWidget *parent = 0);
	~ByySandboxWidget();

	virtual void init();
	virtual void release();

	virtual void writeToMapNode();

protected slots:
	void add2D();
	void placeFinish(int result);

	void move();
	void remove();
	void rename();

	void moveFinish(int result);

	void itemChanged();

	virtual void processScale(double value);

protected:
	virtual void processElementChanged();

signals:
	void modified();

protected:
	ByySceneController* d_scene;
	osg::ref_ptr<osg::Group> d_root;
};

#endif // SANDBOXWIDGET_H
