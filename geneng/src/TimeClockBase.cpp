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
 * TimeClockBase.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// TimeClockBase class
// Refer to TimeClockBase.h for more info.
//==============================================================================
#include "TimeClockBase.h"
#include <sstream>
// using math.h since Microsoft's cmath is not in the std namespace
#include <math.h>

// A3 namespace start
namespace A3
{
TimeClockBase::TimeClockBase()
    : m_hdlCurrentJ2000Time(new TimeJ2000(0.0)),
      m_clockStart(0.0),
      m_clockEnd(0.0),
      m_isCTWI(true)
{;}

TimeClockBase::TimeClockBase(const TimeJ2000 &start, const TimeJ2000 &end)
    : m_hdlCurrentJ2000Time(new TimeJ2000(start)),
      m_clockStart(start),
      m_clockEnd(end),
      m_isCTWI(true)
{;}

TimeClockBase::TimeClockBase(const TimeClockBase& clk)
    : m_hdlCurrentJ2000Time(new TimeJ2000(*clk.m_hdlCurrentJ2000Time)),
      m_clockStart(clk.m_clockStart),
      m_clockEnd(clk.m_clockEnd),
      m_isCTWI(clk.m_isCTWI)
{
    if (1 != clk.m_hdlCurrentJ2000Time.GetReferenceCount()) {
        std::ostringstream oss;
        oss << "TimeClockBase::TimeClockBase(const TimeClockBase& clk)\n"
            << "copy from object, clk, current time handle, "
            << "m_hdlCurrentJ2000Time,\nmust have a reference count of 1\n"
            << "reference count = "
            << clk.m_hdlCurrentJ2000Time.GetReferenceCount()
            << "clk state:\n";
        clk.ReportState(oss);
        throw std::runtime_error(oss.str());
    }
}

// -------------------------------------------------------------------------
TimeClockBase& TimeClockBase::operator=(const TimeClockBase& clk)
{
    if (this != &clk) {
        if (1 != clk.m_hdlCurrentJ2000Time.GetReferenceCount()) {
            std::ostringstream oss;
            oss << "TimeClockBase::operator=(const TimeClockBase& clk)\n"
                << "copy from object, clk, current time handle, "
                << "m_hdlCurrentJ2000Time,\nmust have a reference count of 1\n"
                << "reference count = "
                << clk.m_hdlCurrentJ2000Time.GetReferenceCount()
                << "clk state:\n";
            clk.ReportState(oss);
            throw std::runtime_error(oss.str());
        }

        m_hdlCurrentJ2000Time = new TimeJ2000(*clk.m_hdlCurrentJ2000Time);
        m_clockStart = clk.m_clockStart;
        m_clockEnd = clk.m_clockEnd;
        m_isCTWI = clk.m_isCTWI;
    }

    return *this;
}

std::ostream& TimeClockBase::ReportState( std::ostream& os ) const
{
    os << "Time                               = " << *GetHdlConstTime()
       << "\nTime Hdl Ref Count                 = "
       << GetHdlConstTime().GetReferenceCount()
       << "\nStart                              = " << GetStart()
       << "\nEnd                                = " << GetEnd()
       << "\nIs Current Time Withing Interval:  = " << IsCTWI()
       << '\n';
    return os;
}

}// A3 namespace end
