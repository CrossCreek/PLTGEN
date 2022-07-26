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
 * Table2D.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : Table2D.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : Table2D

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V2.0      04/29/2002  JAM  1st A3 library build

//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA

//****************************************************************************//

#ifndef TABLE2D_H
#define TABLE2D_H       "Table2D V2.0"

// ============================================================================
#include "IoSeparator.h"
#include <vector>
#include <utility>
#include <iostream>
#include <iomanip>


// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
template < class XTYPE, class YTYPE >
class Table2D
{
public:
    typedef typename std::vector<XTYPE>::size_type size_type;

    Table2D();

    template < class X_ITR, class Y_ITR >
    Table2D( X_ITR xBegin, X_ITR xEnd, Y_ITR yBegin )
        : m_xVec( xBegin, xEnd ),
          m_yVec( yBegin, yBegin + ( xEnd - xBegin ) )
    { ; }

    virtual ~Table2D() {;}

    virtual YTYPE operator()( const XTYPE& xValue ) const = 0;

    // --------------------------------------------------------------------
    void GetDomain( XTYPE& xStart, XTYPE& xEnd ) const;
    size_type GetNumberOfPoints() const;

    // --------------------------------------------------------------------
    // returns number of records, nRows = 0 reads to eof
    size_type  LoadTable( std::istream& is, unsigned nRows = 0 );
    void DumpTable( std::ostream& os ) const;

protected:
    // std::pair requires operators to be overloaded that are not
    // needed for interpolation, this limits the classes that this
    // class may be used with, therefor the Point array (Point is of
    // type pair) has been replace with two vector arrays
    // vector< Point > m_ptVec;
    std::vector< XTYPE > m_xVec;  // abscissa
    std::vector< YTYPE > m_yVec;  // ordinate
};

// ----------------------------------------------------------------------------
// constructor & destructor
template < class XTYPE, class YTYPE >
inline Table2D< XTYPE, YTYPE >::Table2D() { ; }

// ----------------------------------------------------------------------------
template < class XTYPE, class YTYPE >
inline void
Table2D< XTYPE, YTYPE >::GetDomain( XTYPE& xStart, XTYPE& xEnd ) const
{
    xStart = m_xVec.front();
    xEnd   = m_xVec.back();
}

template < class XTYPE, class YTYPE >
inline
typename Table2D< XTYPE, YTYPE >::size_type
Table2D< XTYPE, YTYPE >::GetNumberOfPoints() const
{
    return m_xVec.size();
}

// ----------------------------------------------------------------------------
template < class XTYPE, class YTYPE >
inline
typename Table2D< XTYPE, YTYPE >::size_type
Table2D< XTYPE, YTYPE >::LoadTable( std::istream& is, unsigned nRows )
{
    XTYPE x;
    YTYPE y;
    m_xVec.clear();
    m_yVec.clear();

    while ( ( !nRows || nRows > m_xVec.size() ) && is >> x >> y ) {
        m_xVec.push_back( x );
        m_yVec.push_back( y );
    }

    return m_xVec.size();
}

template < class XTYPE, class YTYPE >
void Table2D< XTYPE, YTYPE >::DumpTable( std::ostream& os ) const
{
    unsigned w = os.width();
    typename std::vector< XTYPE >::const_iterator xItr = m_xVec.begin();
    typename std::vector< YTYPE >::const_iterator yItr = m_yVec.begin();

    for ( ; m_xVec.end() != xItr; ++xItr, ++yItr ) {
        os << std::setw( w ) << *xItr << ioSeparator
           << std::setw( w ) << *yItr << std::endl;
    }
}

// ============================================================================
}
// A3 namespace end
#endif // TABLE2D_H
// ============================================================================
