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
 * TimeClockCT.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================

//  FILE      : TimeClockCT.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : TimeClockCT

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0      10/24/2000  JAM  clock base created to support
//                                         SIVC application
//              V1.1      03/29/2001  JAM  add clock base class access function
//                                         and dynamic cast checking to support
//                                         Visualization's SSTRM_INTERACTIVE
//                                         mode
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.5      2009-09-14  JAM  Adjusted current time value to the
//                                         V4.5 J2000, TT standard
//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//==============================================================================
/// @file
/// @brief TimeClockCT class definition
///
/// Clock that is typically used in a while loop to set the current time to
/// the computer clock time when Run is called.  The Run method returns
/// true while the current time is within the start/end bounds, inclusive.
/// Note that the smallest time increment is 1 second.
///
/// @author J. McCormick
/// @version V4.5 <BR>
/// Adjusted current time value to the V4.5 J2000, TT standard & documentation
/// update
/// @date 2009-09-14
///
/// @par Units
/// - Function Arg & Return: J2000 sec, TT
/// - Stream I/O:            ioTime manipulator defined
/// - Initialization Files:  N/A
//==============================================================================
#ifndef TIMECLOCKCT_H
#define TIMECLOCKCT_H  "TimeClockCT V4.5"

#include "TimeClockBase.h"

/// Astrodynamics, Allocation and Availability
namespace A3
{
// forward declaration for insertion & extraction operator prototypes
class TimeClockCT;
std::ostream& operator<<(std::ostream& os, const TimeClockCT &clk);
std::istream& operator>>(std::istream& is, TimeClockCT &clk);

/// @brief TimeClockCT class definition
/// @details Clock that is typically used in a while loop to set the
/// current time to the computer clock time when Run is called.  The
/// Run method returns true while the current time is within the start/end
/// bounds, inclusive.  Note that the smallest time increment is 1 second.
class TimeClockCT : public TimeClockBase
{
public:
    TimeClockCT();

    /// Initialize clock's start and end time
    /// @param[in] start clock's start time
    /// @param[in] end clock's end time
    TimeClockCT(const TimeJ2000 &start,
                const TimeJ2000 &end);

    /// @brief Advance the clock
    /// @details Used to control a time loop
    /// @return true within start/end period (inclusive), false otherwise
    virtual bool Run();

    /// Clock increment
    /// @return computer clock seconds between Run() calls
    virtual double GetSecondsPerTick() const;

    virtual const char *GetVersion() const;

    // -----------------------------------------------------------------------
protected:

    // -----------------------------------------------------------------------
private:
    double m_stepSz;
};

// -------------------------------------------------------------------------
inline TimeClockCT::TimeClockCT()
    : m_stepSz(1.0)
{ ; }

inline TimeClockCT::TimeClockCT(const TimeJ2000 &start,
                                const TimeJ2000 &end)
    : TimeClockBase(start, end), m_stepSz(1.0)
{ ; }

inline bool TimeClockCT::Run()
{
    static const TimeJ2000 TIME_NOT_SET;
    TimeJ2000 lstTime(GetTime());

    if (TIME_NOT_SET==lstTime) {
        lstTime = GetStart();
    }

    SetTime(getComputerTime());
    m_stepSz = GetTime() - lstTime;
    return IsCTWI();
}

inline double TimeClockCT::GetSecondsPerTick() const
{
    return m_stepSz;
}

inline const char* TimeClockCT::GetVersion() const
{
    return TIMECLOCKCT_H;
}

} // A3 namespace end
#endif  // TIMECLOCKCT_H
