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
 * OpPeriodTpv.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriodTpv.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriodTpv (abstact class)

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  JAM  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: changed WIN32 marco to _MSC_VER
//              Reason: newer GNU compilers recognize MS predefined macros

//  NOTES :



//****************************************************************************//

//==============================================================================
#ifndef OPPERIODTPV_H
#define OPPERIODTPV_H	"OpPeriodTpv V4.3-009"

// ============================================================================
// disable Microsoft's browser long name warning
#if defined(_MSC_VER) && (_MSC_VER < 1300)
// disables long template class name warning
#   pragma warning( disable : 4786 )
#endif
// ============================================================================

//------------------------------------------------------------------------------
// required headers
#include "OpPeriod.h"
#include "Spatial.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class OpPeriodTpv : public OpPeriod
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    inline OpPeriodTpv( Handle< Spatial > hdlSpatial );
    inline virtual ~OpPeriodTpv();

    //----------------------------------------------------------------------
    // test function
    inline virtual bool Test() = 0;

    //----------------------------------------------------------------------
    // identification functions
    inline virtual OpPeriodType GetType() const;
    inline virtual OpPeriodType GetClass() const;

    //----------------------------------------------------------------------
    // operators
    inline bool operator==( const OpPeriodTpv& opPeriod ) const;
    inline bool operator!=( const OpPeriodTpv& opPeriod ) const;

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    Handle< Spatial > m_hdlSpatial;

    //--------------------------------------------------------------------------
private:
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline OpPeriodTpv::OpPeriodTpv( Handle< Spatial > hdlSpatial )
    : m_hdlSpatial( hdlSpatial ) { ; }

inline OpPeriodTpv::~OpPeriodTpv()
{ ; }

//------------------------------------------------------------------------------
// identification functions
inline OpPeriodTpv::OpPeriodType OpPeriodTpv::GetType() const
{
    return TPV;
}

inline OpPeriodTpv::OpPeriodType OpPeriodTpv::GetClass() const
{
    return TPV;
}

//------------------------------------------------------------------------------
// operators
inline bool OpPeriodTpv::operator==( const OpPeriodTpv& opPeriod ) const
{
    return OpPeriodTpv::operator==( opPeriod )
           && ( m_hdlSpatial.Ref() == opPeriod.m_hdlSpatial.Ref() );
}

inline bool OpPeriodTpv::operator!=( const OpPeriodTpv& opPeriod ) const
{
    return !operator==( opPeriod );
}

//------------------------------------------------------------------------------
// misc functions
inline const char* OpPeriodTpv::GetVersion() const
{
    return OPPERIODTPV_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
