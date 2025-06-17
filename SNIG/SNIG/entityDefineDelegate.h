#ifndef ENTITYDEFINEDELEGATE_H
#define ENTITYDEFINEDELEGATE_H

#include <QStyledItemDelegate>

class QListWidget;

class ByyEntityDefineDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	ByyEntityDefineDelegate(QListWidget *m,QObject *parent);
	~ByyEntityDefineDelegate();

	virtual void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;
	virtual QWidget *createEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

private:
	virtual void setTypeModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;
	virtual QWidget *createTypeEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const;
	virtual void setTypeEditorData(QWidget *editor, const QModelIndex &index) const;


	virtual void setModelModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;
	virtual QWidget *createModelEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const;
	virtual void setModelEditorData(QWidget *editor, const QModelIndex &index) const;

private:
	QListWidget *modelListWidget;
};

#endif // ENTITYDEFINEDELEGATE_H
