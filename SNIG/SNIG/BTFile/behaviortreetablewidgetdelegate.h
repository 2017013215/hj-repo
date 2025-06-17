#ifndef BEHAVIORTREETABLEWIDGETDELEGATE_H
#define BEHAVIORTREETABLEWIDGETDELEGATE_H

#include <QItemDelegate>
#include <QStringList>
#include <QMap>

class QTableWidget;

class BehaviorTreeTableWidgetDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	BehaviorTreeTableWidgetDelegate(QObject *parent);
	~BehaviorTreeTableWidgetDelegate();
	QWidget * createEditor(QWidget * parent,const QStyleOptionViewItem & option,
		const QModelIndex & index) const;
	void setEditorData(QWidget * editor,const QModelIndex & index) const;
	void setModelData(QWidget * editor,QAbstractItemModel * model,
		const QModelIndex & index) const;
	void updateEditorGeometry(QWidget * editor,
		const QStyleOptionViewItem & option,const QModelIndex & index) const;

	void setNameList(QStringList &list);
	void setMap(QMap<QString,QStringList> &map);

private:
	QStringList m_NameList;
	QMap<QString,QStringList> m_Map;
	QTableWidget *m_Parent;
};

#endif // BEHAVIORTREETABLEWIDGETDELEGATE_H
