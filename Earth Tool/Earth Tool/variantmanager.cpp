#include "stdafx.h"
#include "variantmanager.h"

ByyVariantManager::ByyVariantManager(QObject *parent)
	: QtVariantPropertyManager(parent)
{
	connect(this, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
		this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
	connect(this, SIGNAL(propertyDestroyed(QtProperty *)),
		this, SLOT(slotPropertyDestroyed(QtProperty *)));
}

ByyVariantManager::~ByyVariantManager()
{

}

QVariant ByyVariantManager::value( const QtProperty *property ) const
{
	if (propertyToData.contains(property))
		return propertyToData[property].value;
	return QtVariantPropertyManager::value(property);
}

int ByyVariantManager::valueType( int propertyType ) const
{
	if (propertyType == QVariant::Vector3D)
		return QVariant::Vector3D;
	return QtVariantPropertyManager::valueType(propertyType);
}

bool ByyVariantManager::isPropertyTypeSupported( int propertyType ) const
{
	if (propertyType == QVariant::Vector3D)
		return true;
	return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}

QString ByyVariantManager::valueText( const QtProperty *property ) const
{
	if (propertyToData.contains(property)) {
		QVariant v = propertyToData[property].value;
		QVector3D p = qVariantValue<QVector3D>(v);
		const int dec =  propertyToData[property].decimals;
		return QString(tr("(%1, %2, %3)").arg(QString::number(p.x(), 'f', dec))
			.arg(QString::number(p.y(), 'f', dec)).arg(QString::number(p.z(), 'f', dec)));
	}
	return QtVariantPropertyManager::valueText(property);
}

void ByyVariantManager::setValue( QtProperty *property, const QVariant &val )
{
	if (propertyToData.contains(property)) {
		if (val.type() != QVariant::Vector3D && !val.canConvert(QVariant::Vector3D))
			return;
		QVector3D p = qVariantValue<QVector3D>(val);
		Data d = propertyToData[property];
		d.value = p;
		if (d.x)
			d.x->setValue(p.x());
		if (d.y)
			d.y->setValue(p.y());
		if (d.z)
			d.z->setValue(p.z());
		propertyToData[property] = d;
		emit propertyChanged(property);
		emit valueChanged(property, p);
		return;
	}
	QtVariantPropertyManager::setValue(property, val);
}

void ByyVariantManager::initializeProperty( QtProperty *property )
{
	if (propertyType(property) == QVariant::Vector3D) {
		Data d;

		d.value = QVector3D(0, 0, 0);

		ByyVariantManager *that = (ByyVariantManager *)this;

		d.x = that->addProperty(QVariant::Double);
		d.x->setPropertyName(tr("Position X"));
		property->addSubProperty(d.x);
		xToProperty[d.x] = property;

		d.y = that->addProperty(QVariant::Double);
		d.y->setPropertyName(tr("Position Y"));
		property->addSubProperty(d.y);
		yToProperty[d.y] = property;

		d.z = that->addProperty(QVariant::Double);
		d.z->setPropertyName(tr("Position Z"));
		property->addSubProperty(d.z);
		zToProperty[d.z] = property;

		propertyToData[property] = d;
	}
	QtVariantPropertyManager::initializeProperty(property);
}

void ByyVariantManager::uninitializeProperty( QtProperty *property )
{
	if (propertyToData.contains(property)) {
		Data d = propertyToData[property];
		if (d.x)
			xToProperty.remove(d.x);
		if (d.y)
			yToProperty.remove(d.y);
		if (d.z)
			zToProperty.remove(d.z);
		propertyToData.remove(property);
	}
	QtVariantPropertyManager::uninitializeProperty(property);
}

void ByyVariantManager::slotValueChanged( QtProperty *property, const QVariant &value )
{
	if (xToProperty.contains(property)) {
		QtProperty *pointProperty = xToProperty[property];
		QVariant v = this->value(pointProperty);
		QVector3D p = qVariantValue<QVector3D>(v);
		p.setX(qVariantValue<double>(value));
		setValue(pointProperty, p);
	} else if (yToProperty.contains(property)) {
		QtProperty *pointProperty = yToProperty[property];
		QVariant v = this->value(pointProperty);
		QVector3D p = qVariantValue<QVector3D>(v);
		p.setY(qVariantValue<double>(value));
		setValue(pointProperty, p);
	}else if (zToProperty.contains(property)) {
		QtProperty *pointProperty = zToProperty[property];
		QVariant v = this->value(pointProperty);
		QVector3D p = qVariantValue<QVector3D>(v);
		p.setZ(qVariantValue<double>(value));
		setValue(pointProperty, p);
	}
}

void ByyVariantManager::slotPropertyDestroyed( QtProperty *property )
{
	if (xToProperty.contains(property)) {
		QtProperty *pointProperty = xToProperty[property];
		propertyToData[pointProperty].x = 0;
		xToProperty.remove(property);
	} else if (yToProperty.contains(property)) {
		QtProperty *pointProperty = yToProperty[property];
		propertyToData[pointProperty].y = 0;
		yToProperty.remove(property);
	}else if (zToProperty.contains(property)) {
		QtProperty *pointProperty = zToProperty[property];
		propertyToData[pointProperty].z = 0;
		zToProperty.remove(property);
	}
}

void ByyVariantManager::setDecimals( QtProperty *property, int prec )
{
	QMap<const QtProperty *, Data>::iterator it=propertyToData.find(property);
	if (it == propertyToData.end())
		return;

	Data data = it.value();

	if (prec > 13)
		prec = 13;
	else if (prec < 0)
		prec = 0;

	if (data.decimals == prec)
		return;

	data.decimals = prec;

	data.x->setAttribute(QLatin1String("decimals"),prec);
	data.y->setAttribute(QLatin1String("decimals"),prec);
	data.z->setAttribute(QLatin1String("decimals"),prec);

	it.value() = data;
}

int ByyVariantManager::decimals( const QtProperty *property )
{
	QMap<const QtProperty *, Data>::iterator it = propertyToData.find(property);
	if (it == propertyToData.end())
		return 0;
	else
		return it.value().decimals;
}
