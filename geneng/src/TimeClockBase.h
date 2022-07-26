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
 * TimeClockBase.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================

//  FILE      : TimeClockBase.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : TimeClockBase

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0       10/24/2000 JAM  clock base created to support
//                                         SIVC application
//              V1.1       03/29/2001 JAM  add clock base class access function
//                                         and dynamic cast checking to support
//                                         Visualization's SSTRM_INTERACTIVE
//                                         mode
//              V2.0       04/29/2002 JAM  1st A3 library build
//              V2.004-017 12/23/2002 JJO  Move backwards again after get to end
//                                         of scenario when moving forward SstVis
//                                         Added OverrideEnd() public method
//              V3.3-017   06/21/2004 GJK  Move SetTime from protected to public
//              V4.5       2009-07-01 JAM  Added Handle.h include, required
//                                         since HandleConst.h no longer
//                                         includes Handle.h; remove superfluous
//                                         code
//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//==============================================================================
/// @file
/// @brief TimeClockBase abstract class definition
///
/// Defines the minimum class interface for clocks.  Clocks provide
/// start and end times and a pure virtual Run method that is implemented in
/// concrete derived clocks to advance the clock and return true while the
/// current time is within the start/end bounds, inclusive.  Status flag
/// functions are provided.
///
/// @author J. McCormick
/// @version V4.5 <BR>
/// Added Handle.h include, required since HandleConst.h no longer
/// includes Handle.h; removed superfluous code
/// @date 2009-07-01
///
/// @par Units
/// - Function Arg & Return: J2000 sec, TT
/// - Stream I/O:            ioTime manipulator defined
/// - Initialization Files:  N/A
//==============================================================================
#ifndef TIMECLOCKBASE_H
#define TIMECLOCKBASE_H  "TimeClockBase V4.5"

#include "Handle.h"
#include "HandleConst.h"
#include "TimeJ2000.h"

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// @brief Abstract base clock class
///
/// Defines the minimum class interface for clocks.  Clocks provide
/// start and end times and a pure virtual Run method that is implemented in
/// concrete derived clocks to advance the clock and return true while the
/// current time is within the start/end bounds, inclusive.  Status flag
/// functions are provided.
class TimeClockBase
{
public:
    // ---------------------------------------------------------------------
    TimeClockBase();

    /// @brief Copy constructor
    /// @details Copies all values except that it creates a new handle for
    /// the current time and copies the time from the old handle, if the
    /// old handle has a reference count greater than 1 and exception is
    /// thrown
    /// @param clk copy from clock
    TimeClockBase(const TimeClockBase& clk);

    /// Initialize clock's start and end time
    /// @param[in] start clock's start time
    /// @param[in] end clock's end time
    TimeClockBase(const TimeJ2000 &start, const TimeJ2000 &end);

    virtual ~TimeClockBase() {;}

    /// @brief Advances the clock
    /// @details Used to control a time loop, first call sets time to start
    /// @return true within start/end period (inclusive), false otherwise
    virtual bool Run() = 0;

    /// Handle to current time
    /// @return handle to the current time, J2000 sec, TT,
    /// keeps current as time advances
    HandleConst<TimeJ2000> GetHdlConstTime() const;

    /// Current time
    /// @return current time, J2000 sec, TT
    TimeJ2000 GetTime() const;

    /// Clock start time
    /// @return start time, J2000 sec, TT
    TimeJ2000 GetStart() const;

    /// Clock end time
    /// @return end time, J2000 sec, TT
    TimeJ2000 GetEnd() const;

    /// Clock increment
    /// @return seconds per clock tick
    virtual double GetSecondsPerTick() const = 0;

    /// Explicitly change the current time, overrides Run()
    /// @param[in] time current time, J2000 sec, TT
    /// @return true if time is between start and end, inclusive,
    /// false otherwise
    virtual bool SetTime(const TimeJ2000& time);

    /// Change end time (support for SATSIM VIS)
    /// @param[in] time new end time, J2000 sec, TT
    void OverrideEnd(const TimeJ2000 &time);

    /// Write clock state to stream
    /// @param[in,out] os output stream for writing state
    /// @return output stream
    virtual std::ostream& ReportState(std::ostream& os) const;

    /// @brief Assignment operator
    /// @details Copies all values except that it creates a new handle for
    /// the current time and copies the time from the old handle, if the
    /// old handle has a reference count greater than 1 and exception is
    /// thrown
    /// @param clk copy from clock
    /// @return reference to this
    TimeClockBase& operator=(const TimeClockBase& clk);

    virtual const char *GetVersion() const;

    // -----------------------------------------------------------------------
protected:
    // Is Current Time Within Interval
    bool IsCTWI() const
    {
        return m_isCTWI;
    }

    void SetStart(const TimeJ2000& time);
    void SetEnd(const TimeJ2000& time);

    virtual void Reset();

    // ----------------------------------------------------------------------
private:
    bool UpdateStatus();

    // current time, J2000
    Handle<TimeJ2000> m_hdlCurrentJ2000Time;

    // start time in J2000 seconds
    TimeJ2000 m_clockStart;

    // clock end time, seconds from epoch
    TimeJ2000 m_clockEnd;

    // is the clock currently running
    bool m_isCTWI;
};

// -------------------------------------------------------------------------
inline bool TimeClockBase::UpdateStatus()
{
    if (m_clockStart < m_clockEnd)
        m_isCTWI = *m_hdlCurrentJ2000Time >= m_clockStart
                   && *m_hdlCurrentJ2000Time <= m_clockEnd;
    else
        m_isCTWI = *m_hdlCurrentJ2000Time >= m_clockEnd
                   && *m_hdlCurrentJ2000Time <= m_clockStart;

    return m_isCTWI;
}

inline bool TimeClockBase::SetTime(const TimeJ2000& time)
{
    *m_hdlCurrentJ2000Time = time;
    return UpdateStatus();
}

inline void TimeClockBase::SetStart(const TimeJ2000& time)
{
    m_clockStart = time;
}

inline void TimeClockBase::SetEnd(const TimeJ2000& time)
{
    m_clockEnd = time;
}

inline void TimeClockBase::OverrideEnd(const TimeJ2000 &time)
{
    SetEnd(time);
}

inline TimeJ2000 TimeClockBase::GetStart() const
{
    return m_clockStart;
}

inline TimeJ2000 TimeClockBase::GetEnd() const
{
    return m_clockEnd;
}

inline HandleConst<TimeJ2000> TimeClockBase::GetHdlConstTime() const
{
    return m_hdlCurrentJ2000Time;
}

inline TimeJ2000 TimeClockBase::GetTime() const
{
    return *m_hdlCurrentJ2000Time;
}

inline void TimeClockBase::Reset()
{
    *m_hdlCurrentJ2000Time = GetStart();
    m_isCTWI = true;
}

inline const char* TimeClockBase::GetVersion() const
{
    return TIMECLOCKBASE_H;
}

} // A3 namespace end
#endif  // TimeClockBase
