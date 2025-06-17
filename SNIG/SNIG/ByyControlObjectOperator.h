#ifndef BYYCONTROLOBJECTOPERATOR_H
#define BYYCONTROLOBJECTOPERATOR_H

#include <QObject>

class ByyApp;

class ByyControlObjectOperator : public QObject
{
	Q_OBJECT

public:
	~ByyControlObjectOperator();

	void setApp(ByyApp* app);

	static ByyControlObjectOperator& instance();

	void editVertex(ByyControlObject* obj);
	void editAttribute(ByyControlObject* obj);
	void remove(ByyControlObject* obj);

public slots:
	void showContextMenu();
	void processDoubleClicked();

protected slots:
	void onEditVertex();
	void onEditAttribute();
	void onRemove();

private:
	ByyControlObjectOperator(QObject *parent=0);
	ByyControlObject* getSelectionObject();

protected:
	ByyApp* myApp;

	QAction *myEditVertexAction;
	QAction *myEditAttributeAction;
	QAction *myRemoveAction;
};

#endif // BYYCONTROLOBJECTOPERATOR_H
