#ifndef ANIMATIONPATHEDITWIDGET_H
#define ANIMATIONPATHEDITWIDGET_H

#include <QWidget>
#include "ui_animationPathEditWidget.h"
#include "animationPath.h"

class ByyIG;

Q_DECLARE_METATYPE(osg::AnimationPath::ControlPoint);

class ByyAnimationPathEditWidget : public QWidget
{
	Q_OBJECT

public:
	ByyAnimationPathEditWidget(ByyIG& ig,QWidget * parent = 0, Qt::WindowFlags f = 0);
	~ByyAnimationPathEditWidget();

public slots:
	void newFile();
	void openFile();

	void clear();

	bool saveFile();
	bool saveAsFile();

	void add();
	void remove();

private slots:
	void cellClicked(int row,int column);

private:
	bool okToContinue();
	bool saveFile(const QString& filename);

	void addToTable(const MyControlPoint& cp);
	MyControlPoint getControlPointFromTable(int row);

protected:
	void closeEvent(QCloseEvent *e);

private:
	Ui::ByyAnimationPathEditWidget ui;

	bool needSave;

	QString currentFilename;

	ByyIG& myIG;
};

#endif // ANIMATIONPATHEDITWIDGET_H
