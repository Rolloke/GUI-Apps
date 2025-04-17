#include "main.h"
#include "qgroupeditwithunit.h"
#include "data_exchange.h"

#include <limits>
#include <QChildEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QStyleOptionFocusRect>

QGroupEditWithUnit::QGroupEditWithUnit(QWidget *parent) : QGroupBox(parent)
  , mEdit(0)
  , mUnitLabel(0)
  , mMinValue(0)
  , mMaxValue(1)
  , mOldValue(0)
  , mDrawFrame(false)
  , mTextFlags(Qt::AlignLeft|Qt::AlignVCenter)
{
    disableRangeCheck();
}

void QGroupEditWithUnit::childEvent( QChildEvent * c )
{
    QLineEdit* fEdit  = qobject_cast<QLineEdit*>(c->child());
    if (fEdit)
    {
        mEdit = fEdit;
        QObject::connect(mEdit, SIGNAL(editingFinished()), this, SLOT(on_editingFinished()));
    }
    QLabel* fLabel  = qobject_cast<QLabel*>(c->child());
    if (fLabel)
    {
        mUnitLabel = fLabel;
        updateUnit();
    }
}

void QGroupEditWithUnit::wheelEvent( QWheelEvent* event )
{
    if (mPrefix.getPrefixChange() == UnitPrefix::wheel_change)
    {
        double fValue = value();
        mPrefix.changePrefix(event->angleDelta().x() > 0);
        updateUnit();
        value(fValue);
    }
}

void QGroupEditWithUnit::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

     QStyleOptionFocusRect option;
     option.initFrom(this);
     option.backgroundColor = palette().color(QPalette::Window);
     if (mDrawFrame)
     {
        style()->drawPrimitive(QStyle::PE_FrameGroupBox, &option, &painter, this);
     }
     style()->drawItemText(&painter, rect(), mTextFlags, palette(), true, title());
}

void QGroupEditWithUnit::setRange(double aMin, double aMax)
{
    mMinValue = aMin;
    mMaxValue = aMax;
}

void   QGroupEditWithUnit::disableRangeCheck()
{
    mMaxValue = std::numeric_limits<double>::max();
    mMinValue = -mMaxValue;
}

void QGroupEditWithUnit::setUnit(const QString& aUnit)
{
    mUnit = aUnit;
    updateUnit();
}

void QGroupEditWithUnit::checkPrefixChanged()
{
    double fValue = value();
    if (mPrefix.determinePrefix(fabs(fValue)))
    {
        updateUnit();
        value(fValue);
    }
}

void QGroupEditWithUnit::setPrefix(UnitPrefix::ePrefix aPrefix)
{
    if (mEdit)
    {
        if (aPrefix != mPrefix.getPrefix())
        {
            double fValue = value();
            mPrefix.setPrefix(aPrefix);
            updateUnit();
            value(fValue);
        }
    }
    else
    {
        mPrefix.setPrefix(aPrefix);
    }
}

void  QGroupEditWithUnit::setPrefixChange(UnitPrefix::eChangePrefix aPrefixChange)
{
    mPrefix.setPrefixChange(aPrefixChange);
}


void QGroupEditWithUnit::setValueModifier(boost::function<double (double)> &aModFunction)
{
    mModifyValue = aModFunction;
}

void  QGroupEditWithUnit::setDrawframe(bool aDraw)
{
    mDrawFrame = aDraw;
}

bool QGroupEditWithUnit::drawframe()
{
    return mDrawFrame;
}

void  QGroupEditWithUnit::setTextFlags(int aFlags)
{
    mTextFlags = aFlags;
}

double QGroupEditWithUnit::value()
{
    mOldValue = DataExchange::getValue(mEdit, mMinValue, mMaxValue, mOldValue, mPrefix.getPrefixMultiplicator());
    return mOldValue;
}

bool   QGroupEditWithUnit::value(double aValue)
{
    if (aValue >= mMinValue && aValue <= mMaxValue)
    {
        if (mPrefix.determinePrefix(fabs(aValue)))
        {
            updateUnit();
        }
        DataExchange::setValue(mEdit, aValue/mPrefix.getPrefixMultiplicator());
        mOldValue = aValue;
        return true;
    }
    return false;
}


void QGroupEditWithUnit::updateUnit()
{
    Q_EMIT send_prefix(mPrefix.getPrefix());
    if (mUnitLabel)
    {
        if (mUnit.size() == 0)
        {
            mUnit = mUnitLabel->text();
        }
        mUnitLabel->setText(mPrefix.getPrefixName()+mUnit);
    }
    else
    {
        TRACE(Logger::warning, "Unit label not set of %s", title().toStdString().c_str());
    }
}

void QGroupEditWithUnit::on_editingFinished()
{
    if (mModifyValue)
    {
        Q_EMIT send_value(mModifyValue(value()));
    }
}

void QGroupEditWithUnit::update_prefix(int aPrefix)
{
    mPrefix.setPrefix(static_cast<UnitPrefix::ePrefix>(aPrefix));
    updateUnit();
}

