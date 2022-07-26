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
 * TimeJ2000.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
//  FILE      : TimeJ2000.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-001  09/26/1997  JAM  Initial Release
//              V1.3-006  12/20/1999  JAM  reorganized manipulator decl & defs
//                                         to reduce compile time overhead;
//                                         made IoManipulator more
//                                         flexible; changed ioSeparator from
//                                         global to stream dependent (hence
//                                         more flexible) like other manips
//              V1.3-016  01/26/2000  JAM  made destructor virtual
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.5      07/01/2009  JAM  Added time conversion
//                                         functions, added extraction
//                                         exception handling
//              V5.0      2010-01-18  JAM  Replaced A3::timeRefConv,
//                                         A3::TimeDB::dat, A3::TimeDB::dtat
//                                         globals with  functions, changed
//                                         timeRefConv operator() to const,
//                                         deleted private statics
//                                         s_TIME_NOT_SET and s_TIME_FOREVER
//                                         deleted namespace constant TIME_NOT_SET
//                                         replaced ioSpc with new ioDelim,
//                                         moved class static defs

//  Doxygen Documentation
/// @file
/// @brief TimeJ2000 class and time system conversions
///
/// TimeJ2000 class, leap second database (DAT), rotational-dynamical
/// database (DUT), and conversions between terrestrial time (TT), international
/// atomic time (TAI), coordinated universal time (UTC) and universal time
/// corrected (UT1).
/// <BR> The standard epoch, J2000.0, is 2000 January 1, 12h TT
/// (JD 2451545.0 TT) at the geocenter.
/// <BR> Internally time is J2000 TT unless noted otherwise.
/// @par
/// Per USNO CIRCULAR NO. 179, 2005-10-20
/// <BR> UT1 is now defined to be a linear function of a the Earth Rotation
/// Angle (theta).
///
/// @author J. McCormick
/// @version V5.0 <BR>
/// Replaced A3::timeRefConv, A3::TimeDB::dat, A3::TimeDB::dtat globals with
/// functions, changed timeRefConv operator() to const <BR>
/// deleted private statics s_TIME_NOT_SET and s_TIME_FOREVER <BR>
/// replaced ioSpc with new ioDelim
/// @date 2010-01-18
///
/// @author J. McCormick
/// @version V4.5 <BR>
/// Added time conversion functions, added extraction exception handling
/// @date 2009-07-01
///
/// @author J. McCormick
/// @version 2.0 A3 library build
/// @date 2002-04-26
///
/// @author J. McCormick
/// @version V1.3-016 made destructor virtual
/// @date 2000-01-26
///
/// @author J. McCormick
/// @version V1.3-006
/// @date 1999-12-20
///
/// @author J. McCormick
/// @version V1.2-001 Initial release
/// @date 1997-09-26
///
/// @par Units
/// - Function Arg & Return: sec
/// - Stream I/O:            manipulator set
/// - Initialization Files:  defined per file i/f spec
// =============================================================================
#ifndef TIMEJ2000_H
#define TIMEJ2000_H   __FILE__ " " __DATE__

#include "TimeBase.h"
#include "Handle.h"
#include "constants.h"
#include "EnumMap.h"
#include <istream>
#include <utility>
#include <map>
#include <time.h>

/// Astrodynamics, Allocation and Availability
namespace A3
{
// forward declaration
class TimeCalendar;

// TimeJ2000 represents time w.r.t. epoch J2000 terrestrial time
class TimeJ2000;

/// TimeR2000 represents time w.r.t. a NON-terrestrial time
/// 1 January 2000 12:00:00 epoch
typedef TimeJ2000 TimeR2000;

/// @brief Represents time since epoch J2000 terrestrial time (TT)
///
/// Internally time is J2000 TT seconds unless noted otherwise.
/// <BR> Epoch J2000 = 1 January 2000 12:00:00 TT
class TimeJ2000
{
public:
    TimeJ2000();

    /// Conversion constructor, seconds
    /// @param[in] seconds from epoch J2000


    TimeJ2000(const double& seconds);
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit TimeJ2000(const double& seconds);

    /// Conversion constructor, calendar time
    /// @param[in] cal calendar time


    TimeJ2000( const TimeCalendar& cal );
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit TimeJ2000( const TimeCalendar& cal );

    virtual ~TimeJ2000() { ; }

    /// Casting operator
    /// @return seconds from epoch J2000
    operator const double&() const;

    /// Modified Julian Date
    /// @return modified julian date, days
    double MJD_Day() const;

    /// Julian Date
    /// @return modified julian date, days
    double JD_Day() const;

    /// Addition assignment operator
    /// @param[in] deltaTimeSeconds add to *this
    /// @return *this reference
    TimeJ2000& operator+=(const double& deltaTimeSeconds);

    /// Subtraction assignment operator
    /// @param[in] deltaTimeSeconds subtract from *this
    /// @return *this reference
    TimeJ2000& operator-=(const double& deltaTimeSeconds);

    /// Valid range of J2000 values
    /// @return [minimum, maximum] limits
    static const std::pair<double, double>& Limits();

protected:
    double m_seconds; // seconds from J2000

private:
    // class static members
    static const double s_1600X;
    static const int s_CENTURY_INC;
    static const int s_YEAR_INC;
    static const int  s_MONTHCUM[];

    // TimeJ2000 limits
    static const std::pair<double, double> s_J2000_LIMITS;
};

// -------------------------------------------------------------------------
inline TimeJ2000::operator const double&() const
{
    return m_seconds;
}

inline double TimeJ2000::MJD_Day() const
{
    return m_seconds*SEC_TO_DAY+MJD_J2000_DAY;
}

inline double TimeJ2000::JD_Day() const
{
    return m_seconds*SEC_TO_DAY+JD_J2000_DAY;
}

// -------------------------------------------------------------------------
/// TimeJ2000 insertion operator
/// @param[in, out] os stream receiving TimeJ2000
/// @param[in] t TimeJ2000 inserted into stream
/// @return stream reference
std::ostream& operator<<(std::ostream& os, const TimeJ2000 &t);

/// TimeJ2000 extraction operator
/// @param[in, out] is stream containing TimeJ2000
/// @param[out] t TimeJ2000 extracted from stream
/// @return stream reference
std::istream& operator>>(std::istream& is, TimeJ2000 &t);

/// Retrieve value indicating that time is not set
/// @return time not set value
inline const TimeJ2000& timeNotSet()
{
    static const TimeJ2000 TNS;
    return TNS;
}

// =========================================================================
/// Time Database
namespace TimeDB
{
/// Leap second database
class DAT
{
public:
    DAT();
    virtual ~DAT() {;}

    /// Time offset
    /// @param[in] utcR2000 universal time, coordinated, sec
    /// @return time offset, sec
    double OffsetUTC(const TimeR2000& utcR2000);

    /// Time offset
    /// @param[in] ttJ2000 terrestrial time, J2000 sec
    /// @return time offset, sec
    double OffsetTT(const TimeJ2000& ttJ2000);

    /// Write state to stream
    /// @param[in, out] os stream receiving state info
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:
    void Initialize();

    /// Initialize from stream
    /// @param[in, out] is stream containing time offsets
    /// @return input stream
    std::istream& Initialize(std::istream& is);

private:
    typedef std::pair<TimeJ2000, TimeJ2000> J2000Rng;
    typedef std::pair<TimeR2000, TimeR2000> R2000Rng;

    R2000Rng m_utcRng; // [start, end)
    double m_curOffsetR;

    J2000Rng m_j2000Rng; // [start, end)
    double m_curOffsetJ;

    std::map<TimeR2000, double> m_ctUtcDelta;
    std::map<TimeJ2000, double> m_ctTtDelta;

    std::string m_defFileNm;
};

// ---------------------------------------------------------------------
/// Delta-TUT = TT - UT1 database
class DTUT
{
public:
    DTUT();
    virtual ~DTUT() {;}

    /// Time offset
    /// @param[in] utR2000 universal time, coordinated, sec
    /// @return time offset, sec
    double OffsetUT(const TimeR2000& utR2000);

    /// Time offset
    /// @param[in] ttJ2000 terrestrial time, J2000 sec
    /// @return time offset, sec
    double OffsetTT(const TimeJ2000& ttJ2000);

    /// Write state to stream
    /// @param[in, out] os stream receiving state info
    /// @return output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:
    void Initialize();

    /// Initialize from stream
    /// @param[in, out] is stream containing time offsets
    /// @return input stream
    std::istream& Initialize(std::istream& is);

private:
    typedef std::pair<TimeJ2000, TimeJ2000> J2000Rng;
    typedef std::pair<TimeR2000, TimeR2000> R2000Rng;
    typedef std::pair<double, double> OffsetRng;

    bool m_bDoInit;

    R2000Rng m_utRng; // [start, end)
    OffsetRng m_offRngR;
    TimeR2000 m_curR2000;
    double m_curOffsetR;

    J2000Rng m_j2000Rng; // [start, end)
    OffsetRng m_offRngJ;
    TimeJ2000 m_curJ2000;
    double m_curOffsetJ;

    std::map<TimeR2000, double> m_ctUtDelta;
    std::map<TimeJ2000, double> m_ctTtDelta;

    std::string m_defFileNm;
};

/// Leap second database
/// UTC may repeat a second or skip a second
/// @return leap second database
inline DAT& dat()
{
    static DAT dat;
    return dat;
}

/// Delta-TUT database
/// Delta-TUT = TT - UT1
/// Includes measured and predicted values for USNO Long-term Delta T
/// @return delta-TUT database
inline DTUT& dtut()
{
    static DTUT dtut;
    return dtut;
}
}

// ----------------------------------------------------------------------------
/// International atomic time to terrestrial time
/// @param[in] tai international atomic time, r2000 sec
/// @return terrestrial time, TT, TDT, j2000 sec
inline TimeJ2000 taiToTt(const TimeR2000& tai)
{
    return tai+DTA_S;
}

/// Terrestrial time to International atomic time
/// @param[in] tt terrestrial time, j2000 sec
/// @return international atomic time, TAI, r2000 sec
inline TimeR2000 ttToTai(const TimeJ2000& tt)
{
    return tt-DTA_S;
}

/// International atomic time to GPS time
/// @param[in] tai international atomic time, r2000 sec
/// @return GPS time, TGP, r2000 sec
inline TimeR2000 taiToTgps(const TimeR2000& tai)
{
    return tai-DGA_S;
}

/// GPS time to international atomic time
/// @param[in] tgp GPS time, r2000 sec
/// @return international atomic time, TAI, r2000 sec
inline TimeR2000 tgpsToTai(const TimeR2000& tgp)
{
    return tgp+DGA_S;
}

/// Terrestrial time to GPS time
/// @param[in] tt terrestrial time, j2000 sec
/// @return GPS time, TGP, r2000 sec
inline TimeR2000 ttToTgps(const TimeJ2000& tt)
{
    return tt-DTA_S-DGA_S;
}

/// GPS time to international atomic time
/// @param[in] tgp GPS time, r2000 sec
/// @return terrestrial time, TT, j2000 sec
inline TimeJ2000 tgpsToTt(const TimeR2000& tgp)
{
    return tgp+DTA_S+DGA_S;
}

/// Terrestrial time to universal time, coordinated
/// @param[in] tt terrestrial time, j2000 sec
/// @return universal time, coordinated, UTC, r2000 sec
inline TimeR2000 ttToUtc(const TimeJ2000& tt)
{
    return tt-DTA_S-TimeDB::dat().OffsetTT(tt);
}

/// Universal Time, Coordinated to terrestrial time
/// @param[in] utc universal time, coordinated, r2000 sec
/// @return terrestrial time, TT, j2000 sec
inline TimeJ2000 utcToTt(const TimeR2000& utc)
{
    return utc+DTA_S+TimeDB::dat().OffsetUTC(utc);
}

/// International atomic time to universal time, coordinated
/// @param[in] tai international atomic time, r2000 sec
/// @return universal time, coordinated, UTC, r2000 sec
inline TimeR2000 taiToUtc(const TimeR2000& tai)
{
    return tai-TimeDB::dat().OffsetTT(taiToTt(tai));
}

/// Universal Time, Coordinated to international atomic time
/// @param[in] utc universal time, coordinated, r2000 sec
/// @return international atomic time, TAI, r2000 sec
inline TimeR2000 utcToTai(const TimeR2000& utc)
{
    return utc+TimeDB::dat().OffsetUTC(utc);
}

/// Universal time to terrestrial time
/// @param[in] ut1 universal time, r2000 sec
/// @return Terrestrial time, TT, j2000 sec
inline TimeR2000 ut1ToTt(const TimeR2000& ut1)
{
    return ut1+TimeDB::dtut().OffsetUT(ut1);
}

/// Terrestrial time to universal time
/// @param[in] tt terrestrial time, j2000 sec
/// @return universal time, UT, UT1, r2000 sec
inline TimeR2000 ttToUt1(const TimeR2000& tt)
{
    return tt-TimeDB::dtut().OffsetTT(tt);
}

/// Computer time
/// @return time from computer clock, TT, j2000 sec
inline TimeJ2000 getComputerTime()
{
    // time returns seconds since January 1, 1970, UTC
    static const time_t SYS_TIME_J2000(946728000);
    time_t gmTime(0);
    time( &gmTime );
    double tmUtc(static_cast<double>(gmTime - SYS_TIME_J2000));
    return utcToTt(tmUtc);
}

/// Time Reference A to Time Reference B Conversion
class TimeRefConv
{
public:
    TimeRefConv();

    /// Time Reference Converstion Operator
    /// @param[in] tmIn time expressed in source time reference
    /// @param[in] tmRefIn source time reference
    /// @param[in] tmRefOut destination time reference
    /// @return time expressed in destination time reference
    TimeR2000 operator()(const TimeR2000& tmIn, unsigned tmRefIn,
                         unsigned tmRefOut) const;

    typedef std::pair<TimeRef::key_type, TimeRef::key_type> Key;
    typedef TimeR2000 (*tcf)(const TimeR2000&);

private:
    struct lessKey {
        bool operator()(const Key& a, const Key& b) const
        {
            if (a.first<b.first||(a.first==b.first&&a.second<b.second)) {
                return true;
            }

            return false;
        }
    };

    static std::map<Key, tcf, lessKey> s_tcfmap;
};

/// Time Reference A to Time Reference B Conversion function
/// @param[in] tmIn time expressed in source time reference
/// @param[in] tmRefIn source time reference
/// @param[in] tmRefOut destination time reference
/// @return time expressed in destination time reference
inline TimeR2000 timeRefConv(const TimeR2000& tmIn, unsigned tmRefIn,
                             unsigned tmRefOut)
{
    static const TimeRefConv trc;
    return trc(tmIn, tmRefIn, tmRefOut);
}

} // A3
#endif // TIMEJ2000_H
