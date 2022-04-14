#ifndef _FUNCTIONS_H_INCLUDED_
#define _FUNCTIONS_H_INCLUDED_

#include <functional>
#include <math.h>

/// multibeam library
namespace multibeam
{
    /*!
    \brief helper to return a value between upper and lower boundary
    \param aValue value to be limited
    \param aLowerBoundary lower boundary
    \param aUpperBoundary upper boundary
    \return value limited by upper and lower boundary
    */
    template<class T>
    T range(const T& aValue,const T& aLowerBoundary,const T& aUpperBoundary)
    {
        if (aValue < aLowerBoundary) return aLowerBoundary;
        if (aValue > aUpperBoundary) return aUpperBoundary;
        return aValue;
    }

    /*!
    \brief IsBetween(..) helper to check wether a value is between two boundaries or equal to any boundary
    \param aValue value to be checked
    \param aLowerBoundary lower boundary
    \param aUpperBoundary upper boundary
    \return true if aValue is between the boundaries, false if not
    */
    template<class T>
    bool isBetween(const T& aValue,const T& aLowerBoundary,const T& aUpperBoundary)
    {
        if (aValue >= aLowerBoundary && aValue <= aUpperBoundary)
        {
            return (true);
        }
        else
        {
            return(false);
        }
    }

    /*!
    \brief calculates the polynome value of the given x value
    \param x x value (T)
    \param polynome vector with polynome coefficients (const vector<T>&)
    \example value = p[0]*x^0+p[1]*x^1+p[2]*x^2+...+p[n]*x^n
    \return polynome value
    */
    template<class T>
    T polynome_value_horner(T x, const std::vector<T>&polynome)
    {
        //info http://de.wikipedia.org/wiki/Horner-Schema
        int i = (int)(polynome.size());
        T value = polynome[--i];
        while (i > 0)
        {
            value *= x;
            value += polynome[--i];
        }
        return value;
    }

    /// unary functions to calculate each value in an iteration using transform(...)
    /// \example std::transform(src.begin(), src.end(), dest.begin(), multiply_value<T>(value));
    /// \brief multiplies each element by a factor
    template<class _Ty>
    struct multiply_value : public std::unary_function<_Ty, _Ty> {
        /// \brief constructor with argument initialization
        /// \param val factor for the elements
        /// \return product
        multiply_value(_Ty val) : _val(val) {};
        /// \brief operator ()
        /// \param _Left value of an element
        /// \return product
        _Ty operator()(const _Ty& _Left) const {
            return _Left * _val;
        }
    private:
        _Ty _val;
    };

    /// \brief adds a value to each element
    template<class _Ty>
    struct add_value : public std::unary_function<_Ty, _Ty> {
        /// \brief constructor with argument initialization
        /// \param val summand for the elements
        /// return sum of element and summand
        add_value(_Ty val) : _val(val) {};
        /// \brief operator ()
        /// \param _Left value of an element
        /// \return sum of element and summand
        _Ty operator()(const _Ty& _Left) const {
            return _Left + _val;
        }
    private:
        _Ty _val;
    };

    /// \brief calculates each element to the power of a value
    template<class _Ty>
    struct power : public std::unary_function<_Ty, _Ty> {
        /// \brief constructor with argument initialization
        /// \param val argument for the pow(..) function
        /// \return element value raised to the power of val
    	power(_Ty val) : _val(val) {};
        /// \brief operator ()
        /// \param _Left value of an element
        /// \return element value raised to the power of val
        _Ty operator()(const _Ty& _Left) const {
            return pow(_Left, _val);
        }
    private:
        _Ty _val;
    };


    /// \brief calculates the square of each element
    template<class _Ty>
    struct square: public std::unary_function<_Ty, _Ty>  {
        /// \brief constructor
        /// \return element value raised to the power of 2
        square() {};
        /// \brief operator ()
        /// \param _Left value of an element
        /// \return element value raised to the power of 2
        _Ty operator()(const _Ty& _Left) const {
            return _Left * _Left;
        }
    };

    /// \brief calculates the square root of each element
    template<class _Ty>
    struct square_root: public std::unary_function<_Ty, _Ty> {
        /// \brief constructor
        /// \return square root of the element
        square_root() {};
        /// \brief operator ()
        /// \param _Left value of an element
        /// \return square root of the element
        _Ty operator()(const _Ty& _Left) const
        {
            return sqrt(_Left);
        }
    };

    /// \brief calculates the exponential of each element
    template<class _Ty>
    struct exponent: public std::unary_function<_Ty, _Ty> {
        /// \brief constructor
        /// \return exponential value of the element
        exponent() {};
        /// \brief operator ()
        /// \param _Left value of an element
        /// \return exponential value of the element
        _Ty operator()(const _Ty& _Left) const
        {
            return exp(_Left);
        }
    };

    /// \brief rounds each element up to an integer value
    template<class _Ty>
    struct round_up: public std::unary_function<_Ty, _Ty> {
        /// \brief constructor
        /// \return element value rounded up to next integer value
        round_up() {};
        /// \brief operator ()
        /// \param _Left value of an element
        /// \return element value rounded up to next integer value
        _Ty operator()(const _Ty& _Left) const
        {
            return ceil(_Left);
        }
    };

    /// \brief rounds each element down to an integer value
    template<class _Ty>
    struct round_down: public std::unary_function<_Ty, _Ty> {
        /// \brief constructor
        /// \return element value rounded down to previous integer value
        round_down() {};
        /// \brief operator ()
        /// \param _Left value of an element
        /// \return element value rounded down to previous integer value
        _Ty operator()(const _Ty& _Left) const
        {
            return floor(_Left);
        }
    };
    /// \brief adds a given gradient multiplied by the index to succeeding elements
    template<class _Ty>
    struct gradient : public std::unary_function<_Ty, _Ty> {
        /// \brief constructor with argument initialization
        /// \param factor gradient to increase succeeding values of the elements
        /// \return index based gradient added to original value
    	gradient(_Ty factor=1) : _index(0), _factor(factor) {};
        /// \brief retrieves index based gradient
        /// \param _Left value of an element
        /// \return index based gradient added to original value
        _Ty operator()(const _Ty& _Left) {
            return _Left + _index++ * _factor;
        }
    private:
        _Ty _index;
        _Ty _factor;
    };

    /// classes to calculate a result from each value in an iteration
    /// \example double avg = std::for_each(values.begin(), values.end(), Average<double>());
    /// \brief calculates the average of all elements
    template <class Type>
    class Average {
    private:
       long num;      // The number of elements
       Type sum;      // The sum of the elements
    public:
       // Constructor initializes the values with zero
       Average ( ) : num ( 0 ) , sum ( 0 ) { }

        /// \brief operator () to calculate the sum and count the values
        /// \param elem value of an element
       void operator ( ) ( Type elem ) 
       {
          ++num;      // Increment the element count
          sum += elem;   // Add the value to the partial sum
       }

       // return Average
        /// \brief operator ()
        /// \return average of the elements
       operator Type ( ) {
          return  static_cast <double> (sum) / static_cast <double> (num);
       }
    };

    /// \brief calculates the sum of all elements
    template <class Type>
    class Sum {
    private:
       Type sum;      // The sum of the elements
    public:
       /// Constructor initializes the sum with zero
       Sum ( ) : sum ( 0 ) { }
       /// \brief operator () to calculate the sum
       /// \param elem value of an element
       void operator ( ) ( Type elem ) {
          sum += elem;   // Add the value to the partial sum
       }
       /// \brief operator () to return the sum
       operator Type ( ) { return  sum; }
    };

    /// \brief calculates the sum of all elements
    template <class Type>
    class SquareSum {
    private:
       Type sum;      // The sum of the elements
    public:
       /// Constructor initializes the sum with zero
       SquareSum ( ) : sum ( 0 ) { }
       /// \brief operator () to calculate the sum
       /// \param elem value of an element
       void operator ( ) ( Type elem ) {
          sum += (elem*elem);   // Add the value to the partial sum
       }
       /// \brief operator () to return the sum
       operator Type ( ) { return  sum; }
    };

    template <class Type>
    class Polynome {
    private:
       Type _Temp;      // The temporay value
       Type _Value;
       bool _Initialized;
    public:
       // Constructor initializes the members
       Polynome (Type aValue) : _Temp (0), _Value (aValue), _Initialized(false) { }

       /// \brief operator () to calculate the sum of the polynome coefficients
       /// \param elem value of an element
       void operator ( ) ( Type elem ) 
       {
           if (_Initialized)
           {
               _Temp *= _Value;
               _Temp += elem;
           }
           else
           {
               _Temp = elem;
               _Initialized = true;
           }
       }

       // return Polynome value
        /// \brief operator ()
        /// \return value of the polynome stored in the array
       operator Type ( ) {
          return _Temp;
       }
    };

};

/// helper to get the absolute value of a complex number
namespace std
{
    /*!
    \brief calculates the absolute value of the complex number
    \param val complex number
    \return norm of this complex number
    */
    template<class T>
    double fabs(std::complex<T> val)
    {
        return hypot(val.real(), val.imag());
    }
}

#endif
