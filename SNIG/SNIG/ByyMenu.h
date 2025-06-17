#ifndef BYYMENU_H
#define BYYMENU_H

#include <QMenu>

class ByyApp;

class ByyMenu : public QMenu
{
public:
	ByyMenu(const QString& objectName,QWidget * parent = 0);
	virtual ~ByyMenu();
};

#endif // BYYMENU_H
