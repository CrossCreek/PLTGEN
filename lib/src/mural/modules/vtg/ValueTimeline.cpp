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
 * ValueTimeline.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "ValueTimeline.h"
#include "CrisisData.h"
#include "mural/Region.h"
#include "ValueTimelineElement.h"

#include "general/time/TimePiece.h"

//----------------------------------------------------------------------
//
//  Method Name: ValueTimeline::ValueTimeline
//
//  Purpose:
//    Constructor for the ValueTimeline class.
//
//  Calling Method:
//     ValueTimeline();
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
//----------------------------------------------------------------------

#include "ValueTimeline.h"

ValueTimeline::ValueTimeline()
    : maximumTimelineScore_a(0.0), ptrTimelineElementArray_a(NULL)
{
    ptrTimelineElementArray_a = new
    ArrayOfPointers<ValueTimelineElement>(TimePiece::GetNumberOfTimeSteps());
    return;
}

ValueTimeline::ValueTimeline(const ValueTimeline &copyMe)
    : maximumTimelineScore_a(copyMe.maximumTimelineScore_a),
      ptrTimelineElementArray_a(NULL)
{
    ptrTimelineElementArray_a = new
    ArrayOfPointers<ValueTimelineElement>(*(copyMe.ptrTimelineElementArray_a));
    return;
}

ValueTimeline&
ValueTimeline::operator = (const ValueTimeline &copyMe)
{
    maximumTimelineScore_a = copyMe.maximumTimelineScore_a;
    ptrTimelineElementArray_a = new
    ArrayOfPointers<ValueTimelineElement>(*(copyMe.ptrTimelineElementArray_a));
    return(*this);
}

bool
ValueTimeline::operator == (const ValueTimeline &compareMe) const
{
    return( (maximumTimelineScore_a == compareMe.maximumTimelineScore_a) &&
            (*ptrTimelineElementArray_a == *(compareMe.ptrTimelineElementArray_a)) );
}

bool
ValueTimeline::operator != (const ValueTimeline &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
ValueTimeline::AddValueTimelineElement(int timeIndex,
                                       ValueTimelineElement *ptrNewElement)
{
    ptrTimelineElementArray_a->AddElement(timeIndex, ptrNewElement);
    return;
}

void
ValueTimeline::DetermineMaximumTimelineScore()
{
    int       timeIndex = 0;
    const int numberOfTimeSteps = ptrTimelineElementArray_a->Size();

    while (timeIndex < numberOfTimeSteps) {
        double timelineScore = 0.0;
        timelineScore = GetScore(timeIndex);

        if (timelineScore > maximumTimelineScore_a) {
            maximumTimelineScore_a = timelineScore;
        }

        ++timeIndex;
    }

    return;
}

ValueTimelineElement*
ValueTimeline::GetValueTimelineElement(int timeIndex) const
{
    ValueTimelineElement *ptrElement = NULL;

    try {
        ptrElement = (*ptrTimelineElementArray_a)[timeIndex];
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetValueTimelineElement(int)");
        throw;
    }

    return(ptrElement);
}

void
ValueTimeline::SetScore(int timeIndex,
                        double newScore)
{
    try {
        GetValueTimelineElement(timeIndex)->SetScore(newScore);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "SetScore(int, const double&)");
        throw;
    }

    return;
}

double
ValueTimeline::GetImagedRegion(int timeIndex,
                               int resourceIndex) const
{
    double imagedRegion = 0.0;

    try {
        imagedRegion = GetValueTimelineElement(timeIndex)->GetImagedRegion(resourceIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetImagedRegion(int, int int&)");
        throw;
    }

    return(imagedRegion);
}

double
ValueTimeline::GetOverflownRegion(int timeIndex) const
{
    double overflownRegion = 0;

    try {
        overflownRegion = GetValueTimelineElement(timeIndex)->GetOverflownRegion();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetOverflownRegion(int)");
        throw;
    }

    return(overflownRegion);
}

int
ValueTimeline::GetSensorNumber(int timeIndex,
                               int resourceIndex) const
{
    int sensorNumber = 0;

    try {
        sensorNumber = GetValueTimelineElement(timeIndex)->GetSensorNumber(resourceIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetSensorNumber(int, int)");
        throw;
    }

    return(sensorNumber);
}

int
ValueTimeline::GetHighestScoreSensorNumber(int timeIndex) const
{
    int sensorNumber = 0;

    try {
        sensorNumber
            = GetValueTimelineElement(timeIndex)->GetHighestScoreSensorNumber();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetHighestScoreSensorNumber(int)");
        throw;
    }

    return(sensorNumber);
}

int
ValueTimeline::GetMissionNumber(int timeIndex,
                                int resourceIndex) const
{
    int missionNumber = 0;

    try {
        missionNumber = GetValueTimelineElement(timeIndex)->GetMissionNumber(resourceIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetMissionNumber(int, int)");
        throw;
    }

    return(missionNumber);
}

int
ValueTimeline::GetCrisisLevel(int timeIndex) const
{
    int crisisLevel = 0;

    try {
        crisisLevel = GetValueTimelineElement(timeIndex)->GetCrisisLevel();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetCrisisLevel(int)");
        throw;
    }

    return(crisisLevel);
}

int
ValueTimeline::GetSubPriorityLevel(int timeIndex,
                                   int resourceIndex) const
{
    int subPriorityLevel = 0;

    try {
        subPriorityLevel
            = GetValueTimelineElement(timeIndex)->GetSubPriorityLevel(resourceIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetSubPriorityLevel(int, int)");
        throw;
    }

    return(subPriorityLevel);
}

double
ValueTimeline::GetScore(int timeIndex,
                        int resourceIndex) const
{
    double score = 0.0;

    try {
        score = GetValueTimelineElement(timeIndex)->GetScore(resourceIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetScore(int, int)");
        throw;
    }

    return(score);
}

double
ValueTimeline::GetObjectiveValueScore(int timeIndex,
                                      const string &userDesignator,
                                      int resourceIndex) const
{
    int              crisisLevel = 0;
    double           objectiveValueScore;
    ValueTimelineElement *ptrElement = GetValueTimelineElement(timeIndex);
    objectiveValueScore = ptrElement->GetScore(resourceIndex);
    crisisLevel = ptrElement->GetCrisisLevel();

    if (crisisLevel > 0) {
        objectiveValueScore += (maximumTimelineScore_a
                                * CrisisData::GetTimelineScoreMultiplier(crisisLevel, userDesignator));
    }
    return(objectiveValueScore);
}

double
ValueTimeline::GetMaxPossibleBitsCollected(int timeIndex,
        int resourceIndex) const
{
    double bitsCollected = 0.0;

    try {
        bitsCollected
            = GetValueTimelineElement(timeIndex)->GetMaxPossibleBitsCollected(resourceIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetMaxPossibleBitsCollected(int, int)");
        throw;
    }

    return(bitsCollected);
}

double
ValueTimeline::GetMaximumMaxPossibleBitsCollected(int timeIndex) const
{
    double maxPossibleBitsCollected = 0.0;

    try {
        maxPossibleBitsCollected
            = GetValueTimelineElement(timeIndex)->GetMaximumMaxPossibleBitsCollected();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetMaximumMaxPossibleBitsCollected(int)");
        throw;
    }

    return(maxPossibleBitsCollected);
}

double
ValueTimeline::GetCollectionTime(int timeIndex,
                                 int resourceIndex) const
{
    double collectionTime = 0.0;

    try {
        collectionTime = GetValueTimelineElement(timeIndex)->GetCollectionTime(resourceIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetCollectionTime(int, int)");
        throw;
    }

    return(collectionTime);
}

double
ValueTimeline::GetMaximumCollectionTime(int timeIndex) const
{
    double maxCollectionTime = 0.0;

    try {
        maxCollectionTime = GetValueTimelineElement(timeIndex)->GetMaximumCollectionTime();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetMaximumCollectionTime(int)");
        throw;
    }

    return(maxCollectionTime);
}

double
ValueTimeline::GetResourceFactor(int timeIndex,
                                 int resourceIndex) const
{
    double resourceFactor = 0.0;

    try {
        resourceFactor = GetValueTimelineElement(timeIndex)->GetResourceFactor(resourceIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetResourceFactor(int, int)");
        throw;
    }

    return(resourceFactor);
}

RegionData*
ValueTimeline::GetMostValuableRegionData(int timeIndex) const
{
    RegionData *ptrMVRegionData = NULL;

    try {
        ptrMVRegionData = GetValueTimelineElement(timeIndex)->GetMostValuableRegionData();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetMostValuableRegionData(int)");
        throw;
    }

    return(ptrMVRegionData);
}

LinkList<RegionData>*
ValueTimeline::GetRegionDataList(int timeIndex) const
{
    LinkList<RegionData> *ptrReturnList = NULL;

    try {
        ptrReturnList = GetValueTimelineElement(timeIndex)->GetRegionDataList();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetRegionDataList(int)");
        throw;
    }

    return(ptrReturnList);
}

void
ValueTimeline::RemoveRegionData(int timeIndex)
{
    try {
        GetValueTimelineElement(timeIndex)->RemoveAllRegionData();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "RemoveRegionData(int)");
        throw;
    }
}

ValueTimeline::~ValueTimeline()
{
    if (ptrTimelineElementArray_a != NULL) {
        ptrTimelineElementArray_a->Delete();
        delete ptrTimelineElementArray_a;
        ptrTimelineElementArray_a = NULL;
    }

    return;
}
