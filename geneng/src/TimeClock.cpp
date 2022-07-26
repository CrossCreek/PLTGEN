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
 * TimeClock.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// TimeClock class
// Refer to TimeClock.h for more info.
//==============================================================================
#include "TimeClock.h"
#include "IoSeparator.h"
#include <stdexcept>
// using math.h since Microsoft's cmath is not in the std namespace
#include <math.h>

// A3 namespace start
namespace A3
{
TimeClock::TimeClock()
    : m_nTicks(0),
      m_remainder(0.0),
      m_nSamples(0),
      m_secondsPerTick(0.0),
      m_isDaySampleModeSet(false),
      m_startOfDay(0.0),
      m_daysSampled(0),
      m_nSampleDays(0),
      m_dSampleDays(0)
{;}

TimeClock::TimeClock(const TimeJ2000 &start,
                     const TimeJ2000 &end,
                     double secondsPerClockTick)
    : TimeClockBase(start, end),
      m_nTicks(0),
      m_remainder(0.0),
      m_nSamples(0),
      m_secondsPerTick(secondsPerClockTick),
      m_isDaySampleModeSet(false),
      m_startOfDay(start),
      m_daysSampled(0),
      m_nSampleDays(0),
      m_dSampleDays(0)
{;}

TimeClock::TimeClock(const TimeJ2000 &start,
                     unsigned nDays,
                     int dDays,
                     double secondsPerClockTick)
    : TimeClockBase(start, start),
      m_nTicks(0),
      m_remainder(0.0),
      m_nSamples(0),
      m_secondsPerTick(secondsPerClockTick),
      m_isDaySampleModeSet(true),
      m_startOfDay(start),
      m_daysSampled(0),
      m_nSampleDays(nDays),
      m_dSampleDays(dDays)
{
    // calculate the end time
    SetEnd(start - m_secondsPerTick + DAY_TO_SEC
           * static_cast<double>((m_nSampleDays - 1)
                                 * m_dSampleDays + 1));
}

// -------------------------------------------------------------------------
bool TimeClock::AdvanceClock(long nTicks)
{
    // adding the current time vs. incrementing the time by the
    // time increment reduces round off error
    m_nTicks += nTicks;
    TimeClockBase::SetTime(GetStart() + static_cast<double>(m_nTicks)
                           * m_secondsPerTick + m_remainder);

    if (m_isDaySampleModeSet) {
        double secondsIntoDay = GetTime() > m_startOfDay
                                ? GetTime() - m_startOfDay
                                : m_startOfDay  - GetTime();

        if (secondsIntoDay >= DAY_TO_SEC) {
            m_daysSampled++;
            m_startOfDay += static_cast<double>(m_dSampleDays)
                            * DAY_TO_SEC;
            TimeClockBase::SetTime(m_startOfDay);
        }
    }

    ++m_nSamples;
    return IsCTWI();
}

bool TimeClock::SetTime(const TimeJ2000& time)
{
    if (m_isDaySampleModeSet)
        throw std::runtime_error(
            "TimeClock: calling SetTime in day sample mode is invalid");

    // adding the current time vs. incrementing the time by the
    // time increment reduces round off error
    bool status = TimeClockBase::SetTime(time);
    m_nTicks    = static_cast<long>((time-GetStart()) / m_secondsPerTick);
    m_remainder = time - GetStart()
                  - static_cast<double>(m_nTicks) * m_secondsPerTick;
    m_nSamples++;
    return status;
}

void TimeClock::Reset()
{
    TimeClockBase::Reset();
    m_nTicks       = 0;
    m_remainder    = 0.0;
    m_startOfDay   = 0;
    m_nSamples     = 0;
    m_daysSampled  = 0;
}

void TimeClock::SetSecondsPerTick(double secondsPerClockTick)
{
    if (!secondsPerClockTick) {
        std::string strMsg("TimeClock::SetSecondsPerTick(");
        strMsg += "long secondsPerClockTick)\n";
        strMsg += "Invalid argument: secondsPerClockTick = 0";
        throw std::runtime_error(strMsg.c_str());
    }

    m_secondsPerTick = secondsPerClockTick;
    double secFromStart = GetTime() - GetStart();
    m_nTicks = static_cast<long>(secFromStart / m_secondsPerTick);
    m_remainder = secFromStart - static_cast<double>(m_nTicks)
                  * m_secondsPerTick;
}

double TimeClock::GetSecondsPerTick() const
{
    return m_secondsPerTick;
}

long TimeClock::GetNSamples() const
{
    return m_nSamples;
}

long TimeClock::GetDaysSampled() const
{
    if (m_isDaySampleModeSet) {
        return m_daysSampled;
    }

    return static_cast<long>(GetTime() / DAY_TO_SEC);
}

// -------------------------------------------------------------------------
std::ostream& TimeClock::ReportState(std::ostream& os)
{
    TimeClockBase::ReportState(os);
    std::ios_base::fmtflags fg(os.flags());
    os.setf(std::ios_base::boolalpha);
    os << "Time Increment     = " << GetSecondsPerTick() << " seconds\n"
       << "Day Sample Mode    = " << m_isDaySampleModeSet << '\n'
       << "Num Samp Days      = " << m_nSampleDays << '\n'
       << "Samp Day Delta     = " << m_dSampleDays << " days\n";
    os.flags(fg);
    return os;
}

std::ostream& operator<<(std::ostream& os, const TimeClock &clk)
{
    os << clk.GetStart() << ioSeparator
       << clk.GetEnd() << ioSeparator
       << clk.GetSecondsPerTick();
    return os;
}

std::istream& operator>>(std::istream& is, TimeClock &clk)
{
    TimeJ2000 start, end;
    double secondsPerTick;
    is >> start >> end >> secondsPerTick;
    clk = TimeClock(start, end, secondsPerTick);
    return is;
}

} // A3 namespace end
