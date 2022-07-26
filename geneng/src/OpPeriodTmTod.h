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
 * OpPeriodTmTod.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriodTmTod.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriodTmTod

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  JAM  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :



//****************************************************************************//

//==============================================================================
#ifndef OPPERIODTMTOD_H
#define OPPERIODTMTOD_H	"OpPeriodTmTod V2.0"

//------------------------------------------------------------------------------
// required headers
#include "OpPeriodTm.h"
#include "Normalize.h"
#include "constants.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class OpPeriodTmTod : public OpPeriodTm
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    inline OpPeriodTmTod( HandleConst< TimeJ2000 > hdlCurrentTime,
                          double timeOfDayStartSeconds,
                          double timeOfDayEndSeconds );
    // inline OpPeriodTmTod( const OpPeriodTmTod& opPeriod );
    inline virtual ~OpPeriodTmTod();

    //----------------------------------------------------------------------
    // test function
    inline virtual bool Test();

    //----------------------------------------------------------------------
    // identification functions
    inline virtual OpPeriodType GetType() const;

    //----------------------------------------------------------------------
    // operators
    // inline OpPeriodTmTod& operator=( const OpPeriodTmTod& opPeriod );

    inline bool operator==( const OpPeriodTmTod& opPeriod ) const;
    inline bool operator!=( const OpPeriodTmTod& opPeriod ) const;

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
    TestBounds< double > m_bndTimeOfDay;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline OpPeriodTmTod::OpPeriodTmTod( HandleConst< TimeJ2000 > hdlCurrentTime,
                                     double timeOfDayStart,
                                     double timeOfDayEnd )
    : OpPeriodTm( hdlCurrentTime ),
      m_bndTimeOfDay( timeOfDayStart, timeOfDayEnd )
{
    Test();
}

/***
inline OpPeriodTmTod::OpPeriodTmTod( const OpPeriodTmTod& opPeriod )
: OpPeriodTm( opPeriod ),
  m_hdlCurrentTime( opPeriod.m_hdlCurrentTime ),
  m_bndTimeOfDay( opPeriod.m_bndTimeOfDay ) { ; }
***/

inline OpPeriodTmTod::~OpPeriodTmTod()
{ ; }

//------------------------------------------------------------------------------
// test function
inline bool OpPeriodTmTod::Test()
{
    static const Normalize< double > normalizeTOD( 0.0, DAY_TO_SEC );
    static const double halfDay = DAY_TO_SEC / 2.0;
    double seconds = fmod( m_hdlCurrentTime.Ref(), DAY_TO_SEC )
                     + halfDay;
    seconds = normalizeTOD( seconds );
    m_bLastStatus = m_bndTimeOfDay.Test( seconds );
    return m_bLastStatus;
}

//------------------------------------------------------------------------------
// identification functions
inline OpPeriodTmTod::OpPeriodType OpPeriodTmTod::GetType() const
{
    return TIME_TOD;
}

//------------------------------------------------------------------------------
// operators
/***
inline OpPeriodTmTod& OpPeriodTmTod::operator=( const OpPeriodTmTod& opPeriod )
{
    OpPeriodTm::operator=( opPeriod );
    m_bndTimeOfDay = opPeriod.m_bndTimeOfDay;
    m_hdlCurrentTime = opPeriod.m_hdlCurrentTime;
    return *this;
}
***/

inline bool OpPeriodTmTod::operator==( const OpPeriodTmTod& opPeriod ) const
{
    return OpPeriodTm::operator==( opPeriod )
           && m_bndTimeOfDay == opPeriod.m_bndTimeOfDay;
}

inline bool OpPeriodTmTod::operator!=( const OpPeriodTmTod& opPeriod ) const
{
    return !operator==( opPeriod );
}

//------------------------------------------------------------------------------
// misc functions
inline const char* OpPeriodTmTod::GetVersion() const
{
    return OPPERIODTMTOD_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
