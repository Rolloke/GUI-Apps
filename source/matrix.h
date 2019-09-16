#ifndef _MATRIX_H_INCLUDED_
#define _MATRIX_H_INCLUDED_

#include <vector>
#include <complex>
#include <limits>
#include <algorithm>
#include <functional>
#include <stdexcept>

#include "functions.h"

/// Algorithmics
namespace Algorithmics
{
    /// VectorX
    /*!
     <HR>
     <h1>Template class vectorX derived from vector with 2D and 3D vector functions</h1>
     <HR>
     This class contains constructors to create 2D an 3D vectors and functions to work
     with these vectors.
     <UL>
     <LI><b>absolute_value()</b>: calculates the amount of this vector</LI>
     <LI><b>square_absolute_value()</b>: calculates the square amount of this vector</LI>
     <LI><b>norm()</b>: normalizes this vector</LI>
     <LI><b>sin()</b>: calculates the sine of the angle between two vectors</LI>
     <LI><b>cos()</b>: calculates the cosine of the angle between two vectors</LI>
     <LI><b>tan()</b>: calculates the tangens of the angle between two vectors</LI>
     <LI><b>spat()</b>: calculates the volume of the parallel epiped consisting of 3 3D vectors</LI>
     <LI><b>cross_product()</b>: calculates the cross product of two vectors</LI>
     <LI><b>scalar_product()</b>: calculates the scalar product of two vectors</LI>
     <LI><b>operator+()</b>: adds 2 vectors element by element</LI>
     <LI><b>operator+()</b>: subtracts 2 vectors element by element</LI>
     <LI><b>operator()+</b>: adds a value to each element of a vector</LI>
     <LI><b>operator()-</b>: subtracts a value from each element of a vector</LI>
     </UL>
    */
    namespace VectorX
    {
        /// class implementing a special vector
        template<class T>
        class vectorND : public std::vector<T>
        {
        public:
            /// vectorX iterator
            typedef typename std::vector<T>::iterator iterator;
            /// vectorX const iterator
            typedef typename std::vector<T>::const_iterator const_iterator;
            /// dummy vector with no size as default return value
            static vectorND<T> dummy()
            {
                static vectorND<T> dummy;
                return dummy;
            }
            vectorND() {}

            vectorND(const std::vector<T>& aV)
            {
                vectorND<T> &v = *this;
                v.assign(aV.begin(), aV.end());
            }
            /*!
            \brief constructor with arguments
            \param size new size of the vector
            \param value initial value for the elements
            */
            vectorND(size_t size, const T& value)
            {
                this->resize(size, value);
            }

            /*!
            \brief constructor with arguments for 3D vector
            \param x x component
            \param y y component
            \param z z component
            */
            vectorND(const T& x, const T& y, const T& z)
            {
                vectorND<T> &v = *this;
                v.resize(3);
                v[0] = x;
                v[1] = y;
                v[2] = z;
            }


            /*!
            \brief constructor with arguments for 2D vector
            \param x x component
            \param y y component
            \param VectorX2d discriminate 2D vector from other constructor
            */
            vectorND(const T& x, const T& y)
            {
                vectorND<T> &v = *this;
                v.resize(2);
                v[0] = x;
                v[1] = y;
            }

            /*!
            \brief calculates the sqare amount of the vector
            \return square absolute value
            */
            T square_absolute_value() const
            {
                T val = 0;
                const_iterator it = this->begin();
                for ( ; it!=this->end(); ++it)
                {
                    val += ((*it) * (*it));
                }
                return val;
            }

            /*!
            \brief calculates the amount of the vector
            \return absolute value
            */
            T absolute_value() const
            {
                return sqrt(square_absolute_value());
            }

            /*!
             \brief normalizes the vector to a unit vector
           */
            void norm()
            {
                T val = absolute_value();
                if (val > std::numeric_limits<T>::epsilon())
                {
                    val = 1 / val;
                    iterator it = this->begin();
                    for ( ; it!=this->end(); ++it)
                    {
                        (*it) *= val;
                    }
                }
                else
                {
                    throw std::domain_error("vectorX::norm() failed");
                }
            }

            /*!
            \brief calculates the cross product of 2 3D vectors and saves result in calling object
            \param v1 vector 1 (vectorX&)
            \param v2 vector 2 (vectorX&)
            */
            void cross_product(const vectorND& v1, const vectorND& v2)
            {
                size_t n = v1.size();
                if (n == 3 && n == v2.size())
                {
                    vectorND<T> &r = *this;
                    r.resize(n);
                    r[0] = v1[1]*v2[2] - v1[2]*v2[1];
                    r[1] = v1[2]*v2[0] - v1[0]*v2[2];
                    r[2] = v1[0]*v2[1] - v1[1]*v2[0];
                }
                else
                {
                    throw std::out_of_range("cross_product is only defined for 3D vectors");
                }
                // for more than 3 dimensions
                // http://en.wikipedia.org/wiki/Seven-dimensional_cross_product
                // http://de.wikipedia.org/wiki/Kreuzprodukt
                // http://en.wikipedia.org/wiki/Cross_product
                // http://en.wikipedia.org/wiki/Einstein_summation_convention
            }

            /*!
            \brief calculates the cross product of 2 3D vectors
            \param v1 vector 1 (vectorX&)
            \param v2 vector 2 (vectorX&)
            \return cross product as a 3D vector
            */
            friend vectorND cross_product(const vectorND& v1, const vectorND& v2)
            {
                vectorND<T> r;
                r.cross_product(v1, v2);
                return r;
            }

            /*!
            \brief calculates the scalar product of two multi dimensional vectors
            \param v1 vector 1 (vectorX&)
            \param v2 vector 2 (vectorX&)
            \return scalar product
            */
            friend T scalar_product(const vectorND& v1, const vectorND& v2)
            {
                if (v1.size() == v2.size())
                {
                    T r = 0;
                    size_t i, n = v1.size();
                    for (i=0; i<n; i++)
                    {
                        r += (v1[i] * v2[i]);
                    }
                    return r;
                }
                return 0;
            };

            /*!
            \brief calculate the sine of the angle between two 3D vectors
            \param v1 vector 1 (vectorX&)
            \param v2 vector 2 (vectorX&)
            \return sine of the angle
            */
            friend T sin(const vectorND&v1, const vectorND&v2)              // Sinus des Winkels zwischen zwei Vektoren   (*)
            {
                T val = sqrt(v1.square_absolute_value() * v2.square_absolute_value());
                if (val > std::numeric_limits<T>::epsilon())
                {
                    vectorND<T> v;
                    v.cross_product(v1, v2);
                    return v.absolute_value() / val;
                }
                return 0;
            }

            /*!
            \brief calculate the cosine of the angle between two 2D or 3D vectors
            \param v1 vector 1 (vectorX&)
            \param v2 vector 2 (vectorX&)
            \return cosine of the angle
            */
            friend T cos(const vectorND&v1, const vectorND&v2)              // Cosinus des Winkels zwischen zwei Vektoren (*)
            {
                T val = sqrt(v1.square_absolute_value() * v2.square_absolute_value());
                if (val > std::numeric_limits<T>::epsilon())
                {
                    return scalar_product(v1, v2) / val;
                }
                return 0;
            }

            /*!
            \brief calculate the tangens of the angle between two 3D vectors
            \param v1 vector 1 (vectorX&)
            \param v2 vector 2 (vectorX&)
            \return tangens of the angle
            */
            friend T tan(const vectorND&v1, const vectorND&v2)
            {
                vectorND<T> v;
                v.cross_product(v1, v2);
                T val = v.absolute_value();
                if (val > std::numeric_limits<T>::epsilon())
                {
                    return scalar_product(v1, v2) / val;
                }
                return 0;
            }

            /*!
            \brief calculates the volume of the parallel epiped consisting of 3 3D vectors
            \param v1 vector 1 (vectorX&)
            \param v2 vector 2 (vectorX&)
            \param v3 vector 3 (vectorX&)
            \return volume
            */
            friend T spat(const vectorND&v1, const vectorND&v2, const vectorND&v3)
            {
                vectorND vx;
                vx.cross_product(v2, v3);
                return scalar_product(v1, vx);
            }

            /*!
            \brief adds two multi dimensional vectors
            \param v1 vector 1 (vectorX&)
            \param v2 vector 2 (vectorX&)
            \return sum vector
            */
            friend vectorND operator+(const vectorND& v1, const vectorND& v2)
            {
                if (v1.size() == v2.size())
                {
                    size_t i, n = v1.size();
                    vectorND<T> r(v1.size(), 0);
                    for (i=0; i<n; i++)
                    {
                        r[i] = v1[i] + v2[i];
                    }
                    return r;
                }
                return dummy();
            };


            /*!
            \brief adds a vector to this vector, and saves result in calling object
            \param v2 vector
            */
            void operator += (const vectorND& v2)
            {
                vectorND<T> &v1 = *this;
                size_t size = v2.size();
                if (v1.size() != size)
                {
                    v1.resize(size, 0);
                }
                size_t i;
                for (i=0; i<size; i++)
                {
                    v1[i] += v2[i];
                }
            }

            /*!
            \brief subtracts two multi dimensional vectors
            \param v1 vector 1 (vectorX&)
            \param v2 vector 2 (vectorX&)
            \return difference vector
            */
            friend vectorND operator-(const vectorND& v1, const vectorND& v2)
            {
                if (v1.size() == v2.size())
                {
                    size_t i, n = v1.size();
                    vectorND<T> r(v1.size(), 0);
                    for (i=0; i<n; i++)
                    {
                        r[i] = v1[i] - v2[i];
                    }
                    return r;
                }
                return dummy();
            };

            /*!
            \brief subtracts a vector from this vector, and saves result in calling object
            \param v2 vector
            */
            void operator -= (const vectorND& v2)
            {
                vectorND<T> &v1 = *this;
                size_t size = v2.size();
                if (v1.size() != size)
                {
                    v1.resize(size, 0);
                }
                size_t i;
                for (i=0; i<size; i++)
                {
                    v1[i] -= v2[i];
                }
            }

            /*!
            \brief multiplies each element of a multi dimensional vector with a value
            \param v vector
            \param val value
            \return result vector
            */
            friend vectorND operator*(const vectorND& v, T val)
            {
                vectorND<T> r;
                size_t i, n = v.size();
                r.resize(n);
                for (i=0; i<n; ++i)
                {
                    r[i] = v[i] * val;
                }
                return r;
            };

            /*!
            \brief multiplies each element of a multi dimensional vector with a value
            \param v vector
            \param val value
            \return result vector
            */
            friend vectorND operator*(T val, const vectorND& v)
            {
                return v * val;
            }
            /*!
            \brief multiplies each element of a multi dimensional vector with a value
            */
            void operator *= (T val)
            {
                iterator it = this->begin();
                for ( ; it!=this->end(); ++it)
                {
                    (*it) *= val;
                }
            }

            /*!
            \brief divides each element of a multi dimensional vector by a value
            \param v vector
            \param val value
            \return result vector
            */
            friend vectorND operator/(const vectorND& v, T val)
            {
                if (fabs(val) > std::numeric_limits<T>::epsilon())
                {
                    val = 1 / val;
                    return v * val;
                }
                else
                {
                    throw std::domain_error("division by zero");
                }
            }

            /*!
            \brief divides each element of the vector by a value
            \param val value
            */
            void operator /= (T val)
            {
                if (fabs(val) > std::numeric_limits<T>::epsilon())
                {
                    val = 1 / val;
                    iterator it = this->begin();
                    for ( ; it!=this->end(); ++it)
                    {
                        (*it) *= val;
                    }
                }
                else
                {
                    throw std::domain_error("division by zero");
                }
            }

            /*!
            \brief adds a value to the vector
            \param val value
            \return reference of this object
            */
            vectorND<T>& operator << (const T&val)
            {
                this->push_back(val);
                return *this;
            }

        };

    } // VectorX

    /*!
     <HR>
     <h1>Defines the a 2D matrix derived from <i>std::vector<T></i></h1>
     <p>
     The matrix is organized in equally sized rows of the type <i>vector<T></i>, the rows are stored
     in a vector of rows defined as: <i>vector< vector<T> ></i><br>
     Note: if a previously define <i>vector< vector<T> ></i> is replaced by <i>matrix<T></i> it is

     necessary to replace the size() function by rows().<br>
     The function <i>matrix<T>::size()</i> returns rows()*columns() that is the total number of elements
     in the matrix.
     <p>
     The matrix class contains functions to modify the size and the content of the 2D
     matrix and functions to request the size or to retrieve the content of the matrix.
     The function names are similar to the function names of the vector class:
     <UL>
      <LI><b>size()</b>: retrieves the number of elements of the matrix (rows*columns)</LI>
      <LI><b>empty()</b>: returns true, if the matrix is empty</LI>
      <LI><b>assign()</b>: assigns data of an array or a vector<T> to the matrix row by row</LI>
      <LI><b>operator[][]</b>: access to a single element of the matrix</LI>
     </UL>
     The function matrix<T>::row() returns a <i>const vector<T>&</i> reference of the desired row of the
     matrix.<br>
     Other matrix specific functions are:
     <UL>
      <LI><b>column()</b>: returns the column of the matrix</LI>
      <LI><b>columns()</b>: retrieves the number of columns</LI>
      <LI><b>row()</b>: returns the row of the matrix</LI>
      <LI><b>rows()</b>: retrieves the number of rows</LI>
      <LI><b>is_square()</b>: determines whether the matrix dimensions are square</LI>
      <LI><b>print()</b>: prints the content of the matrix to an out stream</LI>
      <LI><b>transpose()</b>: transposes the source matrix</LI>
      <LI><b>det()</b>: calculate the determinant of the matrix *1</LI>
      <LI><b>adjoint()</b>: calculates the adjoint of the matrix *1</LI>
      <LI><b>invert()</b>: calculates the inverse of the matrix *1</LI>

      <LI><b>copy_to()</b>: copies the content of the matrix to a vector (counterpart to assign)</LI>
     </UL>
     <h2>The matrix provides iterators for iteration through rows, columns or the  whole matrix elements</h2>
     <UL>
      <LI><b>row_begin()</b>: provides an iterator through a row</LI>
      <LI><b>row_rbegin()</b>: provides a reverse iterator through a row</LI>
      <LI><b>column_begin()</b>: provides an iterator through a column</LI>
      <LI><b>column_rbegin()</b>: provides a reverse iterator through a column</LI>
      <LI><b>matrix_begin()</b>: provides an iterator through all matrix elements</LI>
      <LI><b>row_end()</b>: provides the stop iterator for a row</LI>
      <LI><b>row_rend()</b>: provides the reverse stop iterator for a row</LI>
      <LI><b>column_end()</b>: provides the stop iterator for a column</LI>
      <LI><b>column_rend()</b>: provides the reverse stop iterator for a column</LI>
      <LI><b>matrix_end()</b>:provides the stop iterator for the matrix</LI>
     </UL>

     <h2>Functions to combine matrixes or operate with matrixes</h2>
     <UL>
      <LI><b>add()</b>: adds two matrixes</LI>
      <LI><b>subtract()</b>: subtract a matrix from another</LI>
      <LI><b>multiply()</b>: multiplies two matrixes</LI>
      <LI><b>add()</b>: adds a value to each matrix element</LI>
      <LI><b>subtract()</b>: subtracts a value from each matrix element</LI>
      <LI><b>multiply()</b>: multiplies a value to each matrix element</LI>
      <LI><b>divide()</b>: divides each matrix element by a value</LI>

     </UL>
     <p>
     <h2>4D matrixes for 3D coordinate translation</h2>
     <UL>
     <LI><b>identity()</b>: creates an identity matrix</LI>
     <LI><b>rotate_x()</b>: creates a rotation matrix</LI>
     <LI><b>rotate_y()</b>: creates a rotation matrix</LI>
     <LI><b>rotate_z()</b>: creates a rotation matrix</LI>
     <LI><b>rotate()</b>: creates a rotation matrix</LI>
     <LI><b>translate()</b>: creates a translation matrix</LI>
     <LI><b>scale()</b>: creates a scale matrix</LI>
     <LI><b>shear()</b>: creates a shear matrix</LI>
     <LI><b>perspective_projection()</b>: creates a perspective projection matrix</LI>
     <LI><b>ortographic_projection()</b>: creates an ortographic projection matrix</LI>
     </UL>
     These matrices represent counterclockwise rotations by an angle of phi around the
     x, y, and z axes, respectively. The default direction of the rotation is determined by the
     right-hand rule:
     <UL>
     <LI>Rx rotates the y-axis towards the z-axis</LI>
     <LI>Ry rotates the z-axis towards the x-axis</LI>
     <LI>Rz rotates the x-axis towards the y-axis</LI>
     </UL>
     The right hand system can be changed to the left hand system by function parameter.

     Other rotation matrices can be obtained from these three using matrix multiplication.<br>
     For example, the product:
     <CENTER><b>Rx(gamma)*Ry(beta)*Rz(alpha)</b></CENTER><br>

     represents a rotation whose yaw, pitch, and roll are alpha, beta and gamma respectively.<br>
     Similarly, the product:
     <CENTER><b>Rz(gamma)*Rx(beta)*Rz(alpha)</b></CENTER><br>
     represents a rotation whose Euler angles are alpha, beta and gamma using the z-x-z convention
     for Euler angles.
     In both cases the matrices are assumed to act on column vectors.
     For the matrix multiplication use the function <i>multibeam::matrix<T>::multiply(...)</i>
     For the projection the ortographic or perspective projection matrixes may be used.
     The perspective projection needs an additional division of the x,y,z coordinates by the
     4th parameter w of the result vector.

     <h2>Gauss least squares, gauss elimination and polynome regression</h2>
     Based on this matrix class function to calculate polynome regression by
     the gaussian method of least squares are implemented.
     <UL>
     <LI><b>gauss_least_square()</b>: multiplies a matrix and vector with its own transposed</LI>
     <LI><b>gauss_elimination()</b>: solves an equation defined by a matrix and a result vector</LI>
     <LI><b>calc_gauss_seidel()</b>: solves an equation defined by a matrix and a result vector</LI>
     <LI><b>fitCurveToPolynome()</b>: fits the polynome function coefficients to given x and y values</LI>
     <LI><b>derivePolynome()</b>: derives the coefficients of the polynome stored in vector elements</LI>
     <LI><b>calcPolynomeValueByHorner()</b>: calculates the result of a polynome based on the variables stored vector elements</LI>

     </UL>

     <h2>Some additional functions</h2>
     <UL>
     <LI><b>isBetween()</b>: checks a value within a given ranges</LI>
     </UL>
    */

    /// Matrix
    namespace Matrix
    {

        /// class implementing a two dimensional matrix
        template<class T>
        class matrix : public std::vector< std::vector<T> >
        {
            /// coordinate system for coordinate translation
            enum eSystem { right, left };
        public:
            /// matrix type
            typedef typename std::vector< std::vector<T> > matrix_type;
            /// const iterator
            typedef typename matrix_type::const_iterator const_iterator;
            /// iterator
            typedef typename matrix_type::iterator iterator;
            /// const reverse iterator
            typedef typename matrix_type::const_reverse_iterator const_reverse_iterator;
            /// reverse iterator
            typedef typename matrix_type::reverse_iterator reverse_iterator;

            /// const reference
            typedef typename std::vector<T>::const_reference const_vector_reference;
            /// reference
            typedef typename std::vector<T>::reference vector_reference;
            /// row iterator
            typedef typename std::vector<T>::iterator row_iterator;
            /// const row iterator
            typedef typename std::vector<T>::const_iterator const_row_iterator;
            /// reverse row iterator
            typedef typename std::vector<T>::reverse_iterator reverse_row_iterator;
            /// const reverse row iterator
            typedef typename std::vector<T>::const_reverse_iterator const_reverse_row_iterator;

            matrix() { /* cout << "matrix()" << endl; */ }

            /*!
            \brief constructor initializing size of matrix
            \param row number of rows
            \param col number of columns
            \param bzero initialize all values with zero
            */
            matrix(size_t row, size_t col, bool bzero=false)
            {
                //cout << "matrix(" << (unsigned int)row << ", " << (unsigned int)col << ", " << bzero << endl;
                resize(row, col, bzero);
            }

            /*!
            \brief resizes the matrix
            \param nrows number of rows (size_t)
            \param ncols number of columns (size_t)
            \param bzero initialize all values with zero (bool)
            */
            void resize(size_t nrows, size_t ncols, bool bzero=false)
            {
                if (nrows == rows() && ncols == columns() && !bzero) return;
                matrix<T> &m = *this;
                m.matrix_type::resize(nrows);
                for (size_t row = 0; row < nrows; row++)
                {
                    if (bzero) m[row].resize(ncols, 0);
                    else       m[row].resize(ncols);
                }
            }

            /*!
            \brief resizes the matrix
            \param rows number of rows (size_t)
            \param cols number of columns (size_t)
            \param value initialize all elements with given value (T)
            \return void
            */
            void resize(size_t rows, size_t cols, T value)
            {
                matrix_type::resize(rows);
                for (size_t row = 0; row < rows; row++)
                {
                    (*this)[row].resize(cols, value);
                }
            }

            /*!
            \brief determines whether the matrix is empty
            \return (true, false)
            */
            bool empty( ) const
            {
                if (rows())
                {
                    return (*this)[0].empty();
                }
                return true;
            }

            /*!
            \brief determines whether the matrix is a square matrix
            \return (true, false)
            */
            bool is_square( ) const
            {
                return (rows() == columns());
            }

            /*!
            \brief determines the number of elements of the matrix
            \return elements of the matrix (size_t)
            */
            size_t size() const
            {
                return rows()*columns();
            }

            /*!
            \brief determines the number of rows of the matrix
            \return rows of the matrix (size_t)
            */
            size_t rows() const
            {
                return  matrix_type::size();
            }

            /*!
            \brief determines the number of columns of the matrix
            \return columns of the matrix (size_t)
            */
            size_t columns() const
            {
                if (!empty())
                {
                    return (*this)[0].size();
                }
                return 0;
            }

            /*!
            \brief returns a const reference of the row of the matrix
            \param row index of the row to be returned (size_t)
            \return row of the matrix (const vector<T>)
            */
            const std::vector<T>& row(size_t row) const
            {
                if (row < rows())
                {
                    return (*this)[row];
                }
                else
                {
                    static std::vector<T> dummy;
                    return dummy;
                }
            }

           /*!
            \brief returns a copy of the row of the matrix
            \param row index of the column to be returned (size_t)
            \param v vector to receive the content of the row
            \param [start] first element to copy
            \param [end] last element to copy
            \note if start and end are not defined, the whole row is copied
            \return (true, false)
            */
            bool row(size_t row, std::vector<T>& v, size_t start=0, size_t end=0) const
            {
                if (row < rows())
                {
                    size_t fColumns = columns();
                    if (start >= fColumns) return false;
                    if (end == 0) end = fColumns - start;
                    if (end <= start) return false;
                    v.assign(row_begin(row)+start, row_begin(row)+end);
                    return true;
                }
                else
                {
                    return false;
                }
            }
           /*!
            \brief returns a copy of the column of the matrix
            \param col index of the column to be returned (size_t)
            \param v vector to receive the content of the column
            \param [start] first element to copy
            \param [end] last element to copy
            \note if start and end are not defined, the whole column is copied
            \return (true, false)
            */
            bool column(size_t col, std::vector<T>& v, size_t start=0, size_t end=0) const
            {
                size_t fRows = rows();
                if (col < columns())
                {
                    if (start >= fRows) return false;
                    if (end == 0) end = fRows - start;

                    if (end <= start) return false;
                    v.assign(column_begin(col)+start, column_begin(col)+end);
                    return true;
                }
                return false;
            }

    /////////////////////////////////////////////////////////////////////////////////////////
            /// iterator to iterate through all matrix elements
            class matrix_iterator : private iterator
            {
            public:
                using iterator_category = std::random_access_iterator_tag;
                using value_type = typename matrix::value_type;
                using difference_type = std::ptrdiff_t;
                using pointer = typename matrix::pointer;
                using reference = vector_reference;

                /// constructor
                matrix_iterator() {  }
                /// constructor
                matrix_iterator(iterator aPtr) : iterator(aPtr) {  }
                /// constructor
                matrix_iterator(iterator aPtr, iterator aColEnd, row_iterator aIt, row_iterator aEnd) :
                    iterator(aPtr), _Row(aIt), _RowEnd(aEnd), _ColEnd(aColEnd) {  }
                /// returns reference of the element
                vector_reference operator*() const { return *_Row; }
                /// postincrement
                matrix_iterator operator++(int) {
                    matrix_iterator _Tmp = *this;
                    ++(*this);
                    return (_Tmp);
                }
                /// preincrement
                matrix_iterator& operator++() {
                    ++_Row;
                    if (_Row == _RowEnd) {
                        if (iterator::operator++() != _ColEnd) {
                            _Row = (iterator::operator*()).begin();
                            _RowEnd = (iterator::operator*()).end();
                        }
                    }
                    return (*this);
                }
                
                /// assignement operator
                matrix_iterator& operator=(const iterator& _Right) const {
                    return matrix_iterator(_Right);
                }
                /// difference operator
                friend difference_type operator-(const matrix_iterator _Left, matrix_iterator _Right) {
                    if (_Left < _Right)
                    {
                        return -(_Right - _Left);
                    }
                    auto rows = iterator(_Left) - iterator(_Right);
                    auto first = _Right._RowEnd - _Right._Row;
                    auto last = (_Left._Row != _Left._RowEnd) ? _Left._Row - (_Left.iterator::operator*()).begin() : 0;
                    auto size = (_Right.iterator::operator*()).end() - (_Right.iterator::operator*()).begin();
                    return (rows - 1) * size + first + last;
                }
                /// operator iterator + position
                matrix_iterator operator+(difference_type _Pos) const {
                    auto size = (iterator::operator*()).end() - (iterator::operator*()).begin();
                    auto rows = _Pos / size;
                    auto columns = abs(_Pos) % size;
                    auto start = this->_Row - (iterator::operator*()).begin();
                    matrix_iterator _Temp(iterator(*this) + rows);
                    _Temp._ColEnd = _ColEnd;
                    if (iterator(_Temp) != iterator(_ColEnd))
                    {
                        _Temp._Row = _Pos > 0 ? (_Temp.iterator::operator*()).begin() + (start + columns)
                                              : (_Temp.iterator::operator*()).end() - (start + columns);
                        _Temp._RowEnd = (_Temp.iterator::operator*()).end();
                    }
                    else
                    {
                        matrix_iterator _Temp2(iterator(*this) + size-1);
                        _Temp._Row = (_Temp2.iterator::operator*()).end();
                        _Temp._RowEnd = (_Temp2.iterator::operator*()).end();
                    }
                    return _Temp;
                }

                matrix_iterator& operator+=(difference_type _Pos) {
                    auto fTmp = *this + _Pos;
                    std::swap(fTmp, *this);
                    return *this;
                }

                bool operator== (matrix_iterator const& aRhs) const
                {
                    return (static_cast<BaseClass const&>(*this) == aRhs)
                        && (_Row == aRhs._Row)
                        && (_RowEnd == aRhs._RowEnd)
                        && (_ColEnd == aRhs._ColEnd);
                }

                bool operator!= (matrix_iterator const& aRhs) const
                {
                    return not (*this == aRhs);
                }

            private:
                using BaseClass = iterator;

                row_iterator _Row;
                row_iterator _RowEnd;
                iterator     _ColEnd;
            };

            /// const iterator to iterate through all matrix elements
            class const_matrix_iterator : private const_iterator
            {
            public:
                using iterator_category = std::random_access_iterator_tag;
                using value_type = typename matrix::value_type;
                using difference_type = std::ptrdiff_t;
                using pointer = typename matrix::pointer;
                using reference = const_vector_reference;

                /// constructor
                const_matrix_iterator() {  }
                /// constructor
                const_matrix_iterator(const_iterator aPtr) : const_iterator(aPtr) {  }
                /// constructor
                const_matrix_iterator(const_iterator aPtr, const_iterator aColEnd, const_row_iterator aIt, const_row_iterator aEnd) :
                    const_iterator(aPtr), _Row(aIt), _RowEnd(aEnd), _ColEnd(aColEnd) {  }
                /// returns reference of the element
                const_vector_reference operator*() const { return *_Row; }
                /// postincrement
                const_matrix_iterator operator++(int) {
                    const_matrix_iterator _Tmp = *this;
                    ++(*this);
                    return (_Tmp);
                }
                /// preincrement
                const_matrix_iterator& operator++() {
                    ++_Row;
                    if (_Row == _RowEnd) {
                        if (const_iterator::operator++() != _ColEnd) {
                            _Row = (const_iterator::operator*()).begin();
                            _RowEnd = (const_iterator::operator*()).end();
                        }
                    }
                    return (*this);
                }
               
                /// assignement operator
                const_matrix_iterator& operator=(const const_iterator& _Right) const {
                    return const_matrix_iterator(_Right);
                }
                /// difference operator
                friend difference_type operator-(const const_matrix_iterator _Left, const_matrix_iterator _Right) {
                    if (_Left < _Right)
                    {
                        return -(_Right - _Left);
                    }
                    auto rows =  const_iterator(_Left) - const_iterator(_Right);
                    auto first = _Right._RowEnd - _Right._Row;
                    auto last = (_Left._Row != _Left._RowEnd) ? _Left._Row - (_Left.const_iterator::operator*()).begin() : 0;
                    auto size = (_Right.const_iterator::operator*()).end() - (_Right.const_iterator::operator*()).begin();
                    return (rows - 1) * size + first + last;
                }
                /// operator iterator + position
                const_matrix_iterator operator+(difference_type _Pos) const {
                    auto size = (const_iterator::operator*()).end() - (const_iterator::operator*()).begin();
                    auto rows = _Pos / size;
                    auto columns = abs(_Pos) % size;
                    auto start = this->_Row - (const_iterator::operator*()).begin();
                    const_matrix_iterator _Temp(const_iterator(*this) + rows);
                    _Temp._ColEnd = _ColEnd;
                    if (const_iterator(_Temp) != const_iterator(_ColEnd))
                    {
                        _Temp._Row = _Pos > 0 ? (_Temp.const_iterator::operator*()).begin() + (start + columns)
                                              : (_Temp.const_iterator::operator*()).end() - (start + columns);
                        _Temp._RowEnd = (_Temp.const_iterator::operator*()).end();
                    }
                    else
                    {
                        const_matrix_iterator _Temp2(const_iterator(*this) + size-1);
                        _Temp._Row = (_Temp2.const_iterator::operator*()).end();
                        _Temp._RowEnd = (_Temp2.const_iterator::operator*()).end();
                    }
                    return _Temp;
                }


                const_matrix_iterator& operator+=(difference_type _Pos) {
                    auto fTmp = *this + _Pos;
                    std::swap(fTmp, *this);
                    return *this;
                }

                bool operator== (const_matrix_iterator const& aRhs) const
                {
                    return (static_cast<BaseClass const&>(*this) == aRhs)
                        && (_Row == aRhs._Row)
                        && (_RowEnd == aRhs._RowEnd)
                        && (_ColEnd == aRhs._ColEnd);
                }

                bool operator!= (const_matrix_iterator const& aRhs) const
                {
                    return not (*this == aRhs);
                }

            private:
                using BaseClass = const_iterator;

                const_row_iterator _Row;
                const_row_iterator _RowEnd;
                const_iterator     _ColEnd;
            };
    /////////////////////////////////////////////////////////////////////////////////////////
            /*!
            \brief returns an iterator for the matrix
            \param aRow index of the start row (size_t)
            \param aCol index of the start column (size_t)
            \return iterator (matrix_iterator)
            */
            matrix_iterator matrix_begin(size_t aRow=0, size_t aCol=0)
            {
                return matrix_iterator(this->begin()+aRow, this->end(), this->at(aRow).begin()+aCol, this->at(aRow).end());
            }

            /*!
            \brief returns an iterator for the matrix
            \param aRow index of the start row (size_t)
            \param aCol index of the start column (size_t)
            \return iterator (const_matrix_iterator)
            */
            const_matrix_iterator matrix_begin(size_t aRow=0, size_t aCol=0) const
            {
                return const_matrix_iterator(this->begin()+aRow, this->end(), this->at(aRow).begin()+aCol, this->at(aRow).end());
            }

            /*!
            \brief returns the end iterator for the matrix
            \return iterator (matrix_iterator)
            */
            matrix_iterator matrix_end()
            {
                if (!empty())
                {
                    size_t fRow = rows()-1;
                    return matrix_iterator(this->end(), this->end(), this->at(fRow).end(), this->at(fRow).end());
                }
                return matrix_iterator();
            }

            /*!
            \brief returns the end iterator for the matrix
            \return iterator (const_matrix_iterator)
            */
            const_matrix_iterator matrix_end() const
            {
                if (!empty())
                {
                    size_t fRow = rows()-1;
                    return const_matrix_iterator(this->end(), this->end(), this->at(fRow).end(), matrix_type::at(fRow).end());
                }
                return const_matrix_iterator();
            }

    /////////////////////////////////////////////////////////////////////////////////////////
    /// const iterator to iterate through all rows in a column
            class const_column_iterator : private const_iterator
            {
            public:
                using iterator_category = std::random_access_iterator_tag;
                using value_type = typename matrix::value_type;
                using difference_type = std::ptrdiff_t;
                using pointer = typename matrix::pointer;
                using reference = const_vector_reference;

                /// constructor
                const_column_iterator() { _Column = 0; }
                /// constructor
                const_column_iterator(const_iterator aPtr, size_t aCol) : const_iterator(aPtr), _Column(aCol) {  }
                /// returns reference of the element
                const_vector_reference operator*() const
                { return (const_iterator::operator*())[_Column]; }
                /// returns iterator increased by an offset
                const_column_iterator operator+(difference_type _Off) const   {
                    const_iterator _Tmp = *this;
                    return const_column_iterator(_Tmp + _Off, _Column); }

                const_column_iterator operator+=(difference_type _Off) {
                    BaseClass::operator+=(_Off);
                    return *this;
                }

                const_column_iterator operator-(difference_type _Off) const {
                    const_iterator _Tmp = *this;
                    return const_column_iterator(_Tmp - _Off, _Column);
                }

                const_column_iterator operator-=(difference_type _Off) {
                    BaseClass::operator-=(_Off);
                    return *this;
                }

                const_column_iterator& operator++()
                {
                    BaseClass::operator++();
                    return *this;
                }

                const_column_iterator& operator++(int)
                {
                    BaseClass::operator++();
                    return *this;
                }

                difference_type operator-(const_column_iterator const& aRhs) const {
                    return static_cast<BaseClass const&>(*this) - static_cast<BaseClass const&>(aRhs);
                }

                /// assignement operator
                const_column_iterator& operator=(const const_iterator& _Right) const {
                    return const_column_iterator(_Right); }

                bool operator== (const_column_iterator const& aRhs) const
                {
                    return (static_cast<BaseClass const&>(*this) == static_cast<BaseClass const&>(aRhs))
                        && (_Column == aRhs._Column);
                }

                bool operator!= (const_column_iterator const& aRhs) const
                {
                    return not (*this == aRhs);
                }

            private:
                using BaseClass = const_iterator;

                size_t _Column;
            };

            /// iterator to iterate through all rows in a column
            class column_iterator : private iterator
            {
            public:
                using iterator_category = std::random_access_iterator_tag;
                using value_type = typename matrix::value_type;
                using difference_type = std::ptrdiff_t;
                using pointer = typename matrix::pointer;
                using reference = vector_reference;

                /// constructor
                column_iterator() { _Column = 0; }
                /// constructor
                column_iterator(iterator aPtr, size_t aCol) : iterator(aPtr), _Column(aCol) {  }
                /// returns reference of the element
                vector_reference operator*() const
                { return (iterator::operator*())[_Column]; }

                /// returns iterator increased by an offset
                column_iterator operator+(difference_type _Off) const   {
                    iterator _Tmp = *this;
                    return column_iterator(_Tmp += _Off, _Column); }

                column_iterator operator+=(difference_type _Off) {
                    BaseClass::operator+=(_Off);
                    return *this;
                }

                column_iterator operator-(difference_type _Off) const {
                    iterator _Tmp = *this;
                    return column_iterator(_Tmp -= _Off, _Column);
                }

                column_iterator operator-=(difference_type _Off) {
                    BaseClass::operator-=(_Off);
                    return *this;
                }

                column_iterator& operator++()
                {
                    BaseClass::operator++();
                    return *this;
                }

                column_iterator& operator++(int)
                {
                    BaseClass::operator++();
                    return *this;
                }

                difference_type operator-(column_iterator const& aRhs) const {
                    return BaseClass::operator-(aRhs);
                }

                /// assignement operator
                column_iterator& operator=(const iterator& _Right) const {
                    return column_iterator(_Right); }

                bool operator== (column_iterator const& aRhs) const
                {
                    return (static_cast<BaseClass const&>(*this) == (aRhs))
                        && (_Column == aRhs._Column);
                }

                bool operator!= (column_iterator const& aRhs) const
                {
                    return not (*this == aRhs);
                }

             private:
                using BaseClass = iterator;
                size_t _Column;
            };

    /////////////////////////////////////////////////////////////////////////////////////////
            /*!
            \brief returns an iterator for this column
            \param aCol index of the column for iteration (size_t)
            \return iterator (const_column_iterator)
            */
            const_column_iterator column_begin(size_t aCol) const
            {
                return const_column_iterator(this->begin(), aCol);
            }

            /*!
            \brief returns the end iterator for this column
            \return iterator (const_column_iterator)
            */
            const_column_iterator column_end(size_t aCol) const
            {
                return const_column_iterator(this->end(), aCol);
            }

            /*!
            \brief returns an iterator for this column
            \param aCol index of the column for iteration (size_t)
            \return iterator (column_iterator)
            */
            column_iterator column_begin(size_t aCol)
            {
                return column_iterator(this->begin(), aCol);
            }

            /*!
            \brief returns the end iterator for this column
            \return iterator (column_iterator)
            */
            column_iterator column_end(size_t aCol)
            {
                return column_iterator(this->end(), aCol);
            }

    /////////////////////////////////////////////////////////////////////////////////////////
            /// const reverse iterator to iterate through all rows in a column
            class const_reverse_column_iterator : public const_reverse_iterator
            {
            public:
                /// constructor
                const_reverse_column_iterator() { _Column = 0; }
                /// constructor
                const_reverse_column_iterator(const_reverse_iterator aPtr, size_t aCol) : const_reverse_iterator(aPtr), _Column(aCol) {  }
                /// returns reference of the element
                const_vector_reference operator*() const
                {   return (this->const_reverse_iterator::operator*())[_Column]; }

            private:
                size_t _Column;
            };

            /// reverse iterator to iterate through all rows in a column
            class reverse_column_iterator : public reverse_iterator
            {
            public:
                /// constructor
                reverse_column_iterator() { _Column = 0; }
                /// constructor
                reverse_column_iterator(reverse_iterator aPtr, size_t aCol) : reverse_iterator(aPtr), _Column(aCol) {  }

                /// returns reference of the element
                vector_reference operator*() const
                {   return (this->reverse_iterator::operator*())[_Column]; }

            private:
                size_t _Column;
            };

    /////////////////////////////////////////////////////////////////////////////////////////
            /*!
            \brief returns an iterator for this column
            \param aCol index of the column for reverse iteration (size_t)
            \return reverse iterator (const_reverse_column_iterator)
            */
            const_reverse_column_iterator column_rbegin(size_t aCol) const
            {
                return const_reverse_column_iterator(this->rbegin(), aCol);
            }

            /*!
            \brief returns an iterator for this column
            \param aCol index of the column for reverse iteration (size_t)
            \return reverse iterator (reverse_column_iterator)
            */
            reverse_column_iterator column_rbegin(size_t aCol)
            {
                return reverse_column_iterator(this->rbegin(), aCol);
            }

            /*!
            \brief returns the end iterator for this column
            \return reverse iterator (const_reverse_column_iterator)
            */
            const_reverse_column_iterator column_rend(size_t aCol) const
            {
                return const_reverse_column_iterator(this->rend(), aCol);
            }

            /*!
            \brief returns the end iterator for this column
            \return reverse iterator (reverse_column_iterator)
            */
            reverse_column_iterator column_rend(size_t aCol)
            {
                return reverse_column_iterator(this->rend(), aCol);
            }

    /////////////////////////////////////////////////////////////////////////////////////////
            /*!
            \brief returns an iterator for this row
            \param aRow index of the row for iteration (size_t)
            \return iterator (const_row_iterator)
            */
            const_row_iterator row_begin(size_t aRow) const
            {
                return this->at(aRow).begin();
            }

            /*!
            \brief returns an iterator for this row
            \param aRow index of the row for iteration (size_t)
            \return iterator (row_iterator)
            */
            row_iterator row_begin(size_t aRow)
            {
                return this->at(aRow).begin();
            }

            /*!
            \brief returns the end iterator for this row
            \param aRow index of the row for iteration (size_t)
            \return iterator (const_row_iterator)
            */
            const_row_iterator row_end(size_t aRow) const
            {
                return this->at(aRow).end();
            }

            /*!
            \brief returns the end iterator for this row
            \param aRow index of the row for iteration (size_t)
            \return iterator (row_iterator)
            */
            row_iterator row_end(size_t aRow)
            {
                return this->at(aRow).end();
            }

    /////////////////////////////////////////////////////////////////////////////////////////
            /// functions to retrieve iterators to reverse iterate through all columns in a row
            /*!
            \brief returns a reverse iterator for this row
            \param aRow index of the row for reverse iteration (size_t)
            \return reverse iterator (const_reverse_row_iterator)
            */
            const_reverse_row_iterator row_rbegin(size_t aRow) const
            {
                return this->at(aRow).rbegin();
            }

            /*!
            \brief returns a reverse iterator for this row
            \param aRow index of the row for reverse iteration (size_t)
            \return reverse iterator (reverse_row_iterator)
            */
            reverse_row_iterator row_rbegin(size_t aRow)
            {
                return this->at(aRow).rbegin();
            }

            /*!
            \brief returns the end iterator for this row
            \param aRow index of the row for reverse iteration (size_t)
            \return reverse iterator (const_reverse_row_iterator)
            */
            const_reverse_row_iterator row_rend(size_t aRow) const
            {
                return this->at(aRow).rend();
            }

            /*!
            \brief returns the end iterator for this row
            \param aRow index of the row for reverse iteration (size_t)
            \return reverse iterator (reverse_row_iterator)
            */
            reverse_row_iterator row_rend(size_t aRow)
            {
                return this->at(aRow).rend();
            }

    /////////////////////////////////////////////////////////////////////////////////////////

            /*!
             \brief returns a copy of a part of this matrix
             \param startrow start index for the rows to be copied (size_t)
             \param rows number rows to be copied (size_t)
             \param startcolumn start index for the columns to be copied (size_t)
             \param columns number columns to be copied (size_t)
             \param sub resulting matrix of size rows * columns (matrix<T>&)
             \return (true, false)
             */
             bool get_sub_matrix(size_t startrow, size_t rows, size_t startcolumn, size_t columns, matrix<T>& sub) const
             {
                 const matrix<T> &m = *this;
                 if (   startrow    + rows    <= m.rows()
                     && startcolumn + columns <= m.columns())
                 {
                     size_t i, j;
                     sub.resize(rows, columns);
                     for (i=0; i<rows; ++i)
                     {
                         for (j=0; j<columns; ++j)
                         {
                             sub[i][j] = m[startrow+i][startcolumn+j];
                         }
                     }
                     return true;
                 }
                 return false;
             }
        /*!
             \brief returns a copy of a part of this matrix
             \param row_not row not to be copied (size_t)
             \param column_not column not to be copied (size_t)
             \param sub resulting matrix of size rows-1 * columns-1 (matrix<T>&)
             \return (true, false)
             */
             bool get_sub_matrix(size_t row_not, size_t column_not, matrix<T>& sub) const
             {
                 const matrix<T> &mat = *this;
                 size_t i, j, m, n, rows = mat.rows(), columns = mat.columns();
                 if (rows == columns)
                 {
                     --rows;
                     --columns;
                     sub.resize(rows, columns);
                     for (i=0,m=0; i<rows; ++i,++m)
                     {
                         if (m==row_not) ++m;
                         for (j=0,n=0; j<columns; ++j,++n)
                         {
                             if (n==column_not) ++n;
                             sub[i][j] = mat[m][n];
                         }
                     }
                     return true;
                 }
                 return false;
             }

            /*!
            \brief assigns the elements of the matrix with the content of the vector
            \note the matrix is initialized row by row, all elements are initialized by repeating the vector content
            \param vec vector of elements (T)
            \param len length of the vector, if 1 all elements have the same content
            \return (true, false)
            */
            bool assign(const T *vec, size_t len=1)
            {
                matrix<T> &m = *this;
                size_t i = 0;
                size_t nrows = m.rows();
                size_t ncols = m.columns();
                for (size_t row = 0; row < nrows; ++row)
                {
                    for (size_t col = 0; col < ncols; ++col)
                    {
                        m[row][col] = vec[i++];
                        if (i >= len) i = 0;
                    }
                }
                return i != 0 ? true : false;
            }

            /*!
            \brief assigns the elements of the matrix with the content of the vector
            \note the matrix is initialized row by row
            \param vec vector of elements (vector<T>)
            \return (true, false)
            */
            bool assign(const std::vector<T>& vec)
            {
                return assign(&vec[0], vec.size());
            }

            /*!
            \brief concatenates the elements of the matrix to the vector row by row
            \note the matrix is initialized row by row
            \param vec vector of elements (vector<T>)
            \return (true, false)
            */
            bool copy_to(std::vector<T>&vec)
            {
                vec.assign(this->matrix_begin(), this->matrix_end());
                return !this->empty();
            }

            /*!
            \brief creates a square identity matrix with the given size
            \param size matrix dimension (size_t)
            */
            void identity(size_t size)
            {
                size_t i;
                matrix<T> &m = *this;
                m.resize(size, size, true);
                T val = 0;
                m.assign(&val);
                for (i=0; i<size; i++)
                {
                    m[i][i] = (T) 1.0;
                }
            }

            /// method to calculate determinant
            enum eDetMethod { eLaplace, eGauss};
            /*!
            \brief calculates the determinant of this matrix
            \param aMethod method for calcualtionof the determinant (eLaplace, <b>eGauss</b>)
            \return value of determinant
            */
            T det(eDetMethod aMethod = eGauss) const
            {
                const matrix<T> &m1 = *this;
                size_t nrows = m1.rows();
                size_t ncolumns = m1.columns();
                if (nrows == 2 && ncolumns == 2)
                {
                    return m1[0][0] * m1[1][1] - m1[0][1] * m1[1][0];
                }
                else if (nrows == 3 && ncolumns == 3)
                {
                    return (m1[0][0] * m1[1][1]* m1[2][2] + m1[0][1] * m1[1][2]* m1[2][0] + m1[0][2] * m1[1][0]* m1[2][1]) -
                           (m1[0][2] * m1[1][1]* m1[2][0] + m1[0][1] * m1[1][0]* m1[2][2] + m1[0][0] * m1[1][2]* m1[2][1]);
                }
                else if (nrows > 3 && m1.is_square())
                {
                    double det = 0;
                    if (aMethod == eLaplace) // calculation by laplace
                    {   // runtime is n * (n-1)!
                        matrix<T> sub;
                        size_t i, nm1 = nrows-1;
                        sub.resize(nm1, nm1);
                        for (i=0; i<nrows; i++)
                        {
                            if (m1[i][0] != 0)
                            {
                                get_sub_matrix(i, 0, sub);
                                if (i&1) det -= m1[i][0] * sub.det(aMethod);
                                else     det += m1[i][0] * sub.det(aMethod);
                            }
                        }
                    }
                    else if (aMethod == eGauss) // calculation by gauss elimination
                    {   // runtime is n^3
                        int         i,j,k,n;
                        int         pivot_row, length, permutations=0;
                        std::vector<int> index;
                        T           pivot,factor;
                        row_iterator ptr_from, ptr_to, ptr_pivot;
                        matrix<T>   mat = *this;
                        double depsilon = fabs(std::numeric_limits<T>::epsilon());
                        double abs_pivot;
                        if (depsilon == 0.0)
                        {
                            depsilon = std::numeric_limits<double>::epsilon();
                        }

                        n = (int) mat.rows();

                        index.resize(n);
                        for(i=0; i<n; i++) index[i] = i;

                        for (i=0;i<n;i++)
                        {
                            pivot      = mat[index[i]][i];
                            abs_pivot  = fabs(pivot);
                            pivot_row = i;

                            for (j=i+1;j<n;j++)                     // search pivot row
                            {
                                if (fabs(mat[index[j]][i])>abs_pivot)
                                {
                                    pivot      = mat[index[j]][i];
                                    abs_pivot  = fabs(pivot);
                                    pivot_row = j;
                                }
                            }

                            if (abs_pivot < depsilon) break;        // equation is not solveable

                            if (i!=pivot_row)                       // if other pivot element found
                            {
                                permutations++;                     // count permutations
                                std::swap(index[i], index[pivot_row]);// swap rows
                            }

                            pivot  = (T)(1.0) / pivot;
                            length = n-i-1;
                            if (length)
                            {
                                ptr_pivot = mat[index[i]].begin()+i+1;
                            }

                            for (j=i+1;j<n;j++) // eliminate to triangular matrix
                            {
                                ptr_to = mat[index[j]].begin()+i;
                                if (fabs(*ptr_to)!=0.0)
                                {
                                    factor  = *(ptr_to++) * pivot;
                                    ptr_from = ptr_pivot;

                                    for (k=0;k<length;k++)
                                    {
                                        *(ptr_to++) -= factor* *(ptr_from++);
                                    }
                                }
                            }
                        }
                        det =  mat[index[0]][0]; // calculate determinant from diagonal elements
                        for (i=1;i<n;i++)
                        {
                            det *=  mat[index[i]][i];
                        }
                        // odd number of permutations changes sign of determinant
                        if (permutations&1) det = -det;
                    }
                    return det;
                }
                return 0;
            }

            /*!
            \brief calculates the adjoint of the given matrix
            \param m source matrix (matrix<T>)
            \return (true, false)
            */
            bool adjoint(const matrix<T> &m)
            {
                matrix<T> &This = *this;
                size_t rows = m.rows(), columns = m.columns();
                if (rows == 2 && columns == 2)
                {
                    This.resize(rows, columns);
                    This[0][0] =  m[1][1];
                    This[1][1] =  m[0][0];
                    This[0][1] = -m[1][0];
                    This[1][0] = -m[0][1];
                    return true;
                }
                else if (rows > 2 && m.is_square())
                {   // rule of cramer
                    This.resize(rows, columns);
                    matrix<T> sub;
                    size_t row, col, sum;
                    double det;
                    for (row = 0; row < rows; row++)
                    {
                        for (col = 0; col < columns; col++)
                        {
                            m.get_sub_matrix(row, col, sub);
                            det = sub.det();
                            sum = row + col;
                            if (sum & 1) This[col][row] = -det;
                            else         This[col][row] =  det;
                        }
                    }
                    return true;
                }
                return false;
            }

            /*!
            \brief calculates the inverse of the given matrix and saves result in calling object
            \param m source matrix (matrix<T>)
            \return (true, false)
            */
            bool invert(const matrix<T> &m)
            {
                matrix<T> &This = *this;
                if (This.adjoint(m))
                {
                    This /= m.det();
                    return true;
                }
                return false;
            }

            /*!
            \brief calculates the transpose of the given matrix and saves result in calling object
            \param m source matrix (matrix<T>)
            \return (true, false)
            */
            bool transpose(const matrix<T> &m)
            {
                matrix<T> &This = *this;
                if (&m == this) return false;
                size_t i, j, rows = m.rows(), columns = m.columns();
                This.resize(columns, rows);
                for (i=0; i<columns; i++)
                {
                    for (j=0; j<rows; j++)
                    {
                        This[i][j] = m[j][i];
                    }
                }
                return true;
            }

            /*!
            \brief calculates the transpose of the given vector and saves result in calling object
            \param aV source vector (matrix<T>)
            \return (true, false)
            */
            bool transpose(const std::vector<T> &aV)
            {
                matrix<T> &This = *this;
                size_t i, size = aV.size();
                This.resize(size, 1);
                for (i=0; i<size; i++)
                {
                    This[i][0] = aV[i];
                }
                return true;
            }

            /*!
            \brief prints matrix content
            \param os output stream (ostream)
            \return output stream
            */
            std::ostream& print(std::ostream& os) const
            {
                const matrix<T>& m = *this;
                size_t row, rows = m.rows();
                os << "[" << std::endl;
                for (row = 0; row < rows; row++)
                {
                    os << " " << m[row];
                    os << std::endl;
                }
                os << "]";
                return os;
            }

            /*!
            \brief multiplies 2 matrices
            \note columns of m1 must be equal to rows of m2
            \param m1 matrix 1 (const matrix<T>&)
            \param m2 matrix 2 (const matrix<T>&)
            \return new product matrix (matrix<T>)
            */
            friend const matrix<T> operator * (const matrix<T> &m1, const matrix<T>& m2)
            {
                matrix<T> mat;
                mat.multiply(m1, m2);
                return mat;
            }

            /*!
            \brief multiplies 2 matrices and saves result in calling object
            \note columns of m1 must be equal to rows of m2
                  M(r,c) = Sum(i=1,N){ M1(r,i) * M2(i,c) }
            \param m1 matrix 1 (const matrix<T>&)
            \param m2 matrix 2 (const matrix<T>&)
            */
            void multiply(const matrix<T> &m1, const matrix<T>& m2)
            {
                size_t rows=0, cols=0, row, col, i, ni;
                size_t rows1 = m1.rows()   , rows2 = m2.rows();
                size_t cols1 = m1.columns(), cols2 = m2.columns();
                matrix<T> &mat = *this;
                if (cols1 == rows2)
                {
                    rows = rows1;
                    cols = cols2;
                }
                ni = cols1;

                mat.resize(rows, cols);
                T val = 0;
                mat.assign(&val);
                for (row = 0; row < rows; ++row)
                {
                    for (col = 0; col < cols; ++col)
                    {
                        for (i = 0; i < ni; ++i)
                        {
                            mat[row][col] += (m1[row][i] * m2[i][col]);
                        }
                    }
                }
            }

            VectorX::vectorND<T> transformX(VectorX::vectorND<T>& aInput)
            {
                VectorX::vectorND<T> fResult;
                if (is_square())
                {
                    if (aInput.size() == rows()-1)
                    {
                        matrix<T> &mat = *this;
                        size_t fSize = aInput.size();
                        fResult.resize(fSize);
                        for (size_t row = 0; row < fSize; ++row)
                        {
                            fResult[row] = mat[row][fSize];
                            for (size_t col = 0; col < fSize; ++col)
                            {
                                fResult[row] += mat[row][col] * aInput[col];
                            }
                        }
                    }
                    else
                    {
                         throw std::out_of_range("input vector must have size n-1 of matrix");
                    }
                }
                else
                {
                     throw std::out_of_range("transformX: matrix must be square");
                }
                return fResult;
            }

            /*!
            \brief matrix addition
            \note rows and columns must have the same size
            \param m1 matrix 1
            \param m2 matrix 2
            \return new sum matrix (matrix<T>)
            */
            friend const matrix<T> operator + (const matrix<T> &m1, const matrix<T>& m2)
            {
                matrix<T> mat;
                mat.add(m1, m2);
                return mat;
            }
            /*!
            \brief matrix addition
            \note rows and columns must have the same size
            \param m2 matrix 2
            */
            void operator += (const matrix<T>& m2)
            {
                add(*this, m2);
            }
            /*!
            \brief matrix  addition of two matrixes, saves result in calling object
            \note rows and columns must have the same size
            \param m1 matrix 1
            \param m2 matrix 2
            */
            void add(const matrix<T> &m1, const matrix<T>& m2)
            {
                size_t rows1 = m1.rows();
                size_t cols1 = m1.columns();
                matrix<T> &mat = *this;
                if (rows1 == m2.rows() && cols1 == m2.columns())
                {
                    mat.resize(rows1, cols1, true);
                    std::transform(m1.matrix_begin(), m1.matrix_end(), m2.matrix_begin(), mat.matrix_begin (), std::plus<T>());
                }
                else
                {
                    throw std::out_of_range("add: matrices must have the same size");
                }
            }

            /*!
            \brief matrix subtraction
            \note rows and columns must have the same size
            \param m2 matrix 2
            */
            void operator -= (const matrix<T>& m2)
            {
                subtract(*this, m2);
            }

            /*!
            \brief matrix subtraction of two matrixes, saves result in calling object
            \note rows and columns must have the same size
            \param m1 matrix 1
            \param m2 matrix 2
            \return new difference matrix (matrix<T>)
            */
            friend const matrix<T> operator - (const matrix<T> &m1, const matrix<T>& m2)
            {
                matrix<T> mat;
                mat.subtract(m1, m2);
                return mat;
            }
            /*!
            \brief matrix subtraction of two matrixes, saves result in calling object
            \note rows and columns must have the same size
            \param m1 matrix 1
            \param m2 matrix 2
            */
            void subtract(const matrix<T> &m1, const matrix<T>& m2)
            {
                size_t rows1 = m1.rows();
                size_t cols1 = m1.columns();
                matrix<T> &mat = *this;
                if (rows1 == m2.rows() && cols1 == m2.columns())
                {
                    mat.resize(rows1, cols1, true);
                    std::transform(m1.matrix_begin(), m1.matrix_end(), m2.matrix_begin(), mat.matrix_begin (), std::minus<T>());
                }
                else
                {
                    throw std::out_of_range("subtract: matrices must have the same size");
                }
            }

            /*!
            \brief multiplies a value to this matrix elements
            \param value multiplicator for the matrix elements (T)
            */
            void operator *= (T value)
            {
                multiply(*this, value);
            }
            /*!
            \brief multiplies a value to the matrix elements
            \param m matrix
            \param value (T)
            \return new product matrix (matrix<T>)
            */
            friend const matrix<T> operator * (const matrix<T> &m, T value)
            {
                matrix<T> mat;
                mat.multiply(m, value);
                return mat;
            }
            /*!
            \brief multiplies a value to the matrix elements
            \param m matrix
            \param value (T)
            \return new product matrix (matrix<T>)
            */
            friend const matrix<T> operator * (T value, const matrix<T> &m)
            {
                return m * value;
            }
            /*!
            \brief multiplies a value to the matrix elements and saves result in calling object
            \param m matrix
            \param value (T)
            */
            void multiply(const matrix<T> &m, T value)
            {
                this->resize(m.rows(), m.columns());
                std::transform(m.matrix_begin(), m.matrix_end(), this->matrix_begin(), Helper::Transform::multiply_value<T>(value));
            }

            /*!
            \brief adds a value to this matrix elements
            \param value summand for the matrix elements (T)
            */
            void operator += (T value)
            {
                add(*this, value);
            }
            /*!
            \brief adds a value to the matrix elements
            \param m matrix
            \param value (T)
            \return new sum matrix (matrix<T>)
            */
            friend const matrix<T> operator + (const matrix<T> &m, T value)
            {
                matrix<T> mat;
                mat.add(m, value);
                return mat;
            }
            /*!
            \brief adds a value to the matrix elements
            \param m matrix
            \param value (T)
            \return new sum matrix (matrix<T>)
            */
            friend const matrix<T> operator + (T value, const matrix<T> &m)
            {
                return m + value;
            }

            /*!
            \brief adds a value to the matrix elements and saves result in calling object
            \param m matrix
            \param value (T)
            */
            void add(const matrix<T> &m, T value)
            {
                this->resize(m.rows(), m.columns());
                std::transform(m.matrix_begin(), m.matrix_end(), this->matrix_begin(), Helper::Transform::add_value<T>(value));
            }

            /*!
            \brief subtracts a value from the matrix elements
            \param m source matrix (const matrix<T>)
            \param value minuent for the matrix elements (T)
            \return new difference matrix (matrix<T>)
            */
            friend const matrix<T> operator - (const matrix<T> &m, T value)
            {
                matrix<T> mat;
                mat.subtract(m, value);
                return mat;
            }
            /*!
            \brief subtracts a value from this matrix elements
            \param value minuent for the matrix elements (T)
            */
            void operator -= (T value)
            {
                subtract(*this, value);
            }
            /*!
            \brief subtracts a value from the matrix elements and saves result in calling object
            \param m source matrix (const matrix<T>)
            \param value minuent for the matrix elements (T)
            */
            void subtract(const matrix<T> &m, T value)
            {
                this->resize(m.rows(), m.columns());
                value = -value;
                std::transform(m.matrix_begin(), m.matrix_end(), this->matrix_begin(), Helper::Transform::add_value<T>(value));
            }

            /*!
            \brief divides this matrix elements by a value
            \param value (T) divident
            */
            void operator /= (T value)
            {
                divide(*this, value);
            }
            /*!
            \brief divides the matrix elements by a value
            \param m matrix
            \param value (T) divident
            \return new quotient matrix (matrix<T>)
            */
            friend const matrix<T> operator / (const matrix<T> &m, T value)
            {
                matrix<T> mat;
                mat.divide(m, value);
                return mat;
            }
            /*!
            \brief divides the matrix elements by a value and saves result in calling object
            \param m matrix
            \param value (T) divident
            */
            void divide(const matrix<T> &m, T value)
            {
                if (fabs(value) > std::numeric_limits<T>::epsilon())
                {
                    value = 1.0 / value;
                    this->resize(m.rows(), m.columns());
                    std::transform(m.matrix_begin(), m.matrix_end(), this->matrix_begin(), Helper::Transform::multiply_value<T>(value));
                }
                else
                {
                    throw std::domain_error("division by zero");
                }
            }

            /*!
            \brief creates a 4x4 rotation matrix around the x axis
            \param angle rotation angle in rad (T)
            \param system matrix system default: right hand system (right, left)
            */
            void rotate_x(T angle, eSystem system=right)
            {
                matrix<T> &mat = *this;
                identity(4);
                T sine   = sin(angle);
                T cosine = cos(angle);
                if (system == left) sine = -sine;
              /*mat[0][0] = 1,   mat[0][1] = 0,      mat[0][2] = 0; */
              /*mat[1][0] = 0,*/ mat[1][1] = cosine, mat[1][2] = -sine;
              /*mat[2][0] = 0,*/ mat[2][1] = sine,   mat[2][2] = cosine;
            }

            /*!
            \brief creates a 4x4 rotation matrix around the y axis
            \param angle rotation angle in rad (T)
            \param system matrix system default: right hand system (right, left)
            */
            void rotate_y(T angle, eSystem system=right)
            {
                matrix<T> &mat = *this;
                identity(4);
                T sine   = sin(angle);
                T cosine = cos(angle);
                if (system == left) sine = -sine;
                mat[0][0] = cosine,/* mat[0][1] = 0,*/ mat[0][2] = -sine;
              /*mat[1][0] = 0,        mat[1][1] = 1,   mat[1][2] = 0 */
                mat[2][0] = sine,  /* mat[2][1] = 0,*/ mat[2][2] = cosine;
            }

            /*!
            \brief creates a 4x4 rotation matrix around the z axis
            \param angle rotation angle in rad (T)
            \param system matrix system default: right hand system (right, left)
            */
            void rotate_z(T angle, eSystem system=right)
            {
                matrix<T> &mat = *this;
                identity(4);
                T sine   = sin(angle);
                T cosine = cos(angle);
                if (system == left) sine = -sine;
                mat[0][0] = cosine, mat[0][1] = -sine; /* mat[0][2] = 0 */
                mat[1][0] = sine,   mat[1][1] = cosine;/* mat[1][2] = 0 */
              /*mat[2][0] = 0,      mat[2][1] = 0,        mat[2][2] = 1;*/
            }

            /*!
            \brief creates a 4x4 rotation matrix around an arbitrary unit vector
            \param aV vector as rotation axis (const vector<T>&)
            \param angle rotation angle in rad (T)
            \param system matrix system default: right hand system (right, left)
            */
            void rotate(const std::vector<T>&aV, T angle, eSystem system = right)
            {
                matrix<T> &mat = *this;
                if (aV.size() != 3)
                {
                    throw std::out_of_range("rotation vector must be size 3");
                }
                VectorX::vectorND<T> v(aV);
                v.norm();
                identity(4);
                T sine   = sin(angle);
                T cosine = cos(angle);
                if (system == left) sine = -sine;
                mat[0][0] = v[0] * v[0] * (1 - cosine) + cosine;
                mat[0][1] = v[0] * v[1] * (1 - cosine) - v[2] * sine;
                mat[0][2] = v[0] * v[2] * (1 - cosine) + v[1] * sine;

                mat[1][0] = v[1] * v[0] * (1 - cosine) + v[2] * sine;
                mat[1][1] = v[1] * v[1] * (1 - cosine) + cosine;
                mat[1][2] = v[1] * v[2] * (1 - cosine) - v[0] * sine;

                mat[2][0] = v[2] * v[0] * (1 - cosine) - v[1] * sine;
                mat[2][1] = v[2] * v[1] * (1 - cosine) + v[0] * sine;
                mat[2][2] = v[2] * v[2] * (1 - cosine) + cosine;
            }

            /*!
            \brief creates a (n+1) x (n+1) translation matrix
            \param translation translation vector (const vector<T>&)
            */
            void translate(const std::vector<T>& translation)
            {
                matrix<T> &mat = *this;
                size_t i, size = translation.size()+1;
                identity(size);
                size--;
                for (i=0; i<size; i++)
                {
                    mat[i][size] = translation[i];
                }
            }

            /*!
            \brief creates a (n+1) x (n+1) scale matrix
            \param scale_vec scale vector (const vector<T>&)
            */
            void scale(const std::vector<T>& scale_vec)
            {
                matrix<T> &mat = *this;
                size_t i, size = scale_vec.size()+1;
                identity(size);
                size--;
                for (i=0; i<size; i++)
                {
                    mat[i][i] = scale_vec[i];
                }
            }

            /*!
            \brief creates a (n+1) x (n+1) scale matrix
            \param shear_vec scale vector (vector<T>)
            */
            void shear(const std::vector<T>& shear_vec)
            {
                matrix<T> &mat = *this;
                size_t i, j, size = shear_vec.size()+1;
                identity(size);
                size--;
                for (i=0; i<size-1; i++)
                {
                    for (j=0; j<size; j++)
                    {
                        mat[j][(i+1)%size] = shear_vec[j];
                    }
                }
            }

            /*!
            \brief creates a 4x4 perspective projection matrix
            */
            void perspective_projection()
            {
                matrix<T> &mat = *this;
                identity(4);
                mat[3][2] = mat[3][3];
                mat[3][3] = 0;
            }

            /*!
            \brief creates a 4x4 ortographic projection matrix
            \param left left side of representation volume (T)
            \param right right side of representation volume (T)
            \param top top of representation volume (T)
            \param bottom bottom of representation volume (T)
            \param far point of representation volume (T)
            \param near point of representation volume (T)
            */
            void ortographic_projection(T aLeft, T aRight, T aTop, T aBottom, T aFar, T aNear)
            {
                matrix<T> &mat = *this;
                resize(4, 4, true);
                mat[0][0] = 2 / (aRight-aLeft);
                mat[1][1] = 2 / (aTop-aBottom);
                mat[2][2] = 2 / (aFar-aNear);
                mat[3][3] = 1;
                mat[0][3] = (aRight+aLeft) / (aRight-aLeft);
                mat[1][3] = (aTop+aBottom) / (aTop-aBottom);
                mat[2][3] = (aFar+aNear)   / (aFar-aNear);
            }

            /*!
            \brief writes the matrix content row by row to file
            \param pf file pointer (FILE*)
            \param startr first row written (size_t)
            \param endr last row written (size_t)
            \param startc first column written (size_t)
            \param endc last column written (size_t)
            \return output stream
            */
            bool write_matrix(FILE*pf, bool transpose=false, size_t startr=0, size_t endr=0, size_t startc=0, size_t endc=0)
            {
                const matrix<T> &m = *this;
                size_t r, rows = m.rows(), columns = m.columns();
                // check row output
                if (startr >= rows) return false;
                if (endr == 0) endr = rows - startr;
                if (endr <= startr) return false;
                // check column output
                if (startc >= columns) return false;
                if (endc == 0) endc = columns - startc;
                if (endc <= startc) return false;
                rows = endr-startr;
                columns= endc-startc;
                if (transpose)
                {
                    fwrite(&columns, sizeof(rows), 1, pf);
                    fwrite(&rows, sizeof(rows), 1, pf);
                }
                else
                {
                    fwrite(&rows, sizeof(rows), 1, pf);
                    fwrite(&columns, sizeof(rows), 1, pf);
                }
                for (r=startr; r<endr; r++)
                {
                    fwrite(&m[r][startc], sizeof(T), endc-startc, pf);
                }
                return true;
            }

            /*!
            \brief read the matrix content row by row from file
            \param pf file pointer (FILE*)
            \return output stream
            */
            bool read_matrix(FILE*pf, bool transpose=false)
            {
                matrix<T> &m = *this;
                size_t r, rows, columns;
                try
                {
                    if (transpose)
                    {
                        if (fread(&columns, sizeof(rows), 1, pf) == 0) throw errno;
                        if (fread(&rows, sizeof(rows), 1, pf) == 0)    throw errno;
                    }
                    else
                    {
                        if (fread(&rows, sizeof(rows), 1, pf) == 0)    throw errno;
                        if (fread(&columns, sizeof(rows), 1, pf) == 0) throw errno;
                    }
                    m.resize(rows, columns);
                    for (r=0; r<rows; r++)
                    {
                        if (fread(&m[r][0], sizeof(T), columns, pf) == 0) throw errno;
                    }
                }
                catch (int )
                {
                    return false;
                }
                return true;
            }

            /*!
            \brief prints the matrix content
            \param os output stream (ostream&)
            \param m matrix (matrix<T>&)
            \return output stream
            */
            friend std::ostream& operator<< (std::ostream& os, const matrix<T> &m)
            {
                return m.print(os);
            }
        };

        /*!
        \brief applies gaussian method of least sqares to the given matrix
        \param aIn matrix to multiply with its own transposed
        \param aOut resulting square matrix
        \return nothing
        */
        template<class T>
        void gauss_least_square(const matrix<T> &aIn, matrix<T>& aOut)
        {
            size_t n = aIn.rows(), m = aIn.columns();
            size_t i, j, k;
            aOut.resize(m, m);
            for (i=0; i<m; i++)
            {
                for (j=i; j<m; j++)
                {
                    aOut[i][j] = aIn[0][i]*aIn[0][j];
                }
            }

            for (i=0; i<m; i++)
            {
                for (j=i; j<m; j++)
                {
                    for (k=1; k<n; k++)
                        aOut[i][j] += aIn[k][i]*aIn[k][j];
                    aOut[j][i] = aOut[i][j];
                }
            }
        }

        /*!
        \brief applies gaussian method of least sqares to matrix and vector
        \param mat matrix that is multiplied as transposed (const matrix<T>&)
        \param vec vector to multiply with transposed of matrix equ (const vector<T>&)
        \param result resulting vector of multiplication (vector<T>&)
        \return (true, false)
        */
        template<class T>
        bool gauss_least_square(const matrix<T> &mat, const std::vector<T>& vec, std::vector<T>& result)
        {
            size_t n = mat.rows(), m = mat.columns();
            size_t i, j;
            if (vec.size() == n)
            {
                result.resize(m);
                for (i=0; i<m; i++)
                {
                    result[i] = mat[0][i] * vec[0];
                    for (j=1; j<n; j++)
                    {
                        result[i] += mat[j][i]*vec[j];
                    }
                }
            }
            return false;
        }
        /*!
        \brief solves this equation by the gaussian elimination method
        \note the matrix has to be a square matrix
        \note the number of rows and columns have to be equal to the size of the vectors
        \note the matrix and the result vector is modified
        \param mat matrix with equation elements (matrix<T>)
        \param vres result vector of the equation (vector<T>)
        \param solution solution of the equation (vector<T>)
        \return (true, false)
        */
        template<class T>
        bool gauss_elimination(matrix<T>& mat, std::vector<T>& vres, std::vector<T>& solution)
        {
            typedef typename std::vector<T>::iterator row_iterator;
            typedef typename std::vector<T>::const_iterator const_row_iterator;

            int         i,j,k,n;
            int         pivot_row, length;
            std::vector<int> index;
            T           pivot, factor;
            const_row_iterator ptr_from;
            row_iterator ptr_to, ptr_pivot;
            bool        bReturn = false;
            long double depsilon = fabs(std::numeric_limits<T>::epsilon());
            long double abs_pivot;
            if (depsilon == 0.0)
            {
                depsilon = std::numeric_limits<double>::epsilon();
            }

            n = (int) mat.rows();
            if (n == 0) return false;
            if (n != (int)mat.columns()) return false;
            if (n != (int)vres.size()) return false;

            index.resize(n, 0);
            std::transform(index.begin(), index.end(), index.begin(), Helper::Transform::gradient<int>());
            solution.resize(n);

            for (i=0;i<n;i++)
            {
                pivot      = mat[index[i]][i];
                abs_pivot  = fabs(pivot);
                pivot_row = i;

                for (j=i+1;j<n;j++)                         // search pivot row
                {
                    if (fabs(mat[index[j]][i])>abs_pivot)
                    {
                        pivot      = mat[index[j]][i];
                        abs_pivot  = fabs(pivot);
                        pivot_row = j;
                    }
                }

                if (i!=pivot_row)                          // if other pivot element found
                {
                    std::swap(index[i], index[pivot_row]);
                }

                if (abs_pivot < depsilon) break;         // equation is not solveable

                pivot  = (T)(1.0) / pivot;
                length = n-i-1;
                if (length)
                {
                    ptr_pivot = mat[index[i]].begin()+i+1;
                }

                for (j=i+1;j<n;j++)
                {
                    ptr_to = mat[index[j]].begin()+i;
                    if (fabs(*ptr_to)!=0.0)
                    {
                        factor  = *ptr_to++ * pivot;
                        ptr_from = ptr_pivot;
                        for (k=0;k<length;k++)
                        {
                            *ptr_to++ -= factor* *ptr_from++;
                        }
                        vres[index[j]] -= factor*vres[index[i]];
                    }
                }
            }

            if (i==n)
            {
                for(i=n-1;i>=0;i--)
                {
                    vres[index[i]] = factor = vres[index[i]]/mat[index[i]][i];
                    for (j=i-1;j>=0;j--)
                    {
                        vres[index[j]] -= factor * mat[index[j]][i];
                    }
                }

                for (i=0; i<n; i++)
                {
                    solution[i] = vres[index[i]];
                }
                bReturn = true;
            }

            return bReturn;
        }

        /*!
        \brief solves this equation by the gauss seidel method
        \note the matrix has to be a square matrix<br>
              the number of rows and columns have to be equal to the size of the vectors
        \param aA matrix with equation elements (matrix<T>)
        \param aB result vector of the equation (vector<T>)
        \param aX solution of the equation (vector<T>)
        \param [aAccuracy] desired accuracy of the solution (T)
        \param [aIterations] maximum number of iterations (size_t)
        \return number of iterations (size_t)
        */
        template<class T>
        size_t calc_gauss_seidel(const matrix<T> &aA, const std::vector<T> &aB, std::vector<T>&aX, T aAccuracy=0, size_t aIterations=0)
        {
            size_t rows = aA.rows(), columns = aA.columns(), row, col, k=0;
            T fSum, fTmp, fDiff;
            aX.resize(rows, 0.0);
            if (aIterations == 0) aIterations = rows*2;
            do
            {
                fDiff = 0;
                for (row=0; row<rows; ++row)
                {
                    fSum = 0;
                    for (col=0; col<row; ++col)
                    {
                        fSum += aA[row][col] * aX[col];
                    }
                    for (col=row+1; col<columns; ++col)
                    {
                        fSum += aA[row][col] * aX[col];
                    }
                    fTmp    = (aB[row] - fSum) / aA[row][row];
                    fDiff  += fabs(aX[row] - fTmp);
                    aX[row] = fTmp;
                }
            } while(k++<aIterations && fDiff > aAccuracy);
            return k;
        }

        /*!
        \brief calculates a polynome to fit a curve given by x and y values
               value = p[0]*x^0+p[1]*x^1+p[2]*x^2+...+p[n]*x^n
        \param xarray input array with x values (const vector<T>&)
        \param yarray input array with y values (const vector<T>&)
        \note  the number of input values must be greater than the polynome order
        \param polynome resulting polynome as values from order 1 to 10
        \note  the size of the polynome vector determines the order of the polynome (order n = size-1)
        \param start start index within x and y arrays [optional, default: 0] (size_t)
        \param size length from start index within x and y arrays [optional, default: xarray.size()] (size_t)
        \return function succeeded (true, false)
        */
        template<class T>
        bool fitCurveToPolynome(const std::vector<T> & aXarray, const std::vector<T> & aYarray, std::vector<T>& aPolynome, size_t aStart = 0, size_t aSize = 0)
        {
            size_t m = aPolynome.size();
            if (aSize == 0)
            {
                aSize = aXarray.size();
            }
            if (m == 1 && aSize >= m)    // simple average
            {
                T sum = 0;
                size_t j;
                for (j=0; j<aSize; j++)
                {
                    sum += aYarray[j+aStart];
                }
                aPolynome[0] = sum / aSize;
            }
            else if (aSize >= m)         // fit polynome of order m
            {
                size_t i, j, n = aSize;
                matrix<T> mat(n, m), mls;
                std::vector<T> vls, reduced;
                for (i=0;i<m; i++)   // fill [over determined] equation
                {
                    for (j=0; j<n; j++)
                    {
                        if (i==0) mat[j][i] = 1.0;
                        else      mat[j][i] = mat[j][i-1]*aXarray[j+aStart];
                    }
                }
                if (aSize == m)
                {
                    vls.assign(aYarray.begin()+aStart, aYarray.begin()+aStart+aSize);
                    return gauss_elimination(mat, vls, aPolynome);
                }
                else // if over determined
                {
                    gauss_least_square(mat, mls);
                    gauss_least_square(mat, std::vector<T>(aYarray.begin()+aStart, aYarray.begin()+aStart+aSize), vls);
                    return gauss_elimination(mls, vls, aPolynome);
                }
            }
            return false;
        }

        /*!
        \brief calculates a polynome to fit a curve given by x and y values
        \example value = p[0]*x^0+p[1]*x^1+p[2]*x^2+...+p[n]*x^n
        \param aXfirst input array with x values (const vector<T>&)
        \param aXlast input array with y values (const vector<T>&)
        \param aYfirst start index within x and y arrays (size_t)
        \param aYlast length from start index within x and y arrays (size_t)
        \note  the number of input values must be greater than the polynome order n
        \param polynome resulting polynome as values from order 1 to 10
        \note  the size of the polynome vector determines the order of the polynome (order n = size-1)
        \return function succeeded (true, false)
        */
        template<class _InIt1, class _InIt2, class T>
        bool fitCurveToPolynome(_InIt1 aXfirst, _InIt1 aXlast, _InIt2 aYfirst, _InIt2 aYlast, std::vector<T>&polynome)
        {
            size_t m    = polynome.size();
            size_t size = aXlast - aXfirst;
            if (m == 1 && size >= m)    // simple average
            {
                T sum = 0;
                for (; aYfirst != aYlast; ++aYfirst)
                {
                    sum += *aYfirst;
                }
                polynome[0] = sum / size;
            }
            else if (size >= m)         // fit polynome of order m
            {
                size_t i, j, n = size;
                matrix<T> mat(n, m), mls;
                std::vector<T> vls, reduced;

                for (i=0;i<m; i++)   // fill [over determined] equation
                {
                    _InIt1 aXit = aXfirst;
                    for (j=0; aXit != aXlast; ++aXit, ++j)
                    {
                        if (i==0) mat[j][i] = 1.0;
                        else      mat[j][i] = mat[j][i-1]* *aXit;
                    }
                }
                if (size == m)
                {
                    std::vector<T> fTmp(aYfirst, aYlast);
                    return gauss_elimination(mat, fTmp, polynome);
                }
                else // if over determined
                {
                    gauss_least_square(mat, mls);
                    gauss_least_square(mat, std::vector<T>(aYfirst, aYlast), vls);
                    return gauss_elimination(mls, vls, polynome);
                }
            }
            return false;
        }

    } // namespace Matrix

} // namespace Algorithmics

#endif // _MATRIX_H_INCLUDED_
