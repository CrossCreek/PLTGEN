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
 * Normalize.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : Normalize.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V1.4      08/25/2000  JAM  moved from global to A3 namespace
//                                         namespace directive no longer
//                                         applied to std
//                                         applied 80 character line limit
//                                         moved includes from header to source
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: changed template argument for Angle to double
//              Reason: using class Angle for units of degrees is deprecated

//              V4.5-001   2009-07-01 JAM
//              Moved the initialization of Normalize functions objects
//              from Normalize.cpp to Initialize.cpp

//              V5.0    2010-01-18 JAM
//              Replaced Normalize namespace objects w/functions

//  NOTES : base header file for the GenEngr library
//          contains common contants, unit conversions,
//          general functions and function objects

//          Contents derive from SE and SST Ver 1.0.

//          note: angular units are radians
//                time units are seconds
//                length units are as indicated for constaints
//                length units are argument dependent for functions

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  FUNCTION OBJECTS

//  Name:           normalizePlueMinusPI( double )
//  Purpose:        normalizes an angle between [-PI, PI) radians
//  Return Type:    double
//  Return Value:   angle between [-PI, PI) radians
//  Argument Type:  double
//  Argument Value: unconstrained angle in radians
//  Source Class:   Normalize

//  Name:           normalizePlusMinus180( double )
//  Purpose:        normalizes an angle between [-180, 180) degrees
//  Return Type:    double
//  Return Value:   angle between [-180, 180) degrees
//  Argument Type:  double
//  Argument Value: unconstrained angle in degrees
//  Source Class:   Normalize

//  Name:           normalizeZeroTwoPi( double )
//  Purpose:        normalizes an angle between [0, 2*PI) radians
//  Return Type:    double
//  Return Value:   angle between [0, 2*PI) radians
//  Argument Type:  double
//  Argument Value: unconstrained angle in radians
//  Source Class:   Normalize

//  Name:           normalizeZero360( double )
//  Purpose:        normalizes an angle between [0, 360) degrees
//  Return Type:    double
//  Return Value:   angle between [0, 360) degrees
//  Argument Type:  double
//  Argument Value: unconstrained angle in degrees
//  Source Class:   Normalize

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA

//****************************************************************************//

//=============================================================================

#ifndef NORMALIZE_H
#define NORMALIZE_H   "GenEngr: V4.3-009"

// ----------------------------------------------------------------------------
// headers
// using math.h since Microsoft's cmath is not in the std namespace
#include "Angle.h"
#include "constants.h"
#include <math.h>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// function objects
// ----------------------------------------------------------------------------
// normalization function objects
// normailize between lower and upper bounds
// floating point types only
template < class FPT >
class Normalize
{
public:



    explicit Normalize( FPT upperHalfBound );

    Normalize( FPT lowerBound, FPT upperBound );

    FPT operator()( FPT value ) const;

    FPT GetLowerBound() const;
    FPT GetUpperBound() const;

private:
    Normalize();
    void ThrowBadBounds( FPT lowerBound, FPT upperBound );

    FPT m_lowerBound, m_upperBound;
    FPT m_distance, m_halfDistance, m_translate;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Normalize functions
// normalizePlueMinusPI: returns angle between -PI and PI deg
// extern Normalize<Angle> normalizePlusMinusPi;
inline Angle normalizePlusMinusPi(const Angle& a)
{
    static const Normalize<Angle> n(PI);
    return n(a);
}

// normalizePlusMinus180: returns angle between -180 and 180 deg
// extern Normalize<double> normalizePlusMinus180;
inline double normalizePlusMinus180(const double& a)
{
    static const Normalize<double> n(180.0);
    return n(a);
}

// normalizeZeroTwoPi: returns angle between 0 and 2*PI deg
// extern Normalize<Angle> normalizeZeroTwoPi;
inline Angle normalizeZeroTwoPi(const Angle& a)
{
    static const Normalize<Angle> n(0.0, TWO_PI);
    return n(a);
}

// normalizeZero360: returns angle between 0 and 360 deg
// extern Normalize<double> normalizeZero360;
inline double normalizeZero360(const double& a)
{
    static const Normalize<double> n(0.0, 360.0);
    return n(a);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Normalize inline functions
template < class FPT >
inline
Normalize< FPT >::Normalize()
    : m_lowerBound(-0.5),
      m_upperBound(0.5),
      m_distance(1.0),
      m_halfDistance(0.5),
      m_translate(0.0) {;}

template < class FPT >
inline
Normalize< FPT >::Normalize( FPT upperHalfBound )
    : m_lowerBound(-upperHalfBound),
      m_upperBound(upperHalfBound),
      m_distance(upperHalfBound+upperHalfBound),
      m_halfDistance(upperHalfBound),
      m_translate(0.0)
{
    if ( m_lowerBound >= m_upperBound ) {
        ThrowBadBounds( m_lowerBound, m_upperBound );
    }
}

template < class FPT >
inline
Normalize< FPT >::Normalize( FPT lowerBound, FPT upperBound )
    : m_lowerBound(lowerBound),
      m_upperBound(upperBound),
      m_distance(upperBound-lowerBound),
      m_halfDistance((upperBound-lowerBound)/2.0),
      m_translate((upperBound+lowerBound)/2.0)
{
    if ( m_lowerBound >= m_upperBound ) {
        ThrowBadBounds( m_lowerBound, m_upperBound );
    }
}

template < class FPT >
inline FPT
Normalize< FPT >::operator()( FPT value ) const
{
    if ( value<m_lowerBound || value>=m_upperBound ) {
        if ( FPT(0.0) != m_translate ) {
            value -= m_translate;
        }

        value = fmod( value, m_distance );

        if      ( value >= m_halfDistance ) {
            value -= m_distance;
        } else if ( value < -m_halfDistance ) {
            value += m_distance;
        }

        if ( FPT(0.0) != m_translate ) {
            value += m_translate;
        }
    }

    return value;
}

template < class FPT >
inline FPT
Normalize< FPT >::GetLowerBound() const
{
    // return m_translate - m_halfDistance;
    return m_lowerBound;
}

template < class FPT >
inline FPT
Normalize< FPT >::GetUpperBound() const
{
    // return m_translate + m_halfDistance;
    return m_upperBound;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================

#endif // NORMALIZE_H
