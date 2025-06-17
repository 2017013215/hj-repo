#include "stdafx.h"
#include "CustomDelegate.h"
#include <QComboBox>
#include <QFileDialog>

//////////////////////////////////////////////////////////////////////////

CustomPathLineEdit::CustomPathLineEdit(QWidget *parent /*= 0*/)
	:QLineEdit(parent)
{
	m_Btn = new QPushButton(this);
	m_Btn->setCursor(Qt::PointingHandCursor);
	m_Btn->setText("...");
	m_Btn->setFixedSize(22,22);
	m_Btn->setToolTip(QString::fromLocal8Bit("浏览"));
	QMargins margins = this->textMargins();
	this->setTextMargins(0, 0, m_Btn->width()+1, 0);
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addStretch();
	layout->addWidget(m_Btn);
	layout->addSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);
	connect(m_Btn,SIGNAL(clicked()),this, SLOT(on_Clicked()));
}

void CustomPathLineEdit::setPathFileSuffix(QString& suffix)
{
	m_Suffix = suffix;
}

void CustomPathLineEdit::on_Clicked()
{
	QString dir = "./";
	QString text = this->text();
	if (!text.isEmpty())
	{
		QFileInfo fi(text);
		if (fi.exists())
		{
			dir = fi.absolutePath();
		}
	}
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择规则文件"),
		dir,QString("(*.%1)").arg(m_Suffix));
	if (!fileName.isEmpty())
	{
		this->setText(fileName);
	}
}

//////////////////////////////////////////////////////////////////////////

CCustomDelegate::CCustomDelegate(QObject *parent, DELEGATE_TYPE type)
{
	m_Parent = dynamic_cast<QTreeWidget *>(parent);
	m_Type = type;
	m_Suffix = "xml";
}

CCustomDelegate::~CCustomDelegate()
{

}

QWidget * CCustomDelegate::createEditor( QWidget * parent,const QStyleOptionViewItem & option,const QModelIndex & index ) const
{
	QWidget * widget = NULL;
	if (index.column() == 2)
	{
		if (m_Type == ENUM_TYPE)
		{
			QStringList list = m_EnumMap.values();
			QComboBox * ComboBox = new QComboBox(parent);
			ComboBox->addItems(list);
			//QString value = m_Parent->itemAt(index.)
			//ComboBox->setCurrentIndex(list.indexOf(value));
			widget = ComboBox;
		}
		else if(m_Type == PATH_TYPE)
		{
			CustomPathLineEdit * lineEdit = new CustomPathLineEdit(parent);
			QString fileSuffix = m_Suffix;
			lineEdit->setPathFileSuffix(fileSuffix);
			widget = lineEdit;
		}
	}
	return widget;
}

void CCustomDelegate::setEditorData( QWidget * editor, const QModelIndex & index ) const
{
	if(index.column() == 2)
	{
		if (m_Type == ENUM_TYPE)
		{
			QComboBox * box = dynamic_cast<QComboBox*>(editor);
			box->setCurrentIndex(box->findText(index.data(Qt::DisplayRole).toString()));
		}
		else if(m_Type == PATH_TYPE)
		{
			CustomPathLineEdit * lineEdit = dynamic_cast<CustomPathLineEdit*>(editor);
			lineEdit->setText(index.data(Qt::DisplayRole).toString());
		}
	}
}

void CCustomDelegate::setModelData( QWidget * editor,QAbstractItemModel * model,const QModelIndex & index ) const
{
	if(index.column() == 2)
	{
		if (m_Type == ENUM_TYPE)
		{
			QComboBox * box = dynamic_cast<QComboBox*>(editor);
			model->setData(index,box->currentText(),Qt::DisplayRole);
		}
		else if (m_Type == PATH_TYPE)
		{
			CustomPathLineEdit * lineEdit = dynamic_cast<CustomPathLineEdit*>(editor);
			model->setData(index,lineEdit->text(),Qt::DisplayRole);
		}
	}
}

void CCustomDelegate::updateEditorGeometry( QWidget * editor,const QStyleOptionViewItem & option,const QModelIndex & index ) const
{
	editor->setGeometry(option.rect);
}

void CCustomDelegate::setEnumMap(QMap<int, QString>& map)
{
	m_EnumMap = map;
}

void CCustomDelegate::setPathFileSuffix(QString& suffix)
{
	if (!suffix.isEmpty())
	{
		m_Suffix = suffix;
	}
}
