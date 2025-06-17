#include "StdAfx.h"
#include "behaviortreetablewidgetdelegate.h"
#include <QGridLayout>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QStringList>
#include <QTimeEdit>
#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QTableWidget>

BehaviorTreeTableWidgetDelegate::BehaviorTreeTableWidgetDelegate(QObject *parent)
	: QItemDelegate(parent)
{
	m_Parent = dynamic_cast<QTableWidget *>(parent);
}

BehaviorTreeTableWidgetDelegate::~BehaviorTreeTableWidgetDelegate()
{

}

QWidget * BehaviorTreeTableWidgetDelegate::createEditor(QWidget * parent,const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QWidget * widget = NULL;
	if (index.column() == 1)
	{
		QString name = m_Parent->item(index.row(),0)->text();
		if (m_NameList.contains(name))
		{
			QStringList list = m_Map.value(name);
			QComboBox * ComboBox = new QComboBox(parent);
			ComboBox->addItems(list);
			QString value = m_Parent->item(index.row(),index.column())->text();
			ComboBox->setCurrentIndex(list.indexOf(value));
			widget = ComboBox;
		}
		else
		{
			QLineEdit * lineEdit = new QLineEdit(parent);
			widget = lineEdit;
		}
	}
	return widget;
}

void BehaviorTreeTableWidgetDelegate::setEditorData(QWidget * editor,const QModelIndex & index) const
{
	if (index.column() == 1)
	{
		QString name = m_Parent->item(index.row(),0)->text();
		if (m_NameList.contains(name))
		{
			QString text = index.model()->data(index,Qt::EditRole).toString();
			QComboBox * ComboBox = static_cast<QComboBox*>(editor);
			int index = ComboBox->findText(text);
			ComboBox->setCurrentIndex(index);
		}
		else
		{
			QString text = index.model()->data(index,Qt::EditRole).toString();
			QLineEdit * lineEdit = static_cast<QLineEdit*>(editor);
			lineEdit->setText(text);
		}
	}
}

void BehaviorTreeTableWidgetDelegate::setModelData(QWidget * editor,QAbstractItemModel * model, const QModelIndex & index) const
{
	if (index.column() == 1)
	{
		QString name = m_Parent->item(index.row(),0)->text();
		if (m_NameList.contains(name))
		{
			QComboBox * ComboBox = static_cast<QComboBox*>(editor);
			QString text = ComboBox->currentText();
			model->setData(index,text,Qt::EditRole);
		}
		else
		{
			QLineEdit * lineEdit = static_cast<QLineEdit*>(editor);
			QString text = lineEdit->text();
			model->setData(index,text,Qt::EditRole);
		}
	}
}

void BehaviorTreeTableWidgetDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option,const QModelIndex & index) const
{
	editor->setGeometry(option.rect);
}

void BehaviorTreeTableWidgetDelegate::setNameList(QStringList &list)
{
	m_NameList = list;
}

void BehaviorTreeTableWidgetDelegate::setMap(QMap<QString,QStringList> &map)
{
	m_Map = map;
}
