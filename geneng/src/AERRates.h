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
 * AERRates.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : AERRates.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0      09/27/2001  JAM  initial release
//                                         derived from GGXLAR
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: deleted unused variables COSSQTHRESHOLD & COSTHRESHOLD
//              Reason: stops unused variable warning
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//                      support stringstream, newer compilers support it
//              Change: added newline at end of file
//              Reason: stops no newline at end of file warning

//              V4.5-001   2009-07-01 JAM  Moved tolerance and threshold
//                                         constants from function scope to
//                                         file scope unnamed namespace;
//                                         eliminates need for redundant defs
//                                         Moved initialization of aerRates
//                                         functor to Initialize.cpp

//              V5.0       2010-01-28 JAM  Replace AerRates class with POFs

//  NOTES : internal angular units are radians

//****************************************************************************//

#ifndef AERRATES_H
#define AERRATES_H __FILE__ " " __DATE__

#include "VectorCartesian.h"
#include "VectorAzimElevRange.h"

namespace A3
{
Angle aerRates(const VectorCartesian& relativePosition,
               const VectorCartesian& relativeVelocity);

Angle aerRates(const VectorCartesian& relativePosition,
               const VectorCartesian& relativeVelocity,
               VectorAzimElevRange& AERPosition,
               VectorAzimElevRange& AERRates);

Angle aerRates(const VectorCartesian& relPos,
               const VectorCartesian& relVel,
               const VectorCartesian& relAcc,
               VectorAzimElevRange& AERPosition,
               VectorAzimElevRange& AERRates,
               VectorAzimElevRange& AERAccel);
}

#endif // AERRATES_H
