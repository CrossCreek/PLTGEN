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
 * TimePeriod.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TIME_PERIOD_H
#define TIME_PERIOD_H "TimePeriod"

#include <iostream>
#include <string>
#include <vector>

#include "general/array/Array.h"
#include "general/array/LinkList.h"

using namespace std;

//------------------------------------------------------------------------------
// Class Attribute Descriptions
//
// Name                         Description (units)
// -------------                ------------------------------------------
// startTime_a                  Start Time of time period
// endTime_a                    End Time of time period
//
//------------------------------------------------------------------------------

class TimePeriod
{
public:

    TimePeriod();
    TimePeriod(int newStartTime, int newEndTime);
    TimePeriod(const TimePeriod &copyMe);
    virtual ~TimePeriod();

    TimePeriod&     operator =  (const TimePeriod &copyMe);
    bool            operator == (const TimePeriod &compareMe) const;
    bool            operator != (const TimePeriod &compareMe) const;
    bool            operator <  (const TimePeriod &compareMe) const;

    /**
     * For heritage compatability
     */
    static LinkList<TimePeriod>* GroupIntoPeriods(const Array<bool> *ptrTimeStepArray,
            bool longestToShortest = false);

    /**
     * Version which uses standard vector instead of custom Array
     */
    static LinkList<TimePeriod>* GroupIntoPeriods(const vector<bool> &timeStepArray,
            bool longestToShortest = false);

    bool   ContainsTimePeriod(const TimePeriod *ptrOtherTimePeriod) const;

    bool   OverlapsTimePeriod(const TimePeriod *ptrOtherTimePeriod) const;
    bool   OverlapsReportingPeriod();

    inline void SetStartTime(int newStartTime);
    inline void SetEndTime(int newEndTime);

    inline const int GetStartTime() const;
    inline const int GetEndTime() const;
    inline const int GetDuration() const;
    inline const int ConvertTimeStepToIndex(int timeStep) const;

    inline const bool IsDuringTimePeriod(int timeIndex,
                                         int frontPadding = 0,
                                         int backPadding = 0) const;

protected:

    int                startTime_a;
    int                endTime_a;

private:



};

inline
const bool
TimePeriod::IsDuringTimePeriod(int timeIndex,
                               int frontPadding,
                               int backPadding) const
{
    return((timeIndex >= (startTime_a - frontPadding)) && (timeIndex <= (endTime_a + backPadding)));
}

inline
const int
TimePeriod::GetStartTime() const
{
    return(startTime_a);
}

inline
const int
TimePeriod::GetEndTime() const
{
    return(endTime_a);
}

inline
const int
TimePeriod::GetDuration() const
{
    return(endTime_a - startTime_a + 1);
}

inline
const int
TimePeriod::ConvertTimeStepToIndex(int timeStep) const
{
    return(timeStep - startTime_a);
}

inline
void
TimePeriod::SetStartTime(int newStartTime)
{
    startTime_a = newStartTime;
}

inline
void
TimePeriod::SetEndTime(int newEndTime)
{
    endTime_a = newEndTime;
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
TimePeriod::GetClassName()
{
    return(TIME_PERIOD_H);
}
*/

#endif
