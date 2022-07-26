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
 * OpPeriodTpvAltitude.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriodTpvAltitude.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriodTpvAltitude

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  JAM  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES : Assumes constant Earth radius



//****************************************************************************//

//==============================================================================
#ifndef OPPERIODTPVALTITUDE_H
#define OPPERIODTPVALTITUDE_H	"OpPeriodTpvAltitude V2.0"

//------------------------------------------------------------------------------
// required headers
#include "OpPeriodTpv.h"
#include "TestValue.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class OpPeriodTpvAltitude : public OpPeriodTpv
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    inline OpPeriodTpvAltitude( Handle< Spatial > hdlSpatial,
                                double minAltitude,
                                double maxAltitude );
    inline virtual ~OpPeriodTpvAltitude();

    //----------------------------------------------------------------------
    // test function
    inline virtual bool Test();

    //----------------------------------------------------------------------
    // identification functions
    inline virtual OpPeriodType GetType() const;

    //----------------------------------------------------------------------
    // operators
    inline bool operator==( const OpPeriodTpvAltitude& opPeriod ) const;
    inline bool operator!=( const OpPeriodTpvAltitude& opPeriod ) const;

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
    TestBounds<double> m_bndRadius;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline OpPeriodTpvAltitude::OpPeriodTpvAltitude( Handle< Spatial > hdlSpatial,
        double minAltitude,
        double maxAltitude )
    : OpPeriodTpv( hdlSpatial ),
      m_bndRadius( minAltitude + EARTH_RADIUS_NMI, maxAltitude + EARTH_RADIUS_NMI )
{
    Test();
}

inline OpPeriodTpvAltitude::~OpPeriodTpvAltitude()
{ ; }

//------------------------------------------------------------------------------
// test function
inline bool OpPeriodTpvAltitude::Test()
{
    m_bLastStatus = m_bndRadius.Test( m_hdlSpatial->GetRadius() );
    return m_bLastStatus;
}

//------------------------------------------------------------------------------
// identification functions
inline OpPeriodTpvAltitude::OpPeriodType OpPeriodTpvAltitude::GetType() const
{
    return TPV_ALTITUDE;
}

//------------------------------------------------------------------------------
// operators
inline bool OpPeriodTpvAltitude::operator==( const OpPeriodTpvAltitude& opPeriod ) const
{
    return OpPeriodTpv::operator==( opPeriod )
           && ( m_bndRadius == opPeriod.m_bndRadius );
}

inline bool OpPeriodTpvAltitude::operator!=( const OpPeriodTpvAltitude& opPeriod ) const
{
    return !operator==( opPeriod );
}

//------------------------------------------------------------------------------
// misc functions
inline const char* OpPeriodTpvAltitude::GetVersion() const
{
    return OPPERIODTPVALTITUDE_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
