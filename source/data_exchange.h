#ifndef DATA_EXCHANGE_H
#define DATA_EXCHANGE_H
#include "main.h"

#include <QString>
#include <boost/lexical_cast.hpp>

namespace DataExchange
{
    template <typename Type, typename TElement>
    static void setValue(TElement* anElement, Type aValue)
    {
        anElement->setText(QString::number(aValue));
    }


    template <typename Type, typename TElement>
    Type getValue(TElement* anElement, Type aMin, Type aMax, Type aDefault = 0, Type aMultiplicator = 1)
    {
        Type fValue = aDefault;
        try
        {
             fValue = boost::lexical_cast<Type>(anElement->text().toStdString()) * aMultiplicator;
             if (fValue < aMin)
             {
                 fValue = aMin;
                 setValue(anElement, fValue);
             }
             if (fValue > aMax)
             {
                 fValue = aMax;
                 setValue(anElement, fValue);
             }
        }
        catch (boost::bad_lexical_cast& anException)
        {
            TRACE(Logger::error, "Error getValue %s\n", anException.what());
        }

        return fValue;
    }
}

#endif // DATA_EXCHANGE_H
