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
 * TimeClockCT.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// TimeClockCT class
// Refer to TimeClockCT.h for more info.
//==============================================================================
#include "TimeClockCT.h"
#include "IoSeparator.h"

// A3 namespace start
namespace A3
{
std::ostream& operator<<(std::ostream& os, const TimeClockCT &clk)
{
    os << clk.GetStart() << ioSeparator
       << clk.GetEnd();
    return os;
}

std::istream& operator>>(std::istream& is, TimeClockCT &clk)
{
    TimeJ2000 start, end;
    is >> start >> end;
    clk = TimeClockCT(start, end);
    return is;
}

} // A3 namespace end
