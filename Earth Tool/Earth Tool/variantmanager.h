#ifndef VARIANTMANAGER_H
#define VARIANTMANAGER_H

#include "qtpropertybrowser-2.5_1/qtvariantproperty.h"

#include <QVector3D>

class ByyVariantManager : public QtVariantPropertyManager
{
	Q_OBJECT
public:
	ByyVariantManager(QObject *parent=0);
	~ByyVariantManager();

	virtual QVariant value(const QtProperty *property) const;
	virtual int valueType(int propertyType) const;
	virtual bool isPropertyTypeSupported(int propertyType) const;
	void setDecimals(QtProperty *property, int prec);
	int decimals(const QtProperty *property);

QString valueText(const QtProperty *property) const;

public slots:
	virtual void setValue(QtProperty *property, const QVariant &val);
protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
	private slots:
		void slotValueChanged(QtProperty *property, const QVariant &value);
		void slotPropertyDestroyed(QtProperty *property);
private:
	struct Data {
		Data() : decimals(2) {}
		QVariant value;
		int decimals;
		QtVariantProperty *x;
		QtVariantProperty *y;
		QtVariantProperty *z;
	};
	QMap<const QtProperty *, Data> propertyToData;
	QMap<const QtProperty *, QtProperty *> xToProperty;
	QMap<const QtProperty *, QtProperty *> yToProperty;
	QMap<const QtProperty *, QtProperty *> zToProperty;
	
};

#endif // VARIANTMANAGER_H
