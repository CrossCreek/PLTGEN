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
 * timePerigeeApogee.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : timePerigeeApogee.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-001  08/22/1997  JAM  Initial Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :
//	Derived from SE
//	Given a state vector, returns the time past perigee and other stuff.

//****************************************************************************//
#include "timePerigeeApogee.h"
#include "TestValue.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
double timeSincePerigee( const VectorCartesian& eciPosition,
                         const VectorCartesian& eciVelocity,
                         double& meanMotion,
                         double& meanAnomaly,
                         double& trueAnomaly)
{
    double sma, eccentricAnomaly, cosEccentricAnomaly;
    double radius  = eciPosition.Magnitude();
    double rdotv   = eciPosition.DotProduct( eciVelocity );
    double etm0    = eciVelocity.MagnitudeSquared() / EARTH_GRAV_CONST_NMI3_PER_S2 - 1.0 / radius;
    double etm1    = rdotv / EARTH_GRAV_CONST_NMI3_PER_S2;
    VectorCartesian svEccentricity( eciPosition * etm0 - eciVelocity * etm1 );
    double eccentricitySq = svEccentricity.MagnitudeSquared();
    double eccentricity   = sqrt( eccentricitySq );
    double edotr  = svEccentricity.DotProduct( eciPosition );
    double cosAnomaly = testAsinAcosArg( edotr / eccentricity / radius );
    trueAnomaly = rdotv >= 0.0 ? acos( cosAnomaly )
                  : TWO_PI - acos( cosAnomaly);
    sma = radius * ( 1.0 + eccentricity * cosAnomaly )
          / ( 1.0 - eccentricitySq );
    meanMotion = sqrt( EARTH_GRAV_CONST_NMI3_PER_S2
                       / sma / sma / sma );
    cosEccentricAnomaly = (eccentricity + cosAnomaly)
                          / (1.0 + eccentricity * cosAnomaly);
    eccentricAnomaly = rdotv >= 0.0 ? acos( cosEccentricAnomaly )
                       : TWO_PI - acos( cosEccentricAnomaly);
    meanAnomaly = eccentricAnomaly - eccentricity * sin(eccentricAnomaly);
    return meanAnomaly / meanMotion;
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
double timeSincePerigee( const VectorCartesian& eciPosition,
                         const VectorCartesian& eciVelocity,
                         double& meanMotion,
                         double& meanAnomaly )
{
    double sma, eccentricAnomaly, cosEccentricAnomaly;
    double radius  = eciPosition.Magnitude();
    double rdotv   = eciPosition.DotProduct( eciVelocity );
    double etm0    = eciVelocity.MagnitudeSquared()
                     / EARTH_GRAV_CONST_NMI3_PER_S2
                     - 1.0 / radius;
    double etm1    = rdotv / EARTH_GRAV_CONST_NMI3_PER_S2;
    VectorCartesian svEccentricity( eciPosition * etm0 - eciVelocity * etm1 );
    double eccentricitySq = svEccentricity.MagnitudeSquared();
    double eccentricity   = sqrt( eccentricitySq );
    double edotr  = svEccentricity.DotProduct( eciPosition );
    double cosAnomaly = testAsinAcosArg( edotr / eccentricity / radius );
    sma = radius * ( 1.0 + eccentricity * cosAnomaly )
          / ( 1.0 - eccentricitySq );
    meanMotion = sqrt( EARTH_GRAV_CONST_NMI3_PER_S2
                       / sma / sma / sma );
    cosEccentricAnomaly = (eccentricity + cosAnomaly)
                          / (1.0 + eccentricity * cosAnomaly);
    eccentricAnomaly = rdotv >= 0.0 ? acos( cosEccentricAnomaly )
                       : TWO_PI - acos( cosEccentricAnomaly);
    meanAnomaly = eccentricAnomaly - eccentricity * sin(eccentricAnomaly);
    return meanAnomaly / meanMotion;
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
double vectorToTrueAnomaly( const VectorCartesian& eciPosition,
                            const VectorCartesian& eciVelocity )
{
    double radius  = eciPosition.Magnitude();
    double rdotv   = eciPosition.DotProduct( eciVelocity );
    double etm0    = eciVelocity.MagnitudeSquared()
                     / EARTH_GRAV_CONST_NMI3_PER_S2 - 1.0 / radius;
    double etm1    = rdotv / EARTH_GRAV_CONST_NMI3_PER_S2;
    VectorCartesian svEccentricity( eciPosition * etm0 - eciVelocity * etm1 );
    double eccentricitySq = svEccentricity.MagnitudeSquared();
    double eccentricity   = sqrt( eccentricitySq );
    double edotr  = svEccentricity.DotProduct( eciPosition );
    double cosAnomaly = testAsinAcosArg( edotr / eccentricity / radius );
    return rdotv >= 0.0 ? acos( cosAnomaly )
           : TWO_PI - acos( cosAnomaly);
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
