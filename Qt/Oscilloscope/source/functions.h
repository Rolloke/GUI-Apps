#ifndef _FUNCTIONS_H_INCLUDED_
#define _FUNCTIONS_H_INCLUDED_

#include <functional>
#include <math.h>
#include <vector>
#include <algorithm>
#include <stdint.h>

/// Algorithmics
namespace Algorithmics
{
    /// Helper
    namespace Helper
    {
        /// \brief IsBetween(..) helper to check wether a value is between or equal to two boundaries
        /// \param aValue value to be checked
        /// \param aLowerBoundary lower boundary
        /// \param aUpperBoundary upper boundary
        /// \return true if aValue is between the boundaries, false if not
        template<class T>
        bool isBetween(const T& aValue,const T& aLowerBoundary,const T& aUpperBoundary)
        {
            return (aValue >= aLowerBoundary && aValue <= aUpperBoundary);
        }

        /// \brief helper to return a value between upper and lower boundary
        /// \param aValue value to be limited
        /// \param aLowerBoundary lower boundary
        /// \param aUpperBoundary upper boundary
        /// \return value limited by upper and lower boundary
        template<class T>
        T range(const T& aValue,const T& aLowerBoundary,const T& aUpperBoundary)
        {
            if (aValue < aLowerBoundary) return aLowerBoundary;
            if (aValue > aUpperBoundary) return aUpperBoundary;
            return aValue;
        }

        /*!
        \brief applies a median filter to the array data
        \note input and output buffer can be the same buffer
        \param aIn input buffer to be filtered
        \param aStart start index for filter
        \param aEnd stop index for filter
        \param aHalfLength working buffer length for median is aHalfLength*2+1
        \param aOut output buffer
        \return
        */
        template<class T>
        bool median_filter(const std::vector<T>&aIn, int aStart, int aEnd, int aHalfLength, std::vector<T>&aOut)
        {
            int i=0, j=0, n=0, k=0, length=0, size(static_cast<int>(aIn.size()));

            length = 2*aHalfLength + 1;
            if (   aOut.size() == aIn.size()
                && isBetween(aStart, 0, size)
                && isBetween(aEnd, 0, size)
                && isBetween(length, 3, size))
            {
                std::vector<T> work(length), temp(length);

                work.assign(aIn.begin()+aStart, aIn.begin()+(aStart+length));
                for (i=aStart, j=aEnd-1, n=0; n<aHalfLength; ++i, ++n, --j)
                {
                    aOut[i] = aIn[i];
                    aOut[j] = aIn[j];
                }

                int fEnd = aEnd-aHalfLength;
                for(i=aStart+aHalfLength, j=length+aStart, k=0; i<fEnd; ++i, ++j)
                {
                    std::partial_sort_copy(work.begin(), work.end(), temp.begin(), temp.end());
                    if (j<aEnd)
                    {
                        work[k] = aIn[j];
                    }
                    aOut[i] = temp[aHalfLength];
                    if (++k == length) k = 0;
                }
                return true;
            }
            return false;
        }

        /*!
        \brief applies an in place floating avg filter to the input array data
        \note input and output buffer can be the same buffer
        \param aIn input buffer to be filtered
        \param aStart start index for filter
        \param aEnd stop index for filter
        \param aHalfLength working buffer length for floating average is aHalfLength*2+1
        \param aOut output buffer
        */
        template<class Tin, class Tout=Tin>
        bool float_avg_filter(const std::vector<Tin>&aIn, size_t aStart, size_t aEnd, size_t aHalfLength, std::vector<Tout>&aOut)
        {
            size_t fLength = 2 * aHalfLength + 1;
            if (   fLength <= aIn.size()
                && fLength <= aEnd - aStart
                && aEnd    <= aIn.size()
                && aStart  <  aIn.size())
            {
                if (aOut.size() < aIn.size())
                {
                    aOut.resize(aIn.size());
                }
                size_t fIndex1 = 0;
                size_t fIndex2 = 0;
                size_t fWorkIndex1=0;
                size_t fWorkIndex2=0;
                size_t fHalfIndex=0;
                Tout   fWorkStart=0;
                Tout   fWorkEnd=0;
                long double fInvLength = 0;

                //Border Case. Just take those values that we have into average filter
                for (fIndex1 = aStart, fIndex2 = aEnd - 1, fHalfIndex = 0; fHalfIndex < aHalfLength; ++fIndex1, ++fHalfIndex, --fIndex2)
                {
                    fWorkStart = 0;
                    fWorkEnd   = 0;
                    for (fWorkIndex1 = aStart, fWorkIndex2 = aEnd - 1; fWorkIndex1 < aStart + fHalfIndex + aHalfLength + 1; ++fWorkIndex1, --fWorkIndex2)
                    {
                        fWorkStart += aIn[fWorkIndex1];
                        fWorkEnd   += aIn[fWorkIndex2];
                    }
                    size_t fDivlength = fHalfIndex+aHalfLength+1;
                    fInvLength      = 1.0 / static_cast<long double>(fDivlength);
                    aOut[fIndex1]   = fWorkStart * fInvLength;
                    aOut[fIndex2]   = fWorkEnd   * fInvLength;
                }

                std::vector<Tout> fWork;
                fWork.resize(fLength);

                Tout   fSum=0;
                for(fIndex1=aStart, fIndex2=0; fIndex1<=aEnd && fIndex2<fLength; ++fIndex1, ++fIndex2) // calculate the sum for data
                {
                    fWork[fIndex2] = aIn[fIndex1];
                    fSum    += fWork[fIndex2];
                }
                fInvLength = 1.0 / static_cast<long double>(fLength);
                size_t fEnd = aEnd-aHalfLength-1;
                for(fIndex1=aStart+aHalfLength, fIndex2=fLength+aStart, fWorkIndex1=0; fIndex1<=fEnd; ++fIndex1, ++fIndex2)
                {
                    aOut[fIndex1] = fSum * fInvLength;
                    fSum         -= fWork[fWorkIndex1];
                    if (fIndex2 < aEnd)
                    {
                        fSum              += aIn[fIndex2];
                        fWork[fWorkIndex1] = aIn[fIndex2];
                    }
                    if (++fWorkIndex1 == fLength) fWorkIndex1 = 0;
                }
                return true;
            }
            return false;
        }

        /**
         * \brief retrieves the median value from given array values
         * \param aBegin start iterator of array
         * \param aEnd end iterator of array
         * \return median value
         */
        template<class T, class _InIt>
        T get_median_value(_InIt aBegin, _InIt aEnd)
        {
            uint32_t fSize = aEnd-aBegin;
            if (fSize > 0)
            {
                std::vector<T> sorted(fSize);
                std::partial_sort_copy(aBegin, aEnd, sorted.begin(), sorted.end());
                return sorted[sorted.size()/2];
            }
            return 0;
        }

        /// Polynome function dealing with polynome in form of a vector
        namespace Polynome
        {

            /// \brief calculates the polynome value of the given x value
            /// \note  http://de.wikipedia.org/wiki/Horner-Schema
            /// \param aX x value (T)
            /// \param aPolynome vector with polynome coefficients (const vector<T>&)
            /// \note  the polynome must not be zero size
            /// \example value = p[0]*x^0+p[1]*x^1+p[2]*x^2+...+p[n]*x^n
            /// \return polynome value
            /// calcPolynomeValueByHorner
            template<class T>
            T calcPolynomeValueByHorner(T aX, const std::vector<T>&aPolynome)
            {
                int fIndex = (int)(aPolynome.size()-1);
                if (fIndex > 0)
                {
                    T fValue = aPolynome[fIndex--];
                    while (fIndex >= 0)
                    {
                        fValue *= aX;
                        fValue += aPolynome[fIndex--];
                    }
                    return fValue;
                }
                return 0;
            }

            /// \brief calculates the derivative of the given polynome
            /// \param aPolynome vector with polynome coefficients (const vector<T>&)
            /// \param aDereived vector with derived polynome coefficients (vector<T>&)
            /// \example value = p[0]*x^0+p[1]*x^1+p[2]*x^2+...+p[n]*x^n
            /// \return function succeeded (true, false)
            template<class T>
            bool derivePolynome(const std::vector<T>& aPolynome, std::vector<T>& aDereived)
            {
                size_t fSize = aPolynome.size();
                if (fSize > 1)
                {
                    aDereived.resize(fSize-1);
                    for (size_t i=1; i<fSize; ++i)
                    {
                        aDereived[i-1] = aPolynome[i] * static_cast<T>(i);
                    }
                    return true;
                }
                return false;
            }
        }

        /// Transform binary functions to calculate each value in an iteration using transform(...)
        namespace Transform
        {
        /// \example std::transform(src.begin(), src.end(), dest.begin(), multiply_value<T>(value));

#if __cplusplus < 201103L
            /// \brief multiplies all elements of two vectors each by each, if left vector element is not zero
            template<class _Ty>
            struct multiply_if_not_zero : public std::binary_function<_Ty, _Ty, _Ty>
            {
                /// \brief operator ()
                /// \param _Left current value of left vector
                /// \param _Right current value of right vector
                /// \return product of left and right value of the vectors
                _Ty operator()(const _Ty& _Left, const _Ty& _Right) const
                {
                    return (_Left > 0) ? (_Left * _Right) : 0;
                }
            };

            /// \brief divides all elements of two vectors each by each, if right vector element is not zero
            /// \note zero elements in right vector return result zero
            template<class _Ty>
            struct divide_if_not_zero : public std::binary_function<_Ty, _Ty, _Ty>
            {
                /// \brief operator ()
                /// \param _Left current value of left vector
                /// \param _Right current value of right vector
                /// \return quotient of left by right value division of the vectors
                _Ty operator()(const _Ty& _Left, const _Ty& _Right) const
                {
                    return (_Right != 0) ? (_Left / _Right) : 0;
                }
            };

            /// \brief to set bit 0 of right vector element if left vector element is not zero
            template<class _Ty1, class _Ty2>
            struct set_bit_if_not_null : public std::binary_function<_Ty1, _Ty2, _Ty1>
            {
                /// \brief operator ()
                /// \param _Left current value of left vector
                /// \param _Right current value of right vector
                /// \return new bit pattern of right vector element
                _Ty2 operator()(const _Ty1& _Left, const _Ty2& _Right) const
                {
                    return (_Left > 0) ? (_Right|1): _Right;
                }
            };

            /////////////////////////////////////////////////////////////////////////////////////////
            /// unary functions to calculate each value in an iteration using transform(...)
            /// \example std::transform(src.begin(), src.end(), dest.begin(), multiply_value<T>(value));
            /// \brief shift bits right for each vector element
            template<class _Ty>
            struct shift_left : public std::unary_function<_Ty, _Ty>
            {
                /// \brief operator ()
                /// \param _Left current value of left vector
                /// \return bit shift right by 1
                _Ty operator()(const _Ty& _Left) const
                {
                    return _Left << 1;
                }
            };

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

            /// \brief calculates the sine of each element
            template<class _Ty>
            struct sine: public std::unary_function<_Ty, _Ty> {
                /// \brief constructor
                /// \return square root of the element
                sine() {};
                /// \brief operator ()
                /// \param _Left value of an element
                /// \return square root of the element
                _Ty operator()(const _Ty& _Left) const
                {
                    return sin(_Left);
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

            /// \brief compares an array element with a given value
            template<class _Ty>
            struct value_is : public std::unary_function<_Ty, _Ty>
            {
                /// \brief constructor with argument initialization
                /// \param aCmp value to compare with
                value_is(_Ty aCmp) : mCmp(aCmp) {}
                /// \brief retrieves comparison of given value with element value
                /// \param _Left value of an element
                /// \return (true, false)
                bool operator()(const _Ty& _Left) const
                {
                    return _Left == mCmp;
                }
                _Ty mCmp;
            };
#endif
            /// \brief adds a given gradient multiplied by the index to succeeding elements
            template<class _Ty>
            struct gradient //: public std::unary_function<_Ty, _Ty>
            {
                /// \brief constructor with argument initialization
                /// \param factor gradient to increase succeeding values of the elements
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

        } // Transform
        /// ForEach classes to calculate a result from each value in an iteration using for_each
        namespace ForEach
        {
            /// \example double avg = std::for_each(values.begin(), values.end(), Average<double>());

            /// \brief calculates the average of all elements
            template <class Type>
            class Average {
            private:
               int32_t num;      // The number of elements
               Type sum;      // The sum of the elements
            public:
               /// Constructor initializes the values with zero
               Average ( ) : num ( 0 ) , sum ( 0 ) { }

                /// \brief operator () to calculate the sum and count the values
                /// \param elem value of an element
               void operator ( ) ( Type elem )
               {
                  ++num;      // Increment the element count
                  sum += elem;   // Add the value to the partial sum
               }

               /// \brief operator ()
               /// \return average of the elements
               operator Type ( ) {
                  return  static_cast <Type> (sum) / static_cast <Type> (num);
               }
            };

            template <class Type>
            class Median {
            private:
                std::vector<Type> mValues; // temporary vector
            public:
                /// Constructor initializes the array size
                /// \param [aReservedSize] reserves memory space for the vector
                Median (uint32_t aReservedSize = 1 )  {
                    mValues.reserve(aReservedSize);
                }

               /// \brief operator () to calculate the sum and count the values
               /// \param elem value of an element
               void operator ( ) ( Type elem )
               {
                   mValues.push_back(elem);   // Add the value
               }

                /// \brief operator ()
                /// \return median of the elements
               operator Type ( ) {
                   uint32_t fSize = mValues.size();
                   if (fSize) {
                       std::sort(mValues.begin(), mValues.end());
                       return  mValues[fSize/2];
                   }
                   else {
                       return 0;
                   }
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

            /// \brief counts wheather a single bit is set in all elements
            template <class Type>
            class IsBitSet {
            private:
               Type testbit;    // bit value for testing
               uint32_t num;      // set bits count
            public:
                /// Constructor initializes the values
                /// \param aBit bit number for the test
                IsBitSet(int aBit): testbit(1<<aBit), num(0) { }
                /// \brief operator () to count the bits set
                /// \param elem value of an element
                void operator ( ) ( Type elem )
                {
                    if (elem & testbit) ++num;  // Increment the bit count
                }
                /// \brief operator ()
                /// \return number of bits set
                operator uint32_t( ) {
                    return  num;
                }
            };
        } // ForEach

    } // namespace Helper

} // namespace Algorithmics

/// helper to get the absolute value of a complex number
namespace std
{
    /// \brief calculates the absolute value of the complex number
    /// \param val complex number
    /// \return norm of this complex number
    template<class T>
    double fabs(std::complex<T> val)
    {
        return hypot(val.real(), val.imag());
    }
}

/*!
\brief prints the vector content
\param os output stream (ostream&)
\param v vector (matrix<T>&)
\return output stream
*/
template<class T>
std::ostream& operator << (std::ostream& os, const std::vector<T> &v)
{
    uint32_t i, size = v.size();
    os << "[";
    for (i=0; i<size;i++)
    {
        os << v[i];
        if (i < size-1) os << ", ";
    }
    os << "]";
    return (os);
}

#endif
