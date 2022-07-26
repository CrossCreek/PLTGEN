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
 * OpPeriodTm.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriodTm.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriodTm (abstact class)

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  JAM  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :



//****************************************************************************//

//==============================================================================
#ifndef OPPERIODTM_H
#define OPPERIODTM_H	"OpPeriodTm Version 2.0"

//------------------------------------------------------------------------------
// required headers
#include "OpPeriod.h"
#include "TimeJ2000.h"
#include "HandleConst.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class OpPeriodTm : public OpPeriod
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    inline OpPeriodTm( HandleConst< TimeJ2000 > hdlCurrentTime );
    // inline OpPeriodTm( const OpPeriodTm& opPeriod );
    inline virtual ~OpPeriodTm();

    //----------------------------------------------------------------------
    // identification functions
    inline virtual OpPeriodType GetType() const;
    inline virtual OpPeriodType GetClass() const;

    //----------------------------------------------------------------------
    // operators
    // inline OpPeriodTm& operator=( const OpPeriodTm& opPeriod );

    inline bool operator==( const OpPeriodTm& opPeriod ) const;
    inline bool operator!=( const OpPeriodTm& opPeriod ) const;

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    HandleConst< TimeJ2000 > m_hdlCurrentTime;

    //--------------------------------------------------------------------------
private:
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline OpPeriodTm::OpPeriodTm( HandleConst< TimeJ2000 > hdlCurrentTime )
    : m_hdlCurrentTime( hdlCurrentTime ) { ; }

// inline OpPeriodTm::OpPeriodTm( const OpPeriodTm& opPeriod )
// : OpPeriod( opPeriod ) { ; }

inline OpPeriodTm::~OpPeriodTm()
{ ; }

//------------------------------------------------------------------------------
// identification functions
inline OpPeriodTm::OpPeriodType OpPeriodTm::GetType() const
{
    return TIME;
}

inline OpPeriodTm::OpPeriodType OpPeriodTm::GetClass() const
{
    return TIME;
}

//------------------------------------------------------------------------------
// operators
// inline OpPeriodTm& OpPeriodTm::operator=( const OpPeriodTm& opPeriod )
// {
//     if ( this != &opPeriod ) OpPeriod::operator=( opPeriod );
//     return *this;
// }

inline bool OpPeriodTm::operator==( const OpPeriodTm& opPeriod ) const
{
    return OpPeriodTm::operator==( opPeriod )
           && m_hdlCurrentTime.Ref() == opPeriod.m_hdlCurrentTime.Ref();
}

inline bool OpPeriodTm::operator!=( const OpPeriodTm& opPeriod ) const
{
    return !operator==( opPeriod );
}

//------------------------------------------------------------------------------
// misc functions
inline const char* OpPeriodTm::GetVersion() const
{
    return OPPERIODTM_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
