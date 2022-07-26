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
 * PatternLossCCIR.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : PatternLossCCIR.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : PatternLossCCIR (abstract class)

//  AUTHOR(s) : J. McCormick, L. DiGrasso

//  UPDATES   : V1.3-007  10/15/1999
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it

//  NOTES :  system class.



//****************************************************************************//

//==============================================================================
#include "PatternLossCCIR.h"
#include "constants.h"
// using math.h since Microsoft's cmath is not in the std namespace
#include <math.h>
#include <iostream>
#include <sstream>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
const Angle PatternLossCCIR::s_ccirThresAngle3 = .83775804; // 48 deg


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
PatternLossCCIR::PatternLossCCIR(double maxGain, double diameter,
                                 double carrierWaveLength ):
    m_maxGain(maxGain),
    m_antennaDiameterOverWaveLength(diameter / carrierWaveLength)
{
}

//==============================================================================
//Destructor located in .h file


//==============================================================================
// compute antenna pattern loss
double
PatternLossCCIR::GetPatternLoss_dB( Angle offBoreSightAngle ) const
{
    Angle  ccirThresAngle1;
    Angle  ccirThresAngle2;
    double patternLoss( 0.0 );

    try {
        double gain;
        gain = 2.0 + 15.0 * log10(m_antennaDiameterOverWaveLength);
        // simplified math .34906585 = 20 * DEG_TO_RAD
        ccirThresAngle1 = .34906585 / m_antennaDiameterOverWaveLength
                          * sqrt(m_maxGain - gain);

        // 0 < offBoreSightAngle < theta1
        if ( offBoreSightAngle < ccirThresAngle1) {
            double obsRatioProd;
            obsRatioProd = offBoreSightAngle * m_antennaDiameterOverWaveLength;
            // simplified math 8.207016 = .0025 * (180/PI)^2
            gain = m_maxGain - 8.207016 * obsRatioProd*obsRatioProd;
        } else {
            if (m_antennaDiameterOverWaveLength >= 100.0) {
                // simplified math .27663469 = 15.85 * DEG_TO_RAD
                ccirThresAngle2 = .27663469
                                  * pow(m_antennaDiameterOverWaveLength,-0.6);

                // if theta1 < offBoreSightAngle < theta2, do NOT recalc gain
                if ( offBoreSightAngle >= ccirThresAngle2 ) {
                    // theta2 < offBoreSightAngle < theta3 (theta3=48deg)
                    if ( offBoreSightAngle < s_ccirThresAngle3 ) {
                        gain = 32.0 - 25.0
                               * log10(offBoreSightAngle * RAD_TO_DEG);
                    } else { // offBoreSightAngle > theta3 (theta3=48deg)
                        gain = -10.0;
                    }
                }
            } else {
                // 1.7453292 = 100 * Deg_TO_RAD
                ccirThresAngle2 = 1.7453292 / m_antennaDiameterOverWaveLength;

                // if theta1 < offBoreSightAngle < theta2, do NOT recalc gain
                if ( offBoreSightAngle >= ccirThresAngle2 ) {
                    if ( offBoreSightAngle < s_ccirThresAngle3 ) {
                        gain = 52.0
                               - 10.0 * log10( m_antennaDiameterOverWaveLength )
                               - 25.0 * log10( offBoreSightAngle * RAD_TO_DEG);
                    } else if ( offBoreSightAngle <= s_maxOffBoreSightAngle ) {
                        gain = 10.0
                               - 10.0 * log10( m_antennaDiameterOverWaveLength );
                    } else {
                        std::ostringstream stmMsg;
                        stmMsg << ioAngleDegrees
                               << "Antenna pattern CCIR_PATTERN\n"
                               << "angle is greater than 180 deg.\n"
                               << "off bore sight angle = "
                               << offBoreSightAngle << " deg.\n" << std::ends;
                        std::runtime_error except( stmMsg.str() );
                        throw except;
                    }
                }
            }//end if (m_antennaDiameterOverWaveLength >= 100.0)
        }//end if ( offBoreSightAngle < ccirThresAngle1)

        patternLoss = m_maxGain - gain;
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "PatternLossCCIR::GetPatternLoss_dB\n"
                  << std::endl;
        throw;
    }

    return patternLoss;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
