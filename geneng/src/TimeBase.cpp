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
 * TimeBase.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Time manipulators and system time
// Refer to TimeBase.h for more info.
//==============================================================================
#include "TimeBase.h"
#include "IoManip.h"
#include "Units.h"
#include <time.h>

// =============================================================================
// A3 namespace start
namespace A3
{
// time reference enumeration & labels
TimeRef::TimeRef()
{
    insert(value_type(TT, std::string("TT")));
    insert(value_type(UTC, std::string("UTC")));
    insert(value_type(UT1, std::string("UT1")))->second.insert("UT");
    insert(value_type(TGP, std::string("TGP")))->second.insert("TGPS");
    insert(value_type(TAI, std::string("TAI")));
}

// -------------------------------------------------------------------------
// time manipulators (default is seconds, UTC)
namespace
{
class IoManipTime : public IoManip<unsigned>
{
public:


    IoManipTime(size_t refs=0);
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit IoManipTime(size_t refs=0);

    static unsigned Default()
    {
        return Units::SECOND;
    }

    static std::locale::id id;
};

std::locale::id IoManipTime::id;

IoManipTime::IoManipTime(size_t refs)
    : IoManip<unsigned>(Default(), refs) {;}

class IoManipTmRef : public IoManip<unsigned>
{
public:



    explicit IoManipTmRef(size_t refs=0);

    static unsigned Default()
    {
        return TimeRef::UTC;
    }

    static std::locale::id id;
};

std::locale::id IoManipTmRef::id;

IoManipTmRef::IoManipTmRef(size_t refs)
    : IoManip<unsigned>(Default(), refs) {;}
}

// -------------------------------------------------------------------------
std::ios_base& ioTimeSeconds(std::ios_base& stm)
{
    return ioManipSet<IoManipTime>(stm, Units::SECOND);
}

std::ios_base& ioTimeMinutes(std::ios_base& stm)
{
    return ioManipSet<IoManipTime>(stm, Units::MINUTE);
}

std::ios_base& ioTimeHours(std::ios_base& stm)
{
    return ioManipSet<IoManipTime>(stm, Units::HOUR);
}

std::ios_base& ioTimeDays(std::ios_base& stm)
{
    return ioManipSet<IoManipTime>(stm, Units::DAY);
}

std::ios_base& ioTimeYMDH(std::ios_base& stm)
{
    return ioManipSet<IoManipTime>(stm, Units::YMDH);
}

std::ios_base& ioTimeYMDHMS(std::ios_base& stm)
{
    return ioManipSet<IoManipTime>(stm, Units::YMDHMS);
}

std::ios_base& ioTimeExcelDays(std::ios_base& stm)
{
    return ioManipSet<IoManipTime>(stm, Units::DAY_EXCEL);
}

std::ios_base& ioTimeRestore(std::ios_base& stm)
{
    return ioManipRestore<IoManipTime>(stm);
}

unsigned int ioTimeGetFormat(std::ios_base& stm)
{
    return ioManipGet<IoManipTime>(stm);
}

// -------------------------------------------------------------------------
std::ios_base& ioTimeTT(std::ios_base& stm)
{
    return ioManipSet<IoManipTmRef>(stm, TimeRef::TT);
}

std::ios_base& ioTimeUTC(std::ios_base& stm)
{
    return ioManipSet<IoManipTmRef>(stm, TimeRef::UTC);
}

std::ios_base& ioTimeUT1(std::ios_base& stm)
{
    return ioManipSet<IoManipTmRef>(stm, TimeRef::UT1);
}

std::ios_base& ioTimeTAI(std::ios_base& stm)
{
    return ioManipSet<IoManipTmRef>(stm, TimeRef::TAI);
}

std::ios_base& ioTimeGPS(std::ios_base& stm)
{
    return ioManipSet<IoManipTmRef>(stm, TimeRef::TGP);
}

std::ios_base& ioTimeRefRestore(std::ios_base& stm)
{
    return ioManipRestore<IoManipTmRef>(stm);
}

unsigned int ioTimeRefGetFormat(std::ios_base& stm)
{
    return ioManipGet<IoManipTmRef>(stm);
}

// -------------------------------------------------------------------------
std::ios_base&
ioTimeRefSetFormat(std::ios_base& stm, const std::string& ref)
{
    TimeRef::key_type tf = timeRef()(ref);
    return ioManipSet<IoManipTmRef>(stm, tf);
}

// -------------------------------------------------------------------------
/// write formatted time
std::ostream& ioCurrentTime(std::ostream& os)
{
    time_t eventTime = time(NULL);
    os << ctime(&eventTime);
    return os;
}

// write current clock time
const std::string&
ioCurrentTime(const std::string& message)
{
    static std::string strTime;
    time_t eventTime = time(NULL);
    strTime = message + ctime(&eventTime);
    return strTime;
}

// provided for backward compatibility
const std::string&
(*printCurrentTime)(const std::string& message) = ioCurrentTime;

} // A3 namespace end
