#include "entityType.h"
#include <QStringList>

ByyEntityType::ByyEntityType(int kind,int domain,
	int country,int category,
	int subCategory,int specific,int extra)
{
	this->kind=kind;
	this->domain=domain;
	this->country=country;
	this->category=category;
	this->subCategory=subCategory;
	this->specific=specific;
	this->extra=extra;
}

ByyEntityType::ByyEntityType( const char* str )
{
	sscanf(str,"%d:%d:%d:%d:%d:%d:%d",&kind,&domain,&country,&category,&subCategory,&specific,&extra);
}

ByyEntityType::~ByyEntityType()
{

}

bool ByyEntityType::operator!=( const ByyEntityType& other )const
{
	return other.kind!=kind||
		other.domain!=domain||
		other.country!=country||
		other.category!=category||
		other.subCategory!=subCategory||
		other.specific!=specific||
		other.extra!=extra;
}

bool ByyEntityType::operator==( const ByyEntityType& other ) const
{
	return other.kind==kind&&
		other.domain==domain&&
		other.country==country&&
		other.category==category&&
		other.subCategory==subCategory&&
		other.specific==specific&&
		other.extra==extra;
}

QString ByyEntityType::string() const
{
	return QString("%1:%2:%3:%4:%5:%6:%7").arg(kind).arg(domain).arg(country).arg(category).arg(subCategory).arg(specific).arg(extra);
}

bool ByyEntityType::valid() const
{
	return !(kind==-1&&domain==-1&&country==-1&&category==-1&&subCategory==-1&&specific==-1&&extra==-1);
}

std::vector<int> ByyEntityType::toVector() const
{
	std::vector<int> rel(7);

	rel[0]=kind;
	rel[1]=domain;
	rel[2]=country;
	rel[3]=category;
	rel[4]=subCategory;
	rel[5]=specific;
	rel[6]=extra;

	return rel;
}
