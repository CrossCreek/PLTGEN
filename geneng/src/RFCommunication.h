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
 * RFCommunication.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : RFCommunication.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : Communication

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  981021  JAM  alpha development
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES : header file for the general communication
//          contants, functions and classes

//          note: angular units are radians
//                time units are seconds
//                length units are as indicated

//          Contents derive from SE and SST Ver 1.0.



//****************************************************************************//

//=============================================================================

#ifndef RFCOMMUNICATION_H
#define RFCOMMUNICATION_H   "RFCommunication: V2.0"

// ----------------------------------------------------------------------------
// headers
// using math.h since Microsoft's cmath is not in the std namespace
#include <math.h>
#include "constants.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// freeSpacePathLoss: computes the free space path loss given the wave length
//      and range in the same units of length
inline double freeSpacePathLoss( double waveLength, double range )
{
    double srpl = FOUR_PI * range / waveLength;
    return srpl * srpl;
}

inline double freeSpacePathLoss_dB( double waveLength, double range )
{
    return 20.0 * log10( FOUR_PI * range / waveLength );
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif // RFCOMMUNICATION_H
