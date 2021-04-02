#include "xml_functions.h"
#include <QString>
#include <QStringList>

double getValue(const QDomNode& aValue, double aDefault)
{
    if (!aValue.isNull())
    {
        bool ok=false;
        double fValue = aValue.toElement().text().toDouble(&ok);
        if (ok) return fValue;
    }
    return aDefault;
}

int getValue(const QDomNode& aValue, int aDefault)
{
    if (!aValue.isNull())
    {
        bool ok=false;
        int fValue = aValue.toElement().text().toInt(&ok);
        if (ok) return fValue;
    }
    return aDefault;
}

QString getValue(const QDomNode& aValue, const QString& aDefault, bool attribute)
{
    if (!aValue.isNull())
    {
        QString fValue = attribute ? aValue.nodeValue() : aValue.toElement().text();
        return fValue;
    }
    return aDefault;
}

QColor getValue(const QDomNode& aValue, const QColor& aDefault)
{
    if (!aValue.isNull())
    {
        return QColor(aValue.toElement().text());
    }
    return aDefault;
}

QDomElement addElementS(QDomDocument& aDoc, QDomElement& aParent, const QString& aName, const QString& aValue)
{
    QDomElement tag = aDoc.createElement(aName);
    aParent.appendChild(tag);
    QDomText t = aDoc.createTextNode(aValue);
    tag.appendChild(t);
    return tag;
}

QDomNode findElement(const QDomNode& aParent, const QString & aName, bool aRecursively)
{
    QDomNode fReturn;
    QStringList fNameList = aName.split('/');
    QDomNode fParent = aParent;
    for (int i=0; i<fNameList.size(); ++i)
    {
        bool bFound;
        QString sattribute, svalue, ssearch(fNameList.at(i));
        int nAttribute  = (int)aName.indexOf("#"); // attribute is recognized by '#'
        if (nAttribute != -1)
        {
            int nEqual = (int)aName.indexOf("=");  // the value is separated by '='
            if (nEqual != -1)
            {
                sattribute = aName.mid(nAttribute+1, nEqual-nAttribute-1);
                svalue     = aName.mid(nEqual+1, aName.length()-nEqual);
                ssearch    = aName.mid(0, nAttribute);
                nAttribute = 1;                 // use also attribute
            }
            else
            {
                nAttribute = 0;
            }
        }
        else
        {
            nAttribute = 0;
        }

        bFound = false;
        for(QDomNode n = fParent.firstChild(); !bFound && !n.isNull(); n = n.nextSibling())
        {
            if (n.nodeName() == ssearch)          // found name
            {
                if (nAttribute == 1)            // use attribute?
                {                               // compare the attributes value
                    QDomNode fattr = n.attributes().namedItem(sattribute);
                    if (!fattr.isNull() && svalue == fattr.nodeValue())
                    {
                        fParent = n;
                        bFound = true;
                    }
                }
                else
                {
                    fParent = n;
                    bFound = true;
                }
            }
            else if (aRecursively)                               // search recursively
            {
                QDomNode node = findElement(n, aName);
                if (!node.isNull())
                {
                    fParent = node;
                    bFound = true;
                }
            }
        }

        if (bFound && i == fNameList.size()-1)
        {
            fReturn = fParent;
        }
    }
    return fReturn;
}
