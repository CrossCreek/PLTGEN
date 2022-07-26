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
 * TimeClockCTA.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// TimeClockCTA class
// Refer to TimeClockCTA.h for more info.
//==============================================================================
#include "TimeClockCTA.h"
#include "IoSeparator.h"

// A3 namespace start
namespace A3
{
TimeClockCTA::TimeClockCTA(const TimeJ2000 &start,
                           const TimeJ2000 &end,
                           double secondsPerClockTick)
    : TimeClockBase(start, end), m_stepSz(secondsPerClockTick),
      m_secondsPerTick(secondsPerClockTick), m_bUsingCompTime(false)
{
    if (0.0 > m_secondsPerTick) {
        std::string sMsg("TimeClockCTA::TimeClockCTA(");
        sMsg += "const TimeJ2000 &start,\n";
        sMsg += "const TimeJ2000 &end\n";
        sMsg += "double secondsPerClockTick)\n";
        sMsg += "secondsPerClockTick must be positive";
        std::runtime_error(sMsg.c_str());
    }
}

std::ostream& operator<<(std::ostream& os, const TimeClockCTA &clk)
{
    os << clk.GetStart() << ioSeparator
       << clk.GetEnd() << ioSeparator
       << clk.GetSecondsPerTick();
    return os;
}

std::istream& operator>>(std::istream& is, TimeClockCTA &clk)
{
    TimeJ2000 start, end;
    double secondsPerTick;
    is >> start >> end >> secondsPerTick;
    clk = TimeClockCTA(start, end, secondsPerTick);
    return is;
}

} // A3 namespace end
