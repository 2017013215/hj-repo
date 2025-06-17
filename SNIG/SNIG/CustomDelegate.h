#ifndef CUSTOMDELEGATE_H
#define CUSTOMDELEGATE_H

#include <QItemDelegate>
#include <QStringList>
#include <QMap>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>

//////////////////////////////////////////////////////////////////////////
// 带有浏览按键的文本编辑框
class CustomPathLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	CustomPathLineEdit(QWidget *parent = 0);

	void setPathFileSuffix(QString& suffix);
public slots:

	void on_Clicked();
private:
	QPushButton* m_Btn;
	QString m_Suffix;
};

//////////////////////////////////////////////////////////////////////////
enum DELEGATE_TYPE
{
	ENUM_TYPE,
	PATH_TYPE
};

class CCustomDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	CCustomDelegate(QObject *parent, DELEGATE_TYPE type);
	~CCustomDelegate();

	QWidget * createEditor(QWidget * parent,const QStyleOptionViewItem & option,
		const QModelIndex & index) const;
	void setEditorData(QWidget * editor,const QModelIndex & index) const;
	void setModelData(QWidget * editor,QAbstractItemModel * model,
		const QModelIndex & index) const;
	void updateEditorGeometry(QWidget * editor,
		const QStyleOptionViewItem & option,const QModelIndex & index) const;
	// 枚举代理
	void setEnumMap(QMap<int, QString>& map);
	// 路径代理
	void setPathFileSuffix(QString& suffix);
private:
	QTreeWidget* m_Parent;
	// 代理类型
	DELEGATE_TYPE m_Type;
	// 枚举代理
	QMap<int, QString> m_EnumMap;
	// 路径代理
	QString m_Suffix;
};
#endif