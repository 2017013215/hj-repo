#ifndef SANDTABLEWIDGET_H
#define SANDTABLEWIDGET_H

#include <QWidget>
#include <QMenu>
#include "mappingQuery.h"
#include "ui_sandBoxConfigWidget.h"

class ByyArmyTypesEdit;

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
	SandboxElement _ele;
};

class ByySandBoxConfigWidget : public QWidget
{
	Q_OBJECT

public:
	ByySandBoxConfigWidget(QWidget *parent = 0);
	~ByySandBoxConfigWidget();

protected:
	bool eventFilter(QObject *obj, QEvent *event);

protected slots:
	void addClicked();

	void remove(ElementToolbutton *_btn);
	void elementEdit(ElementToolbutton *_btn);
	void elementTriggered(QAction *action);

	void addCategory();
	void removeCategory();
	void renameCategory();

	void update1();
	void updateKind();

private:
	void init();
	void add(SandboxElement& e);

	bool needShow(ElementToolbutton* btn);

signals:
	void modified();

private:
	Ui::ByySandBoxConfigWidget ui;

	QList<ElementToolbutton*> d_elementGroup;

	QMenu elementMenu;
	QAction* elementEditAction;
	QAction* elementRemoveAction;

	QAction* categoryAddAction; 
	QAction* categoryRemoveAction;
	QAction* categoryRenameAction;

	QToolButton* addElementButton;

	QString d_lastName;

};

#endif // SANDTABLEWIDGET_H
