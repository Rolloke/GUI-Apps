#ifndef COMBINECURVES_H
#define COMBINECURVES_H

#include <QObject>
#include <vector>

// todo regard probe settings for calculation

class CombineCurves : public QObject
{
    Q_OBJECT
public:
    enum eType { none, add, subtract, multiply, divide, lissajous };
    explicit CombineCurves(QObject *parent = 0);

    void setType(eType aType);
    void setChannel1(int aC1);
    void setChannel2(int aC2);
    void setMathChannelIndex(int aNoOfChannel);

    eType getType() const;
    int   getChannel1() const;
    int   getChannel2() const;
    int   getMathChannelIndex() const;

    bool combineCurves(const std::vector<double>& aC1, const std::vector<double>& aC2, std::vector<double>& aCout, int aStart, int aStop) const;

Q_SIGNALS:

public Q_SLOTS:

private:

    static double f_add(double a1, double a2);
    static double f_subtract(double a1, double a2);
    static double f_multiply(double a1, double a2);
    static double f_divide(double a1, double a2);


    eType mType;
    int   mChannel1;
    int   mChannel2;
    int   mMathChannelIndex;
};

#endif // COMBINECURVES_H
