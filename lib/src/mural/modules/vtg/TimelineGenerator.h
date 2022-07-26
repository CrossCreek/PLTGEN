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
 * TimelineGenerator.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TIMELINE_GENERATOR_H
#define TIMELINE_GENERATOR_H "TimelineGenerator"

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "general/array/Array2D.h"
#include "general/array/LinkList.h"
#include "general/utility/Constant.h"

using namespace std;

template<class Type> class ArrayOfLinkLists;
template<class Type> class ArrayOfPointers;
class AccessPattern;
class CollectionElement;
class CrisisArea;
class DataProvider;
class Link;
class Optimizer;
class Region;
class RegionData;
class Requirement;
class Sensor;
class SpaceVector;
class TCARegionData;
class TimePeriod;
class UserTransmitAntenna;
class UserVehicle;
class ValueTimelineElement;
class RequirementMap;
class DataProviderCollection;

/**
 * Base class of a timeline generator.  Contains static methods to compute Field Of Regard
 * for field of regard users.
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */
class TimelineGenerator
{
public:

    TimelineGenerator();
    TimelineGenerator(const TimelineGenerator &copyMe);
    virtual ~TimelineGenerator();

    TimelineGenerator& operator =  (const TimelineGenerator &copyMe);
    bool               operator == (const TimelineGenerator &compareMe) const;
    bool               operator != (const TimelineGenerator &compareMe) const;

    static void  ResetRequirements(const shared_ptr<UserVehicle> &ptrUserVehicle);

    static bool LinkWhenPointingAtRegion(Link *ptrTransmitLink,
                                         const shared_ptr<UserVehicle> &ptrUserVehicle,
                                         int timeIndex,
                                         double regionNumber,
                                         double azimuthOffsetAngle,
                                         double cableWrapOffsetAngle,
                                         double additionalSeconds = 0.0);

    static ArrayOfPointers<CollectionElement>* DetermineMostValuableRegions(
        Optimizer *ptrOptimizer,
        const shared_ptr<UserVehicle> &ptrUserVehicle,
        const LinkList<TimePeriod> *ptrValidTimePeriodList,
        const bool &updateRequirements,
        const bool &planCollections,
        string *ptrProblemFileName,
        ofstream &mvrDebugFile,
        Array<double> *ptrBufferMemorySize = NULL,
        ArrayOfPointers<Link> *ptrTransmitLinkArray = NULL,
        Array<double> *ptrAzimuthOffsetArray = NULL,
        Array<double> *ptrCableWrapOffsetArray = NULL);


    /**
     * Map of Regions/sensors/decks to their requirements
     */
    static shared_ptr<RequirementMap> requirementMap_s;

protected:

    void GenerateTimelines(const shared_ptr<DataProviderCollection> &ptrDataProviderCollection);

    virtual double FactorWhenPointingAtRegion(Link *ptrTransmitLink,
            const shared_ptr<UserVehicle> &ptrUserVehicle,
            Region *ptrRegion,
            int timeIndex,
            double regionScore,
            ofstream &valtLogFile,
            double azimuthOffsetAngle = 0.0,
            double cableWrapOffsetAngle = 0.0);

    virtual bool   ValidTimeStep(const shared_ptr<UserVehicle> &ptrUserVehicle,
                                 int timeIndex,
                                 ofstream &valtLogFile);
    virtual bool   ValidRegion(const shared_ptr<UserVehicle> &ptrUserVehicle,
                               Sensor *ptrSensor,
                               Region *ptrRegion,
                               int timeIndex,
                               ofstream &valtLogFile);

private:

    static void   UpdateTimelineElement(ValueTimelineElement *ptrTimelineElement,
                                        RegionData *ptrMostValuableRegionData,
                                        int resourceIndex = -1);
    static void   PrintTimelineDebug(bool &firstRegion,
                                     const string &userDesignator,
                                     int timeIndex,
                                     const string &sensorName,
                                     bool &firstRequirement,
                                     double regionNumber,
                                     double sunAngle,
                                     double targetElevationAngle,
                                     const Sensor *ptrSensor,
                                     double qualityParameter,
                                     int calculatedQuality,
                                     CrisisArea *ptrCrisisArea,
                                     ofstream &valtLogFile,
                                     const shared_ptr<Requirement> &ptrRequirement = NULL);
    static void   PrintMostValuableRegionDebug(const string &userDesignator,
            int timeIndex,
            RegionData *ptrMostValuableRegionData,
            const string &statusString,
            ofstream &valtLogFile,
            int resourceIndex = -1,
            double resourceFactor = 1.0);
    static void   PrintRequirementDeck(const string &outputFileName,

                                       int deckIndex,
                                       int sensorIndex);

    static double  GetOverflownRegion(const SpaceVector &vehiclePosition);

    static string GetBaseDeckFileName(const string &fullDeckFileName,
                                      const string &standardExtension);

    static ArrayOfPointers<CollectionElement>* OptimizeRequirements(Optimizer *ptrOptimizer,
            const shared_ptr<UserVehicle> &ptrUserVehicle,
            const LinkList<TimePeriod> *ptrValidTimePeriodList,
            ArrayOfLinkLists<RegionData> *ptrArrayOfRegionDataLists,
            const bool &updateRequirements,
            const bool &planCollections,
            string *ptrProblemFileName,
            ofstream &mvrDebugFile,
            Array<double> *ptrBufferMemorySize);
    static ArrayOfPointers<CollectionElement>* CreateCollectionElements(
        ArrayOfLinkLists<RegionData> *ptrArrayOfRegionDataLists,
        const shared_ptr<UserVehicle> &ptrUserVehicle,
        const shared_ptr<vector<double>> &ptrSolutionArray,
        double integrality,
        const bool &updateRequirements,
        const bool &planCollections,
        ofstream &mvrDebugFile);

    void  DetermineBasicTimelineElements(const shared_ptr<UserVehicle> &ptrUserVehicle,
                                         ofstream &valtLogFile);
    //RAGUSA 1/10/2018 - New function for re-implementation of Crisis Region mode. Determines if at this timestep,
    //a Crisis Area is in-view in the CRISIS mission pattern list. Also determines if there are more
    //than one crisis areas in-view, it chooses the one with the highest multiplier (assuming that is highest priority)
    //crisis region to be selected as the one for this timestep.
    CrisisArea* GetMostValuableCrisisAreaInview(LinkList<Region>* ptrRegionList, string userDesignator);
    void BuildRequirementsFromTargets(int numberOfTargetDecks);
    void RetrieveRequirementDeckData(int numberOfTargetDecks) const;
    void RetrieveValueTimelineData(Array<string> *ptrRetrieveUserArray);
    void GenerateMostValuableRegionTimeline(const shared_ptr<UserVehicle> &ptrUserVehicle,
                                            ofstream &valtLogFile);
    void GenerateTimeOfClosestApproachTimeline(const shared_ptr<UserVehicle> &ptrUserVehicle,
            ofstream &valtLogFile);
    void GenerateNominalRequestRegionTimeline(const shared_ptr<UserVehicle> &ptrUserVehicle);
    void BuildTCATimeline(const shared_ptr<UserVehicle> &ptrUserVehicle,
                          ArrayOfLinkLists<TCARegionData> *ptrTCADataListArray,
                          ofstream &valtLogFile);
    void DetermineNominalRequestRegionElements(const shared_ptr<UserVehicle> &ptrUserVehicle,
            const Array<bool> *ptrTimeStepOnFlag);

    /**
     * This method to to set up the deck file names from the user vehicle.  It used to be done
     * while the sensor was being read in but this couples the TimelineGenerator to the sensor (BAD)
     *
     * This method loops on the Users/Sensors and sets them up on the Timeline allowing the
     * decoupling of the sensors
     */
    void SetupDeckFileNames(const UserVehicle &ptrUser) const;

    TCARegionData*  FindTCARegionData(const LinkList<TCARegionData> *ptrTCADataList,
                                      double regionNumber) const;

    static inline string GetClassName();

    inline bool IsMostValuableRegion(double regionScore,double mostValuableRegionScore) const; //,
    //RAGUSA 1/11/2018 - don't need anymore
    //const bool &foundCrisisRegion,
    //const bool &crisisRegion) const;

    shared_ptr<DataProviderCollection> ptrDataProviderCollection_a;

    static const int  CRISIS_MISSION_NUMBER_s;
};

inline
bool
TimelineGenerator::IsMostValuableRegion(double regionScore,
                                        double mostValuableRegionScore) const
//RAGUSA 1/11/2018 - don't need anymore
//const bool &foundCrisisRegion,
//const bool &crisisRegion) const
{
    bool mostValuableRegion = false;

    // if this is the most valuable region score so far
    if (regionScore > mostValuableRegionScore) {
        //RAGUSA 1/11/2018 - Yeah, no, not anymore. Make all regions compete. If this
        //timestep has a crisis *area* in-view then the MVR will get the multiplier later.
        //old comment:if no crisis region found yet or a crisis region itself, then
        //old comment: this is your region
        //if ((foundCrisisRegion == false) || (crisisRegion == true)) {
        //    mostValuableRegion = true;
        //}
        mostValuableRegion = true;
    }

    //old comment:if this is the first crisis region found, then this is your region
    //old comment:else if ((foundCrisisRegion == false) && (crisisRegion == true)) {
    //}
    //===============================================================================
    return(mostValuableRegion);
}

inline
string
TimelineGenerator::GetClassName()
{
    return(TIMELINE_GENERATOR_H);
}

#endif

