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
 * MURALTime.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/



#ifndef MURAL_TIME_H
#define MURAL_TIME_H "MURALTime"

#include <iostream>
#include <string>

using namespace std;

/**
 * MURALTime is the class that represents an hour, minute, second, and
 *  fractional second format for MURALTime.
 *
 * Class Attribute Descriptions
 *
 * Name                        Description (units)
 * -------------               ------------------------------------------------
 * hour_a                      hour
 * minute_a                    minute
 * second_a                    second
 * fractionalSeconds_a         fractional seconds
 *
 * @author Rob Lowry
 * @author Brian Fowler
 * @date 08/16/11
 */
class MURALTime
{
public:

    enum HOUR_DISPLAY {
        MILITARY = 0,
        AM = 1,
        PM = 2
    };

    MURALTime();
    MURALTime(int inputHour,
              int inputMinute,
              int inputSecond,
              double fractionalSeconds = 0.0,
              HOUR_DISPLAY inputTime = MILITARY);



    explicit MURALTime(const string &timeString);

    MURALTime(const MURALTime &copyMe);
    virtual ~MURALTime();

    MURALTime&      operator =  (const MURALTime &copyMe);
    bool            operator == (const MURALTime &compareMe) const;
    bool            operator != (const MURALTime &compareMe) const;
    bool            operator <  (const MURALTime &compareMe) const;
    bool            operator <= (const MURALTime &compareMe) const;
    bool            operator >  (const MURALTime &compareMe) const;
    bool            operator >= (const MURALTime &compareMe) const;

    void   ClearTime();

    string GetHourDisplay(const HOUR_DISPLAY &display) const;
    string GetMinuteDisplay() const;
    string GetSecondDisplay() const;
    string GetTimeDisplay(HOUR_DISPLAY display = MILITARY,
                          bool includeHourDisplay = false) const;

    inline int  GetHour() const;
    inline int  GetMinute() const;
    inline int  GetSecond() const;
    inline int  GetCivilianHour() const;

    inline double GetFractionalSeconds() const;

protected:

private:

    static inline string GetClassName();

    inline void   SetHour(int inputHour,
                          HOUR_DISPLAY inputTime = MILITARY);










    static const int  MILITARY_HOUR_MINIMUM_s;
    static const int  MILITARY_HOUR_MAXIMUM_s;
    static const int  MINUTE_MINIMUM_s;
    static const int  MINUTE_MAXIMUM_s;
    static const int  SECOND_MINIMUM_s;
    static const int  SECOND_MAXIMUM_s;
    static const double FRACTIONAL_SECOND_MINIMUM_s;
    static const double FRACTIONAL_SECOND_MAXIMUM_s;
    static const char   TIME_SEPARATOR_s;

    int        hour_a;
    int        minute_a;
    int        second_a;
    double       fractionalSeconds_a;
};

inline
int
MURALTime::GetHour() const
{
    return(hour_a);
}

inline
int
MURALTime::GetCivilianHour() const
{
    int civilianHour = 12;

    if (hour_a > 12) {
        civilianHour = hour_a - 12;
    } else if (hour_a > 0) {
        civilianHour = hour_a;
    }

    return(civilianHour);
}

inline
int
MURALTime::GetMinute() const
{
    return(minute_a);
}

inline
int
MURALTime::GetSecond() const
{
    return(second_a);
}

inline
double
MURALTime::GetFractionalSeconds() const
{
    return(fractionalSeconds_a);
}

inline
void
MURALTime::SetHour(int inputHour,
                   HOUR_DISPLAY inputTime)
{
    if (inputTime == MILITARY) {
        hour_a = inputHour;
    } else if (inputTime == AM) {
        if (inputHour == 12) {   // Midnight
            hour_a = 0;
        } else {
            hour_a = inputHour;
        }
    } else {
        if (inputHour == 12) {   // Noon
            hour_a = inputHour;
        } else {
            hour_a = inputHour + 12;
        }
    }

    return;
}

//CPPCHECK_RELATED
//Removed SetMinute() since it wasn't called in code base
/*
inline
void
MURALTime::SetMinute(int inputMinute)
{
    minute_a = inputMinute;
    return;
}
*/

//CPPCHECK_RELATED
//Removed SetSecond() since it wasn't called in code base
/*
inline
void
MURALTime::SetSecond(int inputSecond)
{
    second_a = inputSecond;
    return;
}
*/

//CPPCHECK_RELATED
//Removed SetFractionalSeconds() since it wasn't called in code base
/*
inline
void
MURALTime::SetFractionalSeconds(double fractionalSeconds)
{
    fractionalSeconds_a = fractionalSeconds;
    return;
}
*/

inline
string
MURALTime::GetClassName()
{
    return(MURAL_TIME_H);
}

#endif

