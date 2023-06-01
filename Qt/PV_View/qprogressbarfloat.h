#ifndef QPROGRESSBARFLOAT_H
#define QPROGRESSBARFLOAT_H

#include <QProgressBar>

class QProgressBarFloat : public QProgressBar
{
public:
    QProgressBarFloat();

    double scale() const;
    void   setScale(double scale);

    void   setRange(double min, double max);
    void   setMinimum(double minimum);
    double minimum() const;
    void   setMaximum(double maximum);
    double maximum() const;
    void   setUnit(const QString& unit);

    double value() const;
    void   setValue(double value);

private:
    virtual QString text() const;

    double  m_scale = 1.0;
    QString m_unit;
    bool    m_generate_text = false;

};

#endif // QPROGRESSBARFLOAT_H
