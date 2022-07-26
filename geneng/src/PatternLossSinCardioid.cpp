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
 * PatternLossSinCardioid.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : PatternLossSinCardioid.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : PatternLossSinCardioid (abstract class)

//  AUTHOR(s) : J. McCormick, L. DiGrasso

//  UPDATES   : V1.3-007  10/15/1999
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: added assignment operator
//              Reason: if the class contains a const member, newer MS compilers
//              don't know how to create a default assignment operator

//  NOTES :  system class.



//****************************************************************************//

//==============================================================================
#include "PatternLossSinCardioid.h"
#include <string>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
PatternLossSinCardioid::PatternLossSinCardioid(double maxGain):
    m_maxGain( maxGain )
{
}

//==============================================================================
//Destructor located in .h file


//==============================================================================
// compute antenna pattern loss
double
PatternLossSinCardioid::GetPatternLoss_dB( Angle ) const // offBoreSightAngle
{
    std::string strMsg( "PatternLossSinCardioid::GetPatternLoss_dB " );
    strMsg += "not implemented yet!";
    throw std::runtime_error( strMsg );
    // patternLoss = m_maxGain - 10.0 * (log10(0.2399 + 0.9083
    //             * fabs(cos(offBoreSightAngle))) );
    // return 0;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
