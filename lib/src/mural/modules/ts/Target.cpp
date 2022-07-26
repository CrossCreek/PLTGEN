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
 * Target.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "Target.h"
#include "mural/Earth.h"
#include "mural/sensor/SpectralSensor.h"

#include "CollectionProblemSet.h"
#include "TargetRegion.h"

#include "general/exception/InputException.h"
#include "general/utility/StringEditor.h"

const int  Target::NUMBER_OF_QUALITIES_s = 9;
const int  Target::NUMBER_OF_PRIORITIES_s = 18;
const double  Target::SCORE_MULTIPLIER_s = 100000.0;
const char   Target::MONO_MODE_IDENTIFIER_s = 'M';
const char   Target::STEREO_MODE_IDENTIFIER_s = 'S';

Target::Target()
    : targetID_a("UNKNOWN"), missionNumber_a(0),
      deckIndex_a(-1), collectSensorIndex_a(-1),
      ptrAchievableQuality_a(NULL), ptrBestAchievableCPS_a(NULL),
      ptrTargetRegionList_a(NULL), ptrCollectionProblemSetList_a(NULL)
{
    return;
}

Target::Target(const string &targetID,
               int missionNumber,
               int deckIndex,
               const string &fcpsID,
               const Array<double> *ptrQuality,
               const Array<int> *ptrPriority,
               const Array<char> *ptrPreference,
               const Array<char> *ptrMode,
               const Array<double> *ptrSensorScore,
               const Array<double> *ptrRegionNumber,
               const Array<double> *ptrRegionArea,
               const bool &scheduleTargets)
    : targetID_a(targetID), missionNumber_a(missionNumber),
      deckIndex_a(deckIndex), collectSensorIndex_a(-1),
      ptrAchievableQuality_a(NULL), ptrBestAchievableCPS_a(NULL),
      ptrTargetRegionList_a(new LinkList<TargetRegion>()),
      ptrCollectionProblemSetList_a(NULL)
{
    int          regionIndex = 0;
    const int    numberOfRegions = ptrRegionNumber->Size();
    const int    numberOfSensors = ptrQuality->Size();
    Region       *ptrRegion = NULL;
    TargetRegion *ptrTargetRegion = NULL;

    if (scheduleTargets == true) {
        ptrAchievableQuality_a = new Array<int>(numberOfSensors, 0);
    }

    AddCollectionProblemSet(fcpsID, ptrQuality, ptrPriority,
                            ptrPreference, ptrMode, ptrSensorScore);

    while (regionIndex < numberOfRegions) {
        double regionNumber;
        regionNumber = ptrRegionNumber->ElementAt(regionIndex);
        ptrRegion = Earth::GetRegion(regionNumber);

        if (ptrRegion == NULL) {
            throw new InputException(GetClassName(),
                                     "Target(const string&, int, const string&, ...)",
                                     ("Couldn't find Region " + StringEditor::ConvertToString(regionNumber)));
        }

        ptrTargetRegion = new TargetRegion(ptrRegionArea->ElementAt(regionIndex),
                                           numberOfSensors, ptrRegion, this,
                                           scheduleTargets);
        ptrTargetRegionList_a->Append(ptrTargetRegion);
        ptrTargetRegion = NULL;
        ++regionIndex;
    }

    return;
}

void
Target::AddCollectionProblemSet(const string &fcpsID,
                                const Array<double> *ptrQuality,
                                const Array<int> *ptrPriority,
                                const Array<char> *ptrPreference,
                                const Array<char> *ptrMode,
                                const Array<double> *ptrSensorScore)
{
    CollectionProblemSet *ptrCPS = NULL;

    try {
        ptrCPS = new CollectionProblemSet(fcpsID, ptrQuality, ptrPriority, ptrPreference,
                                          ptrMode, ptrSensorScore, IsPointTarget());

        if (ptrCollectionProblemSetList_a == NULL) {
            ptrCollectionProblemSetList_a = new LinkList<CollectionProblemSet>();
        }

        ptrCollectionProblemSetList_a->Append(ptrCPS);
        ptrCPS = NULL;
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "AddCollectionProblemSet(const string&, const Array<double>*, ...)");
        throw;
    }

    return;
}

void
Target::DetermineBestCollectionProblemSet()
{
    int                 qualityLevel = 0;
    int                  sensorIndex = 0;
    const int            numberOfSensors = ptrAchievableQuality_a->Size();
    double                 highestScore = 0.0;
    ListIterator<CollectionProblemSet> cpsListIter(ptrCollectionProblemSetList_a);
    CollectionProblemSet              *ptrCPS = cpsListIter.First();

    while (ptrCPS != NULL) {
        double cpsScore;
        cpsScore = 0.0;
        sensorIndex = 0;

        while (sensorIndex < numberOfSensors) {
            qualityLevel = ptrCPS->GetQualityLevel(sensorIndex);

            if ( (qualityLevel > 0) &&
                    (qualityLevel <= ptrAchievableQuality_a->ElementAt(sensorIndex)) ) {
                cpsScore += ptrCPS->GetScore(sensorIndex);
            }

            ++sensorIndex;
        }

        if (cpsScore > highestScore) {
            ptrBestAchievableCPS_a = ptrCPS;
            highestScore = cpsScore;
        }

        ptrCPS = cpsListIter.Next();
    }

    if (ptrBestAchievableCPS_a != NULL) {
        sensorIndex = 0;

        while (sensorIndex < numberOfSensors) {
            qualityLevel = ptrBestAchievableCPS_a->GetQualityLevel(sensorIndex);

            if ( (qualityLevel == 0) ||
                    (qualityLevel > ptrAchievableQuality_a->ElementAt(sensorIndex)) ) {
                (*ptrAchievableQuality_a)[sensorIndex] = SHORT_ZERO;
            }

            ++sensorIndex;
        }
    } else {
        ptrAchievableQuality_a->ResetValues(SHORT_ZERO);
    }

    return;
}

int
Target::GetQualityLevel(int qualityIndex,
                        int sensorIndex)
{
    int qualityLevel = 0;

    if (Sensor::IsSpectralSensor(sensorIndex) == false) {
        qualityLevel = (qualityIndex + 1) * 10;
    } else {
        qualityLevel = (int)(SpectralSensor::GetQualityBinValue((int)qualityIndex) * 100.0);
    }

    return(qualityLevel);
}

int
Target::GetCollectedPriority() const
{
    int collectedPriority = 0;

    if (ptrBestAchievableCPS_a != NULL) {
        if (collectSensorIndex_a >= 0) {
            collectedPriority = ptrBestAchievableCPS_a->GetPriority(collectSensorIndex_a);
        } else {
            int sensorIndex = 0;
            collectedPriority = SHRT_MAX;

            while (sensorIndex < ptrAchievableQuality_a->Size()) {
                if (ptrAchievableQuality_a->ElementAt(sensorIndex) > 0) {
                    collectedPriority = std::min(collectedPriority,
                                                 ptrBestAchievableCPS_a->GetPriority(sensorIndex));
                }

                ++sensorIndex;
            }
        }
    }

    return(collectedPriority);
}

int
Target::GetRequiredQuality(int sensorIndex) const
{
    int requiredQuality = 0;

    if (ptrBestAchievableCPS_a != NULL) {
        requiredQuality = ptrBestAchievableCPS_a->GetQualityLevel(sensorIndex);
    }

    return(requiredQuality);
}

int
Target::GetMissionIndex() const
{
    int missionIndex = ConvertMissionToIndex(missionNumber_a);

    if ((missionIndex == MCG_MONO_BOX) && (ptrBestAchievableCPS_a != NULL)) {
        if (ptrBestAchievableCPS_a->ContainsCollectionMode(STEREO_MODE_IDENTIFIER_s) == true) {
            missionIndex = MCG_STEREO_AREA;
        }
    }

    return(missionIndex);
}

int
Target::GetMissionIndex(int sensorIndex) const
{
    int missionIndex = ConvertMissionToIndex(missionNumber_a);

    if ((missionIndex == MCG_MONO_BOX) && (ptrBestAchievableCPS_a != NULL)) {
        if (ptrBestAchievableCPS_a->GetCollectionMode(sensorIndex) == STEREO_MODE_IDENTIFIER_s) {
            missionIndex = MCG_STEREO_AREA;
        }
    }

    return(missionIndex);
}

int
Target::GetModeIndex(int sensorIndex) const
{
    int modeIndex = -1;

    if (ptrBestAchievableCPS_a != NULL) {
        modeIndex = ptrBestAchievableCPS_a->GetModeIndex(sensorIndex);
    }

    return(modeIndex);
}

int
Target::GetQualityIndex(int sensorIndex) const
{
    int qualityIndex = -1;

    if (ptrBestAchievableCPS_a != NULL) {
        qualityIndex = ptrBestAchievableCPS_a->GetQualityIndex(sensorIndex);
    }

    return(qualityIndex);
}

int
Target::GetPriorityIndex(int sensorIndex) const
{
    int priorityIndex = -1;

    if (ptrBestAchievableCPS_a != NULL) {
        priorityIndex = ptrBestAchievableCPS_a->GetPriorityIndex(sensorIndex);
    }

    return(priorityIndex);
}

int
Target::GetScoreValueForSort(const Sensor *ptrSensor) const
{
    int        missionIndex = ConvertMissionToIndex(missionNumber_a);
    const int  sensorIndex = ptrSensor->GetSensorIndex();
    double       sensorScore = 0.0;
    double       resourceWeight = 0.0;

    if (ptrBestAchievableCPS_a != NULL) {
        sensorScore = ptrBestAchievableCPS_a->GetScore(sensorIndex);

        if ( (missionIndex == MCG_MONO_BOX) &&
                (ptrBestAchievableCPS_a->ContainsCollectionMode(STEREO_MODE_IDENTIFIER_s) == true) ) {
            missionIndex = MCG_STEREO_AREA;
        }

        resourceWeight = ptrSensor->GetResourceWeight(missionIndex,
                         ptrBestAchievableCPS_a->GetQualityIndex(sensorIndex));
    }

    return((int)(sensorScore * resourceWeight * SCORE_MULTIPLIER_s));
}

double
Target::GetSensorScore(int sensorIndex) const
{
    double sensorScore = 0.0;

    if (ptrBestAchievableCPS_a != NULL) {
        sensorScore = ptrBestAchievableCPS_a->GetScore(sensorIndex);
    }

    return(sensorScore);
}

bool
Target::IsStereoMode(int sensorIndex) const
{
    bool stereoRequirement = false;

    if (ptrBestAchievableCPS_a != NULL) {
        if (ptrBestAchievableCPS_a->GetCollectionMode(sensorIndex) == STEREO_MODE_IDENTIFIER_s) {
            stereoRequirement = true;
        }
    }

    return(stereoRequirement);
}

bool
Target::IsCommonCountdown() const
{
    bool commonCountdown = false;

    if (ptrBestAchievableCPS_a != NULL) {
        commonCountdown = ptrBestAchievableCPS_a->IsCommonCountdown();
    }

    return(commonCountdown);
}

double
Target::GetTotalTargetArea() const
{
    double                       totalTargetArea = 0.0;
    ListIterator<TargetRegion>  targetRegionListIter(ptrTargetRegionList_a);
    TargetRegion               *ptrTargetRegion = targetRegionListIter.First();

    while (ptrTargetRegion != NULL) {
        totalTargetArea += ptrTargetRegion->GetTargetArea();
        ptrTargetRegion = targetRegionListIter.Next();
    }

    if (ptrBestAchievableCPS_a != NULL) {
        if (collectSensorIndex_a >= 0) {
            if (ptrBestAchievableCPS_a->GetCollectionMode(collectSensorIndex_a) == STEREO_MODE_IDENTIFIER_s) {
                totalTargetArea *= 2.0;
            }
        } else if (ptrBestAchievableCPS_a->ContainsCollectionMode(STEREO_MODE_IDENTIFIER_s) == true) {
            totalTargetArea *= 2.0;
        }
    }

    return(totalTargetArea);
}

double
Target::GetTargetAreaCollected() const
{
    const int                 numberOfSensors = ptrAchievableQuality_a->Size();
    double                      targetAreaCollected = 0.0;
    ListIterator<TargetRegion> targetRegionListIter(ptrTargetRegionList_a);
    TargetRegion              *ptrTargetRegion = targetRegionListIter.First();

    while (ptrTargetRegion != NULL) {
        int sensorIndex = 0;
        sensorIndex = 0;

        while (sensorIndex < numberOfSensors) {
            targetAreaCollected += ptrTargetRegion->GetCollectedArea(sensorIndex);
            ++sensorIndex;
        }

        ptrTargetRegion = targetRegionListIter.Next();
    }

    return(targetAreaCollected);
}

double
Target::GetTargetAreaCollected(int sensorIndex) const
{
    double                      targetAreaCollected = 0.0;
    ListIterator<TargetRegion> targetRegionListIter(ptrTargetRegionList_a);
    TargetRegion              *ptrTargetRegion = targetRegionListIter.First();

    while (ptrTargetRegion != NULL) {
        targetAreaCollected += ptrTargetRegion->GetCollectedArea(sensorIndex);
        ptrTargetRegion = targetRegionListIter.Next();
    }

    return(targetAreaCollected);
}

double
Target::GetTotalScore() const
{
    double                              totalScore = 0.0;
    ListIterator<CollectionProblemSet> cpsListIter(ptrCollectionProblemSetList_a);
    CollectionProblemSet              *ptrCPS = cpsListIter.First();

    while (ptrCPS != NULL) {
        totalScore += ptrCPS->GetTotalScore();
        ptrCPS = cpsListIter.Next();
    }

    return(totalScore);
}

bool
Target::CanCollectWithSensor(int sensorIndex) const
{
    bool canCollectWithSensor = false;

    if ( (ptrBestAchievableCPS_a != NULL) &&
            (ptrAchievableQuality_a->ElementAt(sensorIndex) > 0) &&
            ((collectSensorIndex_a < 0) || (collectSensorIndex_a == sensorIndex)) ) {
        canCollectWithSensor = true;
    }

    return(canCollectWithSensor);
}

Target::~Target()
{
    if (ptrAchievableQuality_a != NULL) {
        delete ptrAchievableQuality_a;
        ptrAchievableQuality_a = NULL;
    }

    ptrBestAchievableCPS_a = NULL;

    if (ptrTargetRegionList_a != NULL) {
        ptrTargetRegionList_a->Delete();
        delete ptrTargetRegionList_a;
        ptrTargetRegionList_a = NULL;
    }

    if (ptrCollectionProblemSetList_a != NULL) {
        ptrCollectionProblemSetList_a->Delete();
        delete ptrCollectionProblemSetList_a;
        ptrCollectionProblemSetList_a = NULL;
    }

    return;
}
