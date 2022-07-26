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
 * PatternLoss.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : PatternLoss.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : PatternLoss (abstract class)

//  AUTHOR(s) : J. McCormick, L. DiGrasso

//  UPDATES   : V1.3-007  10/08/1999
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: added assignment operator
//              Reason: if the class contains a const member, newer MS compilers
//              don't know how to create a default assignment operator

//  NOTES :  system class.



//****************************************************************************//

//==============================================================================
#include "PatternLoss.h"
#include "constants.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
const Angle PatternLoss::s_maxOffBoreSightAngle = PI;
// ============================================================================
}
// A3 namespace end
// ============================================================================

// -----------------------------------------------------------------------------
