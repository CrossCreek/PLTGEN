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
 * OpPeriodTpvAnomaly.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriodTpvAnomaly.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriodTpvAnomaly

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  JAM  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :



//****************************************************************************//

//==============================================================================
#ifndef OPPERIODTPVANOMALY_H
#define OPPERIODTPVANOMALY_H	"OpPeriodTpvAnomaly V2.0"

//------------------------------------------------------------------------------
// required headers
#include "timePerigeeApogee.h"
#include "OpPeriodTpv.h"
#include "Normalize.h"
#include "TestValue.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class OpPeriodTpvAnomaly : public OpPeriodTpv
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    inline OpPeriodTpvAnomaly( Handle< Spatial > hdlSpatial,
                               double trueAnomalyStart,
                               double trueAnomalyEnd );
    inline virtual ~OpPeriodTpvAnomaly();

    //----------------------------------------------------------------------
    // test function
    inline virtual bool Test();

    //----------------------------------------------------------------------
    // identification functions
    inline virtual OpPeriodType GetType() const;

    //----------------------------------------------------------------------
    // operators
    inline bool operator==( const OpPeriodTpvAnomaly& opPeriod ) const;
    inline bool operator!=( const OpPeriodTpvAnomaly& opPeriod ) const;

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
    TestBounds<double> m_bndTrueAnomaly;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline OpPeriodTpvAnomaly::OpPeriodTpvAnomaly( Handle< Spatial > hdlSpatial,
        double trueAnomalyStart,
        double trueAnomalyEnd )
    : OpPeriodTpv( hdlSpatial ),
      m_bndTrueAnomaly( normalizeZeroTwoPi( trueAnomalyStart ),
                        normalizeZeroTwoPi( trueAnomalyEnd ) )
{
    Test();
}

inline OpPeriodTpvAnomaly::~OpPeriodTpvAnomaly()
{
    ;
}

//------------------------------------------------------------------------------
// test function
inline bool OpPeriodTpvAnomaly::Test()
{
    double trueAnom = vectorToTrueAnomaly( m_hdlSpatial->GetPosition(),
                                           m_hdlSpatial->GetVelocity() );
    m_bLastStatus = m_bndTrueAnomaly.Test( trueAnom );
    return m_bLastStatus;
}

//------------------------------------------------------------------------------
// identification functions
inline OpPeriodTpvAnomaly::OpPeriodType OpPeriodTpvAnomaly::GetType() const
{
    return TPV_ANOMALY;
}

//------------------------------------------------------------------------------
// operators
inline bool OpPeriodTpvAnomaly::operator==( const OpPeriodTpvAnomaly& opPeriod ) const
{
    return OpPeriodTpv::operator==( opPeriod )
           && ( m_bndTrueAnomaly == opPeriod.m_bndTrueAnomaly );
}

inline bool OpPeriodTpvAnomaly::operator!=( const OpPeriodTpvAnomaly& opPeriod ) const
{
    return !operator==( opPeriod );
}

//------------------------------------------------------------------------------
// misc functions
inline const char* OpPeriodTpvAnomaly::GetVersion() const
{
    return OPPERIODTPVANOMALY_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
