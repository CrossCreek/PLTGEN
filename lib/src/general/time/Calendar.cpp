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
 * Calendar.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "Calendar.h"
#include "general/math/SimpleMath.h"
#include "general/data/UnitData.h"
#include "general/utility/StringEditor.h"

Calendar::Calendar()
    : j2000Time_a(0.0), ptrDate_a(new Date()), ptrTime_a(new MURALTime())
{
    try {
        j2000Time_a = CalculateJ2000Time(*ptrDate_a, *ptrTime_a);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "Calendar()");
        throw;
    }

    return;
}

Calendar::Calendar(int year,
                   int month,
                   int day,
                   int hour,
                   int minute,
                   int second,
                   double fractionalSeconds,
                   MURALTime::HOUR_DISPLAY display)
    : j2000Time_a(0.0), ptrDate_a(new Date(year, month, day)), ptrTime_a(new MURALTime(hour, minute, second, fractionalSeconds, display))
{
    try {
        j2000Time_a = CalculateJ2000Time(*ptrDate_a, *ptrTime_a);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "Calendar(int,int,int,int,int,int,double,MURALTime::HOUR_DISPLAY)");
        throw;
    }

    return;
}

Calendar::Calendar(double j2000Time)
    : j2000Time_a(j2000Time), ptrDate_a(NULL), ptrTime_a(NULL)
{
    try {
        int  year = 0;
        int  month = 0;
        int  day = 0;
        int  hour = 0;
        int  minute = 0;
        int  second = 0;
        double fractionalSeconds = 0.0;
        CalculateDateAndTime(j2000Time_a, year, month, day,
                             hour, minute, second, fractionalSeconds);
        ptrDate_a = shared_ptr<Date>(new Date(year, month, day));
        ptrTime_a = shared_ptr<MURALTime>(new MURALTime(hour, minute, second, fractionalSeconds));
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "Calendar(const double&)");
        throw;
    }

    return;
}

Calendar::Calendar(const string &dateString, const string &timeString)
    : j2000Time_a(0.0), ptrDate_a(NULL), ptrTime_a(NULL)
{
    try {
        ptrDate_a = shared_ptr<Date>(new Date(dateString));
        ptrTime_a = shared_ptr<MURALTime>(new MURALTime(timeString));
        j2000Time_a = CalculateJ2000Time(*ptrDate_a, *ptrTime_a);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "Calendar(const string&, const string&)");
        throw;
    }

    return;
}

Calendar::Calendar(const Date &date, const MURALTime &time)
    : j2000Time_a(0.0), ptrDate_a(NULL), ptrTime_a(NULL)
{
    try {
        ptrDate_a = shared_ptr<Date>(new Date(date));
        ptrTime_a = shared_ptr<MURALTime>(new MURALTime(time));
        j2000Time_a = CalculateJ2000Time(*ptrDate_a, *ptrTime_a);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "Calendar(const Date&, const MURALTime&)");
        throw;
    }

    return;
}

Calendar::Calendar(const Calendar &copyMe)
    : j2000Time_a(copyMe.j2000Time_a), ptrDate_a(NULL), ptrTime_a(NULL)
{
    if (copyMe.ptrDate_a != NULL) {
        ptrDate_a = shared_ptr<Date>(new Date(*(copyMe.ptrDate_a)));
    }

    if (copyMe.ptrTime_a != NULL) {
        ptrTime_a = shared_ptr<MURALTime>(new MURALTime(*(copyMe.ptrTime_a)));
    }
}

Calendar&
Calendar::operator = (const Calendar &copyMe)
{
    j2000Time_a = copyMe.j2000Time_a;
    ptrDate_a   = copyMe.ptrDate_a;
    ptrTime_a   = copyMe.ptrTime_a;
    return(*this);
}

bool
Calendar::operator == (const Calendar &compareMe) const
{
    return(j2000Time_a == compareMe.j2000Time_a);
}

bool
Calendar::operator != (const Calendar &compareMe) const
{
    return(j2000Time_a != compareMe.j2000Time_a);
}

bool
Calendar::operator < (const Calendar &compareMe) const
{
    return(j2000Time_a < compareMe.j2000Time_a);
}

bool
Calendar::operator <= (const Calendar &compareMe) const
{
    return(j2000Time_a <= compareMe.j2000Time_a);
}

bool
Calendar::operator > (const Calendar &compareMe) const
{
    return(j2000Time_a > compareMe.j2000Time_a);
}

bool
Calendar::operator >= (const Calendar &compareMe) const
{
    return(j2000Time_a >= compareMe.j2000Time_a);
}

double
Calendar::CalculateJ2000Time(const Date &date, const MURALTime &time)
{
    int        count = 0;
    const int  year = date.GetYear();
    const int  month = date.GetMonth();
    const int  day = date.GetDay();
    const int  hour = time.GetHour();
    const int  minute = time.GetMinute();
    const int  second = time.GetSecond();
    double       j2000Time = 0.0;
    const double fractionalSeconds = time.GetFractionalSeconds();

    // J2000 is the number of seconds from 1/01/2000 @ 12:00:00 (noon)

    if (year < 2000) {
        // subtract whole years
        count = (year + 1);

        while (count < 2000) {
            if (Date::IsLeapYear(count) == false) {
                j2000Time -= SECONDS_PER_YEAR;
            } else {
                j2000Time -= SECONDS_PER_LEAP_YEAR;
            }

            ++count;
        }

        // subtract whole months
        count = (month + 1);

        while (count <= 12) {
            j2000Time -= (Date::GetDaysInMonth(count, year) * SECONDS_PER_DAY);
            ++count;
        }

        // subtract whole days
        j2000Time -= ((Date::GetDaysInMonth(month, year) - day + 1) * SECONDS_PER_DAY);
    } else {
        // add whole years
        count = (year - 1);

        while (count >= 2000) {
            if (Date::IsLeapYear(count) == false) {
                j2000Time += SECONDS_PER_YEAR;
            } else {
                j2000Time += SECONDS_PER_LEAP_YEAR;
            }

            --count;
        }

        // add whole months
        count = (month - 1);

        while (count >= 1) {
            j2000Time += (Date::GetDaysInMonth(count, year) * SECONDS_PER_DAY);
            --count;
        }

        // add whole days
        j2000Time += ((day - 1) * SECONDS_PER_DAY);
    }

    // add difference between time and 12:00:00 (noon)
    j2000Time += ( ((double)(hour - 12) * SECONDS_PER_HOUR) +
                   ((double)minute * SECONDS_PER_MINUTE) +
                   (double)second + fractionalSeconds);
    return(j2000Time);
}

double
Calendar::CalculateJ2000Time(int year,
                             int month,
                             int day,
                             int hour,
                             int minute,
                             int second,
                             double fractionalSeconds)
{
    double     j2000Time = 0.0;
    Date       date;
    MURALTime  time;

    try {
        date = Date(year, month, day);
        time = MURALTime(hour, minute, second, fractionalSeconds);
        j2000Time = CalculateJ2000Time(date, time);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "CalculateJ2000Time(int,int,int,int,int,int,const double&)");
        throw;
    }

    return(j2000Time);
}

void
Calendar::CalculateDateAndTime(double j2000Time,
                               int &year, int &month, int &day,
                               int &hour, int &minute, int &second,
                               double &fractionalSeconds)
{
    double remainingJ2000 = j2000Time + (SECONDS_PER_DAY / 2.0);
    double currentJ2000 = 0.0;
    bool   found = false;

    if (remainingJ2000 >= 0.0) {
        year = 2000;

        while (found == false) {
            if (Date::IsLeapYear(year) == false) {
                currentJ2000 = SECONDS_PER_YEAR;
            } else {
                currentJ2000 = SECONDS_PER_LEAP_YEAR;
            }

            if ((remainingJ2000 - currentJ2000) >= 0.0) {
                remainingJ2000 -= currentJ2000;
                ++year;
            } else {
                found = true;
            }
        }

        month = 1;
        found = false;

        while (found == false) {
            currentJ2000 = (double)Date::GetDaysInMonth(month, year) * SECONDS_PER_DAY;

            if ((remainingJ2000 - currentJ2000) >= 0.0) {
                remainingJ2000 -= currentJ2000;
                ++month;
            } else {
                found = true;
            }
        }

        day = 1;
        found = false;
        currentJ2000 = SECONDS_PER_DAY;

        while (found == false) {
            if ((remainingJ2000 - currentJ2000) >= 0.0) {
                remainingJ2000 -= currentJ2000;
                ++day;
            } else {
                found = true;
            }
        }
    } else {
        year = 1999;

        while (found == false) {
            if (Date::IsLeapYear(year) == false) {
                currentJ2000 = SECONDS_PER_YEAR;
            } else {
                currentJ2000 = SECONDS_PER_LEAP_YEAR;
            }

            if ((remainingJ2000 + currentJ2000) < 0.0) {
                remainingJ2000 += currentJ2000;
                --year;
            } else {
                found = true;
            }
        }

        month = 12;
        found = false;

        while (found == false) {
            currentJ2000 = (double)Date::GetDaysInMonth(month, year) * SECONDS_PER_DAY;

            if ((remainingJ2000 + currentJ2000) < 0.0) {
                remainingJ2000 += currentJ2000;
                --month;
            } else {
                found = true;
            }
        }

        day = Date::GetDaysInMonth(month, year);
        //JOE:
        remainingJ2000 += SECONDS_PER_DAY;

        while (remainingJ2000 < 0.0) {
            --day;
            remainingJ2000 += SECONDS_PER_DAY;
        }
    }

    hour = (int)(remainingJ2000 * HOURS_PER_SECOND);
    remainingJ2000 -= ((double)hour * SECONDS_PER_HOUR);
    minute = (int)(remainingJ2000 * MINUTES_PER_SECOND);
    remainingJ2000 -= ((double)minute * SECONDS_PER_MINUTE);
    second = (int)remainingJ2000;
    fractionalSeconds = remainingJ2000 - (double)second;
    return;
}

void
Calendar::CalculateDateAndTime(double j2000Time,
                               Date &date, MURALTime &time)
{
    int  year = 0;
    int  month = 0;
    int  day = 0;
    int  hour = 0;
    int  minute = 0;
    int  second = 0;
    double fractionalSeconds = 0.0;
    CalculateDateAndTime(j2000Time, year, month, day,
                         hour, minute, second, fractionalSeconds);

    try {
        date = Date(year, month, day);
        time = MURALTime(hour, minute, second, fractionalSeconds);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "CalculateDateAndTime(const double&, Date&, MURALTime&)");
        throw;
    }

    return;
}

double
Calendar::CalculateGreenwichSiderealAngle(double j2000Time)
{
    double siderealAngle;
    const double days = j2000Time * DAYS_PER_SECOND;
    const double fullCircle = UnitData::GetStoreFullCircle();
    /* compute Greenwich Sidereal Angle */
    siderealAngle = SimpleMath::Remainder(4.894961213 + 1.7202791805e-2 * days
                                          + 5.07521e-15 * days * days + fullCircle
                                          * (days - floor(days)), fullCircle);
    return(siderealAngle);
}

Calendar::~Calendar()
{
    return;
}
