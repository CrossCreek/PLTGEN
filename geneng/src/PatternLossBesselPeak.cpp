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
 * PatternLossBesselPeak.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : PatternLossBesselPeak.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : PatternLossBesselPeak (abstract class)

//  AUTHOR(s) : J. McCormick, L. DiGrasso

//  UPDATES   : V1.3-007  10/15/1999
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings
//              Change: added assignment operator
//              Reason: if the class contains a const member, newer MS compilers
//              don't know how to create a default assignment operator

//  NOTES :  system class.



//****************************************************************************//

//==============================================================================
#include "PatternLossBesselPeak.h"
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
const double PatternLossBesselPeak::s_boreNormalAngle      = HALF_PI;

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
PatternLossBesselPeak::PatternLossBesselPeak(double diameter,
        double carrierWaveLength ):
    m_besselThresAngle1( 1.4392354 / (diameter / carrierWaveLength) ),
    m_besselThresAngle2_const (7.015 / (diameter * PI) ),
    m_besselThresAngle3_const (30.55 / (diameter * PI) ),
    m_carrierWaveLength( carrierWaveLength ),
    m_diameter( diameter ),
    m_diaWaveLenRatio( diameter / carrierWaveLength ),
    m_diaWaveLenRatioTimesPI( diameter / carrierWaveLength * PI)
// 1.4392354 = 20 * sqrt(17) * DEG_TO_RAD
{
}

//==============================================================================
//Destructor located in .h file


//==============================================================================
// compute antenna pattern loss
double
PatternLossBesselPeak::GetPatternLoss_dB( Angle offBoreSightAngle ) const
{
    double patternLoss(0.0);
    Angle  besselThresAngle2;
    Angle  besselThresAngle3;

    try {
        if ( offBoreSightAngle < m_besselThresAngle1 ) {
            double obsRatioProd;
            obsRatioProd = offBoreSightAngle * m_diaWaveLenRatio;
            // patternLoss = 0.0025 * obsRatioProd * obsRatioProd;
            // 8.2070159 = .0025 * (180/PI)^2
            patternLoss = 8.2070159 * obsRatioProd * obsRatioProd;
        } else {
            besselThresAngle2 = 0.80 * asin(m_carrierWaveLength
                                            * m_besselThresAngle2_const);

            if ( offBoreSightAngle < besselThresAngle2 ) {
                patternLoss = 17.0;
            } else {
                besselThresAngle3 = asin(m_carrierWaveLength
                                         * m_besselThresAngle3_const);

                if ( offBoreSightAngle < besselThresAngle3 ) {
                    patternLoss = 17.0 * pow(( log10(m_diaWaveLenRatioTimesPI
                                                     * sin( offBoreSightAngle ) )
                                               / log10(m_diaWaveLenRatioTimesPI
                                                       * sin(besselThresAngle2)) )
                                             , 1.23 );
                } else if ( offBoreSightAngle <= s_boreNormalAngle ) {
                    patternLoss = 39.42 * pow(( log10(m_diaWaveLenRatioTimesPI
                                                      * sin(offBoreSightAngle))
                                                / log10(m_diaWaveLenRatioTimesPI
                                                        * sin(besselThresAngle3)) )
                                              , 1.088 );
                } else if ( offBoreSightAngle <= s_maxOffBoreSightAngle ) {
                    patternLoss = 39.42 * pow((log10(m_diaWaveLenRatioTimesPI)
                                               / log10(m_diaWaveLenRatioTimesPI
                                                       * sin(besselThresAngle3)) )
                                              , 1.088 );
                } else {
                    std::ostringstream stmMsg;
                    stmMsg << ioAngleDegrees
                           << "Antenna pattern BESSEL_PEAK\n"
                           << "angle is greater than 180 deg.\n"
                           << "off bore sight angle = "
                           << offBoreSightAngle << " deg.\n" << std::ends;
                    std::runtime_error except( stmMsg.str() );
                    throw except;
                }
            }
        }
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "PatternLossBesselPeak::GetPatternLoss_dB\n"
                  << std::endl;
        throw;
    }

    return patternLoss;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
