#ifndef XML_FUNCTIONS_H
#define XML_FUNCTIONS_H

#include <QtXml/QDomDocument>
#include <QColor>

double  getValue(const QDomNode& aValue, double aDefault);
int     getValue(const QDomNode& aValue, int aDefault);
QString getValue(const QDomNode& aValue, const QString& aDefault, bool attribute=false);
QColor  getValue(const QDomNode& aValue, const QColor& aDefault);


QDomElement addElementS(QDomDocument&doc, QDomElement&aParent, const QString& aName, const QString& aValue);

template<typename Type>
QDomElement addElement(QDomDocument&doc, QDomElement&aParent, const QString& aName, Type aValue)
{
    QDomElement tag = doc.createElement(aName);
    aParent.appendChild(tag);
    QString value;
    value.setNum(aValue);
    QDomText t = doc.createTextNode(value);
    tag.appendChild(t);
    return tag;
}


QDomNode findElement(const QDomNode& aParent, const QString & sname, bool aRecursively=false);



#endif // XML_FUNCTIONS_H
