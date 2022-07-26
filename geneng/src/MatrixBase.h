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
 * MatrixBase.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : MatrixBase.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : SatTools

//  MODULE(s) : MatrixBase

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V0.1-001  2002-04-24  JAM  Alpha Release
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V3.3-012  05/13/2004  JJO  Use typename for .NET compiler

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: added explicit int to static const MAX_WASTE_FACTOR = 2;
//              to static const int MAX_WASTE_FACTOR = 2
//              Reason: implicit int is deprecated
//              Change: added typename to MatrixBase<T>::const_iterator
//              Reason: older compilers didn't support typename, newer compilers
//              require it
//              Change: removed all inline specifications
//              Reason: inline should not be in source files
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings
//              Change: deleted unused variable nElem
//              Reason: stops unused variable warning

//              V4.4-001  2008-06-02  JAM  Added TransposeThis() MatrixBase
//                                         mbr fctn

//              V4.5-001  2009-07-01  JAM  added standard typedef's
//                                         added protected memCopyIn fctn
//                                         added dimension_type to resolve
//                                         ambiguity in matching constructor
//                                         signiture with certain element types

//              V5.0      2010-01-27  JAM  replace old delimiter object with
//                                         the new & improved object in the
//                                         matrix overloaded insertion operator

//  NOTES : Generic 2-D matrix template class

//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA

//****************************************************************************//

//==============================================================================
#ifndef MATRIXBASE_H
#define MATRIXBASE_H __FILE__ " " __DATE__

//------------------------------------------------------------------------------
// required headers
#include "IteratorMatrix.h"
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <utility>
#include <memory.h>
#include <typeinfo>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// foward declaration
template<class T> class MatrixBase;

// i/o prototype
template<class T>
std::ostream& operator<<(std::ostream& os, const MatrixBase<T> &mtx);

// ============================================================================
namespace MatrixExcept
{
typedef size_t size_type;
typedef ptrdiff_t difference_type;

enum ThrowType { BADLEN, NEEDROWCOL, MISMATCH };

void ThrowBadRowLen(const std::string &who,
                    size_type szNeeded, size_type szProvided);

void ThrowBadColLen(const std::string &who,
                    size_type szNeeded, size_type szProvided);

void ThrowNeedRowsOrCols(const std::string &who);
void ThrowLengthMismatch(const std::string &who,
                         size_type szRow, size_type szCol,
                         difference_type dist);
};

// ============================================================================
template<class T>
class MatrixBase
{
public:
    //----------------------------------------------------------------------
    typedef IteratorMatrix<T> iterator;
    typedef ConstIteratorMatrix<T> const_iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T  value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef const T* const_pointer;
    typedef const T& const_reference;

    //----------------------------------------------------------------------
    // constructors & destructor
    MatrixBase();
    MatrixBase(const MatrixBase& mtx);

    // row x column matrix
    // uninitialized elements
    MatrixBase(size_type nRows, size_type nCols);

    // initialize all elements = element_value
    MatrixBase(size_type nRows, size_type nCols, T element_value);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // MatrixBase initialize with iterators
    // in class definition required by VC++
    // if nRows == 0 && nCols != 0 then nRows is computed
    // if nRows != 0 && nCols == 0 then nCols is computed
    // if nRows == 0 && nCols == 0 then throw exception
    template <class ITERATOR>
    MatrixBase(ITERATOR first, ITERATOR last,
               size_type nRows, size_type nCols)
        : m_pMtx(NULL), m_nRow(nRows), m_nCol(nCols),
          m_capacity(static_cast<size_type>(std::distance(first, last))),
          m_pScr(NULL), m_szScr(0)
    {
        try {
            if (0 == m_nRow && 0 == m_nCol) {
                throw MatrixExcept::NEEDROWCOL;
            }

            if (0 != m_capacity) {
                if (0 == nCols) {
                    m_nCol = m_capacity / m_nRow;
                } else if (0 == nRows) {
                    m_nRow = m_capacity / m_nCol;
                }

                if (m_capacity != m_nRow * m_nCol) {
                    throw MatrixExcept::MISMATCH;
                }

                m_pMtx = new T[ m_capacity ];
                std::copy(first, last, m_pMtx);
            } else if (m_nRow) {
                throw MatrixExcept::MISMATCH;
            }
        } catch (MatrixExcept::ThrowType e) {
            std::string who(typeid(*this).name());
            who += "MatrixBase::MatrixBase(ITERATOR first, ";
            who += "ITERATOR last, size_type nRows, size_type nCols)";

            if (MatrixExcept::NEEDROWCOL == e) {
                MatrixExcept::ThrowNeedRowsOrCols(who);
            } else {
                MatrixExcept::ThrowLengthMismatch(who, nRows, nCols,
                                                  m_capacity);
            }
        } catch (...) {
            throw;
        }
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    virtual ~MatrixBase();

    //----------------------------------------------------------------------
    // functions
    void Clear();  // nRows = 0
    size_type Reserve(size_type capacity);  // reserve space for N elements
    size_type ReserveRows(size_type nRows);  // reserve space for N rows

    void ReserveScratch() const; // reserve scratch space for operations
    void ReleaseScratch() const;  // release all scratch space
    bool IsScratchReserved() const;

    size_type GetSize() const;  // elements count
    size_type GetSizeRows() const; // row count
    size_type GetSizeColumns() const; // column count
    size_type GetCapacity() const;  // memory reserved for elements
    size_type GetCapacityRows() const;  // rows that may fit into capacity

    bool IsSquare() const;
    bool IsSquare(const size_type& sz) const;

    //----------------------------------------------------------------------
    // insert a new row at the end of the matrix
    // must be size_column long
    // in class definition required by VC++
    // applicable MatrixBaseOptions: RESERVE_SCRATCH
    template <class ITERATOR>
    size_type AddRow(ITERATOR first, ITERATOR last)
    {
        size_type nCol = std::distance(first, last);

        if (nCol != m_nCol) {
            std::string who(typeid(*this).name());
            who += "::AddRow(ITERATOR, ITERATOR)";
            MatrixExcept::ThrowBadColLen(who, m_nCol, nCol);
        }

        size_type nElem = (m_nRow+1) * m_nCol;

        if (m_capacity < nElem) {
            m_capacity = nElem;
            T* pNewMtx = new T[ m_capacity ];
            std::copy(begin(), end(), m_pMtx);
            delete m_pMtx;
            m_pMtx = pNewMtx;

            if (m_szScr) {
                ReserveScratch();
            }
        }

        copy(first, last, end());
        return ++m_nRow;
    }

    //----------------------------------------------------------------------
    // matrix transpose
    void GetTranspose(MatrixBase& mtxOut) const; // resizes mtxOut if needed
    void TransposeThis(); // transpose this matrix

    //----------------------------------------------------------------------
    // operators
    MatrixBase& operator=(const MatrixBase& mtx);

    bool operator==(const MatrixBase& mtx) const;
    bool operator!=(const MatrixBase& mtx) const;

    iterator operator[](size_type i)
    {
        return begin_row(i);
    }
    const_iterator operator[](size_type i) const
    {
        return begin_row(i);
    }

    //----------------------------------------------------------------------
    // stl like iterators
    iterator begin();
    iterator begin_row(size_type i);
    iterator begin_column(size_type i);
    iterator begin_diagonal();

    iterator end();
    iterator end_row(size_type i);
    iterator end_column(size_type i);
    iterator end_diagonal();

    const_iterator begin() const;
    const_iterator begin_row(size_type i) const;
    const_iterator begin_column(size_type i) const;
    const_iterator begin_diagonal() const;

    const_iterator end() const;
    const_iterator end_row(size_type i) const;
    const_iterator end_column(size_type i) const;
    const_iterator end_diagonal() const;

    size_type size() const;
    size_type size_row() const;
    size_type size_column() const;
    size_type size_diagonal() const;

    size_type capacity() const;
    size_type capacity_row() const;

    //----------------------------------------------------------------------
    // misc functions
    virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    //----------------------------------------------------------------------
    // stl like iterators for the scratch area
    // to be used by derived classes
    iterator begin_scratch() const ;
    iterator begin_row_scratch(size_type i) const ;
    iterator begin_column_scratch(size_type i) const ;
    iterator begin_diagonal_scratch() const ;

    iterator end_scratch() const ;
    iterator end_row_scratch(size_type i) const ;
    iterator end_column_scratch(size_type i) const ;
    iterator end_diagonal_scratch() const ;

    //----------------------------------------------------------------------
    // destructive matrix resize, elements left uninitialized
    virtual void Resize(size_type nRows, size_type nCols);

    //----------------------------------------------------------------------
    // unchecked fast copy
    void copyMemIn(const T *begin, const T *end);
    void copyMemOut(T *begin) const;

    //--------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------
    T* m_pMtx;
    size_type m_nRow;
    size_type m_nCol;


    size_type m_capacity;  // space reserved for matrix, >= m_nCol*m_nRow

    // scratch area
    mutable T* m_pScr;
    mutable size_type m_szScr;
};

//==============================================================================
// iterators
//------------------------------------------------------------------------------
template <class T>
inline typename MatrixBase<T>::iterator MatrixBase<T>::begin()
{
    return iterator(m_pMtx, 0, 1);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::begin_row(size_type i)
{
    if (i >= m_nRow) {
        return iterator(NULL, 0, 0);
    }

    return iterator(m_pMtx, i * m_nCol, 1);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::begin_column(size_type i)
{
    if (i >= m_nCol) {
        return iterator(NULL, 0, 0);
    }

    return iterator(m_pMtx, i, m_nCol);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::begin_diagonal()
{
    return iterator(m_pMtx, 0, m_nCol + 1);
}

template <class T>
inline typename MatrixBase<T>::iterator MatrixBase<T>::end()
{
    return iterator(m_pMtx, m_nRow * m_nCol, 1);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::end_row(size_type i)
{
    if (i >= m_nRow) {
        return iterator(NULL, 0, 0);
    }

    return iterator(m_pMtx, (i + 1) * m_nCol, 1);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::end_column(size_type i)
{
    if (i >= m_nCol) {
        return iterator(NULL, 0, 0);
    }

    return iterator(m_pMtx, i + m_nRow * m_nCol, m_nCol);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::end_diagonal()
{
    size_type start = (m_nCol + 1) * (m_nRow > m_nCol ? m_nCol : m_nRow);
    return iterator(m_pMtx, start, m_nCol + 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
inline typename MatrixBase<T>::const_iterator
MatrixBase<T>::begin() const
{
    return const_iterator(m_pMtx, 0, 1);
}

template <class T>
inline typename MatrixBase<T>::const_iterator
MatrixBase<T>::begin_row(size_type i) const
{
    if (i >= m_nRow) {
        return iterator(NULL, 0, 0);
    }

    return const_iterator(m_pMtx, i * m_nCol, 1);
}

template <class T>
inline typename MatrixBase<T>::const_iterator
MatrixBase<T>::begin_column(size_type i) const
{
    if (i >= m_nCol) {
        return iterator(NULL, 0, 0);
    }

    return const_iterator(m_pMtx, i, m_nCol);
}

template <class T>
inline typename MatrixBase<T>::const_iterator
MatrixBase<T>::begin_diagonal() const
{
    return const_iterator(m_pMtx, 0, m_nCol + 1);
}

template <class T>
inline typename MatrixBase<T>::const_iterator
MatrixBase<T>::end() const
{
    return const_iterator(m_pMtx, m_nRow * m_nCol, 1);
}

template <class T>
inline typename MatrixBase<T>::const_iterator
MatrixBase<T>::end_row(size_type i) const
{
    if (i >= m_nRow) {
        return iterator(NULL, 0, 0);
    }

    return const_iterator(m_pMtx, (i + 1) * m_nCol, 1);
}

template <class T>
inline typename MatrixBase<T>::const_iterator
MatrixBase<T>::end_column(size_type i) const
{
    if (i >= m_nCol) {
        return iterator(NULL, 0, 0);
    }

    return const_iterator(m_pMtx, i + m_nRow * m_nCol, m_nCol);
}

template <class T>
inline typename MatrixBase<T>::const_iterator
MatrixBase<T>::end_diagonal() const
{
    size_type start = (m_nCol + 1) * (m_nRow > m_nCol ? m_nCol : m_nRow);
    return const_iterator(m_pMtx, start, m_nCol + 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// scratch area iterators
template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::begin_scratch() const
{
    if (NULL == m_pScr) {
        return iterator(NULL, 0, 0);
    }

    return iterator(m_pScr, 0, 1);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::begin_row_scratch(size_type i) const
{
    if (NULL == m_pScr) {
        return iterator(NULL, 0, 0);
    }

    return iterator(m_pScr, i * m_nCol, 1);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::begin_column_scratch(size_type i) const
{
    if (NULL == m_pScr) {
        return iterator(NULL, 0, 0);
    }

    return iterator(m_pScr, i, m_nCol);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::begin_diagonal_scratch() const
{
    if (NULL == m_pScr) {
        return iterator(NULL, 0, 0);
    }

    return iterator(m_pScr, 0, m_nCol + 1);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::end_scratch() const
{
    if (NULL == m_pScr) {
        return iterator(NULL, 0, 0);
    }

    return iterator(m_pScr, m_nRow * m_nCol, 1);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::end_row_scratch(size_type i) const
{
    if (NULL == m_pScr) {
        return iterator(NULL, 0, 0);
    }

    return iterator(m_pScr, (i + 1) * m_nCol, 1);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::end_column_scratch(size_type i) const
{
    if (NULL == m_pScr) {
        return iterator(NULL, 0, 0);
    }

    return iterator(m_pScr, i + m_nRow * m_nCol, m_nCol);
}

template <class T>
inline typename MatrixBase<T>::iterator
MatrixBase<T>::end_diagonal_scratch() const
{
    if (NULL == m_pScr) {
        return iterator(NULL, 0, 0);
    }

    size_type start = (m_nCol + 1) * (m_nRow > m_nCol ? m_nCol : m_nRow);
    return iterator(m_pScr, start, m_nCol + 1);
}

//------------------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------------------
template <class T>
inline bool MatrixBase<T>::IsScratchReserved() const
{
    return 0 != m_szScr;
}

template <class T>
inline typename MatrixBase<T>::size_type
MatrixBase<T>::size_row() const
{
    return m_nRow;
}

template <class T>
inline typename MatrixBase<T>::size_type
MatrixBase<T>::GetSizeRows() const
{
    return size_row();
}

template <class T>
inline typename MatrixBase<T>::size_type
MatrixBase<T>::size_column() const
{
    return m_nCol;
}

template <class T>
inline typename MatrixBase<T>::size_type
MatrixBase<T>::GetSizeColumns() const
{
    return size_column();
}

template <class T>
inline typename MatrixBase<T>::size_type
MatrixBase<T>::size_diagonal() const
{
    return m_nRow > m_nCol ? m_nCol : m_nRow;
}

template <class T>
inline typename MatrixBase<T>::size_type
MatrixBase<T>::size() const
{
    return m_nRow * m_nCol;
}

template <class T>
inline typename MatrixBase<T>::size_type
MatrixBase<T>::GetSize() const
{
    return size();
}

template <class T>
inline typename MatrixBase<T>::size_type
MatrixBase<T>::capacity() const
{
    return m_capacity;
}

template <class T>
inline typename MatrixBase<T>::size_type
MatrixBase<T>::GetCapacity() const
{
    return capacity();
}

template <class T>
inline typename MatrixBase<T>::size_type
MatrixBase<T>::capacity_row() const
{
    return 0 != m_nRow ? m_capacity / m_nRow : m_capacity;
}

template <class T>
inline typename MatrixBase<T>::size_type
MatrixBase<T>::GetCapacityRows() const
{
    return capacity_row();
}

template <class T>
inline bool MatrixBase<T>::IsSquare() const
{
    return m_nRow == m_nCol;
}

template <class T>
inline bool MatrixBase<T>::IsSquare(const size_type& sz) const
{
    return sz == m_nRow && sz == m_nCol;
}

template <class T>
inline void MatrixBase<T>::copyMemIn(const T *begin, const T *end)
{
    if (static_cast<size_type>(end-begin) > m_nRow*m_nCol) {
        throw std::runtime_error("MatrixBase: can't m_copy");
    }

    memcpy(m_pMtx, begin, (end-begin)*sizeof(T));
}

template <class T>
inline void MatrixBase<T>::copyMemOut(T *begin) const
{
    memcpy(begin, m_pMtx, size()*sizeof(T));
}

//------------------------------------------------------------------------------
// misc functions
template <class T>
inline const char* MatrixBase<T>::GetVersion() const
{
    return MATRIXBASE_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // MATRIXBASE_H
