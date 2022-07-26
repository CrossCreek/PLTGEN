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
 * TimeClock.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================

//  FILE      : TimeClock.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : TimeClock

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//                                         changed time offset from int to
//                                         double to accomodate fractions
//                                         of a seconds
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V1.3-006  12/20/1999  JAM  reorganized manipulator decl & defs
//                                         to reduce compile time overhead;
//                                         made IoManipulator more
//                                         flexible; changed ioSeparator from
//                                         global to stream dependent (hence
//                                         more flexible) like other manips
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.5-001  2009-07-01  JAM  Added stdexcept include to cpp file
//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//==============================================================================
/// @file
/// @brief TimeClock class definition
///
/// Clock that is typically used in a while loop to automatically increment or
/// decrement time.  Run method is provided to advance the clock and returns
/// true while the current time is within the start/end bounds, inclusive.  A
/// day sampling mode is provided.
///
/// @author J. McCormick
/// @version V4.5 <BR>
/// Added stdexcept include to cpp file & documentation update
/// @date 2009-07-01
///
/// @par Units
/// - Function Arg & Return: J2000 sec, TT
/// - Stream I/O:            ioTime manipulator defined
/// - Initialization Files:  N/A
//==============================================================================
#ifndef TIMECLOCK_H
#define TIMECLOCK_H  "TimeClock V4.5"

#include "TimeClockBase.h"
#include "constants.h"

/// Astrodynamics, Allocation and Availability
namespace A3
{
// forward declaration for insertion & extraction operator prototypes
class TimeClock;
std::ostream& operator<<(std::ostream& os, const TimeClock &clk);
std::istream& operator>>(std::istream& is, TimeClock &clk);

/// @brief TimeClock class definition
///
/// Clock that is typically used in a while loop to automatically increment
/// or decrement time.  Run method is provided to advance the clock and
/// returns true while the current time is within the start/end bounds,
/// inclusive.  The clock can monotonically increase or decrease the time
/// from the start time to the end time.  Alternatively, a day sampling
/// mode is available.  In this mode, the clock runs for a
/// number of days with the start of each day separated by a
/// specified number of days.  The clock runs at the specified
/// time increment for each of these days.
/// (e.g. run the clock for 12 days at 60 second time increments,
/// the start of each day is separated by 30 days).
class TimeClock : public TimeClockBase
{
public:
    TimeClock();

    /// Initialize clock's start and end time and time increment
    /// @param[in] start clock's start time
    /// @param[in] end clock's end time
    /// @param[in] secondsPerClockTick # of second the clocks is changed
    /// for every call to Run()<BR>
    /// Note that start may be before or after end,
    /// to run the clock backwards, set secondsPerClockTick < 0
    TimeClock(const TimeJ2000 &start,
              const TimeJ2000 &end,
              double secondsPerClockTick = 1.0);

    /// Initialize clock for day sampling
    /// @param[in] start clock's start time
    /// @param[in] nDays number of whole days (24hr) to sample
    /// @param[in] dDays between the start of each day sampled
    /// (i.e., dDay-1 not sampled between days that were sampled)
    /// dDays may be negative
    /// @param[in] secondsPerClockTick # of second the clocks is changed
    /// for every call to Run()
    TimeClock(const TimeJ2000 &start,
              unsigned nDays, int dDays,
              double secondsPerClockTick = 1.0);

    virtual ~TimeClock() {;}

    /// @brief Advances the clock
    /// @details Used to control a time loop, first call sets time to start
    /// @return true within start/end period (inclusive), false otherwise
    virtual bool Run();

    /// Explicitly change the current time, overrides Run()
    /// @param[in] time current time, J2000 sec, TT
    /// @return true if time is between start and end, inclusive,
    /// false otherwise<BR>
    /// This function increment the sample counter
    virtual bool SetTime(const TimeJ2000& time);

    /// Change time increment
    /// @param[in] secondsPerClockTick # of second the clocks is changed
    /// for every call to Run()
    virtual void SetSecondsPerTick(double secondsPerClockTick);

    /// @brief Reset the clock
    /// @details The current time is set to the start time
    /// time number of samples is set to 0
    virtual void Reset();

    /// Clock increment
    /// @return seconds per clock tick
    virtual double GetSecondsPerTick() const;

    /// Number of times sampled
    /// @return number of time samples that have been processed since the
    /// clock started running, reseting the clock set this value to 0
    long GetNSamples() const;

    /// Number of days sampled
    /// @return number of days that were completed
    long GetDaysSampled() const;

    /// Write clock state to stream
    /// @param[in,out] os output stream for writing state
    /// @return output stream
    virtual std::ostream& ReportState(std::ostream& os);

    virtual const char *GetVersion() const;

    // ---------------------------------------------------------------------
protected:
    // advance clock nTick time increments
    // increments the number of time samples
    virtual bool AdvanceClock(long nTicks);

    // ---------------------------------------------------------------------
private:
    long m_nTicks;
    double m_remainder;

    // number of times the clock has been incremented
    // the value is valid only if the clock
    // monotomically increases or decreases in time
    // note the start time counts as 1 sample
    // note: clock ticks pass m_clockEnd don't count
    long m_nSamples;

    // number of seconds per time increment, may be negative
    double m_secondsPerTick;

    // is the clock running in day sample mode
    bool m_isDaySampleModeSet;
    // start of current day, seconds from start time
    TimeJ2000 m_startOfDay;
    // number of days completely sampled
    unsigned m_daysSampled;
    // number of days to sample
    unsigned m_nSampleDays;
    // time increment between sample days, units of DAY
    // m_dSampleDays = start_of_day(n) - start_of_day(n-1)
    int m_dSampleDays;
};

// -------------------------------------------------------------------------
inline bool TimeClock::Run()
{
    if (IsCTWI() && 0 < m_nSamples) {
        AdvanceClock(1);
    } else {
        ++m_nSamples;
    }

    return IsCTWI();
}

inline const char* TimeClock::GetVersion() const
{
    return TIMECLOCK_H;
}

} // A3 namespace end
#endif  // TimeClock
