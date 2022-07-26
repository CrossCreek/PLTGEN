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
 * Requirement.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include <algorithm>

#include "Requirement.h"


#include "mural/modules/ts/Target.h"


#include "general/time/TimePiece.h"
#include "mural/sensor/Sensor.h"
#include "general/math/SimpleMath.h"
#include "general/utility/StringEditor.h"

int Requirement::numberOfRequirements_s = 0;

//------------------------------------------------------------------------------
//
//  Method Name: Requirement::Requirement
//
//  Purpose:
//    Default constructor for the Requirement class.
//
//  Calling Method:
//     Requirement();
//
//  Calling Parameters
//  Parameters      i/o/r  description
//  ----------      -----  -----------
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

Requirement::Requirement()
    : missionIndex_a(-1), qualityIndex_a(-1), priorityIndex_a(-1),
      subPriorityLevel_a(0), numberOfPoints_a(0),
      requirementIndex_a(numberOfRequirements_s), score_a(0.0), area_a(0.0),
      ptrRemainingScore_a(NULL), ptrRemainingArea_a(NULL)
{
    ++numberOfRequirements_s;
    return;
}

//------------------------------------------------------------------------------
//
//  Method Name: Requirement::Requirement
//
//  Purpose:
//    Alternate constructor for the Requirement class.
//
//  Calling Method:
//     Requirement();
//
//  Calling Parameters
//  Parameters      i/o/r  description
//  ----------      -----  -----------
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

Requirement::Requirement(int sensorIndex,
                         int missionIndex,
                         int qualityIndex,
                         int priorityIndex,
                         int subPriority,
                         double score,
                         double area,
                         int numberOfPoints,
                         int numberOfResources)
    : missionIndex_a(missionIndex), qualityIndex_a(qualityIndex),
      priorityIndex_a(priorityIndex),
      subPriorityLevel_a(subPriority), numberOfPoints_a(numberOfPoints),
      requirementIndex_a(numberOfRequirements_s),
      score_a(score), area_a(area),
      ptrRemainingScore_a(new Array<double>(numberOfResources, score)),
      ptrRemainingArea_a(new Array<double>(numberOfResources, area))
{
    ++numberOfRequirements_s;
    return;
}

//------------------------------------------------------------------------------
//
//  Method Name: Requirement::Requirement
//
//  Purpose:
//    Copy constructor for the Requirement class.
//
//  Calling Method:
//     Requirement();
//
//  Calling Parameters
//  Parameters      i/o/r  description
//  ----------      -----  -----------
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

Requirement::Requirement(const Requirement &copyMe)
    : missionIndex_a(copyMe.missionIndex_a), qualityIndex_a(copyMe.qualityIndex_a),
      priorityIndex_a(copyMe.priorityIndex_a),
      subPriorityLevel_a(copyMe.subPriorityLevel_a),
      numberOfPoints_a(copyMe.numberOfPoints_a),
      requirementIndex_a(copyMe.requirementIndex_a),
      score_a(copyMe.score_a), area_a(copyMe.area_a),
      ptrRemainingScore_a(new Array<double>(*(copyMe.ptrRemainingScore_a))),
      ptrRemainingArea_a(new Array<double>(*(copyMe.ptrRemainingArea_a)))
{
    return;
}

Requirement&
Requirement::operator = (const Requirement &copyMe)
{
    missionIndex_a        = copyMe.missionIndex_a;
    qualityIndex_a        = copyMe.qualityIndex_a;
    priorityIndex_a       = copyMe.priorityIndex_a;
    subPriorityLevel_a    = copyMe.subPriorityLevel_a;
    numberOfPoints_a      = copyMe.numberOfPoints_a;
    requirementIndex_a    = copyMe.requirementIndex_a;
    score_a               = copyMe.score_a;
    area_a                = copyMe.area_a;
    ptrRemainingScore_a   = copyMe.ptrRemainingScore_a;
    ptrRemainingArea_a    = copyMe.ptrRemainingArea_a;
    return(*this);
}

bool
Requirement::operator == (const Requirement &compareMe) const
{
    return( (missionIndex_a == compareMe.missionIndex_a) &&
            (qualityIndex_a == compareMe.qualityIndex_a) &&
            (priorityIndex_a == compareMe.priorityIndex_a) &&
            (subPriorityLevel_a == compareMe.subPriorityLevel_a) &&
            (numberOfPoints_a == compareMe.numberOfPoints_a) &&
            (score_a == compareMe.score_a) &&
            (area_a == compareMe.area_a) );
}

bool
Requirement::operator != (const Requirement &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
Requirement::PrintRequirement(int sensorIndex,
                              int regionNumber,
                              int subRegionNumber,
                              ofstream &outfile) const
{
    outfile << " " << setw(4) << regionNumber;
    outfile << " " << setw(3) << subRegionNumber;
    outfile << " " << setw(4) << GetMissionNumber();
    outfile << " " << setw(5) << GetQualityLevel(sensorIndex);
    outfile << " " << setw(5) << GetPriorityLevel();
    outfile << " " << setw(4) << subPriorityLevel_a;
    outfile << " " << setw(16) << setprecision(8) << score_a;
    outfile << " " << setw(18) << setprecision(8) << area_a;
    outfile << " " << setw(5) << numberOfPoints_a << NEW_LINE;
    return;
}

void
Requirement::AddScoreAndArea(double score,
                             double area,
                             int priorityIndex,
                             int subPriority)
{
    score_a += score;
    area_a += area;

    if (numberOfPoints_a > 0) {
        ++numberOfPoints_a;
    }

    subPriorityLevel_a = std::min(subPriorityLevel_a, subPriority);
    ResetScoreAndArea();
    return;
}

void
Requirement::ResetScoreAndArea()
{
    ptrRemainingScore_a->ResetValues(score_a);
    ptrRemainingArea_a->ResetValues(area_a);
    return;
}

double
Requirement::GetScore(const Sensor *ptrSensor) const
{
    return(score_a * GetResourceWeight(ptrSensor));
}

double
Requirement::GetTimeToCollect(const Sensor *ptrSensor) const
{
    return(area_a / GetCollectionRate(ptrSensor));
}

int
Requirement::GetRemainingNumberOfPoints(int resourceIndex) const
{
    return((int)((GetRemainingArea(resourceIndex) / area_a) * (double)numberOfPoints_a));
}

double
Requirement::GetRemainingScore(const Sensor *ptrSensor,
                               int resourceIndex) const
{
    double remainingScore = 0.0;

    if (resourceIndex < 0) {
        remainingScore = SimpleMath::Minimum(ptrRemainingScore_a);
    } else {
        remainingScore = ptrRemainingScore_a->ElementAt(resourceIndex);
    }

    return(remainingScore * GetResourceWeight(ptrSensor));
}

double
Requirement::GetRemainingArea(int resourceIndex) const
{
    double remainingArea = 0.0;

    if (resourceIndex < 0) {
        remainingArea = SimpleMath::Minimum(ptrRemainingArea_a);
    } else {
        remainingArea = ptrRemainingArea_a->ElementAt(resourceIndex);
    }

    return(remainingArea);
}

double
Requirement::GetRemainingScoreEfficiency(const Sensor *ptrSensor,
        int resourceIndex) const
{
    double       remainingScoreEfficiency = 0.0;
    const double remainingTTC = GetRemainingTimeToCollect(ptrSensor, resourceIndex);

    if (remainingTTC > 0.0) {
        remainingScoreEfficiency = GetRemainingScore(ptrSensor, resourceIndex)
                                   / remainingTTC;
    }

    return(remainingScoreEfficiency);
}

double
Requirement::GetRemainingTimeToCollect(const Sensor *ptrSensor,
                                       int resourceIndex) const
{
    return(GetRemainingArea(resourceIndex) / GetCollectionRate(ptrSensor));
}

double
Requirement::GetAchievableCollectionTime(const Sensor *ptrSensor,
        double maximumSeconds,
        int resourceIndex) const
{
    double achievableCollectionTime = 0.0;

    if (numberOfPoints_a > 0) {
        int numberOfPoints = 0;
        int remainingPoints = 0;
        double secondsPerPoint;
        remainingPoints = GetRemainingNumberOfPoints(resourceIndex);
        secondsPerPoint = GetSecondsPerPoint(ptrSensor);

        while ( ((achievableCollectionTime + secondsPerPoint) <= maximumSeconds) &&
                (numberOfPoints < remainingPoints) ) {
            achievableCollectionTime += secondsPerPoint;
            ++numberOfPoints;
        }
    } else {
        achievableCollectionTime = std::min(maximumSeconds,
                                            GetRemainingTimeToCollect(ptrSensor, resourceIndex));
    }

    return(achievableCollectionTime);
}

int
Requirement::GetAchievableNumberOfPoints(const Sensor *ptrSensor,
        double maximumSeconds,
        int resourceIndex) const
{
    int numberOfPoints = 0;

    if (numberOfPoints_a > 0) {
        int remainingPoints = 0;
        double collectionTime = 0.0;
        double secondsPerPoint;
        remainingPoints = GetRemainingNumberOfPoints(resourceIndex);
        secondsPerPoint = GetSecondsPerPoint(ptrSensor);

        while ( ((collectionTime + secondsPerPoint) <= maximumSeconds) &&
                (numberOfPoints < remainingPoints) ) {
            collectionTime += secondsPerPoint;
            ++numberOfPoints;
        }
    }

    return(numberOfPoints);
}

double
Requirement::GetAchievableArea(const Sensor *ptrSensor,
                               double maximumSeconds,
                               int resourceIndex) const
{
    double achievableArea = 0.0;

    if (numberOfPoints_a > 0) {
        int achievablePoints = 0;
        achievablePoints = GetAchievableNumberOfPoints(ptrSensor, maximumSeconds, resourceIndex);
        achievableArea = ((double)achievablePoints * area_a) / (double)numberOfPoints_a;
    } else {
        double collectionTime = 0.0;
        achievableArea = GetRemainingArea(resourceIndex);
        collectionTime = GetRemainingTimeToCollect(ptrSensor, resourceIndex);

        if (collectionTime > maximumSeconds) {
            achievableArea *= (maximumSeconds / collectionTime);
        }
    }

    return(achievableArea);
}

double
Requirement::GetAchievableScore(const Sensor *ptrSensor,
                                double maximumSeconds,
                                int resourceIndex) const
{
    double achievableScore = 0.0;

    if (numberOfPoints_a > 0) {
        int achievablePoints = 0;
        achievablePoints = GetAchievableNumberOfPoints(ptrSensor, maximumSeconds, resourceIndex);
        // the achievable number of points can not exceed the number of remaining points
        achievablePoints = std::min(GetRemainingNumberOfPoints(resourceIndex),
                                    achievablePoints);

        if (achievablePoints > 0) {
            achievableScore = GetScore(ptrSensor);

            if (achievablePoints < numberOfPoints_a) {
                achievableScore *= ((double)achievablePoints / (double)numberOfPoints_a);
            }
        }
    } else {
        double remainingTTC = 0.0;
        achievableScore = GetRemainingScore(ptrSensor, resourceIndex);
        remainingTTC = GetRemainingTimeToCollect(ptrSensor, resourceIndex);

        if (remainingTTC > maximumSeconds) {
            // this normalizes the score based on the remaining collection time.
            // Ex: if remaining score is 1000.0, remaining time to collect is 80 seconds,
            //     but only have 60 seconds to collect ... acheivable score is 750.0
            achievableScore *= (maximumSeconds / remainingTTC);
        }
    }

    return(achievableScore);
}

double
Requirement::UpdateRemainingScoreAndArea(const Sensor *ptrSensor,
        double maximumSeconds,
        int resourceIndex)
{
    int  useResourceIndex = 0;
    double remainingScore = 0.0;
    double remainingArea = 0.0;
    double timeToCollect = 0.0;

    if (resourceIndex > 0) {
        useResourceIndex = resourceIndex;
    }

    if (numberOfPoints_a > 0) {
        int achievablePoints = 0;
        achievablePoints = GetAchievableNumberOfPoints(ptrSensor, maximumSeconds, resourceIndex);
        remainingScore = ptrRemainingScore_a->ElementAt(useResourceIndex)
                         - (((double)achievablePoints * score_a) / (double)numberOfPoints_a);
        remainingArea = ptrRemainingArea_a->ElementAt(useResourceIndex)
                        - (((double)achievablePoints * area_a) / (double)numberOfPoints_a);
        timeToCollect = (double)achievablePoints * GetSecondsPerPoint(ptrSensor);
    } else {
        timeToCollect = GetRemainingTimeToCollect(ptrSensor, resourceIndex);

        if (timeToCollect > maximumSeconds) {
            remainingScore = ptrRemainingScore_a->ElementAt(useResourceIndex)
                             * ((timeToCollect - maximumSeconds) / timeToCollect);
            remainingArea = ptrRemainingArea_a->ElementAt(useResourceIndex)
                            * ((timeToCollect - maximumSeconds) / timeToCollect);
            timeToCollect = maximumSeconds;
        }
    }

    (*ptrRemainingScore_a)[useResourceIndex] = remainingScore;
    (*ptrRemainingArea_a)[useResourceIndex] = remainingArea;
    return(timeToCollect);
}

double
Requirement::GetResourceWeight(const Sensor *ptrSensor) const
{
    return(ptrSensor->GetResourceWeight(missionIndex_a, qualityIndex_a));
}

double
Requirement::GetCollectionRate(const Sensor *ptrSensor) const
{
    return(ptrSensor->GetCollectionRate(missionIndex_a, qualityIndex_a));
}

double
Requirement::GetSecondsPerPoint(const Sensor *ptrSensor) const
{
    double secondsPerPoint = 0.0;

    if (numberOfPoints_a > 0) {
        secondsPerPoint = area_a / ((double)numberOfPoints_a * GetCollectionRate(ptrSensor));
    }

    return(secondsPerPoint);
}

int
Requirement::GetMissionNumber() const
{
    return(Target::GetMissionNumber(missionIndex_a));
}

int
Requirement::GetQualityLevel(int sensorIndex) const
{
    return(Target::GetQualityLevel(qualityIndex_a, sensorIndex));
}

int
Requirement::GetPriorityLevel() const
{
    return(Target::GetPriorityLevel(priorityIndex_a));
}

bool
Requirement::IsPointRequirement() const
{
    return(Target::IsPointTarget(missionIndex_a));
}

string
Requirement::GetMissionString() const
{
    return(Target::GetMissionString(missionIndex_a));
}

Requirement::~Requirement()
{
    if (ptrRemainingScore_a != NULL) {
        delete ptrRemainingScore_a;
        ptrRemainingScore_a = NULL;
    }

    if (ptrRemainingArea_a != NULL) {
        delete ptrRemainingArea_a;
        ptrRemainingArea_a = NULL;
    }

    --numberOfRequirements_s;
    return;
}

