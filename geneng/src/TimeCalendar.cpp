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
 * TimeCalendar.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// TimeCalendar class
// Refer to TimeCalendar.h for more info.
//===============================================================================
#include "TimeCalendar.h"
#include "TimeJ2000.h"
#include "constants.h"
#include "Units.h"

#include <iomanip>
#include <stdexcept>
// #include <strstream>
#include <sstream>
// using math.h since Microsoft's cmath is not in the std namespace
#include <math.h>

// ============================================================================
// A3 namespace start
namespace A3
{
// TimeCalendar static data members
const int TimeCalendar::s_YEAR_INC = 365;

const char *const TimeCalendar::s_MONTHSHORT[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

const char *const TimeCalendar::s_MONTHLONG[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

// The second and third character of a month's name uniquely identify it
const std::string TimeCalendar::s_MONTHCMP = "anebarprayunulugepctovec";

// TestBounds class statics should be used for range checking in the future
const TestBounds<double> TimeCalendar::s_SECOND_FRACTION_LIMITS(0.0, 1.0);

const TestBounds<int> TimeCalendar::s_SECOND_LIMITS(0, 59);

const TestBounds<int> TimeCalendar::s_MINUTE_LIMITS(0, 59);

const TestBounds<int> TimeCalendar::s_HOUR_LIMITS(0, 23);

const TestBounds<int> TimeCalendar::s_DAY_LIMITS(1, 366);

const TestBounds<int> TimeCalendar::s_MONTH_LIMITS(1, 12);

const TestBounds<int> TimeCalendar::s_YEAR_LIMITS(1600, 2199);

// constructors & destructors
TimeCalendar::TimeCalendar()
    : m_year(2000), m_month(1),  m_day(1),
      m_hour(12),   m_minute(0), m_second(0.0)
{ ; }

TimeCalendar::TimeCalendar( int year, int month,  int day,
                            int hour, int minute, double second )
    : m_year(year), m_month(month),  m_day(day),
      m_hour(hour), m_minute(minute), m_second(second)
{ ; }

TimeCalendar::TimeCalendar( int year, int month,  int day,
                            double secondsSinceMidnight )
{
    Set( year, month, day, secondsSinceMidnight );
}

TimeCalendar::TimeCalendar( TimeJ2000 j2000 )
{
    TimeJ2000 tmpJ2000;
    m_year    = 2000;
    m_month   = 1;
    m_day     = 1;
    m_hour    = 0;
    m_minute  = 0;
    m_second  = 0.0;
    tmpJ2000 = *this;
    m_year -= (int)((tmpJ2000 - j2000) * SEC_TO_DAY / s_YEAR_INC);
    tmpJ2000 = *this;

    while (j2000 > tmpJ2000) {
        m_year++;
        tmpJ2000 = *this;
    }

    while (j2000 < tmpJ2000) {
        m_year--;
        tmpJ2000 = *this;
    }

    while (tmpJ2000 <= j2000 && m_month < 12) {
        m_month++;
        tmpJ2000 = *this;
    }

    m_month -= tmpJ2000 > j2000 ? 1 : 0;
    tmpJ2000 = *this;
    m_day     = (int)( (j2000 - tmpJ2000) / DAY_TO_SEC ) + 1;
    m_second = j2000 - tmpJ2000 - (double)( m_day-1 ) * DAY_TO_SEC;
    m_hour = (int)( m_second / HR_TO_SEC );
    m_second -= (double)m_hour * HR_TO_SEC;
    m_minute = (int)( m_second / MIN_TO_SEC );
    m_second -= (double)m_minute * MIN_TO_SEC;
    tmpJ2000 = *this;

    if ( fabs(j2000-tmpJ2000) > 0.0001 ) {
        // std::ostrstream stmMsg;
        std::ostringstream stmMsg;
        stmMsg << "Function: TimeCalendar::TimeCalendar( TimeJ2000 time )\n"
               << "J2000 Date to calendar date conversion failed to \n"
               << "converge on the J2000 date.\n" << std::ends;
        std::runtime_error rte( stmMsg.str() );
        // delete stmMsg.str();
        throw rte;
    }
}

TimeCalendar::TimeCalendar( const std::string& sYMDHMS )
{
    try {
        char c;
        std::istringstream is( sYMDHMS );
        is >> ioTimeYMDHMS
           >> m_year >> c
           >> m_month >> c
           >> m_day
           >> m_hour >> c
           >> m_minute >> c
           >> m_second;
    } catch ( std::exception& e ) {
        std::string sMsg( "TimeCalendar( const std::string& sYMDHMS )" );
        sMsg += "\nsYMDHMS = " + sYMDHMS;
        sMsg += "\nCaught the following exception:\n";
        sMsg += e.what();
        throw std::runtime_error( sMsg );
    } catch ( ... ) {
        std::string sMsg( "TimeCalendar( const std::string& sYMDHMS )" );
        sMsg += "\nsYMDHMS = " + sYMDHMS;
        sMsg += "\nCaught an unknown exception.";
        throw std::runtime_error( sMsg );
    }
}

TimeCalendar::operator std::string() const
{
    std::string sYMDHMS;

    try {
        std::ostringstream os;
        os << ioTimeYMDHMS << *this;
        sYMDHMS = os.str();
    } catch ( std::exception& e ) {
        std::string sMsg( "TimeCalendar::operator std::string() const" );
        sMsg += "\nCaught the following exception:\n";
        sMsg += e.what();
        throw std::runtime_error( sMsg );
    } catch ( ... ) {
        std::string sMsg( "TimeCalendar::operator std::string() const" );
        sMsg += "\nCaught an unknown exception.";
        throw std::runtime_error( sMsg );
    }

    return sYMDHMS;
}

// ----------------------------------------------------------------------------
// access functions
void TimeCalendar::Get( int& year, int& month, int& day,
                        int& hour, int& minute, double& second ) const
{
    year = m_year;
    month = m_month;
    day = m_day;
    hour = m_hour;
    minute = m_minute;
    second = m_second;
}

void TimeCalendar::Get( int& year, int& month, int& day,
                        double& secondsSinceMidnight ) const
{
    year = m_year;
    month = m_month;
    day = m_day;
    secondsSinceMidnight = GetSecondsSinceMidnight();
}

void TimeCalendar::Set( int year, int month, int day,
                        int hour, int minute, double second )
{
    m_year = year;
    m_month = month;
    m_day = day;
    m_hour = hour;
    m_minute = minute;
    m_second = second;
}

void TimeCalendar::Set( int year, int month, int day,
                        double secondsSinceMidnight )
{
    m_year  = year;
    m_month = month;
    m_day   = day;
    SetSecondsSinceMidnight( secondsSinceMidnight );
}

int TimeCalendar::GetYear() const
{
    return m_year;
}

int TimeCalendar::GetMonth() const
{
    return m_month;
}

int TimeCalendar::GetDay() const
{
    return m_day;
}

int TimeCalendar::GetHour() const
{
    return m_hour;
}

int TimeCalendar::GetMinute() const
{
    return m_minute;
}

double TimeCalendar::GetSecond() const
{
    return m_second;
}

double TimeCalendar::GetSecondsSinceMidnight() const
{
    return m_hour * HR_TO_SEC + m_minute * MIN_TO_SEC + m_second;
}

void TimeCalendar::GetMonthShort( std::string& strMonth ) const
{
    strMonth = s_MONTHSHORT[ m_month-1 ];
}

void TimeCalendar::GetMonthLong( std::string& strMonth ) const
{
    strMonth = s_MONTHLONG[ m_month-1 ];
}

void TimeCalendar::SetYear( int year )
{
    m_year = year;
}

void TimeCalendar::SetMonth( int month )
{
    m_month = month;
}

void TimeCalendar::SetDay( int day )
{
    m_day = day;
}

void TimeCalendar::SetHour( int hour )
{
    m_hour = hour;
}

void TimeCalendar::SetMinute( int minute )
{
    m_minute = minute;
}

void TimeCalendar::SetSecond( double second )
{
    m_second = second;
}

void TimeCalendar::SetSecondsSinceMidnight( double secondsSinceMidnight )
{
    m_second  = secondsSinceMidnight;
    m_hour    = (int)( m_second * SEC_TO_HR );
    m_second -= (double)m_hour * HR_TO_SEC;
    m_minute  = (int)( m_second * SEC_TO_MIN );
    m_second -= (double)m_minute * MIN_TO_SEC;
}

void TimeCalendar::SetMonth( const std::string& strMonth )
{
    char tempMonth[] = { ' ', ' ', '\0' };
    tempMonth[0] = (char)tolower( strMonth[1] );
    tempMonth[1] = (char)tolower( strMonth[2] );
    std::string::size_type w = s_MONTHCMP.find( tempMonth );

    if ( w == s_MONTHCMP.npos ) {
        throw std::invalid_argument( "TimeCalendar::SetMonth bad string" );
    }

    m_month = (int(w) >> 1) + 1;
}

// ----------------------------------------------------------------------------
// overloaded operators

// ============================================================================
std::ostream& operator<<( std::ostream& os, const TimeCalendar& time )
{
    TimeJ2000 j2000( time );
    return os << j2000;
}

std::istream& operator>>( std::istream& is, TimeCalendar& time )
{
    if (is.good()) {
        TimeJ2000 j2000;
        is >> j2000;
        time = j2000;
    }

    return is;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================

