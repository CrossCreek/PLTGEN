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
 * TargetRegion.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "TargetRegion.h"
#include "mural/Region.h"
#include "Target.h"
#include "mural/sensor/Sensor.h"
#include "general/math/SpaceVector.h"

TargetRegion::TargetRegion()
    : targetArea_a(0.0), ptrCollectedArea_a(NULL), ptrRegion_a(NULL),
      ptrTarget_a(NULL)
{
    return;
}

TargetRegion::TargetRegion(double area,
                           int numberOfSensors,
                           Region *ptrRegion,
                           Target *ptrTarget,
                           const bool &scheduleTargets)
    : targetArea_a(area), ptrCollectedArea_a(NULL),
      ptrRegion_a(ptrRegion), ptrTarget_a(ptrTarget)
{
    // if scheduling targets, add this target region to the
    // corresponding (geographical) region
    if (scheduleTargets == true) {
        ptrCollectedArea_a = new Array<double>(numberOfSensors, 0.0);
    }

    return;
}

void
TargetRegion::SetHighestQuality(int sensorIndex,
                                int quality)
{
    ptrTarget_a->SetAchievableQuality(sensorIndex, quality);
    return;
}

void
TargetRegion::UpdateCollectedArea(int area,
                                  int sensorIndex)
{
    (*ptrCollectedArea_a)[sensorIndex] += area;
    ptrTarget_a->SetCollectSensorIndex(sensorIndex);
    return;
}

bool
TargetRegion::CanCollectWithSensor(int sensorIndex) const
{
    return(ptrTarget_a->CanCollectWithSensor(sensorIndex));
}

bool
TargetRegion::IsPointTarget() const
{
    return(ptrTarget_a->IsPointTarget());
}

bool
TargetRegion::IsAreaTarget() const
{
    return(ptrTarget_a->IsAreaTarget());
}

int
TargetRegion::GetQualityLevel(int sensorIndex) const
{
    return(ptrTarget_a->GetAchievableQuality(sensorIndex));
}

int
TargetRegion::GetRequiredQuality(int sensorIndex) const
{
    return(ptrTarget_a->GetRequiredQuality(sensorIndex));
}

int
TargetRegion::GetDeckIndex() const
{
    return(ptrTarget_a->GetDeckIndex());
}

int
TargetRegion::GetMissionIndex() const
{
    return(ptrTarget_a->GetMissionIndex());
}

int
TargetRegion::GetMissionIndex(int sensorIndex) const
{
    return(ptrTarget_a->GetMissionIndex(sensorIndex));
}

int
TargetRegion::GetQualityIndex(int sensorIndex) const
{
    return(ptrTarget_a->GetQualityIndex(sensorIndex));
}

int
TargetRegion::GetModeIndex(int sensorIndex) const
{
    return(ptrTarget_a->GetModeIndex(sensorIndex));
}

int
TargetRegion::GetPriorityIndex(int sensorIndex) const
{
    return(ptrTarget_a->GetPriorityIndex(sensorIndex));
}

int
TargetRegion::GetNumberOfCPSs() const
{
    return(ptrTarget_a->GetNumberOfCPSs());
}

int
TargetRegion::GetScoreValueForSort() const
{
    return(ptrTarget_a->GetScoreValueForSort());
}

double
TargetRegion::GetScoreEfficiency(const Sensor *ptrSensor,
                                 double remainingTTC) const
{
    const int  sensorIndex = ptrSensor->GetSensorIndex();
    const int  missionIndex = GetMissionIndex(sensorIndex);
    const int  qualityIndex = GetQualityIndex(sensorIndex);
    double       scoreEfficiency = 0.0;
    const double collectionRate = ptrSensor->GetCollectionRate(missionIndex, qualityIndex);

    if (collectionRate > 0.0) {
        double timeToCollect = 0.0;
        timeToCollect = GetRemainingArea(sensorIndex) / collectionRate;

        if (timeToCollect > 0.0) {
            timeToCollect = std::min(timeToCollect, remainingTTC);
            scoreEfficiency = (GetSensorScore(sensorIndex)
                               * ptrSensor->GetResourceWeight(missionIndex, qualityIndex))
                              / timeToCollect;
        }
    }

    return(scoreEfficiency);
}

double
TargetRegion::GetSensorScore(int sensorIndex) const
{
    return(ptrTarget_a->GetSensorScore(sensorIndex));
}

double
TargetRegion::GetRegionNumber() const
{
    return(ptrRegion_a->GetFullRegionNumber());
}

double
TargetRegion::GetRemainingArea(int sensorIndex) const
{
    double remainingArea = 0.0;
    double targetRegionArea = targetArea_a;

    if (ptrTarget_a->CanCollectWithSensor(sensorIndex) == true) {
        if (ptrTarget_a->IsPointTarget() == true) {
            targetRegionArea = 1.0;
        }

        if (ptrTarget_a->IsStereoMode(sensorIndex) == true) {
            targetRegionArea *= 2.0;
        }

        remainingArea = targetRegionArea - ptrCollectedArea_a->GetSum();
    }

    return(remainingArea);
}

SpaceVector
TargetRegion::GetPosition(int timeIndex) const
{
    return(ptrRegion_a->GetRegionCenter(timeIndex));
}

TargetRegion::~TargetRegion()
{
    if (ptrCollectedArea_a != NULL) {
        delete ptrCollectedArea_a;
        ptrCollectedArea_a = NULL;
    }

    ptrRegion_a = NULL;
    ptrTarget_a = NULL;
    return;
}
