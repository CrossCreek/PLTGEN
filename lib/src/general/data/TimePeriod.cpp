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
 * TimePeriod.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "TimePeriod.h"

#include "general/time/TimePiece.h"
#include "general\utility\Utility.h"

//------------------------------------------------------------------------------
//
//  Method Name: TimePeriod::TimePeriod
//
//  Purpose:
//    Constructor for the TimePeriod class.
//
//  Calling Method:
//     TimePeriod();
//
//  Calling Parameters
//  Parameters      i/o  description
//  ----------      ---  -----------
//
//  Returns:
//     None
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

TimePeriod::TimePeriod()
    : startTime_a(0), endTime_a(0)
{
    return;
}

TimePeriod::TimePeriod(int newStartTime, int newEndTime)
    : startTime_a(newStartTime), endTime_a(newEndTime)
{
    return;
}

TimePeriod::TimePeriod(const TimePeriod &copyMe)
    : startTime_a(copyMe.startTime_a), endTime_a(copyMe.endTime_a)
{
    return;
}

TimePeriod&
TimePeriod::operator = (const TimePeriod &copyMe)
{
    startTime_a = copyMe.startTime_a;
    endTime_a = copyMe.endTime_a;
    return(*this);
}

bool
TimePeriod::operator == (const TimePeriod &compareMe) const
{
    return( (startTime_a == compareMe.startTime_a) &&
            (endTime_a == compareMe.endTime_a) );
}

bool
TimePeriod::operator != (const TimePeriod &compareMe) const
{
    return(!(operator == (compareMe)));
}

bool
TimePeriod::operator < (const TimePeriod &compareMe) const
{
    return(startTime_a < compareMe.startTime_a);
}

bool
TimePeriod::ContainsTimePeriod(const TimePeriod *ptrOtherTimePeriod) const
{
    return( (ptrOtherTimePeriod->GetStartTime() >= startTime_a) &&
            (ptrOtherTimePeriod->GetEndTime() <= endTime_a));
}
bool
TimePeriod::OverlapsReportingPeriod()
{
    bool returnValue = true;

    if((startTime_a > TimePiece::GetReportEndIndexTime()) || (endTime_a < TimePiece::GetReportStartIndexTime())) {
        returnValue = false;
    }

    return(returnValue);
}

bool
TimePeriod::OverlapsTimePeriod(const TimePeriod *ptrOtherTimePeriod) const
{
    bool overlapsTimePeriod = true;

    if ( (endTime_a < ptrOtherTimePeriod->GetStartTime()) ||
            (startTime_a > ptrOtherTimePeriod->GetEndTime()) ) {
        overlapsTimePeriod = false;
    }

    return(overlapsTimePeriod);
}

LinkList<TimePeriod>*
TimePeriod::GroupIntoPeriods(const Array<bool> *ptrTimeStepArray,
                             bool longestToShortest)
{
    int                  timeStep = 0;
    int                  startTimeStep = 0;
    int                  sortValue = -1;
    const int            numberOfTimeSteps = ptrTimeStepArray->Size();
    bool                  withinPeriod = false;
    TimePeriod           *ptrTimePeriod = NULL;
    LinkList<TimePeriod> *ptrTimePeriodList = new LinkList<TimePeriod>();

    if (longestToShortest == true) {
        ptrTimePeriodList->SetOrdering(DESCENDING);
    }

    while (timeStep < numberOfTimeSteps) {
        if (ptrTimeStepArray->ElementAt(timeStep) == true) {
            if (withinPeriod == false) {
                startTimeStep = timeStep;
                withinPeriod = true;
            }
        } else if (withinPeriod == true) {
            ptrTimePeriod = new TimePeriod(startTimeStep, (timeStep - 1));

            if (longestToShortest == true) {
                sortValue = ptrTimePeriod->GetDuration();
            }

            ptrTimePeriodList->Append(ptrTimePeriod, sortValue);
            withinPeriod = false;
        }

        ++timeStep;
    }

    if (withinPeriod == true) {
        ptrTimePeriod = new TimePeriod(startTimeStep, (numberOfTimeSteps - 1));

        if (longestToShortest == true) {
            sortValue = ptrTimePeriod->GetDuration();
        }

        ptrTimePeriodList->Append(ptrTimePeriod, sortValue);
    } else if (ptrTimePeriodList->Size() == 0) {
        delete ptrTimePeriodList;
        ptrTimePeriodList = NULL;
    }

    return(ptrTimePeriodList);
}

LinkList<TimePeriod>*
TimePeriod::GroupIntoPeriods(const vector<bool> &timeStepArray,
                             bool longestToShortest)
{
    int                  timeStep = 0;
    int                  startTimeStep = 0;
    int                  sortValue = -1;
    //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
    const int             numberOfTimeSteps = Utility::CastSizetToInt(timeStepArray.size());
    bool                  withinPeriod = false;
    TimePeriod           *ptrTimePeriod = NULL;
    LinkList<TimePeriod> *ptrTimePeriodList = new LinkList<TimePeriod>();

    if (longestToShortest == true) {
        ptrTimePeriodList->SetOrdering(DESCENDING);
    }

    while (timeStep < numberOfTimeSteps) {
        if (timeStepArray.at(timeStep) == true) {
            if (withinPeriod == false) {
                startTimeStep = timeStep;
                withinPeriod = true;
            }
        } else if (withinPeriod == true) {
            ptrTimePeriod = new TimePeriod(startTimeStep, (timeStep - 1));

            if (longestToShortest == true) {
                sortValue = ptrTimePeriod->GetDuration();
            }

            ptrTimePeriodList->Append(ptrTimePeriod, sortValue);
            withinPeriod = false;
        }

        ++timeStep;
    }

    if (withinPeriod == true) {
        ptrTimePeriod = new TimePeriod(startTimeStep, (numberOfTimeSteps - 1));

        if (longestToShortest == true) {
            sortValue = ptrTimePeriod->GetDuration();
        }

        ptrTimePeriodList->Append(ptrTimePeriod, sortValue);
    } else if (ptrTimePeriodList->Size() == 0) {
        delete ptrTimePeriodList;
        ptrTimePeriodList = NULL;
    }

    return(ptrTimePeriodList);
}

TimePeriod::~TimePeriod()
{
    return;
}

