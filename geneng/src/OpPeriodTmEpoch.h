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
 * OpPeriodTmEpoch.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriodTmEpoch.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriodTmEpoch

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  JAM  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :



//****************************************************************************//

//==============================================================================
#ifndef OPPERIODTMEPOCH_H
#define OPPERIODTMEPOCH_H	"OpPeriodTmEpoch V2.0"

//------------------------------------------------------------------------------
// required headers
#include "OpPeriodTm.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class OpPeriodTmEpoch : public OpPeriodTm
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    inline OpPeriodTmEpoch( HandleConst< TimeJ2000 > hdlCurrentTime,
                            TimeJ2000 timeStart,
                            TimeJ2000 timeEnd );
    // inline OpPeriodTmEpoch( const OpPeriodTmEpoch& opPeriod );
    inline virtual ~OpPeriodTmEpoch();

    //----------------------------------------------------------------------
    // test function
    inline virtual bool Test();

    //----------------------------------------------------------------------
    // identification functions
    inline virtual OpPeriodType GetType() const;

    //----------------------------------------------------------------------
    // operators
    // inline OpPeriodTmEpoch& operator=( const OpPeriodTmEpoch& opPeriod );

    inline bool operator==( const OpPeriodTmEpoch& opPeriod ) const;
    inline bool operator!=( const OpPeriodTmEpoch& opPeriod ) const;

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
    TestBounds< TimeJ2000 > m_bndTime;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline OpPeriodTmEpoch::OpPeriodTmEpoch( HandleConst< TimeJ2000 > hdlCurrentTime,
        TimeJ2000 timeStart,
        TimeJ2000 timeEnd )
    : OpPeriodTm(hdlCurrentTime  ),
      m_bndTime( timeStart, timeEnd )
{
    Test();
}

/***
inline OpPeriodTmEpoch::OpPeriodTmEpoch( const OpPeriodTmEpoch& opPeriod )
: OpPeriodTm( opPeriod ),
  m_hdlCurrentTime( opPeriod.m_hdlCurrentTime ),
  m_bndTime( opPeriod.m_bndTime ) { ; }
***/

inline OpPeriodTmEpoch::~OpPeriodTmEpoch()
{ ; }

//------------------------------------------------------------------------------
// test function
inline bool OpPeriodTmEpoch::Test()
{
    m_bLastStatus = m_bndTime.Test( m_hdlCurrentTime.Ref() );
    return m_bLastStatus;
}

//------------------------------------------------------------------------------
// identification functions
inline OpPeriodTmEpoch::OpPeriodType OpPeriodTmEpoch::GetType() const
{
    return TIME_EPOCH;
}

//------------------------------------------------------------------------------
// operators
/***
inline OpPeriodTmEpoch& OpPeriodTmEpoch::operator=( const OpPeriodTmEpoch& opPeriod )
{
    OpPeriodTm::operator=( opPeriod );
    m_bndTime = opPeriod.m_bndTime;
    m_hdlCurrentTime = opPeriod.m_hdlCurrentTime;
    return *this;
}
***/

inline bool OpPeriodTmEpoch::operator==( const OpPeriodTmEpoch& opPeriod ) const
{
    return OpPeriodTm::operator==( opPeriod )
           && m_bndTime == opPeriod.m_bndTime;
}

inline bool OpPeriodTmEpoch::operator!=( const OpPeriodTmEpoch& opPeriod ) const
{
    return !operator==( opPeriod );
}

//------------------------------------------------------------------------------
// misc functions
inline const char* OpPeriodTmEpoch::GetVersion() const
{
    return OPPERIODTMEPOCH_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
