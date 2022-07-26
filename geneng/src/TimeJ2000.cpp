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
 * TimeJ2000.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// TimeJ2000 and utility classes
// Refer to TimeJ2000.h for more info.
//==============================================================================
#include "TimeJ2000.h"
#include "TimeCalendar.h"
#include "constants.h"
#include "IoDelim.h"
#include "Units.h"
#include "GenUtility.h"
#include <typeinfo>
#include <fstream>
#include <iterator>
#include <stdexcept>
// using float.h since Microsoft's cfloat is not in the std namespace
#include <float.h>

// A3 namespace start
namespace A3
{
using namespace TimeDB;

// prototypes
unsigned int ioTimeGetFormat( std::ios_base& stream );
unsigned int ioTimeRefGetFormat( std::ios_base& stream );

// -------------------------------------------------------------------------
// TimeJ2000 static data members
const double TimeJ2000::s_1600X = -2.5 * DAY_TO_SEC; // seconds

const int TimeJ2000::s_CENTURY_INC = 36524;

const int TimeJ2000::s_YEAR_INC = 365;

const int TimeJ2000::s_MONTHCUM[] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334,
    0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335
};

// depends on constants.cxx initialization
const std::pair<double, double>
TimeJ2000::s_J2000_LIMITS(-146097.0*DAY_TO_SEC, 73048.0*DAY_TO_SEC);

// -------------------------------------------------------------------------
// TimeRefConv static data member
std::map<TimeRefConv::Key, TimeRefConv::tcf, TimeRefConv::lessKey>
TimeRefConv::s_tcfmap;

// -------------------------------------------------------------------------
// TimeJ2000 constructors
TimeJ2000::TimeJ2000()
    : m_seconds(s_J2000_LIMITS.first) { ; }

TimeJ2000::TimeJ2000(const double& seconds)
    : m_seconds( seconds ) { ; }

TimeJ2000::TimeJ2000(const TimeCalendar& time)
{
    int    year, month, day;
    double secondsSinceMidnight;
    time.Get( year, month, day, secondsSinceMidnight );
    int centuryIdx = ( year / 100 ) - 16;
    int julCentury = ( centuryIdx - 4 ) * s_CENTURY_INC
                     + ( year - 1201 ) / 400;
    int year0 = year % 100;
    int julYear = year0 * s_YEAR_INC + ( year0 - 1 ) / 4;
    int julMonth = ((year % 400) == 0 || (( year0 % 4 ) == 0 && year0 != 0))
                   ? s_MONTHCUM[month + 11] : s_MONTHCUM[month - 1];
    m_seconds = (double)( julCentury + julYear + julMonth + day )
                * DAY_TO_SEC + secondsSinceMidnight + s_1600X;
}

// -------------------------------------------------------------------------
const std::pair<double, double>& TimeJ2000::Limits()
{
    return s_J2000_LIMITS;
}

// -------------------------------------------------------------------------
// TimeJ2000 operators
TimeJ2000& TimeJ2000::operator+=(const double& deltaTimeSeconds)
{
    m_seconds += deltaTimeSeconds;
    return *this;
}

TimeJ2000& TimeJ2000::operator-=(const double& deltaTimeSeconds)
{
    m_seconds -= deltaTimeSeconds;
    return *this;
}

// -------------------------------------------------------------------------
// TimeJ2000 insertion and extraction
std::ostream& operator<<(std::ostream& os, const TimeJ2000& tm)
{
    TimeR2000 time(timeRefConv(tm, TimeRef::TT, ioTimeRefGetFormat(os)));

    // time format switch
    switch (ioTimeGetFormat(os)) {
        case Units::SECOND:
            os << double(time);
            break;

        case Units::MINUTE:
            os << double(time) * SEC_TO_MIN;
            break;

        case Units::HOUR:
            os << double(time) * SEC_TO_HR;
            break;

        case Units::DAY:
            os << double(time) * SEC_TO_DAY;
            break;

        case Units::DAY_EXCEL:
            os << double(time) * SEC_TO_DAY + EXCEL_J2000_DAY;
            break;

        case Units::YMDHMS: {
            TimeCalendar tmCal(time);
            //WARNING_FIX:
            //1>src\TimeJ2000.cpp(125): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'int', possible loss of data
            long long int oldPrecision = os.precision(2);
            char oldFill = os.fill('0');
            std::ios_base::fmtflags oldFlags = os.setf(std::ios::fixed);
            os << std::setw(4) << tmCal.GetYear()   << '-'
               << std::setw(2) << tmCal.GetMonth()  << '-'
               << std::setw(2) << tmCal.GetDay()    << ' '
               << std::setw(2) << tmCal.GetHour()   << ':'
               << std::setw(2) << tmCal.GetMinute() << ':'
               << std::setw(5) << tmCal.GetSecond();
            os.flags(oldFlags);
            os.fill(oldFill);
            os.precision(oldPrecision);
        }
        break;

        case Units::YMDH: {
            TimeCalendar tmCal(time);
            //WARNING_FIX:
            //1>src\TimeJ2000.cpp(147): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'int', possible loss of data
            long long int oldPrecision = os.precision(2);
            char oldFill = os.fill('0');
            std::ios_base::fmtflags oldFlags = os.setf(std::ios::fixed);
            os << std::setw(4) << tmCal.GetYear()   << '-'
               << std::setw(2) << tmCal.GetMonth()  << '-'
               << std::setw(2) << tmCal.GetDay()    << ' '
               << std::setw(2) << tmCal.GetSecondsSinceMidnight()*SEC_TO_HR;
            os.flags(oldFlags);
            os.fill(oldFill);
            os.precision(oldPrecision);
        }
        break;

        default:
            throw std::runtime_error(
                "operator<<(ostream&,TimeJ2000&) bad time format");
    };

    return os;
}

std::istream& operator>>(std::istream& is, TimeJ2000& tm)
{
    static const TimeJ2000 TIME_NOT_SET;
    std::ios_base::iostate iost(is.exceptions());

    try {
        is.exceptions(iost
                      |std::ios_base::failbit
                      |std::ios_base::badbit);
        unsigned int timeFormat = ioTimeGetFormat( is );
        TimeJ2000 time;

        if (timeFormat != Units::YMDHMS && timeFormat != Units::YMDH) {
            double dTime(TIME_NOT_SET);
            is >> dTime;

            switch (timeFormat) {
                case Units::SECOND:
                    break;

                case Units::MINUTE:
                    dTime *= MIN_TO_SEC;
                    break;

                case Units::HOUR:
                    dTime *= HR_TO_SEC;
                    break;

                case Units::DAY:
                    dTime *= DAY_TO_SEC;
                    break;

                case Units::DAY_EXCEL:
                    dTime = (dTime - EXCEL_J2000_DAY) * DAY_TO_SEC;
                    break;

                default:
                    throw std::runtime_error(
                        "operator>>(istream&,TimeJ2000&) bad time format");
            };

            time = dTime;
        } else {
            TimeCalendar cal;
            double d(0.0);
            int i(0);
            char c(0);

            switch (timeFormat) {
                case Units::YMDHMS:
                    is >> i >> c;
                    cal.SetYear(i);
                    is >> i >> c;
                    cal.SetMonth(i);
                    is >> i;
                    cal.SetDay(i);
                    is >> i >> c;
                    cal.SetHour(i);
                    is >> i >> c;
                    cal.SetMinute(i);
                    is >> d;
                    cal.SetSecond(d);
                    break;

                case Units::YMDH:
                    is >> i >> c;
                    cal.SetYear(i);
                    is >> i >> c;
                    cal.SetMonth(i);
                    is >> i;
                    cal.SetDay(i);
                    is >> d;
                    cal.SetSecondsSinceMidnight(d * HR_TO_SEC);
                    break;

                default:
                    throw std::runtime_error(
                        "operator>>(istream&,TimeJ2000&) bad time format");
            };

            time = cal;
        }

        tm = timeRefConv(time, ioTimeRefGetFormat(is), TimeRef::TT);
    } catch (std::ios_base::failure) {
        if (iost & is.rdstate()) {
            throw;
        }
    } catch (...) {
        throw;
    }

    is.exceptions(iost);
    return is;
}

// =========================================================================
// [epoch, offset] insertion
std::ostream& operator<<(std::ostream& os,
                         const std::map<TimeJ2000, double>::value_type& pr)
{
    os << pr.first << ioDelim << pr.second;
    return os;
}

// -------------------------------------------------------------------------
// leap second database
DAT::DAT()
    : m_curOffsetR(0.0), m_curOffsetJ(0.0), m_defFileNm("DAT.txt") {;}

void DAT::Initialize()
{
    Handle<std::istream> his(findOpenStdFile(m_defFileNm));

    if (!his.IsNullObject()) {
        Initialize(*his);
    } else {
        std::stringstream stmMsg;
        stmMsg << "Cannot find leap second database\n"
               << "database name = " << m_defFileNm << std::endl;
        THROW_RUNTIME_ERROR(stmMsg.str());
    }
}

// -------------------------------------------------------------------------
std::istream& DAT::Initialize(std::istream& is)
{
    std::string sBuf;
    TimeR2000 tm;
    double dt;
    std::getline(is, sBuf);
    is >> ioTimeYMDH >> ioTimeTT;

    while (is >> tm >> dt) {
        m_ctUtcDelta[tm] = dt;
        tm += DTA_S + dt;
        m_ctTtDelta[tm] = dt;
    }

    is >> ioTimeRestore >> ioTimeRefRestore;
    dt = m_ctUtcDelta.begin()->second;
    m_ctUtcDelta[TimeR2000::Limits().first] = dt;
    m_ctTtDelta[TimeR2000::Limits().first] = dt;
    dt = m_ctUtcDelta.rbegin()->second;
    m_ctUtcDelta[TimeR2000::Limits().second] = dt;
    m_ctTtDelta[TimeR2000::Limits().second] = dt;
    return is;
}

double DAT::OffsetUTC(const TimeR2000& utcR2000)
{
    if (m_ctUtcDelta.empty()) {
        Initialize();
    }

    if (m_utcRng.first > utcR2000 || m_utcRng.second <= utcR2000) {
        std::map<TimeR2000, double>::iterator
        itr = m_ctUtcDelta.upper_bound(utcR2000);
        m_utcRng.second = (itr--)->first;
        m_utcRng.first = itr->first;
        m_curOffsetR = itr->second;
    }

    return m_curOffsetR;
}

double DAT::OffsetTT(const TimeJ2000& ttJ2000)
{
    if (m_ctUtcDelta.empty()) {
        Initialize();
    }

    if (m_j2000Rng.first > ttJ2000 || m_j2000Rng.second <= ttJ2000) {
        std::map<TimeJ2000, double>::iterator
        itr = m_ctTtDelta.upper_bound(ttJ2000);
        m_j2000Rng.second = (itr--)->first;
        m_j2000Rng.first = itr->first;
        m_curOffsetJ = itr->second;
    }

    return m_curOffsetJ;
}

std::ostream& DAT::ReportState(std::ostream& os) const
{
    std::string sHeader(typeid(*this).name());
    sHeader += " State";
    os << sHeader << '\n' << std::string(sHeader.size(),'-') << '\n'
       << "Initialized:  " << (0<m_ctUtcDelta.size()?"yes\n":"no\n")
       << "File          " << m_defFileNm << '\n';
    os << ioTimeYMDHMS
       << "\nLeap Second Database, UTC, " << m_ctUtcDelta.size()
       << " records\n";
    std::ostream_iterator<std::map<TimeJ2000, double>::value_type>
    itr(os, "\n");
    std::copy(m_ctUtcDelta.begin(), m_ctUtcDelta.end(), itr);
    os << "\nLeap Second Database, TT, " << m_ctTtDelta.size()
       << " records\n";
    std::copy(m_ctTtDelta.begin(), m_ctTtDelta.end(), itr);
    os << ioTimeRestore;
    return os;
}

// -------------------------------------------------------------------------
// delta-TUT database
DTUT::DTUT()
    : m_bDoInit(true),
      m_offRngR(0.0, 0.0), m_curOffsetR(0.0),
      m_offRngJ(0.0, 0.0), m_curOffsetJ(0.0),
      m_defFileNm("DTUT.txt") {;}

void DTUT::Initialize()
{
    Handle<std::istream> his(findOpenStdFile(m_defFileNm));

    if (!his.IsNullObject()) {
        Initialize(*his);
    } else {
        std::stringstream stmMsg;
        stmMsg << "Cannot find delta TT-UT database\n"
               << "database name = " << m_defFileNm << std::endl;
        THROW_RUNTIME_ERROR(stmMsg.str());
    }

    m_bDoInit = false;
}

// -------------------------------------------------------------------------
std::istream& DTUT::Initialize(std::istream& is)
{
    std::string sBuf;
    TimeR2000 tm;
    double    dt;
    std::getline(is, sBuf);
    // is >> ioTimeYMDH >> ioTimeUTC;
    is >> ioTimeYMDH >> ioTimeTT;

    while (is >> tm >> dt) {
        tm = utcToTt(tm);
        m_ctTtDelta[tm] = dt;
        m_ctUtDelta[tm-dt] = dt;
    }

    is >> ioTimeRestore >> ioTimeRefRestore;
    dt = m_ctTtDelta.begin()->second;
    m_ctTtDelta[TimeR2000::Limits().first] = dt;
    m_ctUtDelta[TimeR2000::Limits().first] = dt;
    dt = m_ctTtDelta.rbegin()->second;
    m_ctTtDelta[TimeR2000::Limits().second] = dt;
    m_ctUtDelta[TimeR2000::Limits().second] = dt;
    return is;
}

double DTUT::OffsetUT(const TimeR2000& utR2000)
{
    // V4.5a update, changed from computing offset as step function
    // to a linear fit for an improved approx.
    // if (m_ctTtDelta.empty()) Initialize();
    if (m_bDoInit) {
        Initialize();
    }

    if (m_curR2000 != utR2000) {
        if (m_utRng.first > utR2000 || m_utRng.second <= utR2000) {
            std::map<TimeR2000, double>::iterator
            itr = m_ctUtDelta.upper_bound(utR2000);
            m_utRng.second   = itr->first;
            m_offRngR.second = (itr--)->second;
            m_utRng.first    = itr->first;
            m_offRngR.first  = itr->second;
        }

        m_curR2000 = utR2000;
        m_curOffsetR = m_offRngR.first + (m_offRngR.second-m_offRngR.first)
                       * (utR2000-m_utRng.first) / (m_utRng.second-m_utRng.first);
    }

    return m_curOffsetR;
}

double DTUT::OffsetTT(const TimeJ2000& ttJ2000)
{
    // V4.5a update, changed from computing offset as step function
    // to a linear fit for an improved approx.
    if (m_ctTtDelta.empty()) {
        Initialize();
    }

    if (m_curJ2000 != ttJ2000) {
        if (m_j2000Rng.first > ttJ2000 || m_j2000Rng.second <= ttJ2000) {
            std::map<TimeJ2000, double>::iterator
            itr = m_ctTtDelta.upper_bound(ttJ2000);
            m_j2000Rng.second = itr->first;
            m_offRngJ.second  = (itr--)->second;
            m_j2000Rng.first  = itr->first;
            m_offRngJ.first   = itr->second;
        }

        m_curJ2000 = ttJ2000;
        m_curOffsetJ = m_offRngJ.first + (m_offRngJ.second-m_offRngJ.first)
                       * (ttJ2000-m_j2000Rng.first) / (m_j2000Rng.second-m_j2000Rng.first);
    }

    return m_curOffsetJ;
}

std::ostream& DTUT::ReportState(std::ostream& os) const
{
    std::string sHeader(typeid(*this).name());
    sHeader += " State";
    os << sHeader << '\n' << std::string(sHeader.size(),'-') << '\n'
       << "Initialized:  " << (0<m_ctUtDelta.size()?"yes\n":"no\n")
       << "File          " << m_defFileNm << '\n';
    os << ioTimeYMDHMS
       << "\nDelta-TUT Database, UT, " << m_ctUtDelta.size()
       << " records\n";
    std::ostream_iterator<std::map<TimeJ2000, double>::value_type>
    itr(os, "\n");
    std::copy(m_ctUtDelta.begin(), m_ctUtDelta.end(), itr);
    os << "\nDelta-TUT Database, TT, " << m_ctTtDelta.size()
       << " records\n";
    std::copy(m_ctTtDelta.begin(), m_ctTtDelta.end(), itr);
    os << ioTimeRestore;
    return os;
}

TimeRefConv::TimeRefConv()
{
    if (s_tcfmap.empty()) {
        s_tcfmap[Key(TimeRef::TAI,TimeRef::TT)]  = taiToTt;
        s_tcfmap[Key(TimeRef::TT,TimeRef::TAI)]  = ttToTai;
        s_tcfmap[Key(TimeRef::TAI,TimeRef::TGP)] = taiToTgps;
        s_tcfmap[Key(TimeRef::TGP,TimeRef::TAI)] = tgpsToTai;
        s_tcfmap[Key(TimeRef::TT,TimeRef::TGP)]  = ttToTgps;
        s_tcfmap[Key(TimeRef::TGP,TimeRef::TT)]  = tgpsToTt;
        s_tcfmap[Key(TimeRef::TT,TimeRef::UTC)] = ttToUtc;
        s_tcfmap[Key(TimeRef::UTC,TimeRef::TT)] = utcToTt;
        s_tcfmap[Key(TimeRef::TAI,TimeRef::UTC)] = taiToUtc;
        s_tcfmap[Key(TimeRef::UTC,TimeRef::TAI)] = utcToTai;
        s_tcfmap[Key(TimeRef::UT1,TimeRef::TT)] = ut1ToTt;
        s_tcfmap[Key(TimeRef::TT,TimeRef::UT1)] = ttToUt1;
    }
}

TimeR2000 TimeRefConv::operator()(const TimeR2000& tmIn,
                                  unsigned tmRefIn, unsigned tmRefOut) const
{
    if (tmRefIn == tmRefOut) {
        return tmIn;
    }

    return s_tcfmap[Key(tmRefIn, tmRefOut)](tmIn);
}

} // A3
