#include "qprogressbarfloat.h"
#include <cmath>

QProgressBarFloat::QProgressBarFloat()
{

}

QString QProgressBarFloat::text() const
{
    if (m_generate_text)
    {
        return tr("%1 %2").arg(QProgressBarFloat::value()).arg(m_unit);
    }
    return QProgressBar::text();
}

void QProgressBarFloat::setScale(double scale)
{
    m_scale = scale;
    m_generate_text = (m_scale != 1.0);
}

double QProgressBarFloat::scale() const
{
    return m_scale;
}

void QProgressBarFloat::setRange(double min, double max)
{
    QProgressBar::setRange(std::lround(min / m_scale), std::lround(max / m_scale));
}

void   QProgressBarFloat::setMinimum(double minimum)
{
    QProgressBar::setMinimum(std::lround(minimum / m_scale));
}

double QProgressBarFloat::minimum() const
{
    return QProgressBar::maximum() * m_scale;
}

void   QProgressBarFloat::setMaximum(double maximum)
{
    QProgressBar::setMaximum(std::lround(maximum / m_scale));
}

double QProgressBarFloat::maximum() const
{
    return QProgressBar::maximum() * m_scale;
}

void QProgressBarFloat::setValue(double value)
{
    QProgressBar::setValue(std::lround(value / m_scale));
}

double QProgressBarFloat::value() const
{
    return QProgressBar::value() * m_scale;
}

void QProgressBarFloat::setUnit(const QString &unit)
{
    m_unit = unit;
    m_generate_text = (m_unit.size() > 0);
}
