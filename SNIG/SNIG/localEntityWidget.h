#ifndef LOCALENTITYWIDGET_H
#define LOCALENTITYWIDGET_H

#include <QWidget>
#include "ui_localEntityWidget.h"

class ByyLocalObjectManager;
class ByyEntityObject;

class ByyLocalEntityWidget : public QWidget
{
	Q_OBJECT

public:
	ByyLocalEntityWidget(ByyLocalObjectManager& objManager,QWidget *parent = 0);
	~ByyLocalEntityWidget();

	void setUserEntity(ByyEntityObject* ent);
	ByyEntityObject* userEntity();

public slots:
	void refresh();
	void onOkClicked();
	void onCancelClicked();

protected slots:
	void onFilter(const QString& text);

signals:
	void accept(ByyEntityObject*);

private:
	Ui::ByyLocalEntityWidget ui;
	ByyLocalObjectManager& myObjManager;

	ByyEntityObject* myUserEntity;
};

#endif // LOCALENTITYWIDGET_H
