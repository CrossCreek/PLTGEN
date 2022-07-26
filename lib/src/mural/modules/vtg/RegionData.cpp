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
 * RegionData.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "RegionData.h"
#include "CrisisArea.h"
#include "MultipleRequirement.h"
#include "Requirement.h"
#include "TimelineGenerator.h"
#include "RequirementMap.h"


#include "general/time/TimePiece.h"
#include "general/math/SpaceVector.h"
#include "general/array/ListIterator.h"
#include "general/utility/Constant.h"
#include "general/utility/StringEditor.h"


#include "mural/Region.h"
#include "mural/modules/ts/Target.h"
#include "mural/modules/ts/TargetRegion.h"
#include "mural/sensor/Sensor.h"


// Initialize Static Variables
const int  RegionData::CRISIS_MISSION_NUMBER_s = 10;
const char   RegionData::REGION_NUMBER_ID_s      = 'R';
const char   RegionData::REQUIREMENT_ID_s        = 'Q';

RegionData::RegionData()
    : solutionIndex_a(INVALID_SOLUTION_INDEX), totalUpperBound_a(0.0),
      ptrCalculatedQualityArray_a(NULL), ptrAssignedRequirementIndex_a(NULL),
      ptrValidMission_a(NULL), ptrRegion_a(NULL),
      ptrSensor_a(NULL), ptrCrisisArea_a(NULL), ptrRequirementList_a(NULL),
      ptrMostValuableRequirementList_a(NULL), ptrResourceMVRequirementList_a(NULL)
{
    return;
}

RegionData::RegionData(Region *ptrRegion,
                       Sensor *ptrSensor,
                       const Array<bool> *ptrValidMission,
                       const bool &scheduleTargets,
                       CrisisArea *ptrCrisis,
                       const shared_ptr<Requirement> &ptrRequirement)
    : solutionIndex_a(INVALID_SOLUTION_INDEX), totalUpperBound_a(0.0),
      ptrCalculatedQualityArray_a(NULL), ptrAssignedRequirementIndex_a(NULL),
      ptrValidMission_a(new Array<bool>(*ptrValidMission)), ptrRegion_a(ptrRegion),
      ptrSensor_a(ptrSensor), ptrCrisisArea_a(ptrCrisis), ptrRequirementList_a(NULL),
      ptrMostValuableRequirementList_a(NULL), ptrResourceMVRequirementList_a(NULL)
{
    if (ptrRequirement != NULL) {
        AddRequirement(ptrRequirement);
    }

    if (scheduleTargets == true) {
        ptrCalculatedQualityArray_a = new Array<int>(Target::GetNumberOfMissions(), 0);
    }

    return;
}

RegionData::RegionData(const RegionData &copyMe)
    : solutionIndex_a(INVALID_SOLUTION_INDEX),
      totalUpperBound_a(copyMe.totalUpperBound_a),
      ptrCalculatedQualityArray_a(NULL), ptrAssignedRequirementIndex_a(NULL),
      ptrValidMission_a(NULL), ptrRegion_a(copyMe.ptrRegion_a),
      ptrSensor_a(copyMe.ptrSensor_a), ptrCrisisArea_a(copyMe.ptrCrisisArea_a),
      ptrRequirementList_a(copyMe.ptrRequirementList_a),
      ptrMostValuableRequirementList_a(NULL),
      ptrResourceMVRequirementList_a(NULL)
{
    return;
}

RegionData&
RegionData::operator = (const RegionData &copyMe)
{
    solutionIndex_a                  = INVALID_SOLUTION_INDEX;
    totalUpperBound_a                = copyMe.totalUpperBound_a;
    ptrCalculatedQualityArray_a      = copyMe.ptrCalculatedQualityArray_a;
    ptrAssignedRequirementIndex_a    = copyMe.ptrAssignedRequirementIndex_a;
    ptrValidMission_a                = copyMe.ptrValidMission_a;
    ptrRegion_a                      = copyMe.ptrRegion_a;
    ptrSensor_a                      = copyMe.ptrSensor_a;
    ptrCrisisArea_a                  = copyMe.ptrCrisisArea_a;
    ptrRequirementList_a             = copyMe.ptrRequirementList_a;
    ptrMostValuableRequirementList_a = copyMe.ptrMostValuableRequirementList_a;
    ptrResourceMVRequirementList_a   = copyMe.ptrResourceMVRequirementList_a;
    return(*this);
}

bool
RegionData::operator == (const RegionData &compareMe)
{
    return( (totalUpperBound_a == compareMe.totalUpperBound_a) &&
            (*ptrSensor_a == *(compareMe.ptrSensor_a)) );
}

bool
RegionData::operator != (const RegionData &compareMe)
{
    return(!(operator == (compareMe)));
}


/* ************************************************************************************** */

// Predicate constructor
RegionData::SortScoreEffDescending::SortScoreEffDescending(Sensor* ptrSensor) : ptrSensor_a(ptrSensor)
{
}

// Predicate operator()
bool RegionData::SortScoreEffDescending::operator() (const shared_ptr<Requirement> &ptrI, const shared_ptr<Requirement> ptrJ)
{
    double scoreEfficency = 0.0;
    double iScoreEff = 0.0;
    double jScoreEff = 0.0;
    scoreEfficency = ptrI->GetRemainingScoreEfficiency(ptrSensor_a);

    if (scoreEfficency > 0.0) {
        iScoreEff = std::min(scoreEfficency * EFFICIENCY_MULTIPLIER,
                             (double)LONG_MAX);
    }

    scoreEfficency = ptrJ->GetRemainingScoreEfficiency(ptrSensor_a);

    if (scoreEfficency > 0.0) {
        jScoreEff = std::min(scoreEfficency * EFFICIENCY_MULTIPLIER,
                             (double)LONG_MAX);
    }

    return (jScoreEff < iScoreEff);
}

/* ************************************************************************************** */

double
RegionData::GetAchievableScore(double fullTimeStep,
                               int resourceIndex,
                               int numberOfResources)
{
    double                                    achievableScore = 0.0;
    shared_ptr<MURALTypes::RequirementVector> ptrNewRequirementList = NULL;

    if (ptrRequirementList_a != NULL) {
        if (resourceIndex < 0) {
            if (ptrMostValuableRequirementList_a == NULL) {
                ptrMostValuableRequirementList_a = shared_ptr<MURALTypes::RequirementVector>(
                                                       new MURALTypes::RequirementVector());
            } else {
                ptrMostValuableRequirementList_a->clear();
            }

            achievableScore += DetermineMVRequirements(ptrMostValuableRequirementList_a,
                               fullTimeStep, resourceIndex);
        } else {
            if (ptrResourceMVRequirementList_a == NULL) {
                ptrResourceMVRequirementList_a = shared_ptr<vector<shared_ptr<MURALTypes::RequirementVector>>>(
                                                     new vector<shared_ptr<MURALTypes::RequirementVector>>(numberOfResources));
            }

            if (ptrResourceMVRequirementList_a->at(resourceIndex) == NULL) {
                ptrNewRequirementList = shared_ptr<MURALTypes::RequirementVector>(
                                            new MURALTypes::RequirementVector());
                (*ptrResourceMVRequirementList_a)[resourceIndex] = ptrNewRequirementList;
            } else {
                ((*ptrResourceMVRequirementList_a)[resourceIndex])->clear();
            }

            achievableScore += DetermineMVRequirements((*ptrResourceMVRequirementList_a)[resourceIndex],
                               fullTimeStep, resourceIndex);
        }
    }

    return(achievableScore);
}

void
RegionData::SetHighestQualityLevel(int sensorIndex,
                                   int quality,
                                   int missionIndex)
{
    if (quality > ptrCalculatedQualityArray_a->ElementAt(missionIndex)) {
        (*ptrCalculatedQualityArray_a)[missionIndex] = quality;
        TimelineGenerator::requirementMap_s->SetHighestQuality(
            ptrRegion_a->GetFullRegionNumber(), sensorIndex, quality, missionIndex);
    }
}

double
RegionData::DetermineMVRequirements(const shared_ptr<MURALTypes::RequirementVector> &ptrMVRequirementList,
                                    double fullTimeStep,
                                    int resourceIndex)
{
    double                     totalRequirementScore = 0.0;
    double                     achievableScore = 0.0;
    double                     remainingTTC = fullTimeStep;
    MURALTypes::RequirementVector              sortedRequirementList(ptrRequirementList_a->size());
    MURALTypes::RequirementVector::iterator    requirementIter;
    shared_ptr<Requirement>                    ptrRequirement = NULL;
    // copy the list into a temporary sorting list
    std::copy(ptrRequirementList_a->begin(), ptrRequirementList_a->end(), sortedRequirementList.begin());
    // sort temporary list descending by score efficency
    SortScoreEffDescending sortScoreEffDescending(ptrSensor_a);
    std::sort(sortedRequirementList.begin(), sortedRequirementList.end(), sortScoreEffDescending);

    // loop on the sorted score efficent list
    //CPPCHECK_FIX Performance - prefix iterator change
    for(requirementIter = sortedRequirementList.begin(); requirementIter != sortedRequirementList.end(); ++requirementIter) {
        ptrRequirement = (*requirementIter);

        if (remainingTTC > 0.0) {
            if ((achievableScore = ptrRequirement->GetAchievableScore(ptrSensor_a,
                                   remainingTTC, resourceIndex)) > 0.0) {
                totalRequirementScore += achievableScore;
                remainingTTC -= ptrRequirement->GetAchievableCollectionTime(ptrSensor_a, remainingTTC,
                                resourceIndex);
                ptrMVRequirementList->push_back(ptrRequirement);
            }
        }
    }

    return(totalRequirementScore);
}

double
RegionData::GetTotalAchievableScore(double fullTimeStep) const
{
    double                     totalAchievableScore = 0.0;
    MURALTypes::RequirementVector::iterator   requirementIter;
    shared_ptr<Requirement>                   ptrRequirement = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    if (ptrRequirementList_a != NULL) {
        for(requirementIter = ptrRequirementList_a->begin(); requirementIter != ptrRequirementList_a->end(); ++requirementIter) {
            ptrRequirement = (*requirementIter);
            totalAchievableScore += ptrRequirement->GetAchievableScore(ptrSensor_a, fullTimeStep);
        }
    }

    return(totalAchievableScore);
}

double
RegionData::GetTotalAchievableTTC(double fullTimeStep) const
{
    double        totalAchievableTTC = 0.0;
    MURALTypes::RequirementVector::iterator   requirementIter;
    shared_ptr<Requirement>                   ptrRequirement = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    if (ptrRequirementList_a != NULL) {
        for(requirementIter = ptrRequirementList_a->begin(); requirementIter != ptrRequirementList_a->end(); ++requirementIter) {
            ptrRequirement = (*requirementIter);
            totalAchievableTTC += ptrRequirement->GetAchievableCollectionTime(ptrSensor_a, fullTimeStep);
        }

        totalAchievableTTC = std::min(totalAchievableTTC, fullTimeStep);
    } else if (ptrCrisisArea_a != NULL) {
        totalAchievableTTC = fullTimeStep;
    }

    return(totalAchievableTTC);
}

double
RegionData::UpdateMostValuableRequirements(int resourceIndex)
{
    double                                    timeToCollect = 0.0;
    double                                    remainingTTC = (double)TimePiece::GetSecondsPerTimeStep();
    shared_ptr<Requirement>                   ptrRequirement = NULL;
    shared_ptr<MURALTypes::RequirementVector> ptrRequirementList = NULL;
    MURALTypes::RequirementVector::iterator   requirementIter;

    if ((resourceIndex < 0) && (ptrMostValuableRequirementList_a != NULL)) {
        ptrRequirementList = ptrMostValuableRequirementList_a;
    } else if ((resourceIndex >= 0) && (ptrResourceMVRequirementList_a != NULL)) {
        ptrRequirementList = ptrResourceMVRequirementList_a->at(resourceIndex);
    }

    //CPPCHECK_FIX Performance - prefix iterator change
    if (ptrRequirementList != NULL) {
        for(requirementIter = ptrRequirementList->begin(); requirementIter != ptrRequirementList->end(); ++requirementIter) {
            double collectionTime;
            ptrRequirement = (*requirementIter);
            collectionTime = ptrRequirement->UpdateRemainingScoreAndArea(ptrSensor_a, remainingTTC,
                             resourceIndex);
            timeToCollect += collectionTime;
            remainingTTC -= collectionTime;
        }

        RemoveMostValuableRequirements(resourceIndex);
    }

    return(timeToCollect);
}

void
RegionData::RemoveMostValuableRequirements(int resourceIndex)
{
    if ((resourceIndex < 0) && (ptrMostValuableRequirementList_a != NULL)) {
        ptrMostValuableRequirementList_a->clear();
    } else if ((resourceIndex >= 0) && (ptrResourceMVRequirementList_a != NULL)) {
        ((*ptrResourceMVRequirementList_a)[resourceIndex])->clear();
    }

    return;
}

void
RegionData::GetAchievableCollection(const shared_ptr<vector<double>> &ptrSolutionArray,
                                    Array<int> *ptrPointsCollected,
                                    Array<double> *ptrAreaCollected) const
{
    int                                       solutionIndex = solutionIndex_a;
    MURALTypes::RequirementVector::iterator   requirementIter;
    shared_ptr<Requirement>                   ptrRequirement = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    try {
        int pointsCollected = 0;
        int priorityIndex = 0;
        double areaCollected = 0.0;
        double collectionTime = 0.0;

        for(requirementIter = ptrRequirementList_a->begin(); requirementIter != ptrRequirementList_a->end(); ++requirementIter) {
            ptrRequirement = (*requirementIter);

            if (ptrAssignedRequirementIndex_a->ValueExists(ptrRequirement->GetIndex()) == true) {
                ++solutionIndex;
                collectionTime = ptrSolutionArray->at(solutionIndex);

                if (collectionTime > 0.0) {
                    priorityIndex = ptrRequirement->GetPriorityIndex();

                    if (Target::IsPointTarget(ptrRequirement->GetMissionIndex()) == true) {
                        pointsCollected = ptrRequirement->GetAchievableNumberOfPoints(ptrSensor_a, collectionTime);
                        (*ptrPointsCollected)[priorityIndex] += pointsCollected;
                    } else {
                        areaCollected = ptrRequirement->GetAchievableArea(ptrSensor_a, collectionTime);
                        (*ptrAreaCollected)[priorityIndex] += areaCollected;
                    }
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetAchievableCollection(const Array<double>*, Array<int>*, Array<double>*)");
        throw;
    }

    return;
}

double
RegionData::GetAchievableScore(const shared_ptr<vector<double>> &ptrSolutionArray) const
{
    int                      solutionIndex = solutionIndex_a;
    double                     achievableScore = 0.0;
    double                     collectionTime = 0.0;
    MURALTypes::RequirementVector::iterator   requirementIter;
    shared_ptr<Requirement>                   ptrRequirement = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    for(requirementIter = ptrRequirementList_a->begin(); requirementIter != ptrRequirementList_a->end(); ++requirementIter) {
        ptrRequirement = (*requirementIter);

        if (ptrAssignedRequirementIndex_a->ValueExists(ptrRequirement->GetIndex()) == true) {
            ++solutionIndex;
            collectionTime = ptrSolutionArray->at(solutionIndex);

            if (collectionTime > 0.0) {
                achievableScore += ptrRequirement->GetAchievableScore(ptrSensor_a, collectionTime);
            }
        }
    }

    return(achievableScore);
}

double
RegionData::GetTimeToCollect(const shared_ptr<vector<double>> &ptrSolutionArray,
                             const bool &updateRequirements)
{
    int                                       solutionIndex = solutionIndex_a;
    double                                    timeToCollect = 0.0;
    MURALTypes::RequirementVector::iterator   requirementIter;
    shared_ptr<Requirement>                   ptrRequirement = NULL;

    if (ptrRequirementList_a != NULL) {
        double collectionTime = 0.0;

        if (ptrMostValuableRequirementList_a != NULL) {
            ptrMostValuableRequirementList_a->clear();
        }

        //CPPCHECK_FIX Performance - prefix iterator change
        for(requirementIter = ptrRequirementList_a->begin(); requirementIter != ptrRequirementList_a->end(); ++requirementIter) {
            ptrRequirement = (*requirementIter);

            if (ptrAssignedRequirementIndex_a->ValueExists(ptrRequirement->GetIndex()) == true) {
                ++solutionIndex;
                collectionTime = ptrSolutionArray->at(solutionIndex);

                if (collectionTime > 0.0) {
                    if (updateRequirements == true) {
                        timeToCollect += ptrRequirement->UpdateRemainingScoreAndArea(ptrSensor_a,
                                         collectionTime);
                    } else {
                        timeToCollect += ptrRequirement->GetAchievableCollectionTime(ptrSensor_a,
                                         collectionTime);
                    }

                    if (ptrMostValuableRequirementList_a == NULL) {
                        ptrMostValuableRequirementList_a = shared_ptr<MURALTypes::RequirementVector>(
                                                               new MURALTypes::RequirementVector());
                    }

                    ptrMostValuableRequirementList_a->push_back(ptrRequirement);
                }
            }
        }
    } else if (ptrCrisisArea_a != NULL) {
        timeToCollect = (double)TimePiece::GetSecondsPerTimeStep();
    }

    return(timeToCollect);
}

int
RegionData::GetGeoRegionIndex() const
{
    return(ptrRegion_a->GetGeoRegionIndex());
}

double
RegionData::GetRegionNumber() const
{
    return(ptrRegion_a->GetFullRegionNumber());
}

bool
RegionData::ValidMissionIndex(int missionIndex) const
{
    return(ptrValidMission_a->ElementAt(missionIndex));
}

SpaceVector
RegionData::GetRegionPosition(int timeIndex) const
{
    return(ptrRegion_a->GetRegionCenter(timeIndex));
}

LinkList<TargetRegion>*
RegionData::GetTargetRegionList() const
{
    return(TimelineGenerator::requirementMap_s->GetTargetRegionList(ptrRegion_a->GetFullRegionNumber()));
}

int
RegionData::GetSensorNumber() const
{
    return(ptrSensor_a->GetSensorNumber());
}

int
RegionData::GetSensorIndex() const
{
    return(ptrSensor_a->GetSensorIndex());
}

double
RegionData::GetSensorDataRate() const
{
    return(ptrSensor_a->GetDataRate());
}

string
RegionData::GetSensorName() const
{
    return(ptrSensor_a->GetSensorName());
}

int
RegionData::GetMissionNumber() const
{
    int missionNumber = 0;

    if (ptrRequirementList_a != NULL) {
        if (ptrRequirementList_a->size() == 1) {
            missionNumber = ((*ptrRequirementList_a)[0])->GetMissionNumber();
        }
    } else if (ptrCrisisArea_a != NULL) {
        missionNumber = CRISIS_MISSION_NUMBER_s;
    }

    return(missionNumber);
}

int
RegionData::GetMostValuableMissionNumber(int resourceIndex) const
{
    int missionNumber = 0;

    if ((resourceIndex < 0) && (ptrMostValuableRequirementList_a != NULL)) {
        if (ptrMostValuableRequirementList_a->size() == 1) {
            missionNumber = ((*ptrMostValuableRequirementList_a)[0])->GetMissionNumber();
        }
    } else if ((resourceIndex >= 0) && (ptrResourceMVRequirementList_a != NULL)) {
        if ( ((*ptrResourceMVRequirementList_a)[resourceIndex])->size() == 1) {
            missionNumber = (((*ptrResourceMVRequirementList_a)[resourceIndex])->at(0))->GetMissionNumber();
        }
    } else if (ptrCrisisArea_a != NULL) {
        missionNumber = CRISIS_MISSION_NUMBER_s;
    }

    return(missionNumber);
}

int
RegionData::GetCrisisLevel() const
{
    int crisisLevel = 0;

    if (ptrCrisisArea_a != NULL) {
        crisisLevel = ptrCrisisArea_a->GetIdentificationNumber();
    }

    return(crisisLevel);
}

int
RegionData::GetSubPriorityLevel() const
{
    int                                       subPriorityLevel = 0;
    MURALTypes::RequirementVector::iterator   requirementIter;
    shared_ptr<Requirement>                   ptrRequirement = NULL;

    if (ptrRequirementList_a != NULL) {
        subPriorityLevel = SHRT_MAX;

        //CPPCHECK_FIX Performance - prefix iterator change
        for(requirementIter = ptrRequirementList_a->begin(); requirementIter != ptrRequirementList_a->end(); ++requirementIter) {
            ptrRequirement = (*requirementIter);
            subPriorityLevel = std::min(subPriorityLevel,
                                        ptrRequirement->GetSubPriorityLevel());
        }
    }

    return(subPriorityLevel);
}

int
RegionData::GetMostValuableSubPriLevel(int resourceIndex) const
{
    int                                       subPriorityLevel = 0;
    shared_ptr<Requirement>                   ptrRequirement = NULL;
    shared_ptr<MURALTypes::RequirementVector> ptrRequirementList = NULL;
    MURALTypes::RequirementVector::iterator   requirementIter;

    if ((resourceIndex < 0) && (ptrMostValuableRequirementList_a != NULL)) {
        ptrRequirementList = ptrMostValuableRequirementList_a;
    } else if ((resourceIndex >= 0) && (ptrResourceMVRequirementList_a != NULL)) {
        ptrRequirementList = ptrResourceMVRequirementList_a->at(resourceIndex);
    }

    if (ptrRequirementList != NULL) {
        subPriorityLevel = SHRT_MAX;

        //CPPCHECK_FIX Performance - prefix iterator change
        for(requirementIter = ptrRequirementList->begin(); requirementIter != ptrRequirementList->end(); ++requirementIter) {
            ptrRequirement = (*requirementIter);
            subPriorityLevel = std::min(subPriorityLevel,
                                        ptrRequirement->GetSubPriorityLevel());
        }
    }

    return(subPriorityLevel);
}

void
RegionData::PrintDebug(const string &statusString,
                       int resourceIndex,
                       double resourceFactor,
                       ofstream &valtLogFile) const
{
    double                                    fullTimeStep = (double)TimePiece::GetSecondsPerTimeStep();
    double                                    remainingTTC = fullTimeStep;
    const string                              sensorName = ptrSensor_a->GetSensorName();
    shared_ptr<Requirement>                   ptrRequirement = NULL;
    shared_ptr<MURALTypes::RequirementVector> ptrRequirementList = NULL;
    MURALTypes::RequirementVector::iterator   requirementIter;

    if ((resourceIndex < 0) && (ptrMostValuableRequirementList_a != NULL)) {
        ptrRequirementList = ptrMostValuableRequirementList_a;
    } else if ((resourceIndex >= 0) && (ptrResourceMVRequirementList_a != NULL)) {
        ptrRequirementList = ptrResourceMVRequirementList_a->at(resourceIndex);
    }

    // Get the first requirement from the list
    if (ptrRequirementList != NULL ) {
        ptrRequirement = (*ptrRequirementList)[0];
    }

    valtLogFile << setw(5) << sensorName << " ";
    Region::PrintFixedWidthRegionNumber(valtLogFile, ptrRegion_a->GetFullRegionNumber());
    valtLogFile << "                                      ";

    if ((ptrCrisisArea_a != NULL) && (ptrRequirement == NULL)) {
        valtLogFile << "CRISIS                             " << statusString << NEW_LINE;
    } else {
        for(requirementIter = ptrRequirementList->begin(); requirementIter != ptrRequirementList->end(); /* incremented by hand */) {
            double timeToCollect;
            double requirementScore;
            ptrRequirement = (*requirementIter);
            timeToCollect = ptrRequirement->GetAchievableCollectionTime(ptrSensor_a, remainingTTC, resourceIndex);
            requirementScore = ptrRequirement->GetAchievableScore(ptrSensor_a, remainingTTC, resourceIndex) * resourceFactor;
            valtLogFile << setw(7) << ptrRequirement->GetMissionString();
            valtLogFile << " " << setw(8) << ptrRequirement->GetQualityLevel(GetSensorIndex());
            valtLogFile << " " << setw(6) << ptrRequirement->GetPriorityLevel();
            valtLogFile << " " << setw(5) << setprecision(1) << timeToCollect;
            valtLogFile << " " << setw(12) << setprecision(3) << requirementScore;
            valtLogFile << "  " << statusString;

            if (ptrCrisisArea_a != NULL) {
                valtLogFile << " *";
            }

            valtLogFile << NEW_LINE;
            // Get the next requirement
            //CPPCHECK_FIX Performance - prefix iterator change
            ++requirementIter;

            if (requirementIter != ptrRequirementList->end()) {
                remainingTTC -= timeToCollect;
                valtLogFile << "                                                                   ";
            }
        }
    }

    return;
}

void
RegionData::AddRequirement(const shared_ptr<Requirement> &ptrRequirement)
{
    if (ptrRequirementList_a == NULL) {
        ptrRequirementList_a = shared_ptr<MURALTypes::RequirementVector>(
                                   new MURALTypes::RequirementVector());
    }

    ptrRequirementList_a->push_back(ptrRequirement);
}

void
RegionData::RemoveAllRequirements()
{
    if (ptrRequirementList_a != NULL) {
        ptrRequirementList_a->clear();
    }

    return;
}

RegionData::~RegionData()
{
    ptrRegion_a = NULL;
    ptrSensor_a = NULL;
    ptrCrisisArea_a = NULL;
    RemoveAllRequirements();

    if (ptrCalculatedQualityArray_a != NULL) {
        delete ptrCalculatedQualityArray_a;
        ptrCalculatedQualityArray_a = NULL;
    }

    if (ptrAssignedRequirementIndex_a != NULL) {
        delete ptrAssignedRequirementIndex_a;
        ptrAssignedRequirementIndex_a = NULL;
    }

    if (ptrValidMission_a != NULL) {
        delete ptrValidMission_a;
        ptrValidMission_a = NULL;
    }

    return;
}
