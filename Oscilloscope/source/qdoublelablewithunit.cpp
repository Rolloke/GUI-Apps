#include "qdoublelablewithunit.h"

#include <cmath>

QDoubleLableWithUnit::QDoubleLableWithUnit(QWidget *parent) : QLabel(parent)
  , mInvalid(false)
{
    initUnit();
}

void QDoubleLableWithUnit::setUnit(const QString& aUnit)
{
    mUnit = aUnit;
}

void QDoubleLableWithUnit::initUnit()
{
    mUnit = text();
}

UnitPrefix& QDoubleLableWithUnit::getPrefix()
{
    return mPrefix;
}

void QDoubleLableWithUnit::value(double aValue)
{
    if (mPrefix.determinePrefix(fabs(aValue)))
    {
        Q_EMIT send_prefix(mPrefix.getPrefix());
    }
    setText(QString::number(aValue/mPrefix.getPrefixMultiplicator(), 'g', 6) + " " + mPrefix.getPrefixName() + mUnit);
}

double QDoubleLableWithUnit::value()
{
    return atof(text().toStdString().c_str())*mPrefix.getPrefixMultiplicator();
}


void QDoubleLableWithUnit::invalidate()
{
    mInvalid = true;
}

bool QDoubleLableWithUnit::isInvalid()
{
    bool fInvalid = mInvalid;
    mInvalid = false;
    return fInvalid;
}
