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
 * RegionData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef REGION_DATA_H
#define REGION_DATA_H "RegionData"

#include <fstream>


#include "general/array/ArrayOfLinkLists.h"
#include "mural/MURALTypes.h"
#include "general\utility\Utility.h"

using namespace std;

class Constraint;
class CrisisArea;
class MultipleRequirement;
class Region;
class Requirement;
class Sensor;
class SolutionIndex;
class SpaceVector;
class TargetRegion;

/**
 * A wrapper data class around a Region containing additional information regarding
 * the collectin sensor, any crisis area, and setup for the requested and allocated
 * MVR LP Problems.
 *
 * Name                              Description (units)
 * -------------                     -------------------------------------------
 * solutionIndex_a                   assigned solution index used for solving in
 *                                     the Optimizer
 * totalUpperBound_a                 total possible time to collect the
 *                                     in this region requirements
 * ptrCalculatedQualityArray_a*      the quality determined for this region at
 *                                     this time (by mission)
 * ptrAssignedRequirementIndex_a*    array of assigned solution index for each
 *                                     valid requirement on the requirement list
 * ptrValidMission_a*                array of booleans by mission indicating if
 *                                     it is a valid mission for this region
 * ptrRegion_a*                      pointer to the Region
 * ptrSensor_a*                      pointer to the collecting Sensor
 * ptrCrisisArea_a*                  pointer to the CrisisArea day (if any)
 * ptrRequirementList_a*             list of requirements for this region/sensor
 *                                     combination
 * ptrMostValuableRequirementList_a* list of the current most valuable (and
 *                                     collectable) requirements for this region
 * ptrResourceMVRequirementList_a*   list of the current resource specific most
 *                                     valuable (and collectable) requirements
 *                                     for this region
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */

class RegionData
{
public:

    RegionData();
    RegionData(Region *ptrRegion,
               Sensor *ptrSensor,
               const Array<bool> *ptrValidMission,
               const bool &scheduleTargets,
               CrisisArea *ptrCrisis,
               const shared_ptr<Requirement> &ptrRequirement = NULL);
    RegionData(const RegionData &copyIndex);
    virtual ~RegionData();

    RegionData& operator =  (const RegionData &copyMe);
    bool        operator == (const RegionData &compareMe);
    bool        operator != (const RegionData &compareMe);

    /**
     * Predicate struct for the std::sort method.  The pointer to the Sensor is required so this
     * structure hold the pointer (used by the operator())
     */
    struct SortScoreEffDescending {

        /**
         * Constructor setting the pointer to the Sensor
         *
         * @param Sensor* pointer to the sensor
         */



        explicit SortScoreEffDescending(Sensor *ptrSensor);

        /**
          * Predicate operator() for the std::sort method to sort Requirements descending by scoreEfficency * EFFICIENCY_MULTIPLIER.
          * Returns true if ptrJ->Get() < ptrI->Get() (for descending)
          *
          * @param ptrI the left Requirement
          * @param ptrJ the right Requirement
          * @return bool
          */
        bool operator() (const shared_ptr<Requirement> &ptrI, const shared_ptr<Requirement> ptrJ);

        const Sensor* ptrSensor_a;
    };

    void   AddRequirement(const shared_ptr<Requirement> &ptrRequirement);
    void   SetHighestQualityLevel(int sensorIndex,
                                  int quality,
                                  int missionIndex);
    void   GetAchievableCollection(const shared_ptr<vector<double>> &ptrSolutionArray,
                                   Array<int> *ptrPointsCollected,
                                   Array<double> *ptrAreaCollected) const;
    void   PrintDebug(const string &statusString,
                      int resourceIndex,
                      double resourceFactor,
                      ofstream &valtLogFile) const;
    void   RemoveAllRequirements();
    void   RemoveMostValuableRequirements(int resourceIndex = -1);

    int  GetCrisisLevel() const;
    int  GetSensorNumber() const;
    int  GetMissionNumber() const;
    int  GetMostValuableMissionNumber(int resourceIndex) const;
    int  GetSubPriorityLevel() const;
    int  GetMostValuableSubPriLevel(int resourceIndex) const;

    int   GetSensorIndex() const;
    int   GetGeoRegionIndex() const;

    double  GetAchievableScore(double fullTimeStep,
                               int resourceIndex = -1,
                               int numberOfResources = 0);
    double  GetTotalAchievableScore(double fullTimeStep) const;
    double  GetTotalAchievableTTC(double fullTimeStep) const;
    double  GetAchievableScore(const shared_ptr<vector<double>> &ptrSolutionArray) const;
    double  GetTimeToCollect(const shared_ptr<vector<double>> &ptrSolutionArray,
                             const bool &updateRequirements);
    double  GetRegionNumber() const;
    double  GetSensorDataRate() const;
    double  UpdateMostValuableRequirements(int resourceIndex);

    bool   ValidMissionIndex(int missionIndex) const;

    string GetSensorName() const;

    SpaceVector GetRegionPosition(int timeIndex) const;

    LinkList<TargetRegion>* GetTargetRegionList() const;

    static inline char   GetRegionNumberID();
    static inline char   GetRequirementID();

    inline int  GetSolutionIndex() const;

    inline int  GetNumberOfRequirements() const;

    inline bool  IsCrisisRegion() const;
    inline bool  IsValid() const;

    inline Region* GetRegion() const;

    inline Sensor* GetSensor() const;

    inline shared_ptr<MURALTypes::RequirementVector> GetRequirementList() const;

protected:

private:

    double  DetermineMVRequirements(const shared_ptr<MURALTypes::RequirementVector> &ptrMVRequirementList,
                                    double fullTimeStep,
                                    int resourceIndex);

    static inline string GetClassName();

    static const int  CRISIS_MISSION_NUMBER_s;
    static const char   REGION_NUMBER_ID_s;
    static const char   REQUIREMENT_ID_s;

    int                                         solutionIndex_a;
    double                                      totalUpperBound_a;
    Array<int>                                 *ptrCalculatedQualityArray_a;
    Array<int>                                 *ptrAssignedRequirementIndex_a;
    Array<bool>                                *ptrValidMission_a;
    Region                                     *ptrRegion_a;
    Sensor                                     *ptrSensor_a;
    CrisisArea                                 *ptrCrisisArea_a;
    shared_ptr<MURALTypes::RequirementVector>   ptrRequirementList_a;
    shared_ptr<MURALTypes::RequirementVector>   ptrMostValuableRequirementList_a;

    // a vector of vectors
    shared_ptr<vector<shared_ptr<MURALTypes::RequirementVector>>>       ptrResourceMVRequirementList_a;
};

inline
char
RegionData::GetRegionNumberID()
{
    return(REGION_NUMBER_ID_s);
}

inline
char
RegionData::GetRequirementID()
{
    return(REQUIREMENT_ID_s);
}

inline
bool
RegionData::IsCrisisRegion() const
{
    return(ptrCrisisArea_a != NULL);
}

inline
int
RegionData::GetSolutionIndex() const
{
    return(solutionIndex_a);
}

inline

int
RegionData::GetNumberOfRequirements() const
{
    int numberOfRequirements = 0;

    if (ptrRequirementList_a != NULL) {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        numberOfRequirements = Utility::CastSizetToInt(ptrRequirementList_a->size());
    }

    return(numberOfRequirements);
}

inline
bool
RegionData::IsValid() const
{
    return((ptrRequirementList_a != NULL) || (ptrCrisisArea_a != NULL));
}

inline
Region*
RegionData::GetRegion() const
{
    return(ptrRegion_a);
}

inline
Sensor*
RegionData::GetSensor() const
{
    return(ptrSensor_a);
}

inline
shared_ptr<MURALTypes::RequirementVector>
RegionData::GetRequirementList() const
{
    return(ptrRequirementList_a);
}

inline
string
RegionData::GetClassName()
{
    return(REGION_DATA_H);
}

#endif

