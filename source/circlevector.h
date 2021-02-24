#ifndef CIRCLEVECTOR_H
#define CIRCLEVECTOR_H

#include <vector>
#include "main.h"

// NOTE _MSVC_STL_VERSION >= 141 has certain security functions regarding iterators compared to end()

template<class T>
class circlevector : public std::vector<T>
{
public:
    /// matrix type
    typedef typename std::vector<T> circlevector_type;
    /// iterator
    typedef typename circlevector_type::iterator iterator;
    typedef typename circlevector_type::size_type size_type;
    /// iterator to iterate through all matrix elements
    class circlevector_iterator : public iterator
    {
    public:
        /// constructors
        circlevector_iterator() :_Begin(0), _End(0)  {}
        circlevector_iterator(iterator aPtr): iterator(aPtr) {}
        circlevector_iterator(iterator aPtr, iterator aBegin, iterator aEnd): iterator(aPtr)
        , _Begin(aBegin), _End(aEnd) {}
        circlevector_iterator(const circlevector_iterator&aIt): iterator(aIt)
        , _Begin(aIt._Begin), _End(aIt._End) {}

        /// postincrement
        circlevector_iterator operator++(int)
        {
            circlevector_iterator _Tmp = *this;
            ++(*this);
            return (_Tmp);
        }
        /// preincrement
        circlevector_iterator& operator++()
        {
#if _MSVC_STL_VERSION >= 141
            int fDistance =  std::distance(const_iterator(*this), _End);
            if (fDistance > 1)
            {
                iterator::operator++();
            }
            else
            {
                TRACE(Logger::trace, "operator++() overflow\n");
                *this =  circlevector_iterator(_Begin, _Begin, _End);;
            }
#else
            if (iterator::operator++() == _End)
            {
                *this =  circlevector_iterator(_Begin, _Begin, _End);;
            }
#endif
            return (*this);
        }

        circlevector_iterator& operator+=(int aStep)
        {
            *this = circlevector_iterator(*this + aStep, _Begin, _End);
            return *this;
        }

        circlevector_iterator operator+(int aStep)
        {
#if _MSVC_STL_VERSION >= 141
            iterator fIt = iterator(*this);
            if (aStep > 0)
            {
                int fDistance =  std::distance(fIt, _End);
                fIt = (aStep < fDistance) ? fIt + aStep:
                                            _Begin + (aStep - fDistance);
            }
            else
            {
                int fDistance =  std::distance(fIt, _Begin);
                fIt = (aStep >= fDistance) ? fIt + aStep:
                                            _Begin + (std::distance(_Begin, _End) + (aStep - fDistance));
            }
            return circlevector_iterator(fIt, _Begin, _End);
#else
            iterator fIt = iterator(*this) + aStep;
            if (fIt >= _End)
            {
                int fDistance =  std::distance(_End, fIt);
                return circlevector_iterator(_Begin + fDistance, _Begin, _End);
            }
            else if (fIt < _Begin)
            {
                int fDistance =  std::distance(_Begin, fIt);
                return circlevector_iterator(_End + fDistance, _Begin, _End);
            }
            else
            {
                return circlevector_iterator(fIt, _Begin, _End);
            }
#endif
        }
        /// postdecrement
        circlevector_iterator operator--(int)
        {
            circlevector_iterator _Tmp = *this;
            --(*this);
            return (_Tmp);
        }
        /// predecrement
        circlevector_iterator& operator--()
        {
#if _MSVC_STL_VERSION >= 141
            int fDistance =  std::distance(_Begin, const_iterator(*this));
            if (fDistance >= 1)
            {
                iterator::operator--();
            }
            else
            {
                TRACE(Logger::trace, "operator++() overflow\n");
                *this = circlevector_iterator(_End-1, _Begin, _End);
            }

#else
            if (iterator::operator--() < _Begin)
            {
                *this = _End-1;
            }
#endif
            return (*this);
        }
    private:
        iterator     _Begin, _End;
    };
    /// getter for iterator
    circlevector_iterator begin()
    {
        return circlevector_iterator(std::vector<T>::begin(), std::vector<T>::begin(), std::vector<T>::end());
    }
    circlevector_iterator begin(int anOffset)
    {
        int fSize = static_cast<int>(std::vector<T>::size());
        anOffset = anOffset % fSize;
        if (anOffset < 0)
        {
            anOffset += fSize;
        }
        return circlevector_iterator(std::vector<T>::begin()+anOffset, std::vector<T>::begin(), std::vector<T>::end());
    }
    circlevector_iterator end()
    {
        return circlevector_iterator(std::vector<T>::end());
    }

    /// const iterator
    typedef typename circlevector_type::const_iterator const_iterator;
    /// iterator to iterate through all matrix elements
    class const_circlevector_iterator : public const_iterator
    {
    public:
        /// constructors
        const_circlevector_iterator() :_Begin({}), _End({})  {}
        const_circlevector_iterator(const_iterator aPtr): const_iterator(aPtr) {}
        const_circlevector_iterator(const_iterator aPtr, const_iterator aBegin, const_iterator aEnd): const_iterator(aPtr)
        , _Begin(aBegin), _End(aEnd) {}

        /// postincrement
        const_circlevector_iterator operator++(int)
        {
            const_circlevector_iterator _Tmp = *this;
            ++(*this);
            return (_Tmp);
        }
        /// preincrement
        const_circlevector_iterator& operator++()
        {
#if _MSVC_STL_VERSION >= 141
            int fDistance =  std::distance(const_iterator(*this), _End);
            if (fDistance > 1)
            {
                const_iterator::operator++();
            }
            else
            {
                TRACE(Logger::trace, "operator++() overflow\n");
                *this = const_circlevector_iterator(_Begin, _Begin, _End);
            }
#else
            if (const_iterator::operator++() == _End)
            {
                TRACE(Logger::trace, "operator++() overflow\n");
                *this = const_circlevector_iterator(_Begin, _Begin, _End);
            }
#endif
            return (*this);
        }

        const_circlevector_iterator& operator+=(int aStep)
        {
            *this = const_circlevector_iterator(*this + aStep, _Begin, _End);
            return *this;
        }

        const_circlevector_iterator operator+(int aStep)
        {
#if _MSVC_STL_VERSION >= 141
            const_iterator fIt = const_iterator(*this);
            if (aStep > 0)
            {
                int fDistance =  std::distance(fIt, _End);
                fIt = (aStep < fDistance) ? fIt + aStep :
                                            _Begin + (aStep - fDistance);
            }
            else
            {
                int fDistance =  std::distance(fIt, _Begin);
                fIt = (aStep >= fDistance) ? fIt + aStep:
                                            _Begin + (std::distance(_Begin, _End) + (aStep - fDistance));
            }
            return const_circlevector_iterator(fIt, _Begin, _End);
#else
            const_iterator fIt = const_iterator(*this) + aStep;
            if (fIt >= _End)
            {
                int fDistance =  std::distance(_End, fIt);
                return const_circlevector_iterator(_Begin + fDistance, _Begin, _End);
            }
            else if (fIt < _Begin)
            {
                int fDistance =  std::distance(_Begin, fIt);
                return const_circlevector_iterator(_End + fDistance, _Begin, _End);
            }
            else
            {
                return const_circlevector_iterator(fIt, _Begin, _End);
            }
#endif
        }

        /// postdecrement
        const_circlevector_iterator operator--(int)
        {
            const_circlevector_iterator _Tmp = *this;
            --(*this);
            return (_Tmp);
        }
        /// predecrement
        const_circlevector_iterator& operator--()
        {
#if _MSVC_STL_VERSION >= 141
            int fDistance =  std::distance(_Begin, const_iterator(*this));
            if (fDistance >= 1)
            {
                const_iterator::operator--();
            }
            else
            {
                TRACE(Logger::trace, "operator++() overflow\n");
                *this = const_circlevector_iterator(_End-1, _Begin, _End);
            }

#else
            if (const_iterator::operator--() < _Begin)
            {
                TRACE(Logger::trace, "operator--() overflow\n");
                *this = const_circlevector_iterator(_End-1, _Begin, _End);
            }
#endif
            return (*this);
        }
    private:
        const_iterator     _Begin, _End;
    };
    /// getter for const_iterator
    const_circlevector_iterator begin() const
    {
        return const_circlevector_iterator(std::vector<T>::begin(), std::vector<T>::begin(), std::vector<T>::end());
    }
    const_circlevector_iterator begin(int anOffset) const
    {
        int fSize = static_cast<int>(std::vector<T>::size());
        anOffset = anOffset % fSize;
        if (anOffset < 0)
        {
            anOffset += fSize;
        }
        return const_circlevector_iterator(std::vector<T>::begin()+anOffset, std::vector<T>::begin(), std::vector<T>::end());
    }
    const_circlevector_iterator end() const
    {
        return const_circlevector_iterator(std::vector<T>::end());
    }
};
#endif // CIRCLEVECTOR_H
