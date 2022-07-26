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
 * interpolLagrangePolyPair.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : interpolLagrangePolyPair.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : interpolLagrangePolyPair

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

#ifndef INTERPOLLAGRANGEPOLYPAIR_H
#define INTERPOLLAGRANGEPOLYPAIR_H  "interpolLagrangePolyPair V4.3-009"

#include <algorithm>
#include <stdexcept>
#include <utility>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// given pairPoint.first, interpolate for pairPoint.second
template < class InputIteratorPair, class NumTypeX, class NumTypeY >
inline void
interpolLagrangePoly( InputIteratorPair pairBegin,
                      InputIteratorPair pairEnd,
                      std::pair< NumTypeX, NumTypeY >& pairPoint )
{
    InputIteratorPair  iPair;
    InputIteratorPair  kPair;
    // The following statement creates and initializes the y value to zero.
    // For user created types that don't support scalar initialization (e.g.,
    // a foreign vector class) the following statement can be replaced with
    // NumTypeY y;
    // y *= 0.0; or y = y * 0.0;
    // The down side of this alternative approach is efficiency.
    // Note that GenEngr vector classes support scalar initialization
    NumTypeY y = (NumTypeY)0.0;
    NumTypeX lk;

    for ( kPair = pairBegin; kPair != pairEnd; kPair++ ) {
        lk = (NumTypeX)1.0;

        for ( iPair = pairBegin; iPair != pairEnd; iPair++ ) {
            if ( iPair != kPair ) {
                lk *= ( pairPoint.first - iPair->first )
                      / ( kPair->first - iPair->first );
            }
        }

        y += kPair->second * lk;
    }

    pairPoint.second = y;
}

// note that the computed value is returned through the argument list
// to permit differ types for x and y values (e.g., time and position)
// x must be scalar and should be a floating point base type
// y may be scalar or vector and should be a floating point base type
template < class InputIteratorPair, class NumTypeX, class NumTypeY >
inline void
interpolLagrangePoly( InputIteratorPair pairBegin,
                      InputIteratorPair pairEnd,
                      unsigned polyOrder,
                      std::pair< NumTypeX, NumTypeY >& pairPoint )
{
    unsigned nDataPoints = pairEnd - pairBegin;
    unsigned nPointsReq  = polyOrder+1;
    int      nHalfPointsReq = (nPointsReq+1) / 2;  // low end bias

    if ( nPointsReq > nDataPoints ) {
        throw std::out_of_range( "<<interpolLagrangePoly>> order too large" );
    }

    if ( !polyOrder ) {
        throw std::out_of_range( "<<interpolLagrangePoly>> order must be greater than 0" );
    }

    InputIteratorPair iPairBegin = std::lower_bound( pairBegin, pairEnd, pairPoint );

    if ( iPairBegin != pairEnd && iPairBegin->first == pairPoint.first ) {
        pairPoint.second = iPairBegin->second;
    } else {
        if ( (iPairBegin - pairBegin) < nHalfPointsReq ) {
            iPairBegin = pairBegin;
        } else if ( (pairEnd - iPairBegin ) < nHalfPointsReq ) {
            iPairBegin = pairEnd - nPointsReq;
        } else {
            iPairBegin -= nHalfPointsReq;
        }

        // iPairEnd is 1 past last point
        InputIteratorPair iPairEnd = iPairBegin + nPointsReq;
        interpolLagrangePoly( iPairBegin, iPairEnd, pairPoint );
    }
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // INTERPOLLAGRANGEPOLYPAIR_H
