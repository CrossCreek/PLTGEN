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
 * timePerigeeApogee.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : timePerigeeApogee.h

//  NAMESPACE : A3

//  FILE      : timeSincePerigee.cpp

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-001  08/22/1997  JAM  Initial Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :
//	Derived from SE
//	Given a state vector, returns the time past perigee and other stuff.

//****************************************************************************//

#ifndef TMPASTPERIGEEAPOGEE_H
#define TMPASTPERIGEEAPOGEE_H  "timeSincePerigee V2.0"

#include "VectorCartesian.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
double timeSincePerigee( const VectorCartesian& eciPosition,
                         const VectorCartesian& eciVelocity,
                         double& meanMotion,
                         double& meanAnomaly,
                         double& trueAnomaly);

double timeSincePerigee( const VectorCartesian& eciPosition,
                         const VectorCartesian& eciVelocity,
                         double& meanMotion,
                         double& meanAnomaly );

double vectorToTrueAnomaly( const VectorCartesian& eciPosition,
                            const VectorCartesian& eciVelocity );

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline double timeSincePerigee( const VectorCartesian& eciPosition,
                                const VectorCartesian& eciVelocity,
                                double& meanMotion )
{
    double meanAnomaly;
    return timeSincePerigee( eciPosition, eciVelocity, meanMotion, meanAnomaly );
}


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline double timeSincePerigee( const VectorCartesian& eciPosition,
                                const VectorCartesian& eciVelocity )
{
    double meanMotion, meanAnomaly;
    return timeSincePerigee( eciPosition, eciVelocity, meanMotion, meanAnomaly );
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline double timeSinceApogee( const VectorCartesian& eciPosition,
                               const VectorCartesian& eciVelocity,
                               double& meanMotion,
                               double& meanAnomaly,
                               double& trueAnomaly )
{
    double tmPastPer = timeSincePerigee( eciPosition, eciVelocity,
                                         meanMotion,  meanAnomaly,
                                         trueAnomaly );
    double halfPeriod = PI / meanMotion;
    return ( tmPastPer > halfPeriod )
           ? tmPastPer - halfPeriod
           : tmPastPer + halfPeriod;
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline double timeSinceApogee( const VectorCartesian& eciPosition,
                               const VectorCartesian& eciVelocity,
                               double& meanMotion,
                               double& meanAnomaly )
{
    double tmPastPer = timeSincePerigee( eciPosition, eciVelocity,
                                         meanMotion,  meanAnomaly );
    double halfPeriod = PI / meanMotion;
    return ( tmPastPer > halfPeriod )
           ? tmPastPer - halfPeriod
           : tmPastPer + halfPeriod;
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline double timeSinceApogee( const VectorCartesian& eciPosition,
                               const VectorCartesian& eciVelocity,
                               double& meanMotion )
{
    double meanAnomaly;
    return timeSinceApogee( eciPosition, eciVelocity,
                            meanMotion,  meanAnomaly );
}


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
inline double timeSinceApogee( const VectorCartesian& eciPosition,
                               const VectorCartesian& eciVelocity )
{
    double meanMotion, meanAnomaly;
    return timeSinceApogee( eciPosition, eciVelocity,
                            meanMotion,  meanAnomaly );
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
