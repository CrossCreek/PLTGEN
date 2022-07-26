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
 * OpPeriod.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriod.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriod (abstract class)

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  JAM  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings
//              Change: changed WIN32 marco to _MSC_VER
//              Reason: newer GNU compilers recognize MS predefined macros

//              V4.5-001   2009-07-01 JAM  Added #include <string> in header
//                                         Moved static OpPeriod string
//                                         descriptions to EnumMap member

//              V5.0       2010-06-01 JAM  Deleted unneeded headers

//  NOTES :



//==============================================================================
#ifndef OPPERIOD_H
#define OPPERIOD_H	"OpPeriod V4.3-009"

//------------------------------------------------------------------------------
// disable Microsoft's browser long name warning
#if defined(_MSC_VER) && (_MSC_VER < 1300)
// disables long template class name warning
#   pragma warning( disable : 4786 )
#endif
//------------------------------------------------------------------------------
// required headers
#include "TestValue.h"
#include <iosfwd>
#include <string>

// =============================================================================
// A3 namespace start
namespace A3
{
// -----------------------------------------------------------------------------
class OpPeriod
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // enumerations
    enum OpPeriodType { UNDEFINED, TIME, TIME_TOD, TIME_EPOCH, TPV,
                        TPV_PERIGEE, TPV_APOGEE, TPV_ANOMALY, TPV_ALTITUDE,
                        TPV_ECLIPSE, TPV_DESCENDING, TPV_ASCENDING,
                        PAYLOAD, LINK_PAIR
                      };

    //----------------------------------------------------------------------
    // constructors & destructors
    OpPeriod();
    OpPeriod( const OpPeriod& opPeriod );
    virtual ~OpPeriod();

    //----------------------------------------------------------------------
    // test function
    virtual bool Test() = 0;

    //----------------------------------------------------------------------
    // reporting functions
    virtual std::ostream& DoReport( std::ostream& osReport ) const;

    void SetReportPlatformNameOn( const std::string& strPlatformNm );
    void SetReportPlatformNameOff();
    bool IsReportPlatformNameOn() const;

    //----------------------------------------------------------------------
    // access functions
    virtual bool GetLastTestResults() const; // true = within op period

    //----------------------------------------------------------------------
    // identification functions
    virtual OpPeriodType GetType() const = 0;
    virtual const std::string& GetTypeName() const;

    virtual OpPeriodType GetClass() const = 0;
    virtual const std::string& GetClassName() const;

    //----------------------------------------------------------------------
    // operators
    virtual OpPeriod& operator=( const OpPeriod& opPeriod );

    bool operator==( const OpPeriod& opPeriod ) const;
    bool operator!=( const OpPeriod& opPeriod ) const;

    //----------------------------------------------------------------------
    // misc functions
    virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    bool m_bLastStatus;  // true = last status passed

    static OpPeriodType s_opPeriodType;

    //--------------------------------------------------------------------------
private:
    unsigned m_id;
    bool m_bReportPlatformNm;
    std::string m_strPlatformNm;

    static unsigned          s_nOpPeriods;
    static unsigned          s_idGen;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------

inline bool OpPeriod::GetLastTestResults() const
{
    return m_bLastStatus;
}

inline void
OpPeriod::SetReportPlatformNameOn( const std::string& strPlatformNm )
{
    m_strPlatformNm = strPlatformNm;
    m_bReportPlatformNm = true;
}

inline void
OpPeriod::SetReportPlatformNameOff()
{
    m_bReportPlatformNm = false;
}

//------------------------------------------------------------------------------
// operators
inline OpPeriod& OpPeriod::operator=( const OpPeriod& opPeriod )
{
    if ( this != &opPeriod ) {
        m_strPlatformNm = opPeriod.m_strPlatformNm;
        m_bLastStatus = opPeriod.m_bLastStatus;
        m_bReportPlatformNm = opPeriod.m_bReportPlatformNm;
        m_id = ++s_idGen;
    }

    return *this;
}

inline bool OpPeriod::operator==( const OpPeriod& opPeriod ) const
{
    // reconsider test of equality--should it be
    // m_id == opPeriod.m_id only?
    return m_id == opPeriod.m_id
           && m_bLastStatus == opPeriod.m_bLastStatus
           && m_strPlatformNm == opPeriod.m_strPlatformNm
           && m_bReportPlatformNm == opPeriod.m_bReportPlatformNm;
}

inline bool OpPeriod::operator!=( const OpPeriod& opPeriod ) const
{
    return !operator==( opPeriod );
}

//------------------------------------------------------------------------------
// misc functions
inline const char* OpPeriod::GetVersion() const
{
    return OPPERIOD_H;
}

//------------------------------------------------------------------------------
}
// A3 namespace end
// =============================================================================
#endif
