#include "stdafx.h"
#include "entityDefineDelegate.h"

ByyEntityDefineDelegate::ByyEntityDefineDelegate( QListWidget *m,QObject *parent ) : QStyledItemDelegate(parent)
{
	modelListWidget=m;
}

ByyEntityDefineDelegate::~ByyEntityDefineDelegate()
{

}

void ByyEntityDefineDelegate::setModelData( QWidget *editor,QAbstractItemModel *model,const QModelIndex &index ) const
{
	switch(index.column())
	{
	case 0:
		setTypeModelData(editor,model,index);
		break;
	case 1:
		setModelModelData(editor,model,index);
		break;
	default:
		QStyledItemDelegate::setModelData(editor,model,index);
	}	
}

QWidget * ByyEntityDefineDelegate::createEditor( QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index ) const
{
	switch(index.column())
	{
	case 0:
		return createTypeEditor(parent,option,index);
		break;
	case 1:
		return createModelEditor(parent,option,index);
		break;
	default:
		return QStyledItemDelegate::createEditor(parent,option,index);
	}
}

void ByyEntityDefineDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
	switch(index.column())
	{
	case 0:
		setTypeEditorData(editor,index);
		break;
	case 1:
		setModelEditorData(editor,index);
		break;
	default:
		QStyledItemDelegate::setEditorData(editor,index);
	}
}

void ByyEntityDefineDelegate::setTypeModelData( QWidget *editor,QAbstractItemModel *model,const QModelIndex &index ) const
{
	QLineEdit *lineEdit=dynamic_cast<QLineEdit*>(editor);

	if (lineEdit)
	{
		model->setData(index,lineEdit->text());
	}
}

QWidget * ByyEntityDefineDelegate::createTypeEditor( QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index ) const
{
	QLineEdit *lineEdit=new QLineEdit(parent);
	lineEdit->setValidator(new QRegExpValidator(QRegExp("^-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*:-?\\d+$*"),lineEdit));
	return lineEdit;
}

void ByyEntityDefineDelegate::setTypeEditorData( QWidget *editor, const QModelIndex &index ) const
{
	QLineEdit *lineEdit=dynamic_cast<QLineEdit*>(editor);
	lineEdit->setText(index.data().toString());
}

void ByyEntityDefineDelegate::setModelModelData( QWidget *editor,QAbstractItemModel *model,const QModelIndex &index ) const
{
	QComboBox *comboBox=qobject_cast<QComboBox*>(editor);
	model->setData(index,comboBox->currentText());
}

QWidget * ByyEntityDefineDelegate::createModelEditor( QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index ) const
{
	QComboBox *comboBox=new QComboBox(parent);
	for (int i=0;i!=modelListWidget->count();++i)
	{
		comboBox->addItem(modelListWidget->item(i)->text());
	}
	return comboBox;
}

void ByyEntityDefineDelegate::setModelEditorData( QWidget *editor, const QModelIndex &index ) const
{
	QComboBox *comboBox=qobject_cast<QComboBox*>(editor);
	QString model=index.data().toString();
	comboBox->setCurrentIndex(comboBox->findText(model));
	comboBox->showPopup();
}
