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
 * Date.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <sstream>

#include "Date.h"
#include "general/utility/Constant.h"
#include "general/utility/StringEditor.h"
#include "general/exception/DateFormatException.h"

const int  Date::YEAR_MINIMUM_s  = 0;
const int  Date::YEAR_MAXIMUM_s  = 9999;
const int  Date::MONTH_MINIMUM_s = 1;
const int  Date::LEAP_MONTH_s    = 2;
const int  Date::DAY_MINIMUM_s   = 1;
const int  Date::DAY_MAXIMUM_s   = 31;
const int  Date::DAYS_IN_MONTH_s[MONTH_MAXIMUM] =
{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const char   Date::DATE_SEPARATOR_s = '/';
const char   Date::SHORT_MONTHS_s[MONTH_MAXIMUM][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
const char   Date::CAPITAL_SHORT_MONTHS_s[MONTH_MAXIMUM][4] = {
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};
const char   Date::LONG_MONTHS_s[MONTH_MAXIMUM][10] = {
    "January", "February",     "March",   "April",      "May",     "June",
    "July",   "August", "September", "October", "November", "December"
};
const char   Date::CAPITAL_LONG_MONTHS_s[MONTH_MAXIMUM][10] = {
    "JANUARY", "FEBRUARY",     "MARCH",   "APRIL",      "MAY",     "JUNE",
    "JULY",   "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"
};

Date::Date()
    : year_a(YEAR_MINIMUM_s), month_a(MONTH_MINIMUM_s), day_a(DAY_MINIMUM_s)
{
    return;
}

Date::Date(int year, int month, int day)
    : year_a(year), month_a(month), day_a(day)
{
    if ( (day_a < DAY_MINIMUM_s) || (day_a > DAY_MAXIMUM_s) ||
            (month_a < MONTH_MINIMUM_s) || (month_a > MONTH_MAXIMUM) ||
            (year_a < YEAR_MINIMUM_s) || (year_a > YEAR_MAXIMUM_s) ) {
        throw new DateFormatException(GetClassName(),
                                      "Date(int, int, int)",
                                      GetDateDisplay());
    }

    return;
}

Date::Date(const string &dateString)
    : year_a(YEAR_MINIMUM_s), month_a(MONTH_MINIMUM_s), day_a(DAY_MINIMUM_s)
{
    string separatedDateString = dateString;
    StringEditor::RemoveCharacter(separatedDateString, DATE_SEPARATOR_s);
    istringstream dateStream(separatedDateString);
    dateStream >> year_a >> month_a >> day_a;

    if ( (day_a < DAY_MINIMUM_s) || (day_a > DAY_MAXIMUM_s) ||
            (month_a < MONTH_MINIMUM_s) || (month_a > MONTH_MAXIMUM) ||
            (year_a < YEAR_MINIMUM_s) || (year_a > YEAR_MAXIMUM_s) ) {
        throw new DateFormatException(GetClassName(), "Date(const string&)",
                                      GetDateDisplay());
    }

    return;
}

Date::Date(const Date &copyMe)
    : year_a(copyMe.year_a), month_a(copyMe.month_a), day_a(copyMe.day_a)
{
    return;
}

Date&
Date::operator = (const Date &copyMe)
{
    year_a  = copyMe.year_a;
    month_a = copyMe.month_a;
    day_a   = copyMe.day_a;
    return(*this);
}

bool
Date::operator == (const Date &compareMe) const
{
    return( (year_a == compareMe.year_a) &&
            (month_a == compareMe.month_a) &&
            (day_a == compareMe.day_a) );
}

bool
Date::operator != (const Date &compareMe) const
{
    return(!(operator == (compareMe)));
}

bool
Date::operator < (const Date &compareMe) const
{
    bool lessThan = false;

    if (year_a < compareMe.year_a) {
        lessThan = true;
    } else if (year_a == compareMe.year_a) {
        if (month_a < compareMe.month_a) {
            lessThan = true;
        } else if ( (month_a == compareMe.month_a) &&
                    (day_a < compareMe.day_a) ) {
            lessThan = true;
        }
    }

    return(lessThan);
}

bool
Date::operator <= (const Date &compareMe) const
{
    return((operator < (compareMe)) || (operator == (compareMe)));
}

bool
Date::operator > (const Date &compareMe) const
{
    bool greaterThan = false;

    if (year_a > compareMe.year_a) {
        greaterThan = true;
    } else if (year_a == compareMe.year_a) {
        if (month_a > compareMe.month_a) {
            greaterThan = true;
        } else if ( (month_a == compareMe.month_a) &&
                    (day_a > compareMe.day_a) ) {
            greaterThan = true;
        }
    }

    return(greaterThan);
}

bool
Date::operator >= (const Date &compareMe) const
{
    return((operator > (compareMe)) || (operator == (compareMe)));
}

int
Date::ConvertMonth(const string &monthString, const MONTH_DISPLAY &form)
{
    int convertedMonth = -1;

    if (form == NUMERIC) {
        convertedMonth = StringEditor::ConvertToInt(monthString);
    } else if (form == SHORT_FORM) {
        for(int monthIndex = 0; ((monthIndex < MONTH_MAXIMUM) && (convertedMonth == -1)); monthIndex++) {
            if ( monthString == SHORT_MONTHS_s[monthIndex] ) {
                convertedMonth = monthIndex + 1;
            }
        }
    } else if (form == CAPITAL_SHORT_FORM) {
        for(int monthIndex = 0; ((monthIndex < MONTH_MAXIMUM) && (convertedMonth == -1)); monthIndex++) {
            if ( monthString == CAPITAL_SHORT_MONTHS_s[monthIndex] ) {
                convertedMonth = monthIndex + 1;
            }
        }
    } else if (form == LONG_FORM) {
        for(int monthIndex = 0; ((monthIndex < MONTH_MAXIMUM) && (convertedMonth == -1)); monthIndex++) {
            if ( monthString == LONG_MONTHS_s[monthIndex] ) {
                convertedMonth = monthIndex + 1;
            }
        }
    } else if (form == CAPITAL_LONG_FORM) {
        for(int monthIndex = 0; ((monthIndex < MONTH_MAXIMUM) && (convertedMonth == -1)); monthIndex++) {
            if ( monthString == CAPITAL_LONG_MONTHS_s[monthIndex] ) {
                convertedMonth = monthIndex + 1;
            }
        }
    }

    return convertedMonth;
}

string
Date::GetDayDisplay() const
{
    return(StringEditor::ConvertToString(day_a, 2));
}

string
Date::GetMonthDisplay(const MONTH_DISPLAY &form) const
{
    string monthDisplay = "";

    if (form == NUMERIC) {
        monthDisplay = StringEditor::ConvertToString(month_a, 2);
    } else if (form == SHORT_FORM) {
        monthDisplay = SHORT_MONTHS_s[month_a - 1];
    } else if (form == CAPITAL_SHORT_FORM) {
        monthDisplay = CAPITAL_SHORT_MONTHS_s[month_a - 1];
    } else if (form == LONG_FORM) {
        monthDisplay = LONG_MONTHS_s[month_a - 1];
    } else if (form == CAPITAL_LONG_FORM) {
        monthDisplay = CAPITAL_LONG_MONTHS_s[month_a - 1];
    }

    return(monthDisplay);
}

string
Date::GetYearDisplay(const YEAR_DISPLAY &form) const
{
    string yearDisplay = "";

    if (form == FOUR_DIGIT) {
        yearDisplay = StringEditor::ConvertToString(year_a);
    } else {
        int year = 0;
        year = year_a - ((year_a / 100) * 100);
        yearDisplay = StringEditor::ConvertToString(year);
    }

    return(yearDisplay);
}

string
Date::GetDateDisplay(MONTH_DISPLAY monthForm,
                     YEAR_DISPLAY yearForm) const
{
    string dateString = "";

    if (monthForm == NUMERIC) {
        dateString = GetYearDisplay(yearForm);
        dateString += StringEditor::ConvertToString(DATE_SEPARATOR_s);
        dateString += GetMonthDisplay(monthForm);
        dateString += StringEditor::ConvertToString(DATE_SEPARATOR_s);
        dateString += GetDayDisplay();
    } else {
        dateString = GetMonthDisplay(monthForm);
        dateString += " ";
        dateString += StringEditor::ConvertToString(day_a);
        dateString += ", ";
        dateString += GetYearDisplay(yearForm);
    }

    return(dateString);
}

int
Date::GetDaysInMonth(int month, int year)
{
    int  numberOfDays = 0;
    string errorString = "";

    if ((month < MONTH_MINIMUM_s) || (month > MONTH_MAXIMUM)) {
        errorString = "Invalid month: ";
        errorString += StringEditor::ConvertToString(month);
        throw new DateFormatException(GetClassName(),
                                      "GetDaysInMonth(int, int)",
                                      errorString);
    }

    numberOfDays = DAYS_IN_MONTH_s[month - 1];

    if ((month == LEAP_MONTH_s) && (IsLeapYear(year) == true)) {
        ++numberOfDays;
    }

    return(numberOfDays);
}

void
Date::ClearDate()
{
    year_a  = YEAR_MINIMUM_s;
    month_a = MONTH_MINIMUM_s;
    day_a   = DAY_MINIMUM_s;
}

Date::~Date()
{
    return;
}
