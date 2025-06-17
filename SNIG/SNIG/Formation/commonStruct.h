#ifndef COMMONSTRUCT_H
#define COMMONSTRUCT_H

#include <QObject>

typedef enum UNITSTYPE
{
	WARSHIP,
	AIRPLANE,
	PANZER

}UNITSTYPE;


typedef struct MissileStu
{
	QTableWidgetItem* item;
	QString type;
	bool isLaunched;
}MissileStuInfo;




#endif // COMMONSTRUCT_H
