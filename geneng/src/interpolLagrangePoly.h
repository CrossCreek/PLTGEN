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
 * interpolLagrangePoly.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : interpolLagrangePoly.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : interpolLagrangePoly

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
//              Change: deleted template function interpolLinear
//              Reason: interpolLinear is dead code; redundant with
//              interpolLagrangePoly

//              V4.5-001   2009-07-01 JAM
//              Applied an explicit cast to eliminate a 64-bit warning
//              Added interpolLagrangePoly that accepts a hint to reduce
//              search time

//****************************************************************************//

#ifndef INTERPOLLAGRANGEPOLY_H
#define INTERPOLLAGRANGEPOLY_H  "interpolLagrangePoly V4.5"

#include <algorithm>
#include <stdexcept>

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
interpolLagrangePoly( InputIteratorX xVecBegin, InputIteratorX xVecEnd,
                      InputIteratorY yVecBegin,
                      const NumTypeX& xValue, NumTypeY& yValue )
{
    InputIteratorX  xi;
    InputIteratorX  xk;
    // The following statement creates and initializes the y value to zero.
    // For user created types that don't support scalar initialization (e.g.,
    // a foreign vector class) the following statement can be replaced with
    // NumTypeY y;
    // y *= 0.0; or y = y * 0.0;
    // The down side of this alternative approach is efficiency.
    // Note that GenEngr vector classes support scalar initialization
    NumTypeY y(static_cast<NumTypeY>(0.0));
    NumTypeX lk;

    for ( xk = xVecBegin; xk != xVecEnd; xk++, yVecBegin++ ) {
        for ( xi = xVecBegin, lk = (NumTypeX)1.0; xi != xVecEnd; xi++ )
            if ( xi != xk ) {
                lk *= ( xValue - *xi ) / ( *xk - *xi );
            }

        y += *yVecBegin * lk;
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
interpolLagrangePoly( InputIteratorX xVecBegin, InputIteratorX xVecEnd,
                      InputIteratorY yVecBegin, unsigned polyOrder,
                      const NumTypeX& xValue,   NumTypeY& yValue )
{
    unsigned nDataPoints(static_cast<unsigned>(xVecEnd-xVecBegin));
    unsigned nPointsReq(polyOrder+1);

    if ( nPointsReq > nDataPoints ) {
        throw std::out_of_range("<<interpolLagrangePoly>> order too large");
    }

    if ( !polyOrder ) {
        throw std::out_of_range("<<interpolLagrangePoly>> order must be greater than 0");
    }

    InputIteratorX xvBegin = std::lower_bound( xVecBegin, xVecEnd, xValue );

    if ( xvBegin != xVecEnd && *xvBegin == xValue ) {
        // order of evaluation matters with pointer arithmetic
        yValue = *( yVecBegin + ( xvBegin - xVecBegin ) );
    } else {
        int nHalfPointsReq((nPointsReq+1)/2);

        if ( xvBegin - xVecBegin < nHalfPointsReq ) {
            xvBegin = xVecBegin;
        } else if ( xVecEnd - xvBegin < nHalfPointsReq ) {
            xvBegin = xVecEnd - nPointsReq;
        } else {
            xvBegin -= nHalfPointsReq;
        }

        InputIteratorX xvEnd = xvBegin + nPointsReq; // xvEnd is 1 past last point
        // order of evaluation matters with pointer arithmetic
        InputIteratorY yvBegin = yVecBegin + ( xvBegin - xVecBegin );
        interpolLagrangePoly( xvBegin, xvEnd, yvBegin, xValue, yValue );
    }
}

// note that the computed value is returned through the argument list
// to permit differ types for x and y values (e.g., time and position)
// x must be scalar and should be a floating point base type
// y may be scalar or vector and should be a floating point base type
// xVecHint = iterator hint where 1st element that is greater or equal
// to x-value, a suggested hint for the next call is returned
template < class InputIteratorX, class InputIteratorY,
           class NumTypeX, class NumTypeY >
inline InputIteratorX
interpolLagrangePoly(InputIteratorX xVecBegin, InputIteratorX xVecEnd,
                     InputIteratorX xVecHint, InputIteratorY yVecBegin,
                     unsigned polyOrder, const NumTypeX& xValue,
                     NumTypeY& yValue)
{
    unsigned nDataPoints(static_cast<unsigned>(xVecEnd-xVecBegin));
    unsigned nPointsReq(polyOrder+1);

    if (nPointsReq > nDataPoints) {
        throw std::out_of_range("<<interpolLagrangePoly>> order too large");
    }

    if (!polyOrder) {
        throw std::out_of_range("<<interpolLagrangePoly>> order must be greater than 0");
    }

    InputIteratorX xvLB;

    if (xVecBegin < xVecHint && xVecEnd > xVecHint && xValue > *(xVecHint-1)) {
        xvLB = std::lower_bound(xVecHint, xVecEnd, xValue);
    } else {
        xvLB = std::lower_bound(xVecBegin, xVecEnd, xValue);
    }

    InputIteratorX xvBegin(xvLB);

    if (xvBegin != xVecEnd && *xvBegin == xValue) {
        // order of evaluation matters with pointer arithmetic
        yValue = *(yVecBegin + (xvBegin - xVecBegin));
    } else {
        int nHalfPointsReq((nPointsReq+1)/2);

        if (xvBegin - xVecBegin < nHalfPointsReq) {
            xvBegin = xVecBegin;
        } else if (xVecEnd - xvBegin < nHalfPointsReq) {
            xvBegin = xVecEnd - nPointsReq;
        } else {
            xvBegin -= nHalfPointsReq;
        }

        InputIteratorX xvEnd = xvBegin + nPointsReq;
        // order of evaluation matters with pointer arithmetic
        InputIteratorY yvBegin = yVecBegin + (xvBegin - xVecBegin);
        interpolLagrangePoly(xvBegin, xvEnd, yvBegin, xValue, yValue);
    }

    return xvLB;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // INTERPOLLAGRANGEPOLY_H
