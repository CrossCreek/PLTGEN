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
 * Date.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef DATE_H
#define DATE_H "Date"

#include <iomanip>
#include <iostream>
#include <string>

#include "general/utility/Constant.h"

using namespace std;

/**
 * Date is the class used to represent the current date in year-
 *  month-day format.
 *
 * Class Attribute Descriptions
 *
 * Name                         Description (units)
 * -------------                ------------------------------------------
 * year_a                       year
 * month_a                      month
 * day_a                        day
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */
class Date
{
public:

    enum MONTH_DISPLAY {
        NUMERIC = 0,
        SHORT_FORM = 1,
        CAPITAL_SHORT_FORM = 2,
        LONG_FORM = 3,
        CAPITAL_LONG_FORM = 4
    };

    enum YEAR_DISPLAY {
        FOUR_DIGIT = 0,
        TWO_DIGIT = 1
    };

    Date();
    Date(int year, int month, int day);



    explicit Date(const string &dateString);

    Date(const Date &copyMe);
    virtual ~Date();

    Date&           operator =  (const Date &copyMe);
    bool            operator == (const Date &compareMe) const;
    bool            operator != (const Date &compareMe) const;
    bool            operator <  (const Date &compareMe) const;
    bool            operator <= (const Date &compareMe) const;
    bool            operator >  (const Date &compareMe) const;
    bool            operator >= (const Date &compareMe) const;

    static int GetDaysInMonth(int month, int year);

    void   ClearDate();

    string GetDayDisplay() const;
    string GetMonthDisplay(const MONTH_DISPLAY &form) const;
    string GetYearDisplay(const YEAR_DISPLAY &form) const;
    string GetDateDisplay(MONTH_DISPLAY monthForm = NUMERIC,
                          YEAR_DISPLAY yearForm = FOUR_DIGIT) const;

    static inline int  GetCummulativeDays(int year,
                                          int month);
    static inline int  GetCummulativeDays(int year,
                                          int month,
                                          int day);

    static inline bool  IsLeapYear(int year);

    static inline string GetShortMonth(int month);

    static int    ConvertMonth (const string &monthString,
                                const MONTH_DISPLAY &form);

    inline void   SetYear(int year);
    inline void   SetMonth(int month);
    inline void   SetDay(int day);

    inline int  GetYear() const;
    inline int  GetMonth() const;
    inline int  GetDay() const;

    inline bool   IsLeapYear() const;

protected:

    int year_a;
    int month_a;
    int day_a;

private:

    static inline string GetClassName();

    static const int  YEAR_MINIMUM_s;
    static const int  YEAR_MAXIMUM_s;
    static const int  MONTH_MINIMUM_s;
    static const int  LEAP_MONTH_s;
    static const int  DAY_MINIMUM_s;
    static const int  DAY_MAXIMUM_s;
    static const int  DAYS_IN_MONTH_s[MONTH_MAXIMUM];
    static const char   DATE_SEPARATOR_s;
    static const char   SHORT_MONTHS_s[MONTH_MAXIMUM][4];
    static const char   CAPITAL_SHORT_MONTHS_s[MONTH_MAXIMUM][4];
    static const char   LONG_MONTHS_s[MONTH_MAXIMUM][10];
    static const char   CAPITAL_LONG_MONTHS_s[MONTH_MAXIMUM][10];
};

inline
string
Date::GetShortMonth(int month)
{
    return(string(CAPITAL_SHORT_MONTHS_s[month - 1]));
}

inline
int
Date::GetYear() const
{
    return(year_a);
}

inline
int
Date::GetMonth() const
{
    return(month_a);
}

inline
int
Date::GetDay() const
{
    return(day_a);
}

inline
int
Date::GetCummulativeDays(int year,
                         int month)
{
    int cummulativeDays = 0;
    int countMonth = 0;

    while (countMonth < month) {
        cummulativeDays += DAYS_IN_MONTH_s[countMonth];
        ++countMonth;
    }

    if ((IsLeapYear(year) == true) && (month >= LEAP_MONTH_s)) {
        ++cummulativeDays;
    }

    return(cummulativeDays);
}

inline
int
Date::GetCummulativeDays(int year,
                         int month,
                         int day)
{
    int cummulativeDays = day;
    int countMonth = 0;

    while (countMonth < (month - 1)) {
        cummulativeDays += DAYS_IN_MONTH_s[countMonth];
        ++countMonth;
    }

    if ((IsLeapYear(year) == true) && (month > LEAP_MONTH_s)) {
        ++cummulativeDays;
    }

    return(cummulativeDays);
}

inline
void
Date::SetYear(int year)
{
    year_a = year;
    return;
}

inline
void
Date::SetMonth(int month)
{
    month_a = month;
    return;
}

inline
void
Date::SetDay(int day)
{
    day_a = day;
    return;
}

inline
bool
Date::IsLeapYear() const
{
    return(IsLeapYear(year_a));
}

inline
bool
Date::IsLeapYear(int year)
{
    return ( ((year % 400) == 0) ||
             (((year % 4) == 0) && ((year % 100) != 0)) );
}

inline
string
Date::GetClassName()
{
    return(DATE_H);
}

#endif

