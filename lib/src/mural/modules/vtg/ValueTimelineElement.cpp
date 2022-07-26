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
 * ValueTimelineElement.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>
#include "ValueTimelineElement.h"
#include "mural/Region.h"
#include "RegionData.h"

#include "general/exception/NullPointerException.h"
#include "general/exception/UpdateException.h"
#include "general/time/TimePiece.h"
#include "general/math/SimpleMath.h"

//------------------------------------------------------------------------------
//
//  Method Name: ValueTimelineElement::ValueTimelineElement
//
//  Purpose:
//    Constructor for the ValueTimelineElement class.
//
//  Calling Method:
//     ValueTimelineElement();
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

ValueTimelineElement::ValueTimelineElement()
    : CollectionElement(), crisisLevel_a(0), ptrSensorNumber_a(NULL),
      ptrMissionNumber_a(NULL), ptrSubPriorityLevel_a(NULL),
      overflownRegion_a(0.0), ptrImagedRegion_a(NULL), ptrCollectionTime_a(NULL),
      ptrMaxPossibleBitsCollected_a(NULL)
{
    return;
}

ValueTimelineElement::ValueTimelineElement(double overRegion,
        int numberOfResources,
        LinkList<RegionData> *ptrValidRegionDataList,
        double score,
        RegionData *ptrMVRegionData,
        int crisisLevel,
        Array<double> *ptrImagedRegion,
        Array<int> *ptrSensorNumber,
        Array<int> *ptrMissionNumber,
        Array<double> *ptrCollectionTime,
        Array<double> *ptrMaxPossibleBitsCollected,
        Array<int> *ptrSubPriorityLevel,
        Array<double> *ptrResourceFactor)
//RAGUSA 1/30/2018 - Collection Element inherits crisisLevel from timelineElement now
    : CollectionElement(score, ptrValidRegionDataList, ptrMVRegionData,crisisLevel,ptrResourceFactor),
      crisisLevel_a(crisisLevel), ptrSensorNumber_a(NULL),
      ptrMissionNumber_a(NULL), ptrSubPriorityLevel_a(NULL),
      overflownRegion_a(overRegion), ptrImagedRegion_a(NULL), ptrCollectionTime_a(NULL),
      ptrMaxPossibleBitsCollected_a(NULL)
{
    if (ptrSensorNumber == NULL) {
        ptrSensorNumber_a = new Array<int>(numberOfResources, 0);
    } else {
        ptrSensorNumber_a = new Array<int>(*ptrSensorNumber);
    }

    if (ptrMissionNumber == NULL) {
        ptrMissionNumber_a = new Array<int>(numberOfResources, 0);
    } else {
        ptrMissionNumber_a = new Array<int>(*ptrMissionNumber);
    }

    if (ptrSubPriorityLevel == NULL) {
        ptrSubPriorityLevel_a = new Array<int>(numberOfResources, 0);
    } else {
        ptrSubPriorityLevel_a = new Array<int>(*ptrSubPriorityLevel);
    }

    if (ptrImagedRegion == NULL) {
        ptrImagedRegion_a = new Array<double>(numberOfResources, 0.00);
    } else {
        ptrImagedRegion_a = new Array<double>(*ptrImagedRegion);
    }

    if (ptrCollectionTime == NULL) {
        ptrCollectionTime_a = new Array<double>(numberOfResources, 0.0);
    } else {
        ptrCollectionTime_a = new Array<double>(*ptrCollectionTime);
    }

    if (ptrMaxPossibleBitsCollected == NULL) {
        ptrMaxPossibleBitsCollected_a = new Array<double>(numberOfResources, 0.0);
    } else {
        ptrMaxPossibleBitsCollected_a = new Array<double>(*ptrMaxPossibleBitsCollected);
    }

    return;
}

ValueTimelineElement::ValueTimelineElement(const ValueTimelineElement &copyMe)
    : CollectionElement(copyMe),
      ptrSensorNumber_a(new Array<int>(*(copyMe.ptrSensorNumber_a))),
      ptrMissionNumber_a(new Array<int>(*(copyMe.ptrMissionNumber_a))),
      ptrSubPriorityLevel_a(new Array<int>(*(copyMe.ptrSubPriorityLevel_a))),
      overflownRegion_a(copyMe.overflownRegion_a),
      ptrImagedRegion_a(new Array<double>(*(copyMe.ptrImagedRegion_a))),
      ptrCollectionTime_a(new Array<double>(*(copyMe.ptrCollectionTime_a))),
      ptrMaxPossibleBitsCollected_a(new Array<double>(*(copyMe.ptrMaxPossibleBitsCollected_a)))
{
    return;
}

ValueTimelineElement&
ValueTimelineElement::operator = (const ValueTimelineElement &copyMe)
{
    return(*this);
}

bool
ValueTimelineElement::operator == (const ValueTimelineElement &compareMe) const
{
    return( (CollectionElement::operator == (compareMe)) &&
            (*ptrSensorNumber_a == *compareMe.ptrSensorNumber_a) &&
            (*ptrMissionNumber_a == *compareMe.ptrMissionNumber_a) &&
            (*ptrSubPriorityLevel_a == *compareMe.ptrSubPriorityLevel_a) &&
            (overflownRegion_a == compareMe.overflownRegion_a) &&
            (*ptrImagedRegion_a == *compareMe.ptrImagedRegion_a) &&
            (*ptrCollectionTime_a == *compareMe.ptrCollectionTime_a) &&
            (*ptrMaxPossibleBitsCollected_a == *compareMe.ptrMaxPossibleBitsCollected_a) );
}

bool
ValueTimelineElement::operator != (const ValueTimelineElement &compareMe) const
{
    return (!(operator == (compareMe)));
}

int
ValueTimelineElement::GetCrisisLevel() const
{
    return(crisisLevel_a);
}

int
ValueTimelineElement::GetSensorNumber(int resourceIndex) const
{
    int sensorNumber = 0;

    try {
        if ((resourceIndex >= 0) && (resourceIndex < ptrSensorNumber_a->Size())) {
            sensorNumber = ptrSensorNumber_a->ElementAt(resourceIndex);
        } else {
            int index = 0;
            sensorNumber = ptrSensorNumber_a->ElementAt(index);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetSensorNumber(int)");
        throw;
    }

    return(sensorNumber);
}

int
ValueTimelineElement::GetHighestScoreSensorNumber() const
{
    int      highestScoreSensorNumber = 0;
    int       numberOfResources = ptrSensorNumber_a->Size();
    const int index = 0;

    if (numberOfResources == 1) {
        highestScoreSensorNumber = ptrSensorNumber_a->ElementAt(index);
    } else {
        int resourceIndex = 0;
        double highestScore = -1.0;

        while (resourceIndex < numberOfResources) {
            double score = 0.0;
            score = GetScore(resourceIndex);

            if (score > highestScore) {
                highestScore = score;
                highestScoreSensorNumber = ptrSensorNumber_a->ElementAt(resourceIndex);
            }

            ++resourceIndex;
        }
    }

    return(highestScoreSensorNumber);
}

int
ValueTimelineElement::GetMissionNumber(int resourceIndex) const
{
    int targetType = 0;

    try {
        if ((resourceIndex >= 0) && (resourceIndex < ptrMissionNumber_a->Size())) {
            targetType = ptrMissionNumber_a->ElementAt(resourceIndex);
        } else {
            int index = 0;
            targetType = ptrMissionNumber_a->ElementAt(index);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetMissionNumber(int)");
        throw;
    }

    return(targetType);
}

int
ValueTimelineElement::GetSubPriorityLevel(int resourceIndex) const
{
    int subPriorityLevel = 0;

    try {
        if ((resourceIndex >= 0) && (resourceIndex < ptrSubPriorityLevel_a->Size())) {
            subPriorityLevel = ptrSubPriorityLevel_a->ElementAt(resourceIndex);
        } else {
            int index = 0;
            subPriorityLevel = ptrSubPriorityLevel_a->ElementAt(index);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetSubPriorityLevel(int)");
        throw;
    }

    return(subPriorityLevel);
}

double
ValueTimelineElement::GetCollectionTime(int resourceIndex) const
{
    double collectionTime = 0.0;

    try {
        if ((resourceIndex >= 0) && (resourceIndex < ptrCollectionTime_a->Size())) {
            collectionTime = ptrCollectionTime_a->ElementAt(resourceIndex);
        } else {
            int index = 0;
            collectionTime = ptrCollectionTime_a->ElementAt(index);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetCollectionTime(int)");
        throw;
    }

    return(collectionTime);
}

double
ValueTimelineElement::GetMaximumCollectionTime() const
{
    return(SimpleMath::Maximum(ptrCollectionTime_a));
}

double
ValueTimelineElement::GetMaxPossibleBitsCollected(int resourceIndex) const
{
    double maxPossibleBitsCollected = 0.0;

    try {
        if ((resourceIndex >= 0) && (resourceIndex < ptrMaxPossibleBitsCollected_a->Size())) {
            maxPossibleBitsCollected
                = ptrMaxPossibleBitsCollected_a->ElementAt(resourceIndex);
        } else {
            int index = 0;
            maxPossibleBitsCollected
                = ptrMaxPossibleBitsCollected_a->ElementAt(index);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetMaxPossibleBitsCollected(int)");
        throw;
    }

    return(maxPossibleBitsCollected);
}

double
ValueTimelineElement::GetMaximumMaxPossibleBitsCollected() const
{
    return(SimpleMath::Maximum(ptrMaxPossibleBitsCollected_a));
}

double
ValueTimelineElement::GetImagedRegion(int resourceIndex) const
{
    double imagedRegion = 0.0;

    try {
        if ((resourceIndex >= 0) && (resourceIndex < ptrImagedRegion_a->Size())) {
            imagedRegion = ptrImagedRegion_a->ElementAt(resourceIndex);
        } else {
            int index = 0;
            imagedRegion = ptrImagedRegion_a->ElementAt(index);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetImagedRegion(int)");
        throw;
    }

    return(imagedRegion);
}

ValueTimelineElement::~ValueTimelineElement()
{
    if (ptrImagedRegion_a != NULL) {
        delete ptrImagedRegion_a;
        ptrImagedRegion_a = NULL;
    }

    if (ptrSensorNumber_a != NULL) {
        delete ptrSensorNumber_a;
        ptrSensorNumber_a = NULL;
    }

    if (ptrMissionNumber_a != NULL) {
        delete ptrMissionNumber_a;
        ptrMissionNumber_a = NULL;
    }

    if (ptrCollectionTime_a != NULL) {
        delete ptrCollectionTime_a;
        ptrCollectionTime_a = NULL;
    }

    if (ptrMaxPossibleBitsCollected_a != NULL) {
        delete ptrMaxPossibleBitsCollected_a;
        ptrMaxPossibleBitsCollected_a = NULL;
    }

    if (ptrSubPriorityLevel_a != NULL) {
        delete ptrSubPriorityLevel_a;
        ptrSubPriorityLevel_a = NULL;
    }

    // the VTE is the class that contains the original RegionData
    // instances, so delete them
    RemoveAllRegionData();
    return;
}
