#ifndef INDEXNODE_H
#define INDEXNODE_H

#include <QWidget>
#include "ui_indexNode.h"

class IndexNode : public QWidget
{
	Q_OBJECT

public:
	IndexNode(QWidget *parent = 0);
	~IndexNode();

	void setTag(QString tag);

	void setValue(QString value);
	
	void setTitle(QString title);
	
	void setQZ(QString qz);
	
	void setQZTitle(QString qzt);

private:
	Ui::IndexNode ui;
};

#endif // INDEXNODE_H
