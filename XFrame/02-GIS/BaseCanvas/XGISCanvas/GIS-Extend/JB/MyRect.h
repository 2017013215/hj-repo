#ifndef MYRECT_H
#define MYRECT_H

#include <QRect>

class MyRect : public QRect
{
public:
	MyRect();
	~MyRect();
public:
	friend bool operator< (const MyRect& rect,const MyRect& other)
	{
		return rect != other;
	}
private:
	
};

#endif // MYRECT_H
