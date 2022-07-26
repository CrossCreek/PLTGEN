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
 * PatternLoss.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : PatternLoss.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : PatternLoss (abstract class)

//  AUTHOR(s) : J. McCormick, L. DiGrasso

//  UPDATES   : V1.3-007  10/09/1999
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: added assignment operator
//              Reason: if the class contains a const member, newer MS compilers
//              don't know how to create a default assignment operator

//  NOTES :  Given am off-bore sight angle, return the pattern loss



//****************************************************************************//

//==============================================================================
#ifndef PATTERNLOSS_H
#define PATTERNLOSS_H	"PatternLoss V4.3-009"

//------------------------------------------------------------------------------
// required headers
#include "Angle.h"
#include <sstream>
#include <stdexcept>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class PatternLoss
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    // virtual destructor is defined since this class will be inherited
    virtual ~PatternLoss() { ; }

    //----------------------------------------------------------------------
    // compute antenna pattern loss
    virtual double
    GetPatternLoss_dB( Angle offBoreSightAngle ) const = 0;

    virtual PatternLoss& operator=( const PatternLoss& )
    {
        return *this;
    }

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    inline static void CheckOffBoreSightAngle( Angle offBoreSightAngle );
    static const Angle s_maxOffBoreSightAngle;
    //--------------------------------------------------------------------------
private:
};

//==============================================================================
inline void PatternLoss::CheckOffBoreSightAngle( Angle offBoreSightAngle )
{
    if ( 0 <= offBoreSightAngle && s_maxOffBoreSightAngle >= offBoreSightAngle ) {
        std::ostringstream stmMsg;
        stmMsg << ioAngleDegrees
               << "PatternLoss::CheckOffBoreSightAngle Exception\n"
               << "Off BoreSight angle out of range (0, 180).\n"
               << "Off BoreSight angle = " << offBoreSightAngle
               << " degs.\n" << std::ends;
        std::range_error re( stmMsg.str() );
        throw re;
    }
}

//------------------------------------------------------------------------------
// misc functions
inline const char* PatternLoss::GetVersion() const
{
    return PATTERNLOSS_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
