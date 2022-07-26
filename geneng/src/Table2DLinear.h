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
 * Table2DLinear.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : Table2DLinear.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : Table2DLinear

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V2.0      04/29/2002  JAM  1st A3 library build

//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA

//****************************************************************************//

#ifndef TABLE2DLINEAR_H
#define TABLE2DLINEAR_H     "Table2DLinear V2.0"

// ============================================================================
#include "Table2D.h"
#include <algorithm>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
template < class XTYPE, class YTYPE >
class Table2DLinear : public Table2D< XTYPE, YTYPE>
{
public:
    inline Table2DLinear();

    template < class X_ITR, class Y_ITR >
    Table2DLinear( X_ITR xBegin, X_ITR xEnd, Y_ITR yBegin )
        : Table2D( xBegin, xEnd, yBegin )
    {
        if ( 2 > m_ptVec.size() )
            throw std::runtime_error(
                "<<Table2DLinear>> need at least 2 points." );
    }

    inline virtual YTYPE operator()( const XTYPE& xValue ) const;

};

// ----------------------------------------------------------------------------
// constructor & destructor
template < class XTYPE, class YTYPE >
inline Table2DLinear< XTYPE, YTYPE >::Table2DLinear() { ; }

// ----------------------------------------------------------------------------
template < class XTYPE, class YTYPE >
inline YTYPE
Table2DLinear< XTYPE, YTYPE >::operator()( const XTYPE& xValue ) const
{
    static const unsigned nPointsReq     = 2;
    static const unsigned nHalfPointsReq = 1;
    YTYPE yValue = *m_yVec.begin();

    if ( xValue != *m_xVec.begin() ) {
        if ( nPointsReq > m_xVec.size() ) {
            throw std::out_of_range( "<<Table2DLinear>> need at least 2 points" );
        }

        std::vector< XTYPE >::const_iterator xvBegin
            = std::lower_bound( std::vector< XTYPE >::const_iterator( m_xVec.begin() + nHalfPointsReq ),
                                std::vector< XTYPE >::const_iterator( m_xVec.end()   - nHalfPointsReq ),
                                xValue );

        if ( *xvBegin == xValue ) {
            // order of evaluation matters with pointer arithmetic
            yValue = *( m_yVec.begin() + ( xvBegin - m_xVec.begin() ) );
        } else {
            xvBegin -= nHalfPointsReq;
            std::vector< YTYPE >::const_iterator yvBegin
                = m_yVec.begin() + ( xvBegin - m_xVec.begin() );
            yValue = *yvBegin
                     + ( xValue - *xvBegin )
                     * ( *( yvBegin+1 ) - *yvBegin )
                     / ( *( xvBegin+1 ) - *xvBegin );
        }
    }

    return yValue;
}

// ============================================================================
}
// A3 namespace end
#endif // TABLE2DLINEAR_H
// ============================================================================
