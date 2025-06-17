#ifndef SIMESR_H
#define SIMESR_H

#include <QString>

class QDomElement;

class ByySimEsr
{
public:
	ByySimEsr();
	~ByySimEsr();

	void fromConfig(QDomElement& element);

	QString ItemClass;
	QString Tag;
	QString Description;
	QString OPDPara;
	QString SCNPara;
	
};

#endif // SIMESR_H
