#ifndef QGROUPEDITWITHUNIT_H
#define QGROUPEDITWITHUNIT_H

#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <boost/function.hpp>

#include "unitprefix.h"


class QGroupEditWithUnit : public QGroupBox
{
    Q_OBJECT
public:

    explicit QGroupEditWithUnit(QWidget *parent = 0);

    void   setRange(double aMin, double aMax);
    void   disableRangeCheck();
    void   setUnit(const QString& aUnit);
    void   setPrefix(UnitPrefix::ePrefix aPrefix);
    void   setPrefixChange(UnitPrefix::eChangePrefix aPrefixChange);
    void   setValueModifier(boost::function< double (double) >& aModFunction);
    void   checkPrefixChanged();
    void   setDrawframe(bool aDraw);
    bool   drawframe();
    void   setTextFlags(int aFlags);

    double value();

Q_SIGNALS:
    bool send_value(double aValue);
    void send_prefix(int aPrefix);

public Q_SLOTS:
    bool value(double aValue);
    void on_editingFinished();
    void update_prefix(int aPrefix);

private Q_SLOTS:

private:
    void childEvent ( QChildEvent* c );
    void wheelEvent ( QWheelEvent* event );
    void paintEvent(QPaintEvent *event);

    void        updateUnit();

    QLineEdit* mEdit;
    QLabel*    mUnitLabel;
    QString    mUnit;
    double     mMinValue;
    double     mMaxValue;
    double     mOldValue;
    UnitPrefix mPrefix;
    boost::function< double (double) > mModifyValue;
    bool       mDrawFrame;
    int        mTextFlags;
};

#endif // QGROUPEDITWITHUNIT_H
