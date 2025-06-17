#ifndef JBGROUPREADER_H
#define JBGROUPREADER_H
#include <QByteArray>
#include <QDomElement>

class JBGroupReader
{
public:
    JBGroupReader();
    virtual QByteArray readGroup(QDomElement& elem) = 0;
};

class ArgReader : public JBGroupReader
{
    virtual QByteArray readGroup(QDomElement& elem);
};

class TextReader :public JBGroupReader
{
    virtual QByteArray readGroup(QDomElement& elem);
};

class RectReader :public JBGroupReader
{
    virtual QByteArray readGroup(QDomElement& elem);
};

class PolygonReader :public JBGroupReader
{
     virtual QByteArray readGroup(QDomElement& elem);
};

class LineReader :public  JBGroupReader
{
     virtual QByteArray readGroup(QDomElement& elem);
};

class EllipseReader : public JBGroupReader
{
    virtual QByteArray readGroup(QDomElement& elem);
};

#endif // JBGROUPREADER_H
