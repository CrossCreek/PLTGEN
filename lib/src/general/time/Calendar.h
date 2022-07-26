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
 * Calendar.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef CALENDAR_H
#define CALENDAR_H "Calendar"

#include <memory>

#include "Date.h"
#include "MURALTime.h"

#include "general/utility/Constant.h"

using namespace std;

/**
 * Calendar is the class that holds a gregorian calendar date and time.
 *
 * Class Attribute Descriptions
 *
 * Name                Description (units)
 * -------------       ---------------------------------------------------------
 * j2000Time_a         seconds from 1/1/2000 @ 12:00:00 (noon) to this date/time
 * ptrDate_a*          pointer to an instance of the Date class
 * ptrTime_a*          pointer to an instance of the MURALTime class
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */

class Calendar
{
public:

    Calendar();
    Calendar(int year,
             int month,
             int day,
             int hour,
             int minute,
             int second,
             double fractionalSeconds = 0.0,
             MURALTime::HOUR_DISPLAY display = MURALTime::MILITARY);


    Calendar(double j2000Time);
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit Calendar(double j2000Time);

    Calendar(const string &dateString,
             const string &timeString);
    Calendar(const Date &date, const MURALTime &time);
    Calendar(const Calendar &copyMe);
    virtual ~Calendar();

    Calendar&       operator =  (const Calendar &copyMe);
    bool            operator == (const Calendar &compareMe) const;
    bool            operator != (const Calendar &compareMe) const;
    bool            operator <  (const Calendar &compareMe) const;
    bool            operator <= (const Calendar &compareMe) const;
    bool            operator >  (const Calendar &compareMe) const;
    bool            operator >= (const Calendar &compareMe) const;

    static void CalculateDateAndTime(double j2000Time,
                                     int &year, int &month, int &day,
                                     int &hour, int &minute, int &second,
                                     double &fractionalSeconds);
    static void CalculateDateAndTime(double j2000Time,
                                     Date &date, MURALTime &time);

    static double CalculateJ2000Time(const Date &date,
                                     const MURALTime &time);
    static double CalculateJ2000Time(int year,
                                     int month,
                                     int day,
                                     int hour,
                                     int minute,
                                     int second,
                                     double fractionalSeconds);
    static double CalculateGreenwichSiderealAngle(double j2000Time);

    inline int  GetYear() const;
    inline int  GetMonth() const;
    inline int  GetDay() const;
    inline int  GetHour() const;
    inline int  GetMinute() const;
    inline int  GetSecond() const;

    inline double GetJ2000Time() const;
    inline double GetFractionalSeconds() const;
    inline double CalculateGreenwichSiderealAngle() const;

    inline bool   IsLeapYear() const;

    inline const string GetDateDisplay() const;
    inline const string GetTimeDisplay() const;
    inline const string GetDateAndTimeDisplay() const;

    inline Date*       GetDate() const;
    inline MURALTime*  GetTime() const;

protected:

private:

    static inline string GetClassName();

    double     j2000Time_a;
    shared_ptr<Date>      ptrDate_a;
    shared_ptr<MURALTime> ptrTime_a;
};

inline
int
Calendar::GetYear() const
{
    return(ptrDate_a->GetYear());
}

inline
int
Calendar::GetMonth() const
{
    return(ptrDate_a->GetMonth());
}

inline
int
Calendar::GetDay() const
{
    return(ptrDate_a->GetDay());
}

inline
int
Calendar::GetHour() const
{
    return(ptrTime_a->GetHour());
}

inline
int
Calendar::GetMinute() const
{
    return(ptrTime_a->GetMinute());
}

inline
int
Calendar::GetSecond() const
{
    return(ptrTime_a->GetSecond());
}

inline
double
Calendar::GetJ2000Time() const
{
    return(j2000Time_a);
}

inline
double
Calendar::GetFractionalSeconds() const
{
    return(ptrTime_a->GetFractionalSeconds());
}

inline
double
Calendar::CalculateGreenwichSiderealAngle() const
{
    return(CalculateGreenwichSiderealAngle(j2000Time_a));
}

inline
bool
Calendar::IsLeapYear() const
{
    return(ptrDate_a->IsLeapYear());
}

inline
const string
Calendar::GetDateDisplay() const
{
    return(ptrDate_a->GetDateDisplay());
}

inline
const string
Calendar::GetTimeDisplay() const
{
    return(ptrTime_a->GetTimeDisplay());
}

inline
const string
Calendar::GetDateAndTimeDisplay() const
{
    return(ptrDate_a->GetDateDisplay() + " " + ptrTime_a->GetTimeDisplay());
}

inline
Date*
Calendar::GetDate() const
{
    return(ptrDate_a.get());
}

inline
MURALTime*
Calendar::GetTime() const
{
    return(ptrTime_a.get());
}

inline
string
Calendar::GetClassName()
{
    return(CALENDAR_H);
}

#endif
