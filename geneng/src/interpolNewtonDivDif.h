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
 * interpolNewtonDivDif.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : interpolNewtonDivDif.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : interpolNewtonDivDif

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: changed nHalfPointsReq from unsigned to int
//              Reason: stops "warning C4018: '<' : signed/unsigned mismatch"
//              from implicit conversion

//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA

//****************************************************************************//

#ifndef INTERPOLNEWTONDIVDIF_H
#define INTERPOLNEWTONDIVDIF_H "interpolNewtonDivDif V4.3-009"

#include <algorithm>
#include <stdexcept>
#include <vector>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// note that the computed value is returned through the argument list
// to permit differ types for x and y values (e.g., time and position)
// x must be scalar and should be a floating point base type
// y may be scalar or vector and should be a floating point base type
template < class InputIteratorX, class InputIteratorY,
           class NumTypeX, class NumTypeY >
inline void
interpolNewtonDivDif( InputIteratorX xVecBegin, InputIteratorX xVecEnd,
                      InputIteratorY yVecBegin,
                      const NumTypeX& xValue, NumTypeY& yValue )
{
    static const unsigned qBufSize = 9;
    static std::vector< NumTypeY > qv( qBufSize*qBufSize, (NumTypeY)0.0 );
    static std::vector< NumTypeX > xv( qBufSize, (NumTypeX)0.0 );
    InputIteratorX xo;
    InputIteratorY yo;
    const NumTypeX& x   = xValue;
    NumTypeY        y   = (NumTypeY)0.0;
    NumTypeY        qii = (NumTypeY)0.0;
    unsigned i = 0, j = 0;
    unsigned np = xVecEnd - xVecBegin;  // number of usable data points
    np = ( np >> 1 ) << 1;

    if ( xv.size() < np ) {
        xv.resize( np );
        qv.resize( np * np );
    }

    InputIteratorX xi = xv.begin();
    InputIteratorY qi = qv.begin();

    if ( (x - *xVecBegin ) <= ( *(xVecEnd-1) - x ) ) {
        xo = xVecBegin;
        yo = yVecBegin;

        for ( i = 0; i < np; i++, yo++, xo++ ) {
            *(qi + i * np ) = *yo;
            *(xi + i )      = *xo;
        }
    } else {
        xo = xVecEnd - 1;
        yo = yVecBegin + ( xVecEnd - xVecBegin ) - 1;

        for ( i = 0; i < np; i++, yo--, xo-- ) {
            *(qi + i * np ) = *yo;
            *(xi + i )      = *xo;
        }
    }

    y  = *qi;

    for ( i = 1; i < np; i++ ) {
        for ( j = 1; j <= i; j++ ) {
            *( qi + i * np + j ) = ( *( qi + i * np + j - 1 )
                                     - *( qi + ( i - 1 ) * np + j - 1 ) )
                                   / ( *( xi + i ) - *( xi + i - j ) );
        }

        qii = *( qi + i * ( np + 1 ) );

        for ( j = 0; j < i; j++ ) {
            qii *= ( x - *( xi + j ) );
        }

        y += qii;
    }

    yValue = y;
}

// note that the computed value is returned through the argument list
// to permit differ types for x and y values (e.g., time and position)
// x must be scalar and should be a floating point base type
// y may be scalar or vector and should be a floating point base type
template < class InputIteratorX, class InputIteratorY,
           class NumTypeX, class NumTypeY >
inline void
interpolNewtonDivDif( InputIteratorX xVecBegin, InputIteratorX xVecEnd,
                      InputIteratorY yVecBegin, unsigned polyOrder,
                      const NumTypeX& xValue,   NumTypeY& yValue )
{
    unsigned nDataPoints = xVecEnd - xVecBegin;
    unsigned nPointsReq  = polyOrder+1;
    int      nHalfPointsReq = (nPointsReq+1) / 2;  // low end bias

    if ( nPointsReq > nDataPoints ) {
        throw std::out_of_range( "<<interpolNewtonDivDif>> order too large" );
    }

    if ( !polyOrder ) {
        throw std::out_of_range( "<<interpolNewtonDivDif>> order must be greater than 0" );
    }

    InputIteratorX xvBegin = std::lower_bound( xVecBegin, xVecEnd, xValue );

    if ( (xvBegin - xVecBegin) < nHalfPointsReq ) {
        xvBegin = xVecBegin;
    } else if ( (xVecEnd - xvBegin ) < nHalfPointsReq ) {
        xvBegin = xVecEnd - nPointsReq;
    } else {
        xvBegin -= nHalfPointsReq;
    }

    InputIteratorX xvEnd = xvBegin + nPointsReq; // xvEnd is 1 past last point
    InputIteratorY yvBegin = yVecBegin + ( xvBegin - xVecBegin );
    interpolNewtonDivDif( xvBegin, xvEnd, yvBegin, xValue, yValue );
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // INTERPOLNEWTONDIVDIF_H
