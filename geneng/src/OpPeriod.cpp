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
 * OpPeriod.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriod.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriod

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings
//              Change: changed WIN32 marco to _MSC_VER
//              Reason: newer GNU compilers recognize MS predefined macros

//              V4.5-001   2009-07-01 JAM  Added #include <string> in header

//              V5.0       2010-06-01 JAM  Deleted unneeded headers

//  NOTES :



//****************************************************************************//
#include "OpPeriod.h"

#include "EnumMap.h"
#include <algorithm>
#include <iomanip>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// OpPeriod helper functions
namespace
{
class OpPeriodMap : public EnumMap<OpPeriod::OpPeriodType>
{
public:
    OpPeriodMap();
};

OpPeriodMap::OpPeriodMap()
{
    /*insert(value_type(OpPeriod::UNDEFINED,"Undefined"));
    insert(value_type(OpPeriod::TIME,"Time"));
    insert(value_type(OpPeriod::TIME_TOD,"Time_of_Day"));
    insert(value_type(OpPeriod::TIME_EPOCH,"Epoch"));
    insert(value_type(OpPeriod::TPV,"Time_Pos_Vel"));
    insert(value_type(OpPeriod::TPV_PERIGEE,"Time_Rel_to_Perigee"));
    insert(value_type(OpPeriod::TPV_APOGEE,"Time_Rel_to_Apogee"));
    insert(value_type(OpPeriod::TPV_ANOMALY,"True_Anomaly_Range"));
    insert(value_type(OpPeriod::TPV_ALTITUDE,"Altitude_Range"));
    insert(value_type(OpPeriod::TPV_ECLIPSE,"Eclipse_Condition"));
    insert(value_type(OpPeriod::TPV_DESCENDING,"Descending_Pass"));
    insert(value_type(OpPeriod::TPV_ASCENDING,"Ascending_Pass"));
    insert(value_type(OpPeriod::PAYLOAD,"Active_Payload"));
    insert(value_type(OpPeriod::LINK_PAIR,"Active_Link_Pair"));*/
}

OpPeriodMap opPeriodMap;

bool lesserStrSz(OpPeriodMap::value_type a, OpPeriodMap::value_type b)
{
    return std::string(a.second).size() < std::string(b.second).size();
}
}

unsigned OpPeriod::s_nOpPeriods = 0;
unsigned OpPeriod::s_idGen = 0;

//------------------------------------------------------------------------------
// constructors & destructors
OpPeriod::OpPeriod()
    : m_bLastStatus( false ),
      m_id( ++s_idGen ),
      m_bReportPlatformNm( false )
{
    ++s_nOpPeriods;
}

OpPeriod::OpPeriod( const OpPeriod& opPeriod )
    : m_bLastStatus( opPeriod.m_bLastStatus ),
      m_id( ++s_idGen ),
      m_bReportPlatformNm( opPeriod.m_bReportPlatformNm ),
      m_strPlatformNm( opPeriod.m_strPlatformNm )
{
    ++s_nOpPeriods;
}

OpPeriod::~OpPeriod()
{
    --s_nOpPeriods;
}

//------------------------------------------------------------------------------
// identification functions
const std::string& OpPeriod::GetTypeName() const
{
    return opPeriodMap(GetType());
}

const std::string& OpPeriod::GetClassName() const
{
    return opPeriodMap(GetClass());
}

//------------------------------------------------------------------------------
// reporting functions
std::ostream& OpPeriod::DoReport( std::ostream& osReport ) const
{
    if ( m_bReportPlatformNm ) {
        osReport << " OPSP__04 "
                 << std::setw(12) << m_strPlatformNm;
    } else {
        osReport << " OPS___03";
    }

    std::string::size_type longStrSz =
        std::max_element(opPeriodMap.begin(), opPeriodMap.end(), lesserStrSz)
        ->second.size();
    osReport << std::setw(4) << m_id << ' '
             << std::setw(std::streamsize(longStrSz)) << GetTypeName() << ' '
             << ( m_bLastStatus ? " Passed " : " Failed " );
    return osReport;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
