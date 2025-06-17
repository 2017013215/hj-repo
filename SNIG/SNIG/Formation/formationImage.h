#ifndef FORMATIONIMAGE_H
#define FORMATIONIMAGE_H

#include <QObject>
#include <QPixmap>

typedef enum EntityImageType
{
	AIRPLANE_YES = 0,
	AIRPLANE_NO,
	SHIP_YES,
	SHIP_NO,
	CENTERITEM,
}EntityImageType;

class formationImage : public QObject
{
	Q_OBJECT

public:
	~formationImage();

private:
	formationImage(QObject *parent = 0);

public:
	static formationImage* getInstance();

	QPixmap* getPixmap(EntityImageType type);

private:
	QMap<EntityImageType, QPixmap*> m_EnPixmap;
	
};

#endif // FORMATIONIMAGE_H
