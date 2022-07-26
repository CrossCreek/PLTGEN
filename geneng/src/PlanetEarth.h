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
 * PlanetEarth.h
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

//  FILE      : PlanetEarth.h

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
#ifndef PLANETEARTH_H
#define PLANETEARTH_H   "PlanetEarth 2.0"

#include "VectorCartesian.h"
#include "TimeJ2000.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
Angle greenwichAngle( TimeJ2000 time );

// ============================================================================
double earthRadiusOfCurvature( double latitude );
double earthRadiusToRefEllipse( double latitude );

//==============================================================================
enum VisibilityCond { OBSCURED, UNOBSCURED };

VisibilityCond checkEarthObsc( double grzHgt,
                               const VectorCartesian& pt1,
                               const VectorCartesian& pt2 );

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // PLANETEARTH_H
