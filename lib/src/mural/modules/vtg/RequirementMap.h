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
 * RequirementMap.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef REQUIREMENT_MAP_H
#define REQUIREMENT_MAP_H "RequirementMap"

#include <map>
#include <memory>
#include <vector>
#include <string>

#include "mural/MURALTypes.h"

template<class Type> class Array;
template<class Type> class LinkList;
class Requirement;
class TargetRegion;

using namespace std;

/**
 * This class maps the Requirements and TargetRegions to their respective Region Objects.
 * This used to be coupled with the Region object itself but was decoupled to create a stand alone
 * access generation library.
 *
 * @author Brian Fowler
 * @date 8/30/11
 */
class RequirementMap
{
public:

    /**
     * Default Constructor
     */
    RequirementMap();
    virtual ~RequirementMap();

    /**
     * Predicate for the std::sort method to sort Requirements ascending by Quality Index value.  Returns true if
     * ptrI->GetQualityIndex() < ptrJ->GetQualityIndex()
     *
     * @param ptrI the left Requirement
     * @param ptrJ the right Requirement
     * @return bool
     */
    static bool sortQualityAscending(const shared_ptr<Requirement> &ptrI, const shared_ptr<Requirement> ptrJ);

    /**
     * Predicate for the std::sort method to sort Requirements descending by Quality Index value.  Returns true if
     * ptrI->GetQualityIndex() > ptrJ->GetQualityIndex()
     *
     * @param ptrI the left Requirement
     * @param ptrJ the right Requirement
     * @return bool
     */
    static bool sortQualityDescending(const shared_ptr<Requirement> &ptrI, const shared_ptr<Requirement> ptrJ);

    /**
     * Method which Prints the requirements for the given deck and sensor.  This will loop on all the unique
     * regions numbers (in the form <region>.<sub region>) and print all the requirements in the list
     *
     * @param deckIndex the deck index for the appropriate requirement list
     * @param sensorIndex the sensor index of the appropriate requirement list
     * @param &outfile the output file stream
     */

    void         PrintRequirements(int deckIndex,
                                   int sensorIndex,
                                   ofstream &outfile) const;

    /**
     * Method which adds a Target Region to the map for the given region number
     *
     * @param fullRegionNumber the region number (to serve as the key for the map)
     * @param *ptrTargetRegion the TargetRegion to add
     */
    void         AddTargetRegion(double fullRegionNumber,
                                 TargetRegion *ptrTargetRegion);

    /**
     * Method which resets the requirement scores and areas for all the regions.  It
     * filters on the deck and flagged sensors via the reset sensor array argument.  ALL
     * regions will get reset with the given deck/sensor combos
     *
     * @param deckIndex the deck filter
     * @param *ptrResetSensor bool array flagging which sensors to reset
     */
    void         ResetScoreAndArea(int deckIndex,
                                   const Array<bool> *ptrResetSensor);

    /**
     * Method to reinitalize the map containing the requirments
     */
    void         DeleteRequirements();

    /**
     * Method which will set the Achievable Quality on the target region for the
     * given sensor/mission filter
     *
     * @param fullRegionNumber the region number key
     * @param sensorIndex the sensor filter
     * @param qualityLevel the achievable quality to pass to the target region
     * @param missionIndex the mission index filter
     */
    void         SetHighestQuality(double fullRegionNumber,
                                   int sensorIndex,
                                   int qualityLevel,
                                   int missionIndex) const;

    /**
     * Method which groups requirements based on similar region/deck/sensor/mission/quality/priority
     * The scores and areas for these grouped targets get added together
     *
     * @param fullRegionNUmber the region number key
     * @param deckIndex the deck filter
     * @param sensorIndex the sensor filter
     * @param missionIndex the mission filter
     * @param qualityIndex the quality filter
     * @param priorityIndex the priortiy filter
     * @param subPriority the sub priority filter
     * @param score the score to arrgregate
     * @param area the area to arrgregate
     * @param numberOfPoints the count of number of points in the requirement
     * @param numberOfDecks the count of number of decks
     * @param numberOfResources the count of number of specific resources
     */
    void         UpdateRequirementData(double fullRegionNUmber,
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
                                       int numberOfResources);

    /**
     * Gets the TargetRegion List from the encoded key string
     *
     * @param fullRegionNumber the region number (<region>.<subRegion>) key for the hash map
     * @param fullRegionNumber the deck index for the hash map
     * @param fullRegionNumber the sensor index for the hash map
     * @return LinkList<Requirement>*
     */
    shared_ptr<MURALTypes::RequirementVector> GetRequirementList(double fullRegionNumber,
            int deckIndex,
            int sensorIndex) const;

    /**
    * Gets the TargetRegion List from the encoded key string
    *
    * @param fullRegionNumber the region number (<region>.<subRegion>) key for the hash map
    * @return LinkList<TargetRegion>*
    */
    LinkList<TargetRegion>* GetTargetRegionList(double fullRegionNumber) const;

private:
    /**
     * Creates the string key for the map from the region number, deck index, and sensor index
    *
    * @param fullRegionNumber the region number (<region>.<subRegion>) to make the key from
    * @param deckIndex the deck index to make the key
    * @param sensorIndex the sensor index to make the key
    * @return the reuirement key
     */
    string GetRequirementKey(double fullRegionNumber, int deckIndex, int sensorIndex) const;

    /**
     * Gets the Requirement List from the encoded key string
     *
     * @param key the key for the hash map
     * @return LinkList<Requirement>*
     */
    shared_ptr<MURALTypes::RequirementVector> GetRequirementList(string key) const;




    /// Maintain a list of region numbers in the hash map of the requirements
    shared_ptr<vector<double>>                                           regionNumberKeys_a;
    /// Map of the requirments to a hash key generated from the sensor/deck/region number
    shared_ptr<map<string, shared_ptr<MURALTypes::RequirementVector> > > requirementMap_a;
    /// Map of the TargetRegions from the region number
    shared_ptr<map<double, LinkList<TargetRegion>*>>                     targetRegionMap_a;
};

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
std::string
RequirementMap::GetClassName()
{
    return(REQUIREMENT_MAP_H);
}
*/

#endif
