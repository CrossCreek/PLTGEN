/*=============================================================================================
 * CLASSIFICATION: Unclassified
 * Government Purpose Rights
 * Contract No: 12-C-0158
 * Contractor Name: Perspecta
 * Contractor Address: 15052 Conference Center Dr. Chantilly, VA 20151
 * Expiration Date: 31 Aug. 2023
 * The Government's rights to use, modify, reproduce, release, perform, display, or disclose these
 * technical data and computer software are restricted by paragraph (b)(1) of clause N52.227-002,
 * Rights in Technical Data and Computer Software: Noncommercial Items, contained in the contract
 * identified above. No restrictions apply after the expiration date shown above. Any reproduction of
 * technical data or computer software, or portions thereof marked with this legend,must also reproduce the markings.
 *
 *
 * MatrixBase.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Generic 2-D matrix template class
// Refer to MatrixBase.h for more info.
// =============================================================================
#include "MatrixBase.h"
#include "IoSeparator.h"
#include "VectorCartesian.h"
#include "Angle.h"
#include <math.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
namespace MatrixExcept
{
namespace
{
void ThrowBadLen(const std::string &who, std::string type,
                 size_type szNeeded, size_type szProvided)
{
    std::ostringstream oss;
    oss << who << "\nBad "<< type << " count.\nSize needed: "
        << szNeeded << "\nSize provided: "
        << szProvided << '\n';
    throw std::range_error(oss.str());
}
}

void ThrowBadRowLen(const std::string &who,
                    size_type szNeeded, size_type szProvided)
{
    ThrowBadLen(who, "row", szNeeded, szProvided);
}

void ThrowBadColLen(const std::string &who,
                    size_type szNeeded, size_type szProvided)
{
    ThrowBadLen(who, "column", szNeeded, szProvided);
}

void ThrowLengthMismatch(const std::string &who,
                         size_type szRow, size_type szCol,
                         difference_type dist)
{
    std::ostringstream oss;
    oss << who <<"\nLength mismatch\nRows specified: " << szRow
        << "\nColumns specified: " << szCol
        << "\n# elements provided: " << dist << '\n';
    throw std::range_error(oss.str());
}

void ThrowNeedRowsOrCols(const std::string &who)
{
    throw std::range_error(who
                           + "\nRow size and column size must not both be 0!");
}
}

// ============================================================================
// constructors & destructors
template <class T>
MatrixBase<T>::MatrixBase()
    : m_pMtx(NULL), m_nRow(0), m_nCol(0), m_capacity(0), m_pScr(NULL), m_szScr(0)
{ ; }

//------------------------------------------------------------------------------
template <class T>
MatrixBase<T>::MatrixBase(const MatrixBase& mtx)
    : m_pMtx(NULL), m_nRow(mtx.m_nRow), m_nCol(mtx.m_nCol),
      m_capacity(mtx.m_capacity), m_pScr(NULL), m_szScr(0)
{
    if (m_capacity) {
        m_pMtx = new T[ m_capacity ];
        std::copy(mtx.begin(), mtx.end(), m_pMtx);

        // don't need to copy scratch values
        if (mtx.IsScratchReserved()) {
            ReserveScratch();
        }
    }
}

//------------------------------------------------------------------------------
template <class T>
MatrixBase<T>::MatrixBase(size_type nRows, size_type nCols)
    : m_pMtx(NULL), m_nRow(nRows), m_nCol(nCols),
      m_capacity(nRows * nCols), m_pScr(NULL), m_szScr(0)
{
    m_pMtx = new T[ m_capacity ];
}

//------------------------------------------------------------------------------
template <class T>
MatrixBase<T>::MatrixBase(size_type nRows, size_type nCols, T value)
    : m_pMtx(NULL), m_nRow(nRows), m_nCol(nCols),
      m_capacity(nRows * nCols), m_pScr(NULL), m_szScr(0)
{
    m_pMtx = new T[ m_capacity ];
    std::fill(begin(), end(), value);
}

//------------------------------------------------------------------------------
template <class T>
MatrixBase<T>::~MatrixBase()
{
    if(m_pMtx != NULL) {
        delete[] m_pMtx;
    }

    if(m_pScr != NULL) {
        delete[] m_pScr;
    }
}

//==============================================================================
// functions
template <class T>
typename MatrixBase<T>::size_type
MatrixBase<T>::Reserve(size_type capacity)
{
    size_type oldCap = m_capacity;
    size_type nElem = m_nRow * m_nCol;

    if (capacity < nElem) {
        capacity = nElem;
    }

    if (capacity != m_capacity) {
        T* pNewMtx = NULL;
        // m_capacity = capacity is not a mistake
        // moved from conditional statement to suppress warnings
        m_capacity = capacity;

        if (m_capacity) {
            pNewMtx = new T[ m_capacity ];
            std::copy(m_pMtx, m_pMtx + nElem, pNewMtx);
        }

        delete m_pMtx;
        m_pMtx = pNewMtx;

        if (IsScratchReserved()) {
            ReserveScratch();    // resize scratch
        }
    }

    return oldCap;
}

//------------------------------------------------------------------------------
template <class T>
typename MatrixBase<T>::size_type
MatrixBase<T>::ReserveRows(size_type nRows)
{
    size_type capacity = nRows * m_nCol;
    return Reserve(capacity);
}

//------------------------------------------------------------------------------
template <class T>
void MatrixBase<T>::Clear()
{
    m_nRow  = 0;
}

//------------------------------------------------------------------------------
template <class T>
void MatrixBase<T>::ReserveScratch() const
{
    static const int MAX_WASTE_FACTOR = 2;

    if (m_szScr < m_capacity || m_szScr > MAX_WASTE_FACTOR * m_capacity) {
        m_szScr = m_capacity;
        delete m_pScr;
        m_pScr = m_szScr ? new T[ m_szScr ] : NULL;
    }
}

//------------------------------------------------------------------------------
template <class T>
void MatrixBase<T>::ReleaseScratch() const
{
    if (m_pScr) {
        delete m_pScr;
        m_pScr = NULL;
        m_szScr = 0;
    }
}

//------------------------------------------------------------------------------
// destructive resize matrix, elements left uninitizalize
// to be used by derived classes
template <class T>
void MatrixBase<T>::Resize(size_type nRows, size_type nCols)
{
    static const int MAX_WASTE_FACTOR = 2;
    size_type nElem = nRows * nCols;
    m_nRow = nCols ? nRows : 0;
    m_nCol = nCols;

    if (m_capacity < nElem || m_capacity > MAX_WASTE_FACTOR * nElem) {
        m_capacity = nElem;
        delete m_pMtx;
        m_pMtx = m_capacity ? new T[ m_capacity ] : NULL;

        if (IsScratchReserved()) {
            ReserveScratch();    // resize scratch
        }
    }
}

//------------------------------------------------------------------------------
template <class T>
void MatrixBase<T>::GetTranspose(MatrixBase& mtxOut) const
{
    mtxOut.Resize(m_nCol, m_nRow);

    for (size_type i = 0; i < m_nRow; i++) {
        std::copy(begin_row(i), end_row(i), mtxOut.begin_column(i));
    }
}

//------------------------------------------------------------------------------
// V4.3-010
template <class T>
void MatrixBase<T>::TransposeThis()
{
    if (IsSquare()) {
        T t;
        //RAGUSA 4/30/2018 - Removed use of size_type i in favor of int i to
        //get rid of 64-bit compiler warning "possible data loss"
        int i = 0;
        //==================================================================
        iterator iRow, iCol, eRow;

        for (i = 0; i < m_nRow; i++) {
            iRow = begin_row(i) + i;
            eRow = end_row(i);
            iCol = begin_column(i) + i;

            for (; iRow != eRow; ++iRow, ++iCol) {
                t     = *iCol;
                *iCol = *iRow;
                *iRow = t;
            }
        }
    } else {
        MatrixBase<T> mtx(*this);
        mtx.GetTranspose(*this);
    }
}

//==============================================================================
// operators
template <class T>
MatrixBase<T>& MatrixBase<T>::operator=(const MatrixBase<T>& mtx)
{
    if (&mtx != this) {
        if (m_capacity != mtx.m_capacity) {
            m_capacity = mtx.m_capacity;
            delete m_pMtx;
            m_pMtx = m_capacity ? new T[ m_capacity ] : NULL;
        }

        m_nRow = mtx.m_nRow;
        m_nCol = mtx.m_nCol;
        std::copy(mtx.begin(), mtx.end(), m_pMtx);

        if (mtx.IsScratchReserved()) {
            ReserveScratch();
        }
    }

    return *this;
}

//------------------------------------------------------------------------------
template <class T>
bool MatrixBase<T>::operator==(const MatrixBase<T>& mtx) const
{
    return mtx.m_nRow == m_nRow
           && mtx.m_nCol == m_nCol
           && std::equal(begin(), end(), mtx.begin());
}

//------------------------------------------------------------------------------
template <class T>
bool MatrixBase<T>::operator!=(const MatrixBase& mtx) const
{
    return !operator==(mtx);
}

// ============================================================================
// insertion operator
template <class T>
std::ostream&
operator<<(std::ostream& os, const MatrixBase<T> &mtx)
{
    typedef typename MatrixBase<T>::size_type size_type;
    typedef typename MatrixBase<T>::const_iterator const_iterator;
    static const std::string newline("\n");
    std::string space(ioDelimGet(os));
    //WARNING_FIX:
    //1>src\MatrixBase.cpp(304): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'unsigned int', possible loss of data
    long long int w = os.width();
    size_type nCol  = mtx.size_column();
    size_type nElem = mtx.size();
    typename MatrixBase<T>::const_iterator itr = mtx.begin();

    for (size_type i = 0; i != nElem; i++) {
        os << std::setw(w) << itr[i]
           << (((i + 1) % nCol) ? space : newline);
    }

    return os;
}

// ============================================================================
// explicit instantiation
template class MatrixBase<double>;
template std::ostream& operator<<(std::ostream&, const MatrixBase<double>&);

template class MatrixBase<float>;
template std::ostream& operator<<(std::ostream&, const MatrixBase<float>&);

template class MatrixBase<char>;
template std::ostream& operator<<(std::ostream&, const MatrixBase<char>&);

template class MatrixBase<unsigned int>;
template std::ostream& operator<<(std::ostream&,
                                  const MatrixBase<unsigned int>&);

template class MatrixBase<int>;
template std::ostream& operator<<(std::ostream&, const MatrixBase<int>&);

template class MatrixBase<bool>;
template std::ostream& operator<<(std::ostream&, const MatrixBase<bool>&);

template class MatrixBase<VectorCartesian>;
template std::ostream& operator<<(std::ostream&,
                                  const MatrixBase<VectorCartesian>&);

template class MatrixBase<Angle>;
template std::ostream& operator<<(std::ostream&,
                                  const MatrixBase<Angle>&);

// ============================================================================
}
// A3 namespace end
// ============================================================================
