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
 * PatternLossBesselPeak.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : PatternLossBesselPeak.h

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

//  NOTES :  Given am off-bore sight angle, return the pattern loss



//****************************************************************************//

//==============================================================================
#ifndef PATTERNLOSSBESSELPEAK_H
#define PATTERNLOSSBESSELPEAK_H	"PatternLossBesselPeak V4.3-009"

//------------------------------------------------------------------------------
// required headers
#include "PatternLoss.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class PatternLossBesselPeak : public PatternLoss
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    // virtual destructor is defined since this class will be inherited
    PatternLossBesselPeak(double diameter, double carrierWaveLength);
    virtual ~PatternLossBesselPeak() { ; }

    //----------------------------------------------------------------------
    // compute antenna pattern loss
    virtual double
    GetPatternLoss_dB( Angle offBoreSightAngle ) const;

    virtual PatternLossBesselPeak&
    operator=( const PatternLossBesselPeak& )
    {
        return *this;
    }

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
    //----------------------------------------------------------------------
    //
    // virtual destructor is defined since this class will be inherited

    static const double s_boreNormalAngle;

    const double m_besselThresAngle1;
    const double m_besselThresAngle2_const;
    const double m_besselThresAngle3_const;
    const double m_carrierWaveLength;
    const double m_diameter;
    const double m_diaWaveLenRatio;
    const double m_diaWaveLenRatioTimesPI;
};

//------------------------------------------------------------------------------
// misc functions
inline const char* PatternLossBesselPeak::GetVersion() const
{
    return PATTERNLOSSBESSELPEAK_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
