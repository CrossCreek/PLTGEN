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
 * TimeClockCTA.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================

//  FILE      : TimeClockCTA.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : TimeClockCTA

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V4.5      2009-09-14  JAM  Initial release
//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//==============================================================================
/// @file
/// @brief TimeClockCTA class definition
///
/// Clock that is typically used in a while loop to set the current time.
/// Run increments the current time with the specified time step until the
/// computer clock time is reached than sets current time with the computer
/// clock thereafter.  The Run method returns true
/// while the current time is within the start/end bounds, inclusive.
/// Note that the smallest computer clock time increment is 1 second.
///
/// @author J. McCormick
/// @version V4.5 <BR>
/// Initial release.
/// @date 2009-09-14
///
/// @par Units
/// - Function Arg & Return: J2000 sec, TT
/// - Stream I/O:            ioTime manipulator defined
/// - Initialization Files:  N/A
//==============================================================================

#ifndef TIMECLOCKCTA_H
#define TIMECLOCKCTA_H  "TimeClockCTA V4.5"

#include "TimeClockBase.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// forward declaration for insertion & extraction operator prototypes
class TimeClockCTA;
std::ostream& operator<<(std::ostream& os, const TimeClockCTA &clk);
std::istream& operator>>(std::istream& is, TimeClockCTA &clk);

/// @brief TimeClockCTA class definition
///
/// Clock that is typically used in a while loop to set the current time.
/// Run increments the current time with the specified time step until the
/// computer clock time is reached than sets current time with the computer
/// clock thereafter.  The Run method returns true
/// while the current time is within the start/end bounds, inclusive.
/// Note that the smallest computer clock time increment is 1 second.
class TimeClockCTA : public TimeClockBase
{
public:
    TimeClockCTA();

    /// Initialize clock's start and end time and time increment
    /// @param[in] start clock's start time
    /// @param[in] end clock's end time
    /// @param[in] secondsPerClockTick # of second the clocks is changed
    /// for every call to Run(), must be positive
    TimeClockCTA(const TimeJ2000 &start,
                 const TimeJ2000 &end,
                 double secondsPerClockTick);

    /// @brief Advance the clock
    /// @details Used to control a time loop
    /// @return true within start/end period (inclusive), false otherwise
    virtual bool Run();

    /// Clock increment
    /// @return computer clock seconds between Run() calls
    virtual double GetSecondsPerTick() const;

    virtual const char *GetVersion() const;

    // ---------------------------------------------------------------------
protected:

    // ---------------------------------------------------------------------
private:
    double m_stepSz;
    double m_remainder;
    double m_secondsPerTick;
    bool m_bUsingCompTime;
};

// -------------------------------------------------------------------------
inline TimeClockCTA::TimeClockCTA()
    : m_stepSz(1.0), m_secondsPerTick(1.0), m_bUsingCompTime(false)
{ ; }

inline bool TimeClockCTA::Run()
{
    TimeJ2000 lstTime(GetTime());

    if (m_bUsingCompTime) {
        SetTime(getComputerTime());
    } else {
        TimeJ2000 nxtTime(lstTime+m_secondsPerTick);
        TimeJ2000 cmpTime(getComputerTime());

        if (nxtTime > cmpTime) {
            SetTime(cmpTime);
            m_bUsingCompTime = true;
        } else {
            SetTime(nxtTime);
        }
    }

    m_stepSz = GetTime() - lstTime;
    return IsCTWI();
}

inline double TimeClockCTA::GetSecondsPerTick() const
{
    return m_stepSz;
}

inline const char* TimeClockCTA::GetVersion() const
{
    return TIMECLOCKCTA_H;
}

} // A3 namespace end
#endif  // TIMECLOCKCTA_H
