#ifndef UNITPREFIX_H
#define UNITPREFIX_H

class UnitPrefix
{
public:
    enum ePrefix
    {
        Invalid  = 1000,
        Percent  = 100,
        Exa      = 18,
        Peta     = 15,
        Tera     = 12,
        Giga     = 9,
        Mega     = 6,
        Kilo     = 3,
        Hecto    = 2,
        Deca     = 1,
        NoPrefix = 0,
        Deci     = -1,
        Centi    = -2,
        Milli    = -3,
        Micro    = -6,
        Nano     = -9,
        Pico     = -12,
        Femto    = -15,
        Atto     = -18,
        Underflow= -21
    };
    enum eChangePrefix
    {
        no_change,
        wheel_change,
        auto_change
    };

    UnitPrefix();

    void        setPrefix(ePrefix aPrefix);
    ePrefix     getPrefix() const;
    void        setPrefixChange(eChangePrefix);
    eChangePrefix getPrefixChange() const;
    bool        changePrefix(bool aUp);
    const char* getPrefixName() const;
    double      getPrefixMultiplicator(ePrefix aPrefix=Invalid) const;
    bool        determinePrefix(double aValue);

private:
    ePrefix    mPrefix;
    eChangePrefix mChangePrefix;
};

#endif // UNITPREFIX_H
