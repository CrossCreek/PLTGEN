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
 * Table2DLagrangePoly.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  SCCS - Release %R%  Delta %I%  from %G%

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : Table2DLagrangePoly

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V2.0      04/29/2002  JAM  1st A3 library build
//				V3.8-019  04/24/2006  MDE  Changed Default Poly Order
//                                         From 0 to 1
//              V4.5-001 2009-07-01  JAM
//              replaced use of "int" in as size type with size_type
//              to eliminate 64-bit warnings

//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA

//****************************************************************************//

#ifndef TABLE2DLAGRANGEPOLY_H
#define TABLE2DLAGRANGEPOLY_H       "Table2DLagrangePoly V3.8-019"

// ============================================================================
#include "Table2D.h"
#include <algorithm>
#include <vector>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
template < class XTYPE, class YTYPE >
class Table2DLagrangePoly : public Table2D< XTYPE, YTYPE>
{
public:



    explicit Table2DLagrangePoly( unsigned polyOrder = 1 )
    {
        SetPolyOrder( polyOrder );
    }

    template < class X_ITR, class Y_ITR >
    Table2DLagrangePoly( X_ITR xBegin, X_ITR xEnd, Y_ITR yBegin,
                         unsigned polyOrder = 1 )
        : Table2D< XTYPE, YTYPE>( xBegin, xEnd, yBegin )
    {
        SetPolyOrder( polyOrder );
    }

    virtual ~Table2DLagrangePoly() {;}

    // --------------------------------------------------------------------
    inline void SetPolyOrder( unsigned polyOrder );
    inline unsigned GetPolyOrder() const;

    // --------------------------------------------------------------------
    inline virtual YTYPE operator()( const XTYPE& xValue ) const;

    // --------------------------------------------------------------------
    // Must define LoadTable in class definition, too complex for Microsoft
    typename Table2D<XTYPE, YTYPE>::size_type
    LoadTable( std::istream& is, unsigned nRows = 0 )
    {
        Table2D< XTYPE, YTYPE >::LoadTable( is, nRows );
        SetPolyOrder( GetPolyOrder() );
        return Table2D<XTYPE,YTYPE>::m_xVec.size();
    }

protected:
    unsigned m_nPointsReq;
    int      m_nHalfPointsReq;
    bool     m_isOrderTooLarge;
};

// ----------------------------------------------------------------------------
// inline operators
template < class XTYPE, class YTYPE >
inline YTYPE
Table2DLagrangePoly< XTYPE, YTYPE >::operator()( const XTYPE& xValue ) const
{
    if ( m_isOrderTooLarge ) {
        throw std::runtime_error( "<<Table2DLagrangePoly>> order too large." );
    }

    // The following statement creates and initializes the y value to zero.
    // For user created types that don't support scalar initialization (e.g.,
    // a foreign vector class) the following statement can be replaced with
    // YTYPE y;
    // y *= 0.0; or y = y * 0.0;
    // The down side of this alternative approach is efficiency.
    // Note that GenEngr vector classes support scalar initialization
    YTYPE yValue = static_cast< YTYPE >( 0.0 );
    typename std::vector< XTYPE >::const_iterator xvBegin
        = std::lower_bound( Table2D<XTYPE,YTYPE>::m_xVec.begin(),
                            Table2D<XTYPE,YTYPE>::m_xVec.end(), xValue );

    if ( xvBegin != Table2D<XTYPE,YTYPE>::m_xVec.end() && *xvBegin == xValue ) {
        // order of evaluation matters with pointer arithmetic
        yValue = *(Table2D<XTYPE,YTYPE>::m_yVec.begin()
                   +(xvBegin - Table2D<XTYPE,YTYPE>::m_xVec.begin()));
    } else {
        if (xvBegin-Table2D<XTYPE,YTYPE>::m_xVec.begin() < m_nHalfPointsReq) {
            xvBegin = Table2D<XTYPE,YTYPE>::m_xVec.begin();
        } else if (Table2D<XTYPE,YTYPE>::m_xVec.end()-xvBegin < m_nHalfPointsReq) {
            xvBegin = Table2D<XTYPE,YTYPE>::m_xVec.end() - m_nPointsReq;
        } else {
            xvBegin -= m_nHalfPointsReq;
        }

        // xvEnd is 1 past last point
        typename std::vector< XTYPE >::const_iterator xvEnd
            = xvBegin +  m_nPointsReq;
        // order of evaluation matters with pointer arithmetic
        typename std::vector< YTYPE >::const_iterator yvBegin
            = Table2D<XTYPE,YTYPE>::m_yVec.begin()
              + (xvBegin-Table2D<XTYPE,YTYPE>::m_xVec.begin());
        // --------------------------------------------------------------------
        typename std::vector< XTYPE >::const_iterator xi, xk;
        typename std::vector< YTYPE >::const_iterator yi = yvBegin;
        XTYPE lk;

        for ( xk = xvBegin; xk != xvEnd; xk++, yi++ ) {
            for ( xi = xvBegin, lk = (XTYPE)1.0; xi != xvEnd; xi++ ) {
                if ( xi != xk ) {
                    lk *= ( xValue - *xi ) / ( *xk - *xi );
                }
            }

            yValue += *yi * lk;
        }
    }

    return  yValue;
}

// ----------------------------------------------------------------------------
template < class XTYPE, class YTYPE >
inline void
Table2DLagrangePoly< XTYPE, YTYPE >
::SetPolyOrder( unsigned polyOrder )
{
    if ( polyOrder >= Table2D<XTYPE,YTYPE>::m_xVec.size() ) {
        m_isOrderTooLarge = true;
    } else {
        m_isOrderTooLarge = false;
    }

    m_nPointsReq     = polyOrder+1;
    m_nHalfPointsReq = ( m_nPointsReq + 1 ) / 2; // low end bias
}

// ----------------------------------------------------------------------------
template < class XTYPE, class YTYPE >
inline unsigned
Table2DLagrangePoly< XTYPE, YTYPE >
::GetPolyOrder() const
{
    return m_nPointsReq-1;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif // TABLE2DLAGRANGEPOLY_H
// ============================================================================
