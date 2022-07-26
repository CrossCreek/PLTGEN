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
 * AERRates.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : AERRates.cpp

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

//  NOTES : internal angular units are radians

//****************************************************************************//

#include "AERRates.h"

#include <math.h>
#include <sstream>
#include <stdexcept>

namespace A3
{
// ============================================================================
// file scope constants and thresholds
namespace
{
const double LOWRANGETOL    =  1.0e-6;
const double COSTHRESHOLD   =  1.0e-9;
const double COSSQTHRESHOLD = COSTHRESHOLD * COSTHRESHOLD;
const double VSQTHRESHOLD   = -1.0e-9;

void ThrowBadTangentialVelocitySq( double tVel )
{
    std::ostringstream stmMsg;
    stmMsg << "Exception thrown by " << std::endl
           << "AERRates( "
           << "const VectorCartesian& relPos," << std::endl
           << "          "
           << "const VectorCartesian& relVel," << std::endl
           << "          "
           << "VectorAzimElevRange& AERPosition& relVel," << std::endl
           << "          "
           << "VectorAzimElevRange& AERRates )" << std::endl
           << "tangent velocity squared is less than 0" << std::endl
           << "tangent velocity squared = " << tVel << std::ends;
    std::runtime_error except( stmMsg.str() );
    throw except;
}

void ThrowBadTangentialAccelerationSq( double tAcc )
{
    std::ostringstream stmMsg;
    stmMsg << "Exception thrown by " << std::endl
           << "AERRates( "
           << "const VectorCartesian& relPos," << std::endl
           << "          "
           << "const VectorCartesian& relVel," << std::endl
           << "          "
           << "const VectorCartesian& relAccl," << std::endl
           << "          "
           << "VectorAzimElevRange& AERPosition," << std::endl
           << "          "
           << "VectorAzimElevRange& AERRates," << std::endl
           << "          "
           << "VectorAzimElevRange& AERAcceleration )" << std::endl
           << "tangent acceleration squared is less than 0" << std::endl
           << "tangent acceleration squared = " << tAcc << std::ends;
    std::runtime_error except( stmMsg.str() );
    throw except;
}

void ThrowAERRateMustNotBeNormalized()
{
    std::ostringstream stmMsg;
    stmMsg << "Exception thrown by " << std::endl
           << "AERRates( "
           << "const VectorCartesian& relPos," << std::endl
           << "          "
           << "const VectorCartesian& relVel," << std::endl
           << "          "
           << "VectorAzimElevRange& AERPosition," << std::endl
           << "          "
           << "VectorAzimElevRange& AERRates )" << std::endl
           << "AERRate must have Normization turned off!"
           << std::endl << std::ends;
    std::runtime_error except( stmMsg.str() );
    throw except;
}

void ThrowAERAccelerationMustNotBeNormalized()
{
    std::ostringstream stmMsg;
    stmMsg << "Exception thrown by " << std::endl
           << "AERRates( "
           << "const VectorCartesian& relPos," << std::endl
           << "          "
           << "const VectorCartesian& relVel," << std::endl
           << "          "
           << "const VectorCartesian& relAccl," << std::endl
           << "          "
           << "VectorAzimElevRange& AERPosition," << std::endl
           << "          "
           << "VectorAzimElevRange& AERRates," << std::endl
           << "          "
           << "VectorAzimElevRange& AERAcceleration )" << std::endl
           << "AERAcceleration must have Normization turned off!"
           << std::endl << std::ends;
    std::runtime_error except( stmMsg.str() );
    throw except;
}
}

// ============================================================================
// Calculates:
//  Angles[ Azimuth, Elevation, Range ]
//  Rates [ Azimuth, Elevation, Range ]
// returns Angular Rate
Angle aerRates(const VectorCartesian& relPos,
               const VectorCartesian& relVel,
               VectorAzimElevRange& AERPosition,
               VectorAzimElevRange& AERRates)
{
    // if the AERRates is has normalization turned on, the stores rate
    // will be wrong, therefore throw exception
    if ( AERRates.IsNormalized() ) {
        ThrowAERRateMustNotBeNormalized();
    }

    // ------------------------------------------------------------------------
    // calculate azimuth, elevation and range
    AERPosition  = relPos;
    double range = AERPosition.GetRange();
    // ------------------------------------------------------------------------
    // calculate rates
    double angularRate = 0.0;

    if ( LOWRANGETOL <= range ) {
        // --------------------------------------------------------------------
        // calculate range rate
        double rangeRate = relPos.DotProduct( relVel ) / range;
        // --------------------------------------------------------------------
        // calculate angular rate
        double tangentialVelocitySq = relVel.MagnitudeSquared()
                                      - rangeRate * rangeRate;

        if ( 0.0 <= tangentialVelocitySq ) {
            angularRate =  sqrt( tangentialVelocitySq ) / range;
        } else if ( tangentialVelocitySq < VSQTHRESHOLD ) {
            ThrowBadTangentialVelocitySq( tangentialVelocitySq );
        }

        // angular rate already initialized to zero
        // else angularRate = 0.0
        // ------------------------------------------------------------------------
        double rhoSq  = relPos[0] * relPos[0] + relPos[1] * relPos[1];

        if ( COSSQTHRESHOLD <= rhoSq / range / range ) {
            double azRate = ( relPos[1] * relVel[0] - relPos[0] * relVel[1] )
                            / rhoSq;
            double elRate = ( range * relVel[2] - relPos[2] * rangeRate )
                            / ( range * sqrt( rhoSq ) );
            AERRates.Set( azRate, elRate, rangeRate );
        } else {
            AERRates.Set( 0.0,
                          AERPosition.GetElevation() < 0.0 ? angularRate : -angularRate,
                          rangeRate );
        }
    }
    // ------------------------------------------------------------------------
    else { // range is less than the mimimum threshold
        // --------------------------------------------------------------------
        // set rates
        AERRates.Set( 0.0, 0.0, relVel.Magnitude() );
        // --------------------------------------------------------------------
        // calculate angular rate, already initialized to zero
        // angularRate = 0.0;
    }

    return angularRate;
}

// ============================================================================
// Calculates:
//  Angles[ Azimuth, Elevation, Range ]
//  Rates [ Azimuth, Elevation, Range ]
//  Acceleration [ Azimuth, Elevation, Range ]
// returns Angular Rate
Angle aerRates(const VectorCartesian& relPos,
               const VectorCartesian& relVel,
               const VectorCartesian& relAcc,
               VectorAzimElevRange& AERPosition,
               VectorAzimElevRange& AERRates,
               VectorAzimElevRange& AERAccel)
{
    // if the AERRates is has normalization turned on, the stores rate
    // and accelerations will be wrong, therefore throw exception
    if ( AERRates.IsNormalized() ) {
        ThrowAERRateMustNotBeNormalized();
    }

    if ( AERAccel.IsNormalized() ) {
        ThrowAERRateMustNotBeNormalized();
    }

    // ------------------------------------------------------------------------
    // calculate azimuth, elevation and range
    AERPosition  = relPos;
    double range = AERPosition.GetRange();
    // ------------------------------------------------------------------------
    // calculate rates
    double angularRate = 0.0;

    if ( LOWRANGETOL <= range ) {
        // --------------------------------------------------------------------
        // calculate range rate
        double rangeRate = relPos.DotProduct( relVel ) / range;
        double tangentialVelocitySq = relVel.MagnitudeSquared()
                                      - rangeRate * rangeRate;
        double rangeAcc = ( relPos.DotProduct( relAcc )
                            + tangentialVelocitySq ) / range;
        double azRate = 0.0;
        double elRate = 0.0;
        double azAcc  = 0.0;
        double elAcc  = 0.0;

        // --------------------------------------------------------------------
        // calculate angular rate
        if ( 0.0 <= tangentialVelocitySq ) {
            angularRate =  sqrt( tangentialVelocitySq ) / range;
        } else if ( tangentialVelocitySq < VSQTHRESHOLD ) {
            ThrowBadTangentialVelocitySq( tangentialVelocitySq );
        }

        // angular rate already initialized to zero
        // else angularRate = 0.0
        // ------------------------------------------------------------------------
        double rhoSq = relPos[0] * relPos[0] + relPos[1] * relPos[1];
        double rho   = sqrt( rhoSq );

        if ( COSSQTHRESHOLD <= rhoSq / range / range ) {
            azRate = ( relPos[1] * relVel[0] - relPos[0] * relVel[1] ) / rhoSq;
            elRate = ( range * relVel[2] - relPos[2] * rangeRate ) / range / rho;
            double s0 = ( relPos[0] * relVel[0] + relPos[1] * relVel[1] ) / rhoSq;
            azAcc = ( relPos[1] * relAcc[0] - relPos[0] * relAcc[1] ) / rhoSq
                    - 2.0 * azRate * s0;
            elAcc = ( range * relAcc[2] - relPos[2] * rangeAcc ) / range / rho
                    - elRate * ( rangeRate / range + s0 );
        } else { // elevation approx +90 deg or -90 deg
            elRate = AERPosition.GetElevation() < 0.0 ? angularRate : -angularRate;
            double tangentialAccSq = relAcc.MagnitudeSquared() - rangeAcc * rangeAcc;
            double angularAcc = 0.0;

            if ( 0.0 <= tangentialAccSq ) {
                angularAcc =  sqrt( tangentialAccSq ) / range;
            } else if ( tangentialAccSq < VSQTHRESHOLD ) {
                ThrowBadTangentialAccelerationSq( tangentialAccSq );
            }

            elAcc = AERPosition.GetElevation() < 0.0 ? angularAcc : -angularAcc;
        }

        AERRates.Set( azRate, elRate, rangeRate );
        AERAccel.Set( azAcc,  elAcc,  rangeAcc );
    }
    // ------------------------------------------------------------------------
    else { // range is less than the mimimum threshold
        // --------------------------------------------------------------------
        // set rates & acc
        AERRates.Set( 0.0, 0.0, relVel.Magnitude() );
        AERAccel.Set( 0.0, 0.0, relAcc.Magnitude() );
        // --------------------------------------------------------------------
        // calculate angular rate, already initialized to zero
        // angularRate = 0.0;
    }

    return angularRate;
}

// ============================================================================
// Returns Angular Rate
Angle  aerRates(const VectorCartesian& relPos,
                const VectorCartesian& relVel)
{
    double range = relPos.Magnitude();
    // ------------------------------------------------------------------------
    // calculate rates
    double angularRate = 0.0;

    if ( LOWRANGETOL <= range ) {
        // --------------------------------------------------------------------
        // calculate range rate
        double rangeRate = relPos.DotProduct( relVel ) / range;
        // --------------------------------------------------------------------
        // calculate angular rate
        double tangentialVelocitySq = relVel.MagnitudeSquared()
                                      - rangeRate * rangeRate;

        if ( 0.0 <= tangentialVelocitySq ) {
            angularRate =  sqrt( tangentialVelocitySq ) / range;
        } else if ( tangentialVelocitySq < VSQTHRESHOLD ) {
            ThrowBadTangentialVelocitySq( tangentialVelocitySq );
        }

        // angular rate already initialized to zero
        // else angularRate = 0.0
    }

    return angularRate;
}


// ============================================================================
} // namespace A3
// ============================================================================
