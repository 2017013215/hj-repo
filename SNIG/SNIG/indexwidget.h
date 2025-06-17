#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include <QDialog>
#include "ui_indexwidget.h"

class IndexSubWidget;
class IndexWidget : public QDialog
{
	Q_OBJECT

public:
	IndexWidget(QWidget *parent = 0);
	~IndexWidget();

	static IndexWidget* instance();
	
	void calcAndShow();
	
private:
	void initIndexTree();

private:
	Ui::IndexWidget ui;
	
private:
	IndexSubWidget* m_pIndexSubWidget;
};

#endif // INDEXWIDGET_H
