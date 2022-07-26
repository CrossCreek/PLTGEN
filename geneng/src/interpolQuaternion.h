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
 * interpolQuaternion.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : interpolQuaternion.h

//  NAMESPACE : A3

//  SUBSYSTEM : GenEngr

//  MODULE(s) : interpolQuaternion

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V5.0  2010-06-01  JAM  Initial release

//****************************************************************************//

#ifndef INTERPOLQUATERNION_H
#define INTERPOLQUATERNION_H  __FILE__ " " __DATE__

#include "Quaternion.h"
#include "TestValue.h"
#include <math.h>
#include <algorithm>
#include <stdexcept>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
inline void negateQuaternion(Quaternion::value_type& v)
{
    v = -v;
}

// ============================================================================
// x must be scalar and should be a floating point base type
// y must be a quaternion of Norm 1
template < class NumTypeX >
inline void
interpolQuaternion( const NumTypeX& x0,   const NumTypeX& x1,
                    const Quaternion& y0, const Quaternion& y1,
                    const NumTypeX& x,    Quaternion& y )
{
    static const double SMALL_DELTA(1.0e-9);
    double ca(testAsinAcosArg(y0.DotProduct(y1)));

    if (1.0-fabs(ca) > SMALL_DELTA) {
        Quaternion y2(y1);

        if (0 > ca) {
            std::for_each(y2.begin(), y2.end(), negateQuaternion);
            ca = -ca;
        }

        NumTypeX xn = (x - x0) / (x1 - x0);
        double a(acos(ca));
        double sa(sin(a));
        double isa(1.0/sa);
        double sta(sin(xn*a));
        double s1a(sin((1-xn)*a));
        Quaternion qa, qb, qc;
        y0.Multiply(s1a, qa);
        y2.Multiply(sta, qb);
        qa.Add(qb, qc);
        qc.Multiply(isa, y);
    } else {
        y = y0;
    }
}

// x must be scalar and should be a floating point base type
// y must be a quaternion of Norm 1
template < class InputIteratorX, class InputIteratorY,
           class NumTypeX, class NumTypeY >
inline void
interpolQuaternion( InputIteratorX xVecBegin, InputIteratorX xVecEnd,
                    InputIteratorY yVecBegin,
                    const NumTypeX& xValue,   NumTypeY& yValue )
{
    const unsigned nDataPoints(static_cast<unsigned>(xVecEnd-xVecBegin));

    if ( 2 > nDataPoints )
        throw std::out_of_range(
            "<<interpolQuaternion>> need at least 2 quaternions");

    InputIteratorX xv0 = std::lower_bound( xVecBegin, xVecEnd, xValue );

    if ( xv0 != xVecEnd && *xv0 == xValue ) {
        // order of evaluation matters with pointer arithmetic
        yValue = *( yVecBegin + ( xv0 - xVecBegin ) );
    } else {
        if ( xv0 - xVecBegin < 1 ) {
            xv0 = xVecBegin;
        } else if ( xVecEnd - xv0 < 1 ) {
            xv0 = xVecEnd - 2;
        } else {
            xv0 -= 1;
        }

        InputIteratorX xv1 = xv0 + 1;
        // order of evaluation matters with pointer arithmetic
        InputIteratorY yv0 = yVecBegin + ( xv0 - xVecBegin );
        InputIteratorY yv1 = yv0 + 1;
        interpolQuaternion( *xv0, *xv1, *yv0, *yv1, xValue, yValue );
    }
}

// x must be scalar and should be a floating point base type
// y must be a quaternion of Norm 1
// xVecHint = iterator hint where 1st element that is greater or equal
// to x-value, a suggested hint for the next call is returned
template < class InputIteratorX, class InputIteratorY,
           class NumTypeX, class NumTypeY >
inline InputIteratorX
interpolQuaternion(InputIteratorX xVecBegin, InputIteratorX xVecEnd,
                   InputIteratorX xVecHint, InputIteratorY yVecBegin,
                   const NumTypeX& xValue,  NumTypeY& yValue)
{
    unsigned nDataPoints(static_cast<unsigned>(xVecEnd-xVecBegin));

    if ( 2 > nDataPoints )
        throw std::out_of_range(
            "<<interpolQuaternion>> need at least 2 quaternions");

    InputIteratorX xvLB;

    if (xVecBegin < xVecHint && xVecEnd > xVecHint && xValue > *(xVecHint-1)) {
        xvLB = std::lower_bound(xVecHint, xVecEnd, xValue);
    } else {
        xvLB = std::lower_bound(xVecBegin, xVecEnd, xValue);
    }

    InputIteratorX xv0(xvLB);

    if (xv0 != xVecEnd && *xv0 == xValue) {
        // order of evaluation matters with pointer arithmetic
        yValue = *(yVecBegin + (xv0 - xVecBegin));
    } else {
        if ( xv0 - xVecBegin < 1 ) {
            xv0 = xVecBegin;
        } else if ( xVecEnd - xv0 < 1 ) {
            xv0 = xVecEnd - 2;
        } else {
            xv0 -= 1;
        }

        InputIteratorX xv1 = xv0 + 1;
        // order of evaluation matters with pointer arithmetic
        InputIteratorY yv0 = yVecBegin + ( xv0 - xVecBegin );
        InputIteratorY yv1 = yv0 + 1;
        interpolQuaternion( *xv0, *xv1, *yv0, *yv1, xValue, yValue );
    }

    return xvLB;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // INTERPOLQUATERNION_H
