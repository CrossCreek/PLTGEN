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
 * PatternLossCCIR.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : PatternLossCCIR.h

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

//  NOTES :  Given am off-bore sight angle, return the pattern loss



//****************************************************************************//

//==============================================================================
#ifndef PATTERNLOSSCCIR_H
#define PATTERNLOSSCCIR_H	"PatternLossCCIR V4.3-009"

//------------------------------------------------------------------------------
// required headers
#include "PatternLoss.h"
#include "Angle.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class PatternLossCCIR : public PatternLoss
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    // virtual destructor is defined since this class will be inherited
    PatternLossCCIR(double maxGain, double diameter, double carrierWaveLength);
    virtual ~PatternLossCCIR() { ; }

    //----------------------------------------------------------------------
    // compute antenna pattern loss
    virtual double
    GetPatternLoss_dB( Angle offBoreSightAngle ) const;

    virtual PatternLossCCIR&
    operator=( const PatternLossCCIR& )
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
    static const Angle  s_ccirThresAngle3;

    const double m_maxGain;
    const double m_antennaDiameterOverWaveLength;
};

//------------------------------------------------------------------------------
// misc functions
inline const char* PatternLossCCIR::GetVersion() const
{
    return PATTERNLOSSCCIR_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
