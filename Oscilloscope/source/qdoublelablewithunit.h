#ifndef QDOUBLELABLEWITHUNIT_H
#define QDOUBLELABLEWITHUNIT_H

#include <QLabel>
#include <QWidget>
#include <QString>
#include <unitprefix.h>

class QDoubleLableWithUnit : public QLabel
{
    Q_OBJECT
public:
    explicit QDoubleLableWithUnit(QWidget *parent = 0);

    void        setUnit(const QString& aUnit);
    void        initUnit();
    UnitPrefix& getPrefix();
    void        invalidate();
    bool        isInvalid();
    double      value();

Q_SIGNALS:
    void send_prefix(int aPrefix);

public Q_SLOTS:
    void value(double aValue) ;

private:
    QString    mUnit;
    UnitPrefix mPrefix;
    bool       mInvalid;
};

#endif // QDOUBLELABLEWITHUNIT_H
