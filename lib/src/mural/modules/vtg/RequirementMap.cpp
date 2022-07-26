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
 * RequirementMap.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <sstream>
#include <string>
#include <vector>

#include "RequirementMap.h"
#include "Requirement.h"


#include "general/array/LinkList.h"
#include "general/array/ListIterator.h"
#include "general/utility/Utility.h"


#include "mural/Region.h"
#include "mural/modules/ts/TargetRegion.h"
#include "mural/sensor/Sensor.h"

RequirementMap::RequirementMap() :
    regionNumberKeys_a(new vector<double>()),
    requirementMap_a(new map<string, shared_ptr<MURALTypes::RequirementVector>>),
    targetRegionMap_a(new map<double, LinkList<TargetRegion>*>)
{
    return;
}

/* ***************************************************************************** */

bool
RequirementMap::sortQualityAscending(const shared_ptr<Requirement> &ptrI, const shared_ptr<Requirement> ptrJ)
{
    return ( ptrI->GetQualityIndex() < ptrJ->GetQualityIndex() );
}

/* ***************************************************************************** */

bool
RequirementMap::sortQualityDescending(const shared_ptr<Requirement> &ptrI, const shared_ptr<Requirement> ptrJ)
{
    return ( ptrJ->GetQualityIndex() < ptrI->GetQualityIndex() );
}

/* ***************************************************************************** */

void

RequirementMap::PrintRequirements(int deckIndex,
                                  int sensorIndex,
                                  ofstream &outfile) const
{
    vector<double>::iterator fullRegionIter;
    // sort by region number
    std::stable_sort(regionNumberKeys_a->begin(), regionNumberKeys_a->end());

    // Loop on the region number keys

    for( fullRegionIter = regionNumberKeys_a->begin(); fullRegionIter != regionNumberKeys_a->end(); ++fullRegionIter) {
        shared_ptr<MURALTypes::RequirementVector> requirementVector = GetRequirementList((*fullRegionIter), deckIndex, sensorIndex);
        MURALTypes::RequirementVector::iterator   requirementIter;
        shared_ptr<Requirement>                   ptrRequirement = NULL;
        // compute the subregion from the <region>.<sub region> double
        int regionNumber = (int)(*fullRegionIter);
        //RAGUSA 5/2/2018 - Casting double to int for 64-bit processing
        int subRegion = Utility::CastDoubleToInt(((*fullRegionIter) - regionNumber) * Region::SUB_REGION_DIVIDER_s);

        for( requirementIter = requirementVector->begin(); requirementIter != requirementVector->end(); ++requirementIter) {
            ptrRequirement = (*requirementIter);
            ptrRequirement->PrintRequirement(sensorIndex,  regionNumber,  subRegion, outfile);
        }
    }

    return;
}

/* ***************************************************************************** */

void
RequirementMap::AddTargetRegion(double fullRegionNumber, TargetRegion *ptrTargetRegion)
{
    LinkList<TargetRegion>* ptrTargetRegionList = GetTargetRegionList(fullRegionNumber);
    ptrTargetRegionList->Append(ptrTargetRegion);
}

/* ***************************************************************************** */

void
RequirementMap::UpdateRequirementData(
    double fullRegionNumber,
    int deckIndex,
    int sensorIndex,
    int missionIndex,
    int qualityIndex,
    int priorityIndex,
    int subPriority,
    double score,
    double area,
    int numberOfPoints,

    int numberOfDecks,
    int numberOfResources)
{
    bool                                      foundRequirement = false;
    shared_ptr<MURALTypes::RequirementVector> requirementVector = GetRequirementList(fullRegionNumber, deckIndex, sensorIndex);
    MURALTypes::RequirementVector::iterator   requirementIter;
    shared_ptr<Requirement>                   ptrRequirement = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    for( requirementIter = requirementVector->begin(); ((requirementIter != requirementVector->end()) && (foundRequirement == false)); ++requirementIter) {
        ptrRequirement = (*requirementIter);

        if (ptrRequirement->EqualRequirement(missionIndex, qualityIndex,
                                             priorityIndex) == true) {
            ptrRequirement->AddScoreAndArea(score, area, priorityIndex, subPriority);
            foundRequirement = true;
            ptrRequirement = NULL;
        }
    }

    if (foundRequirement == false) {
        // Get the appropriate Requirement list
        requirementVector = GetRequirementList(fullRegionNumber, deckIndex, sensorIndex);
        ptrRequirement = shared_ptr<Requirement>(new Requirement(sensorIndex, missionIndex, qualityIndex,
                         priorityIndex, subPriority, score, area,
                         numberOfPoints, numberOfResources));

        if (Sensor::IsSpectralSensor(sensorIndex) == false) {
            // NIIRS/NRIS sorted from worst (10) to best (90) quality
            requirementVector->push_back(ptrRequirement);
            // sort ascending
            std::stable_sort(requirementVector->begin(), requirementVector->end(), sortQualityAscending);
        } else {
            // GSD sorted from worst (100) to best (0.01) quality
            requirementVector->push_back(ptrRequirement);
            // sort descending
            std::stable_sort(requirementVector->begin(), requirementVector->end(), sortQualityDescending);
        }

        ptrRequirement = NULL;
    }

    return;
}

/* ***************************************************************************** */

void
RequirementMap::SetHighestQuality(double fullRegionNumber,
                                  int sensorIndex,
                                  int qualityLevel,
                                  int missionIndex) const
{
    ListIterator<TargetRegion> targetRegionListIter(GetTargetRegionList(fullRegionNumber));
    TargetRegion              *ptrTargetRegion = targetRegionListIter.First();

    while (ptrTargetRegion != NULL) {
        if (ptrTargetRegion->GetMissionIndex() == missionIndex) {
            ptrTargetRegion->SetHighestQuality(sensorIndex, qualityLevel);
        }

        ptrTargetRegion = targetRegionListIter.Next();
    }

    return;
}

/* ***************************************************************************** */

/* ***************************************************************************** */

void
RequirementMap::ResetScoreAndArea(int deckIndex,
                                  const Array<bool> *ptrResetSensor)
{
    const int   numberOfSensors = ptrResetSensor->Size();
    shared_ptr<MURALTypes::RequirementVector> requirementVector = NULL;
    MURALTypes::RequirementVector::iterator   requirementIter;
    shared_ptr<Requirement>                   ptrRequirement = NULL;
    vector<double>::iterator fullRegionIter;

    //CPPCHECK_FIX Performance - prefix iterator change
    for( fullRegionIter = regionNumberKeys_a->begin(); fullRegionIter != regionNumberKeys_a->end(); ++fullRegionIter) {
        int sensorIndex = 0;

        //CPPCHECK_FIX Remove as redundant since move as above (Joe)
        //sensorIndex = 0;

        while (sensorIndex < numberOfSensors) {
            if (ptrResetSensor->ElementAt(sensorIndex) == true) {
                requirementVector = GetRequirementList((*fullRegionIter), deckIndex, sensorIndex);

                //CPPCHECK_FIX Performance - prefix iterator change
                for(requirementIter = requirementVector->begin(); requirementIter != requirementVector->end(); ++requirementIter) {
                    ptrRequirement = (*requirementIter);
                    ptrRequirement->ResetScoreAndArea();
                }
            }

            ++sensorIndex;
        }
    }

    return;
}

/* ***************************************************************************** */

string
RequirementMap::GetRequirementKey(double fullRegionNumber, int deckIndex, int sensorIndex) const
{
    stringstream keyStream;
    keyStream << fullRegionNumber << "_" << deckIndex << "_" << sensorIndex;
    return keyStream.str();
}

/* ***************************************************************************** */

shared_ptr<MURALTypes::RequirementVector>
RequirementMap::GetRequirementList(double fullRegionNumber,
                                   int deckIndex,
                                   int sensorIndex) const
{
    // Keep record of the unique region numbers
    if ( Utility::VectorContains((*regionNumberKeys_a), fullRegionNumber) == false ) {
        regionNumberKeys_a->push_back(fullRegionNumber);
    }

    string mapkey = GetRequirementKey(fullRegionNumber, deckIndex, sensorIndex);
    return( GetRequirementList(mapkey) );
}

/* ***************************************************************************** */

shared_ptr<MURALTypes::RequirementVector>
RequirementMap::GetRequirementList(string key) const
{
    shared_ptr<MURALTypes::RequirementVector> ptrRequirementList;

    if ( requirementMap_a->find(key) != requirementMap_a->end()) {
        ptrRequirementList = requirementMap_a->find(key)->second;
    } else {
        ptrRequirementList = shared_ptr<MURALTypes::RequirementVector>(new MURALTypes::RequirementVector() );
        requirementMap_a->insert(std::pair<string, shared_ptr<MURALTypes::RequirementVector>>(key, ptrRequirementList) );
    }

    return ( ptrRequirementList );
}

/* ***************************************************************************** */

LinkList<TargetRegion>*
RequirementMap::GetTargetRegionList(double fullRegionNumber) const
{
    LinkList<TargetRegion>* ptrTargetRegionList;

    if ( targetRegionMap_a->find(fullRegionNumber) != targetRegionMap_a->end()) {
        ptrTargetRegionList = targetRegionMap_a->find(fullRegionNumber)->second;
    } else {
        ptrTargetRegionList = new LinkList<TargetRegion>();
        targetRegionMap_a->insert(std::pair<double, LinkList<TargetRegion>*>(fullRegionNumber, ptrTargetRegionList) );
    }

    return ( ptrTargetRegionList );
}

/* ***************************************************************************** */

void
RequirementMap::DeleteRequirements()
{
    requirementMap_a->clear();
}

/* ***************************************************************************** */

RequirementMap::~RequirementMap()
{
    return;
}

