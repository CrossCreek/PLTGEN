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
 * OpPeriodTpvAscending.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriodTpvAscending.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriodTpvAscending

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  JAM  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :



//****************************************************************************//

//==============================================================================
#ifndef OPPERIODTPVASCENDING_H
#define OPPERIODTPVASCENDING_H	"OpPeriodTpvAscending Version 2.0"

//------------------------------------------------------------------------------
// required headers
#include "OpPeriodTpv.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class OpPeriodTpvAscending : public OpPeriodTpv
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    inline OpPeriodTpvAscending( Handle< Spatial > hdlSpatial );
    inline virtual ~OpPeriodTpvAscending();

    //----------------------------------------------------------------------
    // test function
    inline virtual bool Test();

    //----------------------------------------------------------------------
    // identification functions
    inline virtual OpPeriodType GetType() const;

    //----------------------------------------------------------------------
    // operators
    inline bool operator==( const OpPeriodTpvAscending& opPeriod ) const;
    inline bool operator!=( const OpPeriodTpvAscending& opPeriod ) const;

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline OpPeriodTpvAscending::OpPeriodTpvAscending( Handle< Spatial > hdlSpatial )
    : OpPeriodTpv( hdlSpatial ) { ; }

inline OpPeriodTpvAscending::~OpPeriodTpvAscending()
{ ; }

//------------------------------------------------------------------------------
// test function
inline bool OpPeriodTpvAscending::Test()
{
    m_bLastStatus = m_hdlSpatial->GetVelocity().GetElement2() > 0.0;
    return m_bLastStatus;
}

//------------------------------------------------------------------------------
// identification functions
inline OpPeriodTpvAscending::OpPeriodType OpPeriodTpvAscending::GetType() const
{
    return TPV_DESCENDING;
}

//------------------------------------------------------------------------------
// operators
inline bool OpPeriodTpvAscending::operator==( const OpPeriodTpvAscending& opPeriod ) const
{
    return OpPeriodTpv::operator==( opPeriod );
}

inline bool OpPeriodTpvAscending::operator!=( const OpPeriodTpvAscending& opPeriod ) const
{
    return !operator==( opPeriod );
}

//------------------------------------------------------------------------------
// misc functions
inline const char* OpPeriodTpvAscending::GetVersion() const
{
    return OPPERIODTPVASCENDING_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
