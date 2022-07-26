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
 * PlanetEarth.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
// This is the future home of class PlanetEarth, a member of the Planet
// class family.

//  FILE      : PlanetEarth.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatTools

//  SUBSYSTEM : GenEngr

//  MODULE(s) : PlanetEarth

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :



//****************************************************************************//

//==============================================================================

#include "PlanetEarth.h"
#include "constants.h"
#include "TestValue.h"
#include "Angle.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
Angle greenwichAngle( TimeJ2000 time )
{
    double tm = time * SEC_TO_DAY;
    /* compute Greenwich Sidereal Angle */
    return fmod(4.894961213 + 1.7202791805e-2 * tm
                + 5.07521e-15 * tm * tm + TWO_PI
                * (tm - floor(tm)), TWO_PI);
}

// ============================================================================
// calculate the radius of curvature for the given latitude
double earthRadiusOfCurvature( double latitude )
{
    double sinLat = sin( latitude );
    return EARTH_RADIUS_NMI
           / sqrt( 1.0 - EARTH_ECCENTRICITY_SQ * sinLat * sinLat );
}

// calculate the radius to the reference ellipse for the
// given latitude
double earthRadiusToRefEllipse( double latitude )
{
    double cosLat = cos( latitude );
    double sinLat = sin( latitude );
    double eta = (1.0 - EARTH_ECCENTRICITY_SQ) * sinLat;
    double radiusCurvSq = EARTH_RADIUS_SQ_NMI2
                          / ( 1.0 - EARTH_ECCENTRICITY_SQ * sinLat * sinLat );
    return sqrt( radiusCurvSq * ( cosLat * cosLat + eta * eta ) );
}

// ============================================================================
VisibilityCond checkEarthObsc( double grzHgt,
                               const VectorCartesian& pt1,
                               const VectorCartesian& pt2 )
{
    double r1 = pt1.Magnitude();
    double r2 = pt2.Magnitude();
    // assumption/approximation to accomodate terrestrial points that
    // were described in geodetic coordinates
    double rEff = grzHgt
                  + ( r1 < EARTH_RADIUS_NMI ? r1
                      : ( r2 < EARTH_RADIUS_NMI ? r2
                          : EARTH_RADIUS_NMI ) );

    if ( r1 < rEff || r2 < rEff ) {
        return OBSCURED;
    }

    double maxGeocentricAngle = acos(rEff / r1) + acos(rEff / r2);
    double cosang = testAsinAcosArg( pt1.DotProduct( pt2 ) / r1 / r2 );
    double geocentricAngle = acos(cosang);
    return ( geocentricAngle < maxGeocentricAngle
             ? UNOBSCURED : OBSCURED );
}

// ============================================================================
}
// A3 namespace end
// ============================================================================

