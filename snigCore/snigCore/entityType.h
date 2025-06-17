#ifndef ENTITYTYPE_H
#define ENTITYTYPE_H

#include <QString>
#include <vector>

#include "snigcore_global.h"

class SNIGCORE_EXPORT ByyEntityType
{
public:
	ByyEntityType(int kind=-1,int domain=-1,
		int country=-1,int category=-1,
		int subCategory=-1,int specific=-1,int extra=-1);

	ByyEntityType(const char* str);

	~ByyEntityType();
	
	bool operator!=(const ByyEntityType& other)const;
	bool operator==(const ByyEntityType& other)const;

	QString string()const;

	bool valid()const;

	std::vector<int> toVector()const;

	int	kind;
	int	domain;
	int	country;
	int	category;
	int	subCategory;
	int	specific;
	int	extra;
};

#endif // ENTITYTYPE_H
