#include "entityDefineDelegate.h"
#include "mappingQuery.h"

#include <QLineEdit>
#include <QComboBox>
#include "junbiaoIdEditWidget.h"

ByyEntityDefineDelegate::ByyEntityDefineDelegate( QObject *parent ) : QStyledItemDelegate(parent)
{
	
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
	case 2:
		setJunBiaoModelData(editor,model,index);
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
	case 2:
		return createJunBiaoEditor(parent,option,index);
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
	case 2:
		setJunBiaoEditorData(editor,index);
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
	model->setData(index,comboBox->itemData(comboBox->currentIndex()).toInt(),Qt::UserRole);
}

QWidget * ByyEntityDefineDelegate::createModelEditor( QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index ) const
{
	QComboBox *comboBox=new QComboBox(parent);
	QVector<ModelDefine> modelDefines=ByyMappingQuery::instance()->getModelDefines();

	for (int i=0;i!=modelDefines.size();++i)
	{
		comboBox->addItem(modelDefines[i].name,modelDefines[i].id);
	}
	comboBox->model()->sort(0);
	return comboBox;
}

void ByyEntityDefineDelegate::setModelEditorData( QWidget *editor, const QModelIndex &index ) const
{
	QComboBox *comboBox=qobject_cast<QComboBox*>(editor);
	QString model=index.data().toString();
	comboBox->setCurrentIndex(comboBox->findText(model));
	comboBox->showPopup();
}

void ByyEntityDefineDelegate::setJunBiaoModelData( QWidget *editor,QAbstractItemModel *model,const QModelIndex &index ) const
{
	ByyJunbiaoIdEditWidget *editWidget=qobject_cast<ByyJunbiaoIdEditWidget*>(editor);
	
	int libId=editWidget->libId();
	int markId=editWidget->markId();

	model->setData(index,editWidget->string());
	model->setData(index,libId,Qt::UserRole);
	model->setData(index,markId,Qt::UserRole+1);
}

QWidget * ByyEntityDefineDelegate::createJunBiaoEditor( QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index ) const
{
	ByyJunbiaoIdEditWidget *editWidget=new ByyJunbiaoIdEditWidget(parent);

	return editWidget;
}

void ByyEntityDefineDelegate::setJunBiaoEditorData( QWidget *editor, const QModelIndex &index ) const
{
	ByyJunbiaoIdEditWidget *editWidget=qobject_cast<ByyJunbiaoIdEditWidget*>(editor);

	editWidget->setId(index.data(Qt::UserRole).toInt(),index.data(Qt::UserRole+1).toInt());
}
