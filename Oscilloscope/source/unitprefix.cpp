#include "unitprefix.h"
#include <map>

UnitPrefix::UnitPrefix():
    mPrefix(NoPrefix)
  , mChangePrefix(no_change)
{
}

void UnitPrefix::setPrefix(ePrefix aPrefix)
{
    mPrefix = aPrefix;
}

UnitPrefix::ePrefix UnitPrefix::getPrefix() const
{
    return mPrefix;
}

void UnitPrefix::setPrefixChange(eChangePrefix aChangePrefix)
{
    mChangePrefix = aChangePrefix;
}

UnitPrefix::eChangePrefix UnitPrefix::getPrefixChange() const
{
    return mChangePrefix;
}


const char* UnitPrefix::getPrefixName() const
{
    switch (mPrefix)
    {
    case Percent:  return "%";
    case Exa:      return "E";
    case Peta:     return "P";
    case Tera:     return "T";
    case Giga:     return "G";
    case Mega:     return "M";
    case Kilo:     return "K";
    case Hecto:    return "H";
    case Deca:     return "D";
    case NoPrefix: return "";
    case Deci:     return "d";
    case Centi:    return "z";
    case Milli:    return "m";
    case Micro:    return "u";
    case Nano:     return "n";
    case Pico:     return "p";
    case Femto:    return "f";
    case Atto:     return "a";
    default:       return "";
    }

}

double UnitPrefix::getPrefixMultiplicator(UnitPrefix::ePrefix aPrefix) const
{
    if (aPrefix == Invalid)
    {
        aPrefix = mPrefix;
    }
    switch (aPrefix)
    {
    case Exa:      return    1.0e18;
    case Peta:     return    1.0e15;
    case Tera:     return    1.0e12;
    case Giga:     return    1.0e9;
    case Mega:     return    1.0e6;
    case Kilo:     return    1.0e3;
    case Hecto:    return  100.0;
    case Deca:     return   10.0;
    case NoPrefix: return    1.0;
    case Deci:     return    0.1;
    case Centi:    return    0.01;
    case Percent:  return    0.01;
    case Milli:    return    1.0e-3;
    case Micro:    return    1.0e-6;
    case Nano:     return    1.0e-9;
    case Pico:     return    1.0e-12;
    case Femto:    return    1.0e-15;
    case Atto:     return    1.0e-18;
    case Underflow:return    1.0e-21;

    default:       return    1.0;
    }
}

bool UnitPrefix::determinePrefix(double aValue)
{
#define ENTRY(PREFIX) { getPrefixMultiplicator(PREFIX), PREFIX }
    static std::map<double, ePrefix> fValueToPrefix =
    {
        ENTRY(Exa),
        ENTRY(Peta),
        ENTRY(Tera),
        ENTRY(Giga),
        ENTRY(Mega),
        ENTRY(Kilo),
        ENTRY(NoPrefix),
        ENTRY(Milli),
        ENTRY(Micro),
        ENTRY(Nano),
        ENTRY(Pico),
        ENTRY(Femto),
        ENTRY(Atto),
        ENTRY(Underflow)
    };
#undef ENTRY

    if (mChangePrefix == auto_change)
    {
        auto fFoundPrefix = fValueToPrefix.lower_bound(aValue);
        if (fFoundPrefix != fValueToPrefix.end())
        {
             if (fFoundPrefix != fValueToPrefix.begin()) --fFoundPrefix;
             if (fFoundPrefix->second == Underflow)
             {
                 mPrefix = NoPrefix;
                 return true;
             }
             if (fFoundPrefix->second != mPrefix)
             {
                 mPrefix = fFoundPrefix->second;
                 return true;
             }
        }
    }
    return false;
}

bool UnitPrefix::changePrefix(bool aUp)
{
    ePrefix fPrefix = mPrefix;
    switch (mPrefix)
    {
    case Exa:      fPrefix = aUp ? fPrefix : Peta;     break;
    case Peta:     fPrefix = aUp ? Exa     : Tera;     break;
    case Tera:     fPrefix = aUp ? Peta    : Giga;     break;
    case Giga:     fPrefix = aUp ? Tera    : Mega;     break;
    case Mega:     fPrefix = aUp ? Giga    : Kilo;     break;
    case Kilo:     fPrefix = aUp ? Mega    : NoPrefix; break;
    case NoPrefix: fPrefix = aUp ? Kilo    : Milli;    break;
    case Milli:    fPrefix = aUp ? NoPrefix: Micro;    break;
    case Micro:    fPrefix = aUp ? Milli   : Nano;     break;
    case Nano:     fPrefix = aUp ? Micro   : Pico;     break;
    case Pico:     fPrefix = aUp ? Nano    : Femto;    break;
    case Femto:    fPrefix = aUp ? Pico    : Atto;     break;
    case Atto:     fPrefix = aUp ? Femto   : fPrefix;  break;
    default: break;
    }
    bool fReturn = fPrefix != mPrefix;
    mPrefix = fPrefix;
    return fReturn;
}
