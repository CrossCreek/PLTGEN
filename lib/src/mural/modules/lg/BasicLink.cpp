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
 * BasicLink.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "BasicLink.h"
#include "OverheadData.h"
#include "mural/antenna/Antenna.h"
#include "general\utility\Utility.h"
#include "general/math/SpaceVector.h"
#include "mural/element/DataProvider.h"

BasicLink::BasicLink() :
    ptrInviewStatus_a(NULL), ptrTransmitAntenna_a(NULL), ptrReceiveAntenna_a(NULL)
{
    return;
}

/* ************************************************************************************* */

BasicLink::BasicLink(const shared_ptr<Antenna> &ptrFromAntenna,
                     const shared_ptr<Antenna> &ptrToAntenna,
                     const Array<int> *ptrStatusArray)
    : ptrInviewStatus_a(NULL), ptrTransmitAntenna_a(ptrFromAntenna), ptrReceiveAntenna_a(ptrToAntenna)
{
    // copy the contents of status array
    int statusSize = ptrStatusArray->Size();
    ptrInviewStatus_a = shared_ptr<vector<int>>(new vector<int>(statusSize));

    for (int index = 0; index < statusSize; index++) {
        (*ptrInviewStatus_a)[index] = ptrStatusArray->ElementAt(index);
    }

    return;
}

/* ************************************************************************************* */

BasicLink::BasicLink(const BasicLink &copyMe) :
    ptrTransmitAntenna_a(copyMe.ptrTransmitAntenna_a),
    ptrReceiveAntenna_a(copyMe.ptrReceiveAntenna_a),
    ptrInviewStatus_a(NULL)
{
    if (copyMe.ptrInviewStatus_a != NULL) {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        int statusSize = Utility::CastSizetToInt(copyMe.ptrInviewStatus_a->size());
        ptrInviewStatus_a = shared_ptr<vector<int>>(new vector<int>(statusSize));

        for (int index = 0; index < statusSize; index++) {
            (*ptrInviewStatus_a)[index] = (*copyMe.ptrInviewStatus_a)[index];
        }
    }
}

/* ************************************************************************************* */

const shared_ptr<Antenna>
BasicLink::GetTransmitAntenna() const
{
    return(ptrTransmitAntenna_a);
}

/* ************************************************************************************* */

const shared_ptr<Antenna>
BasicLink::GetReceiveAntenna() const
{
    return(ptrReceiveAntenna_a);
}

/* ************************************************************************************* */

const int
BasicLink::GetLinkStatusNumber(int timeIndex) const
{
    return(ptrInviewStatus_a->at(timeIndex));
}

/* ************************************************************************************* */

BasicLink::LINK_STATUS
BasicLink::GetLinkStatus(int timeIndex) const
{
    return((LINK_STATUS)ptrInviewStatus_a->at(timeIndex));
}

/* ************************************************************************************* */

void
BasicLink::ResetInviews(int startTime,
                        int endTime,
                        const LINK_STATUS &status)
{
    int timeIndex = startTime;

    while (timeIndex <= endTime) {
        SetLinkStatus(timeIndex, status);
        ++timeIndex;
    }

    return;
}

/* ************************************************************************************* */

void
BasicLink::SetLinkStatus(int timeIndex,
                         const LINK_STATUS &status)
{
    (*ptrInviewStatus_a)[timeIndex] = status;
}

/* ************************************************************************************* */

void
BasicLink::UpdateLinkStatus(int timeIndex, const LINK_STATUS &linkStatus)
{
    if (ptrInviewStatus_a == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "UpdateLinkStatus(int,LINK_STATUS)",
                                       "ptrInviewStatus_a");
    }

    if (ptrInviewStatus_a->at(timeIndex) == LINK) {
        (*ptrInviewStatus_a)[timeIndex] = linkStatus;
    }
}

/* ************************************************************************************* */

const bool
BasicLink::IsInview(int timeIndex) const
{
    bool isInview = false;

    if ( (ptrInviewStatus_a != NULL) && (timeIndex >= 0) &&
            (timeIndex < ptrInviewStatus_a->size()) ) {
        if (ptrInviewStatus_a->at(timeIndex) == LINK) {
            isInview = true;
        }
    }

    return(isInview);
}

/* ************************************************************************************* */

const bool
BasicLink::IsInview(int startTimeIndex,
                    int endTimeIndex) const
{
    int timeIndex = startTimeIndex;
    bool isInview = true;

    try {
        while ((timeIndex <= endTimeIndex) && (isInview == true)) {
            isInview = IsInview(timeIndex);
            ++timeIndex;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "IsInview(int, int)");
        throw;
    }

    return(isInview);
}

/* ************************************************************************************* */

const bool
BasicLink::AnyInview(int startTimeIndex,
                     int endTimeIndex) const
{
    int timeIndex = startTimeIndex;
    bool anyInview = false;

    try {
        while ((timeIndex <= endTimeIndex) && (anyInview == false)) {
            anyInview = IsInview(timeIndex);
            ++timeIndex;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "AnyInview(int, int)");
        throw;
    }

    return(anyInview);
}

/* ************************************************************************************* */

int
BasicLink::GetLastInview(int startTime) const
{
    int       last = startTime;
    const int numberOfTimesteps = TimePiece::GetNumberOfTimeSteps();
    bool     foundLast = false;

    while ((foundLast == false) && (last < numberOfTimesteps)) {
        if (IsInview(last) == true) {
            last++;
        } else {
            foundLast = true;
        }
    }

    return(last);
}

/* ************************************************************************************* */

const bool
BasicLink::GetInviewPeriod(int currentTime,
                           int &startInviewPeriod,
                           int &endInviewPeriod,
                           int endCheckTime) const
{
    bool foundInviewPeriod = false;
    startInviewPeriod = GetFirstInviewTimeStep(currentTime);
    endInviewPeriod = startInviewPeriod;

    if (startInviewPeriod <= endCheckTime) {
        ++endInviewPeriod;

        while ( (endInviewPeriod <= endCheckTime) &&
                (foundInviewPeriod == false) ) {
            if (IsInview(endInviewPeriod) == false) {
                foundInviewPeriod = true;
            } else {
                ++endInviewPeriod;
            }
        }

        --endInviewPeriod;
        foundInviewPeriod = true;
    }

    return(foundInviewPeriod);
}

/* ************************************************************************************* */

int
BasicLink::GetFirstInviewTimeStep(int startingTime) const
{
    int earliestInviewTime = startingTime;
    bool foundInview = false;

    while ( (earliestInviewTime < ptrInviewStatus_a->size()) &&
            (foundInview == false) ) {
        if (IsInview(earliestInviewTime) == true) {
            foundInview = true;
        } else {
            ++earliestInviewTime;
        }
    }

    return(earliestInviewTime);
}

/* ************************************************************************************* */

int
BasicLink::GetLastInviewTimeStep(int endingTime) const
{
    int latestInviewTime = endingTime;
    bool foundInview = false;

    while ( (latestInviewTime >= 0) && (foundInview == false) ) {
        if (IsInview(latestInviewTime) == true) {
            foundInview = true;
        } else {
            --latestInviewTime;
        }
    }

    return(latestInviewTime);
}

/* ************************************************************************************* */

int
BasicLink::GetEarliestInviewTimeStep(int startingTime,
                                     int stopCheckTime) const
{
    int earliestInviewTime = startingTime;
    bool foundEarliestInview = false;

    while ((earliestInviewTime >= stopCheckTime) && (foundEarliestInview == false)) {
        if (IsInview(earliestInviewTime) == true) {
            --earliestInviewTime;
        } else {
            foundEarliestInview = true;
        }
    }

    return(++earliestInviewTime);
}

/* ************************************************************************************* */
int
BasicLink::GetLatestInviewTimeStep(int endingTime,
                                   int stopCheckTime) const
{
    int latestInviewTime = endingTime;
    bool foundLatestInview = false;

    while ((latestInviewTime <= stopCheckTime) && (foundLatestInview == false)) {
        if (IsInview(latestInviewTime) == true) {
            ++latestInviewTime;
        } else {
            foundLatestInview = true;
        }
    }

    return(--latestInviewTime);
}

/* ************************************************************************************* */

shared_ptr<DataProvider>
BasicLink::GetTransmitProvider() const
{
    return(ptrTransmitAntenna_a->GetDataProvider());
}

/* ************************************************************************************* */

shared_ptr<DataProvider>
BasicLink::GetReceiveProvider() const
{
    return(ptrReceiveAntenna_a->GetDataProvider());
}

/* ************************************************************************************* */

SpaceVector
BasicLink::GetTransmitPostion(int timeIndex) const
{
    return((ptrTransmitAntenna_a->GetDataProvider())->GetPosition(timeIndex));
}

/* ************************************************************************************* */

SpaceVector
BasicLink::GetTransmitVelocity(int timeIndex) const
{
    return((ptrTransmitAntenna_a->GetDataProvider())->GetVelocity(timeIndex));
}

/* ************************************************************************************* */

SpaceVector
BasicLink::GetReceivePostion(int timeIndex) const
{
    return((ptrReceiveAntenna_a->GetDataProvider())->GetPosition(timeIndex));
}

/* ************************************************************************************* */

SpaceVector
BasicLink::GetReceiveVelocity(int timeIndex) const
{
    return((ptrReceiveAntenna_a->GetDataProvider())->GetVelocity(timeIndex));
}

/* ************************************************************************************* */

SpaceVector
BasicLink::GetLineOfSightVector(int timeIndex) const
{
    return(GetReceivePostion(timeIndex) - GetTransmitPostion(timeIndex));
}

/* ************************************************************************************* */

string
BasicLink::GetTransmitDesignator() const
{
    return(ptrTransmitAntenna_a->GetDataProviderDesignator());
}

/* ************************************************************************************* */

string
BasicLink::GetReceiveDesignator() const
{
    return(ptrReceiveAntenna_a->GetDataProviderDesignator());
}

/* ************************************************************************************* */

string
BasicLink::GetTransmitAntennaDesignator() const
{
    return(ptrTransmitAntenna_a->GetAntennaDesignator());
}

/* ************************************************************************************* */

string
BasicLink::GetReceiveAntennaDesignator() const
{
    return(ptrReceiveAntenna_a->GetAntennaDesignator());
}

/* ************************************************************************************* */

string
BasicLink::GetLinkStatusString(const LINK_STATUS &status)
{
    string returnString = "UNKNOWN";

    if (status == NO_LINK) {
        returnString = "NO_LINK";
    } else if (status == LINK) {
        returnString = "LINK";
    } else if (status == MAX_ELEVATION_TRANSMIT) {
        returnString = "MAX_ELEVATION_TRANSMIT";
    } else if (status == MIN_ELEVATION_TRANSMIT) {
        returnString = "MIN_ELEVATION_TRANSMIT";
    } else if (status == MAX_ELEVATION_RECEIVE) {
        returnString = "MAX_ELEVATION_RECEIVE";
    } else if (status == MIN_ELEVATION_RECEIVE) {
        returnString = "MIN_ELEVATION_RECEIVE";
    } else if (status == MIN_GRAZING_ALTITUDE) {
        returnString = "MIN_GRAZING_ALTITUDE";
    } else if (status == MAX_RANGE_TRANSMIT) {
        returnString = "MAX_RANGE_TRANSMIT";
    } else if (status == MIN_RANGE_TRANSMIT) {
        returnString = "MIN_RANGE_TRANSMIT";
    } else if (status == MAX_SUN_SEPARATION_TRANSMIT) {
        returnString = "MAX_SUN_SEPARATION_TRANSMIT";
    } else if (status == MIN_SUN_SEPARATION_TRANSMIT) {
        returnString = "MIN_SUN_SEPARATION_TRANSMIT";
    } else if (status == MAX_SUN_SEPARATION_RECEIVE) {
        returnString = "MAX_SUN_SEPARATION_RECEIVE";
    } else if (status == MIN_SUN_SEPARATION_RECEIVE) {
        returnString = "MIN_SUN_SEPARATION_RECEIVE";
    } else if (status == MIN_AZIMUTH_TRANSMIT) {
        returnString = "MIN_AZIMUTH_TRANSMIT";
    } else if (status == MIN_AZIMUTH_RECEIVE) {
        returnString = "MIN_AZIMUTH_RECEIVE";
    } else if (status == OUTAGE_TIME_STEP) {
        returnString = "OUTAGE_TIME_STEP";
    } else if (status == FULLY_DEDICATED_TIME_STEP) {
        returnString = "FULLY_DEDICATED_TIME_STEP";
    } else if (status == NO_SUPPORTING_DOWNLINK) {
        returnString = "NO_SUPPORTING_DOWNLINK";
    } else if (status == STATE_OF_HEALTH_TIME_STEP) {
        returnString = "STATE_OF_HEALTH_TIME_STEP";
    } else if (status == REQUIRED_DOWNLINK_OVERHEAD) {
        returnString = "REQUIRED_DOWNLINK_OVERHEAD";
    } else if (status == MIN_SOLAR_INTRUSION) {
        returnString = "MIN_SOLAR_INTRUSION";
    } else if (status == POSSE_SHORT_INVIEW) {
        returnString = "POSSE_SHORT_INVIEW";
    } else if (status == POSSE_NOT_REQUESTING_INVIEW) {
        returnString = "POSSE_NOT_REQUESTING_INVIEW";
    }

    return(returnString);
}

/* ************************************************************************************* */

BasicLink::~BasicLink()
{
    return;
}

