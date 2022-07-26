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
 * MURALTime.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <sstream>
#include "MURALTime.h"
#include "general/utility/Constant.h"
#include "general/utility/StringEditor.h"
#include "general/exception/TimeException.h"

const int  MURALTime::MILITARY_HOUR_MINIMUM_s = 0;
const int  MURALTime::MILITARY_HOUR_MAXIMUM_s = 23;
const int  MURALTime::MINUTE_MINIMUM_s = 0;
const int  MURALTime::MINUTE_MAXIMUM_s = 59;
const int  MURALTime::SECOND_MINIMUM_s = 0;
const int  MURALTime::SECOND_MAXIMUM_s = 59;
const double MURALTime::FRACTIONAL_SECOND_MINIMUM_s = 0.0;
const double MURALTime::FRACTIONAL_SECOND_MAXIMUM_s = 1.0;
const char   MURALTime::TIME_SEPARATOR_s = ':';

MURALTime::MURALTime()
    : hour_a(MILITARY_HOUR_MINIMUM_s), minute_a(MINUTE_MINIMUM_s),
      second_a(SECOND_MINIMUM_s), fractionalSeconds_a(FRACTIONAL_SECOND_MINIMUM_s)
{
    return;
}

MURALTime::MURALTime(int inputHour,
                     int inputMinute,
                     int inputSecond,
                     double fractionalSeconds,
                     HOUR_DISPLAY inputTime)
    : hour_a(MILITARY_HOUR_MINIMUM_s), minute_a(inputMinute),
      second_a(inputSecond), fractionalSeconds_a(fractionalSeconds)
{
    SetHour(inputHour, inputTime);

    if ( (hour_a < MILITARY_HOUR_MINIMUM_s) || (hour_a > MILITARY_HOUR_MAXIMUM_s) ||
            (minute_a < MINUTE_MINIMUM_s) || (minute_a > MINUTE_MAXIMUM_s) ||
            (second_a < SECOND_MINIMUM_s) || (second_a > SECOND_MAXIMUM_s) ||
            (fractionalSeconds_a < FRACTIONAL_SECOND_MINIMUM_s) ||
            (fractionalSeconds_a > FRACTIONAL_SECOND_MAXIMUM_s) ) {
        throw new TimeException(GetClassName(),
                                "MURALTime(int,int,int,...)",
                                GetTimeDisplay());
    }

    return;
}

MURALTime::MURALTime(const string &timeString)
    : hour_a(MILITARY_HOUR_MINIMUM_s), minute_a(MINUTE_MINIMUM_s),
      second_a(SECOND_MINIMUM_s), fractionalSeconds_a(FRACTIONAL_SECOND_MINIMUM_s)
{
    double       fullSeconds = 0.0;
    string       separateTimeString = timeString;
    StringEditor::RemoveCharacter(separateTimeString, TIME_SEPARATOR_s);
    istringstream timeStream(separateTimeString);
    timeStream >> hour_a >> minute_a >> fullSeconds;
    second_a = (int)fullSeconds;
    fractionalSeconds_a = fullSeconds - (double)second_a;

    if ( (hour_a < MILITARY_HOUR_MINIMUM_s) || (hour_a > MILITARY_HOUR_MAXIMUM_s) ||
            (minute_a < MINUTE_MINIMUM_s) || (minute_a > MINUTE_MAXIMUM_s) ||
            (second_a < SECOND_MINIMUM_s) || (second_a > SECOND_MAXIMUM_s) ||
            (fractionalSeconds_a < FRACTIONAL_SECOND_MINIMUM_s) ||
            (fractionalSeconds_a > FRACTIONAL_SECOND_MAXIMUM_s) ) {
        throw new TimeException(GetClassName(), "MURALTime(const string&,",
                                GetTimeDisplay());
    }

    return;
}

MURALTime::MURALTime(const MURALTime &copyMe)
    : hour_a(copyMe.hour_a), minute_a(copyMe.minute_a), second_a(copyMe.second_a),
      fractionalSeconds_a(copyMe.fractionalSeconds_a)
{
    return;
}

MURALTime&
MURALTime::operator = (const MURALTime &copyMe)
{
    hour_a              = copyMe.hour_a;
    minute_a            = copyMe.minute_a;
    second_a            = copyMe.second_a;
    fractionalSeconds_a = copyMe.fractionalSeconds_a;
    return(*this);
}

bool
MURALTime::operator == (const MURALTime &compareMe) const
{
    return( (hour_a == compareMe.hour_a) &&
            (minute_a == compareMe.minute_a) &&
            (second_a == compareMe.second_a) &&
            (fractionalSeconds_a == compareMe.fractionalSeconds_a) );
}

bool
MURALTime::operator != (const MURALTime &compareMe) const
{
    return(!(operator == (compareMe)));
}

bool
MURALTime::operator < (const MURALTime &compareMe) const
{
    bool lessThan = false;

    if (hour_a < compareMe.hour_a) {
        lessThan = true;
    } else if (hour_a == compareMe.hour_a) {
        if (minute_a < compareMe.minute_a) {
            lessThan = true;
        } else if (minute_a == compareMe.minute_a) {
            if (second_a < compareMe.second_a) {
                lessThan = true;
            } else if ( (second_a == compareMe.second_a) &&
                        (fractionalSeconds_a < compareMe.fractionalSeconds_a) ) {
                lessThan = true;
            }
        }
    }

    return(lessThan);
}

bool
MURALTime::operator <= (const MURALTime &compareMe) const
{
    return((operator < (compareMe)) || (operator == (compareMe)));
}

bool
MURALTime::operator > (const MURALTime &compareMe) const
{
    bool greaterThan = false;

    if (hour_a > compareMe.hour_a) {
        greaterThan = true;
    } else if (hour_a == compareMe.hour_a) {
        if (minute_a > compareMe.minute_a) {
            greaterThan = true;
        } else if (minute_a == compareMe.minute_a) {
            if (second_a > compareMe.second_a) {
                greaterThan = true;
            } else if ( (second_a == compareMe.second_a) &&
                        (fractionalSeconds_a > compareMe.fractionalSeconds_a) ) {
                greaterThan = true;
            }
        }
    }

    return(greaterThan);
}

bool
MURALTime::operator >= (const MURALTime &compareMe) const
{
    return((operator > (compareMe)) || (operator == (compareMe)));
}

void
MURALTime::ClearTime()
{
    hour_a   = MILITARY_HOUR_MINIMUM_s;
    minute_a = MINUTE_MINIMUM_s;
    second_a = SECOND_MINIMUM_s;
    fractionalSeconds_a = FRACTIONAL_SECOND_MINIMUM_s;
    return;
}

string
MURALTime::GetHourDisplay(const HOUR_DISPLAY &display) const
{
    string hourDisplay = "";

    if (display == MILITARY) {
        hourDisplay = StringEditor::ConvertToString(hour_a, 2);
    } else {
        hourDisplay = StringEditor::ConvertToString(GetCivilianHour(), 2);
    }

    return(hourDisplay);
}

string
MURALTime::GetMinuteDisplay() const
{
    return(StringEditor::ConvertToString(minute_a, 2));
}

string
MURALTime::GetSecondDisplay() const
{
    string secondDisplay = "";

    if (fractionalSeconds_a == 0.0) {
        secondDisplay = StringEditor::ConvertToString(second_a, 2);
    } else {
        double seconds = 0.0;

        if (second_a < 10) {
            secondDisplay = "0";
        }

        seconds = (double)second_a + fractionalSeconds_a;
        secondDisplay += StringEditor::ConvertToString(seconds);
    }

    return(secondDisplay);
}

string
MURALTime::GetTimeDisplay(HOUR_DISPLAY display,
                          bool includeHourDisplay) const
{
    string timeDisplay = GetHourDisplay(display);
    timeDisplay += StringEditor::ConvertToString(TIME_SEPARATOR_s);
    timeDisplay += GetMinuteDisplay();
    timeDisplay += StringEditor::ConvertToString(TIME_SEPARATOR_s);
    timeDisplay += GetSecondDisplay();

    if (includeHourDisplay == true) {
        if (display == MILITARY) {
            timeDisplay += " MILITARY";
        } else if (hour_a < 12) {
            timeDisplay += " AM";
        } else {
            timeDisplay += " PM";
        }
    }

    return(timeDisplay);
}

MURALTime::~MURALTime()
{
    return;
}
