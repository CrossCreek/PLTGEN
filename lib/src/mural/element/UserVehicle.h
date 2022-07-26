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
 * UserVehicle.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef USER_VEHICLE_H
#define USER_VEHICLE_H "UserVehicle"

#include "DataProvider.h"

#include "general/array/ArrayOfPointers.h"
#include "general/array/Array3D.h"
#include "mural/antenna/UserTransmitAntenna.h"
#include "mural/antenna/Antenna.h"
#include "mural/antenna/DirectDownlinkAntenna.h"
#include "mural/antenna/SelfRelayAntenna.h"
#include "mural/modules/vtg/TimelineCriteria.h"

using namespace std;

class CollectionElement;
class CoordinateFrame;
class RegionData;
class RelaySatellite;
class ValueTimeline;
class ValueTimelineElement;

/**
 * This is a child of DataProvider which represnts a user vehicle satellite.  It holds
 * UserTransmitAntennas (UTAs) which are the "origin"
 * of the user->relay->ground allocation algorithm.
 *
 * The user also has Sensors which "collect" and compute the score for the allocation
 * algorithm.
 *
 * This can also have SelfRelayAntennas for NetworkAllocation algoritms but these
 * are not in the standard MURAL Run.
 *
 * Class Attribute Descriptions
 *
 * Name                              Description (units)
 * -------------                     -------------------------------------------
 * anyDirectDownlinkUsers_s          if any user has direct downlink capability
 * anyInAndOutOfContactUsers_s       if any user is an IAOOC user
 * anyMakeBeforeBreakUsers_s         if any vehicle is an MBB vehicle
 * anyGenerateDumpUsers_s            if any user wants to generate DUMP requests
 *                                     based on allocation losses
 * ptrIdentificationArray_s*         array of all user designators
 * vehiclePriority_a                 priority of the user vehicle
 * minimumProcessingRevisitTime_a    minimum duration a region must be inview
 *                                     to be used for revisit
 * ptrAllocationStartsPerRev_a*      number of allocation starts in each rev
 * numberOfScoreProfiles_a           number of score profiles for this user,
 *                                     typically 1 or the number of relays
 * dailyVehicleDutyCycle_a           amount of time the vehicle may have a
 *                                     sensor active during a 24 hour period
 * ptrRollingRevDutyCycle_a          array of duty cycles on a rolling rev basis
 * bufferMemorySize_a                size (in Gbits) of on-board buffer
 * collectionEfficiency_a            efficiency factor at which the sensor(s)
 *                                     collect data at
 * effectiveBufferEmptyRate_a        effective data rate (Gbps) the buffer
 *                                     empties at (accounts for encoding)
 * worthNormalizationX_a             component used to convert score to worth
 * worthNormalizationY_a             exponent used to convert score to worth
 * ptrAzimuthOffsetAngles_a*         the specific azimuth angles (if any) the
 *                                     vehicle may be at when collection targets
 * ptrLossTimeStepID_a*              loss ID at each time index
 * bodyTiltsForCollection_a          whether or not the vehicle body tilts when
 *                                     pointing at a target for collection
 * multipleRequestDurations_a        indicates if any CSR for this user has
 *                                     multiple request durations (typically
 *                                     BUFF or DUMP CSRs)
 * sunBatheFlag_a                    if the user sun bathes when not requesting
 * useMaximumAllocationsPerRequest_a if the user has any CSRs with a maximum
 *                                     number or RSPs constraint
 * ptrGroupName_a*                   string containing this user's group name
 *                                     used for worth normalization and grouping
 *                                     satisfaction
 * ptrTargetDeckFileName_a*          name of the target deck to be used for
 *                                     generating a value timeline
 * ptrTrailVehicle_a*                pointer to a trail vehicle (if any)
 * ptrTimelineCriteria_a*            pointer to criteria for generating the
 *                                     value timeline
 * ptrRequestCriteria_a*             pointer to criteria for generating requests
 * ptrAllocationCriteria_a*          pointer to criteria for allocating this
 *                                     resource against others
 * ptrSchedulingCriteria_a*          pointer to criteria for scheduling targets
 *                                     or requirements
 * ptrUserTransmitAntenna_a*         pointer to the transmit antenna
 * ptrDirectDownlinkAntenna_a*       pointer to a direct downlink antenna
 * ptrValueTimeline_a                pointer to the user's value timeline
 * ptrUserType_a*                    pointer to a UserVehicleType instance
 *                                     containing all vehicles of this Type
 * ptrUserSystem_a*                  pointer to a UserVehicleType instance
 *                                     containing all vehicles of this System
 * ptrSelfRelayAntennaList_a*        pointer to the list of Self Relay Antennas
 * ptrRequestList_a*                 list of CommunicationServiceRequest
 *                                     instances containing all CSR information
 * ptrArrayOfSchedulingData_a*       array of SchedulingData allocated by time
 *                                     if the target scheduler was run
 * ptrAllocationPathList_a*          list of all possible NetworkAllocationPath
 *                                     instances (for NetworkAllocator)
 *
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class UserVehicle : public DataProvider
{
public:

    enum MBB_VEHICLES_MODELED {
        LEAD_VEHICLE           = 0,
        TRAIL_VEHICLE          = 1,
        NUMBER_OF_MBB_VEHICLES = 2
    };



    explicit UserVehicle(shared_ptr<UserVehicle> ptrTrailVehicle = NULL);

    virtual ~UserVehicle();

    UserVehicle&    operator =  (const UserVehicle &copyMe);
    bool            operator == (const UserVehicle &compareMe) const;
    bool            operator != (const UserVehicle &compareMe) const;

    /**
     * Static constants
     */
    static const char     REAL_TIME_REQUEST_ID_s;
    static const char     FLOATING_REQUEST_ID_s;
    static const char     STORE_REQUEST_ID_s;
    static const char     OVERLAPPING_REQUEST_ID_s;
    static const char     REAL_TIME_LOSS_ID_s;
    static const char     FLOATING_LOSS_ID_s;
    static const char     VISIBILITY_LOSS_ID_s;
    static const char     HANDOVER_LOSS_ID_s;
    static const char     TYPE_NARROWBAND_LOSS_ID_s;
    static const char     TYPE_WIDEBAND_LOSS_ID_s;
    static const char     SYSTEM_NARROWBAND_LOSS_ID_s;
    static const char     SYSTEM_WIDEBAND_LOSS_ID_s;
    static const char     WIDEBAND_CONJUNCTION_LOSS_ID_s;
    static const char     NARROWBAND_CONJUNCTION_LOSS_ID_s;
    static const char     STATE_OF_HEALTH_LOSS_ID_s;
    static const char     MAKE_BEFORE_BREAK_ID_s;
    static const char     DIRECT_DOWNLINK_TIME_STEP_ID_s;

    static bool           anyDirectDownlinkUsers_s;
    static bool           anyInAndOutOfContactUsers_s;
    static bool           anyMakeBeforeBreakUsers_s;
    static bool           anyGenerateDumpUsers_s;
    static Array<string> *ptrIdentificationArray_s;

    static void   RemoveTrailVehicleIdentification(Array<string> *ptrTrailDesignatorArray);
    static void   ReportScheduleBurdenNumbers(ofstream &outfile,
            const int &necessaryOverhead,
            const int &excessOverhead);

    static void   ResetIdentification();
    static void   SetIdentification(const Array<string> *ptrIdentification);
    static void   SortIdentificationArray();

    static bool   ConvertDesignatorToIndex(const string &userDesignator,
                                           int &index);
    static bool   ConvertIndexToDesignator(const int &index,
                                           string &userDesignator);

    void   AddRevisitInviewTime(Array3D<bool> *ptrInviewTime) const;
    void   AddValueTimelineElement(const int &timeIndex,
                                   ValueTimelineElement *ptrNewElement);

    void   CheckTransmitLinkStatus();
    void   DetermineMaximumTimelineScore();

    void   PrintAttributes(ostream &outfile) const;
	void   RemoveRegionData(const int &timeIndex) const;
    void   SetNumberOfScoreProfiles(const int &numberOfProfiles);
    void   SetWidebandTransmission(const Link::WIDEBAND_TRANSMISSION &transmit) const;
    void   UpdateRollingRevs();

    int  GetCrisisLevel(const int &timeIndex) const;
    int  GetMissionNumber(const int &timeIndex,const int &resourceIndex) const;
    int  GetSubPriorityLevel(const int &timeIndex,
                             const int &resourceIndex) const;
    int  GetTimelineSensorNumber(const int &timeIndex,
                                 int resourceIndex = -1) const;

    int   GetNumberOfResources() const;
    int   GetNumberOfTimelineResources() const;
    int   GetDeckIndex(const Sensor *ptrSensor) const;
    int   GetDutyCycle(int revIndex = -1) const;
    int   PrintMakeBeforeBreaks(ofstream &outfile) const;

    double  GetImagedRegion(const int &timeIndex,
                            int resourceIndex = -1) const;
    double  GetOverflownRegion(const int &timeIndex) const;
    double  GetScore(const int &timeIndex,
                     int resourceIndex = -1) const;
    double  GetObjectiveValueScore(const int &timeIndex,
                                   int resourceIndex = -1) const;
    double  GetMaximumTimelineScore() const;
    double  GetCollectionTime(const int &timeIndex,
                              int resourceIndex = -1) const;
    double  GetResourceFactor(const int &timeIndex,
                              const int &resourceIndex) const;

    double CalculateElevationAngle(const int &timeIndex,
                                   const int &resourceIndex) const;
    double CalculateElevationAngle(const int &timeIndex,
                                   const int &resourceIndex,
                                   const double &regionImaging) const;
    double ConvertScoreToWorth(const double &score) const;
    double GetAvailableBitsInDuration(const int &duration) const;
    double GetMaximumCollectionDataRate(const int &timeIndex) const;
    double GetMaximumCollectionDataRate(const int &startTimeIndex,
                                        const int &endTimeIndex) const;
    double GetMinimumCrossLinkDataRate() const;
    //RAGUSA 11/21/2017 - Added method to include direct downlinks as possible minimum link rate
    double GetMinimumDirectDownLinkDataRate() const;
    double GetMinimumOuterConeElevation() const;
    double GetMaxPossibleBitsCollected(const int &timeIndex,
                                       const int &resourceIndex) const;
    double GetEstimatedBitsCollected(const int &startTime,
                                     const int &endTime,
                                     int resourceIndex = -1) const;
    double GetMaximumSensorCollectingDataRate(const int &startTime,
            const int &endTime,
            int resourceIndex = -1) const;
    double GetMaximumTransmitDataRate() const;
    double GetMaximumReceiveDataRate() const;

    bool   ContainsRequirementDeck(const string &deckName) const;
    bool   ConvertResourceDesignatorToIndex(const string &resourceString,
                                            int &resourceIndex) const;
    bool   GetResourceInview(const int &timeIndex) const;
    bool   HasMultipleRequestDurations() const;
    bool   HasReceiveSRA() const;
    bool   HasTransmitSRA() const;
    bool   NeedToResetTimelineRequirements(const int &timeIndex) const;
    bool   UseDutyCycle() const;
    bool   UseRollingRevs() const;
	void   UserVehicle::GetCrisisData(const int &crisisNumber, int &timeInview) const;
    char   GetResourceCharacter(const int &timeIndex, const int &capacityIndex) const;

    string GetResourceDesignator(const int &resourceIndex) const;

    const Antenna::FREQUENCY_BAND GetDirectDownlinkFrequencyBand() const;
    const Antenna::FREQUENCY_BAND GetTransmitFrequencyBand() const;

    Sensor*            GetSensorCollecting(const int &timeIndex,
                                           int resourceIndex = -1) const;

    Link*              GetLinkTo(const string &resourceDesignator) const;
    Link*              GetLinkTo(const int &resourceIndex) const;
    Link*              GetAllocatedCrosslink(const string &relayDesignator,
            const int &timeIndex) const;
    Link*              GetCrosslink(const string &relayDesignator) const;
    Link*              GetCrosslink(const string &relayDesignator,
                                    const string &antennaDesignator) const;
    Link*              GetAllocatedDirectDownlink(const string &groundDesignator,
            const int &timeIndex) const;
    Link*              GetDirectDownlink(const string &groundDesignator) const;
    Link*              GetDirectDownlink(const string &groundDesignator,
                                         const string &antennaDesignator) const;
    Link*              GetTransmitSelfRelayLink(const string &userDesignator,
            const string &antennaDesignator) const;
    Link*              GetReceiveSelfRelayLink(const string &userDesignator,
            const string &antennaDesignator) const;
    Link*              GetAllocatedTransmitSelfRelayLink(const string &userDesignator,
            const int &timeIndex) const;
    Link*              GetTransmitSelfRelayLink(const string &userDesignator) const;
    Link*              GetReceiveSelfRelayLink(const string &userDesignator) const;

    ValueTimelineElement* GetValueTimelineElement(const int &timeIndex) const;

    RegionData*        GetTimelineMostValuableRegionData(const int &timeIndex) const;

    LinkList<Link>*    GetAllTransmitLinks() const;

    LinkList<RegionData>* GetTimelineRegionDataList(const int &timeIndex) const;

    void   VerifyTargetDeck();

    static inline int   GetUserVehicleCount();
    static inline int   GetUserVehicleIndex(const string &designator);

    static inline bool   ValidUserVehicle(const string &designator);

    static inline string GetDesignatorString(const int &index);
    static inline string GetMakeBeforeBreakDesignator(const int &index);

    static inline const Array<string>* GetIdentificationArray();

    inline void   AddUserType(const shared_ptr<UserGroupConstraint> &ptrType);
    inline void   AddUserSystem(const shared_ptr<UserGroupConstraint> &ptrSystem);
    inline void   SetCalculatedWorthNormalizationX(const double &calculatedX);
    inline void   SetCalculatedWorthNormalizationY(const double &calculatedY);
    inline void   RemoveLossID(const int &timeIndex);
    inline void   SetHandoverLoss(const int &startTime,
                                  const int &endTime);
    inline void   SetMultipleRequestDurations(const bool &multipleDurations);
    inline void   UserTypeIsDeleted();
    inline void   UserSystemIsDeleted();

    inline char   GetLossTimeStepID(const int &timeIndex) const;

    inline bool   BodyTiltsForCollection() const;
    inline bool   GenerateResourceSpecificTimeline() const;
    inline bool   IsAMakeBeforeBreakVehicle() const;
    inline bool   IsADirectDownlinkVehicle() const;
    inline bool   IsATransmitSelfRelayingVehicle() const;
    inline bool   IsACrosslinkVehicle() const;
    inline bool   IsARevisitVehicle() const;
    inline bool   IsConductingMakeBeforeBreak(const int &timeIndex) const;
    inline bool   IsAllocatedPreparation(const int &timeIndex) const;
    inline bool   IsAllocatedOverhead(const int &timeIndex) const;
    inline bool   IsAllocatedMission(const int &timeIndex) const;
    inline bool   IsVehicleAllocatedMission(const int &timeIndex) const;
    inline bool   IsAllocatedBuffer(const int &timeIndex) const;
    inline bool   IsSunBathingVehicle() const;
    inline bool   IsThereAnyLink(const int &timeIndex) const;
    inline bool   IsTransmitFixedGimbal() const;
    inline bool   IsUserTransmitAllocated(const int &timeIndex) const;
    inline bool   NeedToDetermineRegionData() const;
    inline bool   PrecludeReceiver(const string &resourceDesignator) const;
    inline bool   ShouldPrintMetricFactors() const;
    inline bool   UseMaximumAllocationsPerRequest() const;
    inline bool   UseDeck() const;
    inline bool   UseTargetDeck() const;
    inline bool   UseRequirementDeck() const;
    inline bool   UseBothDecks() const;
    inline bool   UseElevationFactors() const;
    inline bool   UsingTargetDeck(const string &targetDeckName) const;

    inline shared_ptr<SelfRelayAntenna>      FindSelfRelayAntenna(const string &antennaDesignator) const;

    /**
     * Getters
     */
    inline int  GetMaximumAcquisitionTime() const;
    inline int  GetMinimumAcquisitionTime() const;
    inline int  GetMaximumDroplinkTime() const;
    inline int  GetMinimumDroplinkTime() const;
    inline int  GetMaximumOverheadTime() const;
    inline int  GetNumberOfVehiclesModeled() const;
    inline int  GetPriority() const;
    inline int  GetMinimumProcessingRevisitTime() const;
    inline int  GetUserTransmitCapacity() const;
    inline int  GetDirectDownlinkCapacity() const;
    inline int  GetSelfRelayCapacity() const;
    inline int  GetTotalCapacity() const;

    inline int   GetIndex() const;
    inline int   GetTimeToCollectReset() const;
    inline int   GetNumberOfElevationFactors() const;
    inline int   GetMinimumPreparationTime() const;
    inline int   GetMaximumPreparationTime() const;

    inline int   GetNumberOfInContactPrep(const int &timeIndex) const;
    inline int   GetNumberOfOutOfContactPrep(const int &timeIndex) const;
    inline int   GetNumberOfOverheadAllocations(const int &timeIndex) const;
    inline int   GetNumberOfScoreProfiles() const;
    inline int   GetTotalNumberOfTransmitingSelfRelayAntenna(const string receiveTech) const;
    inline int   GetTotalNumberOfReceiveSelfRelayAntenna(const string &frequencyBand) const;
	//RAGUSA 11/27/2018
	inline int GetServiceTierDefinition() const;

    inline double  GetElevationFactor(const double &elevationAngle,
                                      int position = 0) const;

    inline double GetBufferMemorySize() const;

    inline double GetCollectionEfficiency() const;
    inline double GetEffectiveBufferEmptyRate() const;
    inline double GetMaximumDataRate() const;
    inline double GetMinimumDataRate() const;
    inline double GetTransmitDataRate() const;
    inline double GetFlatScorePerTime() const;
    inline double GetMinAscendingOffRegion(const double &longitude) const;
    inline double GetMinDescendingOffRegion(const double &longitude) const;
    inline double GetMaxAscendingOffRegion(const double &longitude) const;
    inline double GetMaxDescendingOffRegion(const double &longitude) const;
    inline double GetWorthNormalizationX() const;
    inline double GetWorthNormalizationY() const;

    inline Array<double>* GetAzimuthOffsetAngles() const;
    inline Array<double>* GetCableWrapOffsetAngles() const;

    inline string GetGroupName() const;

    inline string GetTargetDeckFileName() const;

    inline TimelineCriteria::TIMELINE_METHOD GetTimelineGenerationMethod() const;

    inline shared_ptr<ValueTimeline>         GetValueTimeline() const;

    inline shared_ptr<UserTransmitAntenna>   GetUserTransmitAntenna() const;
    inline shared_ptr<DirectDownlinkAntenna> GetDirectDownlinkAntenna() const;
    inline shared_ptr<MURALTypes::SRAVector> GetSelfRelayAntennaList() const;

    inline shared_ptr<UserVehicle>           GetTrailVehicle() const;

    inline const LinkList<Link>*  GetTransmitLinksList() const;
    inline const LinkList<Link>*  GetDirectDownlinksList() const;

    inline shared_ptr<UserGroupConstraint>   GetUserVehicleType();
    inline shared_ptr<UserGroupConstraint>   GetUserVehicleSystem();
    inline Array<int>*                       GetAllocationStartsPerRev();

    shared_ptr<string> GetGroupNamePtr() const;
    shared_ptr<string> GetTargetDeckFileNamePtr() const;

    shared_ptr<TimelineCriteria>       GetTimelineCriteria() const;

    /**
     * Setters
     */
    void SetGroupName(shared_ptr<string> ptrGroup);
    void SetBodyTiltsForCollection(const bool &bodyTiltsForCollection);
    void SetTargetDeckFileName(shared_ptr<string> ptrTargetDeck);
	//RAGUSA 11/27/2018
	void SetServiceTierDefinition(int serviceTierDefinition);
    void SetDailyVehicleDutyCycle(int dailyVehicleDutyCycle);
    void SetRollingRevDutyCycle(Array<int> *ptrRollingRev);
    void SetCollectionEfficiency(double collectionEfficiency);
    void SetBufferMemorySize(double bufferMemorySize);
    void SetEffectiveBufferEmptyRate(double effectiveBufferEmptyRate);
    void SetWorthNormCoeffX(double worthNormX);
    void SetWorthNormCoeffY(double worthNormY);
    void SetAzimuthOffsetAngles(Array<double> *ptrAzimuthOffsetAngles);
    void SetVehiclePriority(int vehiclePriority);
    void SetMinimumProcessingRevisitTime(int minimumProcessingRevisitTime);
    void SetSunBatheFlag(bool sunBatheFlag);
    void SetUserTransmitAntenna(const shared_ptr<UserTransmitAntenna> &ptrUTA);
    void SetDirectDownlinkAntenna(const shared_ptr<DirectDownlinkAntenna> &ptrDDA);
    void SetSelfRelayAntennaList(const shared_ptr<MURALTypes::SRAVector> &ptrSelfRelayAntennaList);
    void SetTimelineCriteria(const shared_ptr<TimelineCriteria> &ptrTimelineCriteria);

protected:

private:

    void   AddRevisitTimes(Array3D<bool> *ptrUserInviewTime,
                           const int &startTimeIndex,
                           const int &endTimeIndex,
                           const int &numberOfSubRegions,
                           const int &numberOfTimeSteps) const;

    bool   PossibleNarrowbandConjunctionProblem(const int &timeIndex) const;
    bool   PossibleWidebandConjunctionProblem(const int &startTimeIndex,
            const int &endTimeIndex) const;
    bool   PossibleStateOfHealthProblem(const int &timeIndex) const;

    static inline string GetClassName();

    int                   vehiclePriority_a;
    int                   minimumProcessingRevisitTime_a;
    int                   numberOfScoreProfiles_a;
	//RAGUSA 11/27/2018
	int                   serviceTierDefinition_a;
    int                   dailyVehicleDutyCycle_a;
    double                  bufferMemorySize_a;
    double                  collectionEfficiency_a;
    double                  effectiveBufferEmptyRate_a;
    double                  worthNormalizationX_a;
    double                  worthNormalizationY_a;
    bool                    bodyTiltsForCollection_a;
    bool                    multipleRequestDurations_a;
    bool                    sunBatheFlag_a;
    bool                    useMaximumAllocationsPerRequest_a;

    Array<int>             *ptrAllocationStartsPerRev_a;
    Array<int>             *ptrRollingRevDutyCycle_a;
    Array<double>          *ptrAzimuthOffsetAngles_a;
    Array<char>            *ptrLossTimeStepID_a;

    shared_ptr<TimelineCriteria>        ptrTimelineCriteria_a;
    shared_ptr<ValueTimeline>           ptrValueTimeline_a;

    shared_ptr<UserTransmitAntenna>     ptrUserTransmitAntenna_a;
    shared_ptr<DirectDownlinkAntenna>   ptrDirectDownlinkAntenna_a;
    shared_ptr<MURALTypes::SRAVector>   ptrSelfRelayAntennaList_a;

    shared_ptr<UserGroupConstraint>        ptrUserType_a;
    shared_ptr<UserGroupConstraint>        ptrUserSystem_a;

    shared_ptr<UserVehicle>  ptrTrailVehicle_a;
    shared_ptr<string>       ptrGroupName_a;
    shared_ptr<string>       ptrTargetDeckFileName_a;
};

inline
void
UserVehicle::AddUserType(const shared_ptr<UserGroupConstraint> &ptrType)
{
    ptrUserType_a = ptrType;
    return;
}

inline
void
UserVehicle::AddUserSystem(const shared_ptr<UserGroupConstraint> &ptrSystem)
{
    ptrUserSystem_a = ptrSystem;
    return;
}

inline
void
UserVehicle::UserTypeIsDeleted()
{
    ptrUserType_a = NULL;
    return;
}

inline
void
UserVehicle::UserSystemIsDeleted()
{
    ptrUserSystem_a = NULL;
    return;
}

inline
shared_ptr<UserGroupConstraint>
UserVehicle::GetUserVehicleType()
{
    return ptrUserType_a;
}

inline
shared_ptr<UserGroupConstraint>
UserVehicle::GetUserVehicleSystem()
{
    return ptrUserSystem_a;
}

inline
Array<int>*
UserVehicle::GetAllocationStartsPerRev()
{
    return ptrAllocationStartsPerRev_a;
}

inline
void
UserVehicle::SetCalculatedWorthNormalizationX(const double &calculatedX)
{
    worthNormalizationX_a = calculatedX;
    return;
}

inline
void
UserVehicle::SetCalculatedWorthNormalizationY(const double &calculatedY)
{
    worthNormalizationY_a = calculatedY;
    return;
}

inline
void
UserVehicle::RemoveLossID(const int &timeIndex)
{
    if (ptrLossTimeStepID_a != NULL) {
        (*ptrLossTimeStepID_a)[timeIndex] = BLANK_CHARACTER;
    }

    return;
}

inline
void
UserVehicle::SetHandoverLoss(const int &startTime,
                             const int &endTime)
{
    int timeIndex = startTime;

    if (ptrLossTimeStepID_a != NULL) {
        while (timeIndex <= endTime) {
            (*ptrLossTimeStepID_a)[timeIndex] = HANDOVER_LOSS_ID_s;
            ++timeIndex;
        }
    }

    return;
}

inline
void
UserVehicle::SetMultipleRequestDurations(const bool &multipleDurations)
{
    multipleRequestDurations_a = multipleDurations;
}

inline
int
UserVehicle::GetPriority() const
{
    return(vehiclePriority_a);
}

inline
int
UserVehicle::GetMinimumProcessingRevisitTime() const
{
    return(minimumProcessingRevisitTime_a);
}

inline
int
UserVehicle::GetUserTransmitCapacity() const
{
    int userTransmitCapacity = 0;

    if (ptrUserTransmitAntenna_a != NULL) {
        userTransmitCapacity = ptrUserTransmitAntenna_a->GetCapacity();
    }

    return(userTransmitCapacity);
}

inline
int
UserVehicle::GetDirectDownlinkCapacity() const
{
    int directDownlinkCapacity = 0;

    if (ptrDirectDownlinkAntenna_a != NULL) {
        directDownlinkCapacity = ptrDirectDownlinkAntenna_a->GetCapacity();
    }

    return(directDownlinkCapacity);
}

inline
int
UserVehicle::GetSelfRelayCapacity() const
{
    int selfRelayCapacity = 0;
    MURALTypes::SRAVector::iterator sraIter;
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
        ptrSRA = (*sraIter);
        selfRelayCapacity += ptrSRA->GetCapacity();
    }

    return(selfRelayCapacity);
}

inline
int
UserVehicle::GetTotalCapacity() const
{
    int totalCapacity = 0;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        totalCapacity = ptrUserTransmitAntenna_a->GetCapacity();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        totalCapacity += ptrDirectDownlinkAntenna_a->GetCapacity();
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = *sraIter;
            totalCapacity += ptrSRA->GetCapacity();
        }
    }

    return(totalCapacity);
}

inline
int
UserVehicle::GetNumberOfVehiclesModeled() const
{
    int numberOfVehiclesModeled = 1;

    if (GetUserTransmitCapacity() == (int)NUMBER_OF_MBB_VEHICLES) {
        numberOfVehiclesModeled = (int)NUMBER_OF_MBB_VEHICLES;
    }

    return(numberOfVehiclesModeled);
}

inline
int
UserVehicle::GetMaximumAcquisitionTime() const
{
    int acquisitionTime = 0;
    int maximumAcquisitionTime = 0;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        maximumAcquisitionTime = ptrUserTransmitAntenna_a->GetMaximumAcquisitionTime();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        acquisitionTime = ptrDirectDownlinkAntenna_a->GetMaximumAcquisitionTime();

        if (acquisitionTime > maximumAcquisitionTime) {
            maximumAcquisitionTime = acquisitionTime;
        }
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = *sraIter;
            acquisitionTime = ptrSRA->GetMaximumAcquisitionTime();

            if (acquisitionTime > maximumAcquisitionTime) {
                maximumAcquisitionTime = acquisitionTime;
            }
        }
    }

    if (ptrTrailVehicle_a != NULL) {
        acquisitionTime = ptrTrailVehicle_a->GetMaximumAcquisitionTime();

        if (acquisitionTime > maximumAcquisitionTime) {
            maximumAcquisitionTime = acquisitionTime;
        }
    }

    return(maximumAcquisitionTime);
}

inline
int
UserVehicle::GetMinimumAcquisitionTime() const
{
    int acquisitionTime = 0;
    int minimumAcquisitionTime = SHRT_MAX;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        minimumAcquisitionTime = ptrUserTransmitAntenna_a->GetMinimumAcquisitionTime();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        acquisitionTime = ptrDirectDownlinkAntenna_a->GetMinimumAcquisitionTime();

        if (acquisitionTime < minimumAcquisitionTime) {
            minimumAcquisitionTime = acquisitionTime;
        }
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);
            acquisitionTime = ptrSRA->GetMinimumAcquisitionTime();

            if (acquisitionTime < minimumAcquisitionTime) {
                minimumAcquisitionTime = acquisitionTime;
            }
        }
    }

    if (ptrTrailVehicle_a != NULL) {
        acquisitionTime = ptrTrailVehicle_a->GetMinimumAcquisitionTime();

        if (acquisitionTime < minimumAcquisitionTime) {
            minimumAcquisitionTime = acquisitionTime;
        }
    }

    return(minimumAcquisitionTime);
}

inline
int
UserVehicle::GetMaximumDroplinkTime() const
{
    int droplinkTime = 0;
    int maximumDroplinkTime = 0;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        maximumDroplinkTime = ptrUserTransmitAntenna_a->GetMaximumDroplinkTime();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        droplinkTime = ptrDirectDownlinkAntenna_a->GetMaximumDroplinkTime();

        if (droplinkTime > maximumDroplinkTime) {
            maximumDroplinkTime = droplinkTime;
        }
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);
            droplinkTime = ptrSRA->GetMaximumDroplinkTime();

            if (droplinkTime > maximumDroplinkTime) {
                maximumDroplinkTime = droplinkTime;
            }
        }
    }

    if (ptrTrailVehicle_a != NULL) {
        droplinkTime = ptrTrailVehicle_a->GetMaximumDroplinkTime();

        if (droplinkTime > maximumDroplinkTime) {
            maximumDroplinkTime = droplinkTime;
        }
    }

    return(maximumDroplinkTime);
}

inline
int
UserVehicle::GetMinimumDroplinkTime() const
{
    int droplinkTime = 0;
    int minimumDroplinkTime = SHRT_MAX;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        minimumDroplinkTime = ptrUserTransmitAntenna_a->GetMinimumDroplinkTime();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        droplinkTime = ptrDirectDownlinkAntenna_a->GetMinimumDroplinkTime();

        if (droplinkTime < minimumDroplinkTime) {
            minimumDroplinkTime = droplinkTime;
        }
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);
            droplinkTime = ptrSRA->GetMinimumDroplinkTime();

            if (droplinkTime < minimumDroplinkTime) {
                minimumDroplinkTime = droplinkTime;
            }
        }
    }

    if (ptrTrailVehicle_a != NULL) {
        droplinkTime = ptrTrailVehicle_a->GetMinimumDroplinkTime();

        if (droplinkTime < minimumDroplinkTime) {
            minimumDroplinkTime = droplinkTime;
        }
    }

    return(minimumDroplinkTime);
}

inline
int
UserVehicle::GetMaximumOverheadTime() const
{
    int overheadTime = 0;
    int maximumOverheadTime = 0;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        maximumOverheadTime = ptrUserTransmitAntenna_a->GetMaximumOverheadTime();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        overheadTime = ptrDirectDownlinkAntenna_a->GetMaximumOverheadTime();

        if (overheadTime > maximumOverheadTime) {
            maximumOverheadTime = overheadTime;
        }
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);
            overheadTime = ptrSRA->GetMaximumOverheadTime();

            if (overheadTime > maximumOverheadTime) {
                maximumOverheadTime = overheadTime;
            }
        }
    }

    if (ptrTrailVehicle_a != NULL) {
        overheadTime = ptrTrailVehicle_a->GetMaximumOverheadTime();

        if (overheadTime > maximumOverheadTime) {
            maximumOverheadTime = overheadTime;
        }
    }

    return(maximumOverheadTime);
}

inline
int
UserVehicle::GetMinimumPreparationTime() const
{
    int minimumPrepTime = LONG_MAX;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        minimumPrepTime = ptrUserTransmitAntenna_a->GetPreparationTime();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        if (ptrDirectDownlinkAntenna_a->GetPreparationTime() < minimumPrepTime) {
            minimumPrepTime = ptrDirectDownlinkAntenna_a->GetPreparationTime();
        }
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);

            if (ptrSRA->GetPreparationTime() < minimumPrepTime) {
                minimumPrepTime = ptrSRA->GetPreparationTime();
            }
        }
    }

    return(minimumPrepTime);
}

inline
int
UserVehicle::GetMaximumPreparationTime() const
{
    int maximumPrepTime = 0;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        maximumPrepTime = ptrUserTransmitAntenna_a->GetPreparationTime();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        if (ptrDirectDownlinkAntenna_a->GetPreparationTime() > maximumPrepTime) {
            maximumPrepTime = ptrDirectDownlinkAntenna_a->GetPreparationTime();
        }
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);

            if (ptrSRA->GetPreparationTime() > maximumPrepTime) {
                maximumPrepTime = ptrSRA->GetPreparationTime();
            }
        }
    }

    return(maximumPrepTime);
}

inline
int
UserVehicle::GetIndex() const
{
    return(ptrIdentificationArray_s->FindIndex(GetDesignator()));
}

inline
int
UserVehicle::GetUserVehicleCount()
{
    int numberOfUsers = 0;

    if (ptrIdentificationArray_s != NULL) {
        numberOfUsers = ptrIdentificationArray_s->Size();
    }

    return(numberOfUsers);
}

inline
int
UserVehicle::GetUserVehicleIndex(const string &designator)
{
    int userIndex = -1;

    if (ptrIdentificationArray_s != NULL) {
        userIndex = ptrIdentificationArray_s->FindIndex(designator);
    }

    return(userIndex);
}

inline
int
UserVehicle::GetTimeToCollectReset() const
{
    return(ptrTimelineCriteria_a->GetTimeToCollectReset());
}

inline
int
UserVehicle::GetNumberOfElevationFactors() const
{
    return(ptrTimelineCriteria_a->GetNumberOfElevationFactors());
}

inline
int
UserVehicle::GetNumberOfScoreProfiles() const
{
    return (numberOfScoreProfiles_a);
}
inline
int
UserVehicle::GetNumberOfInContactPrep(const int &timeIndex) const
{
    int inContactPrep = 0;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        inContactPrep = ptrUserTransmitAntenna_a->GetNumberOfAllocatedTransmitPrepIC(timeIndex);
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        inContactPrep += ptrDirectDownlinkAntenna_a->GetNumberOfAllocatedTransmitPrepIC(timeIndex);
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);
            inContactPrep += ptrSRA->GetNumberOfAllocatedTransmitPrepIC(timeIndex);
        }
    }

    if (ptrTrailVehicle_a != NULL) {
        inContactPrep += ptrTrailVehicle_a->GetNumberOfInContactPrep(timeIndex);
    }

    return(inContactPrep);
}

inline
int
UserVehicle::GetNumberOfOutOfContactPrep(const int &timeIndex) const
{
    int outOfContactPrep = 0;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        outOfContactPrep = ptrUserTransmitAntenna_a->GetNumberOfAllocatedTransmitPrepOOC(timeIndex);
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        outOfContactPrep += ptrDirectDownlinkAntenna_a->GetNumberOfAllocatedTransmitPrepOOC(timeIndex);
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);
            outOfContactPrep += ptrSRA->GetNumberOfAllocatedTransmitPrepOOC(timeIndex);
        }
    }

    if (ptrTrailVehicle_a != NULL) {
        outOfContactPrep += ptrTrailVehicle_a->GetNumberOfOutOfContactPrep(timeIndex);
    }

    return(outOfContactPrep);
}

inline
int
UserVehicle::GetNumberOfOverheadAllocations(const int &timeIndex) const
{
    int overheadAllocations = 0;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        overheadAllocations = ptrUserTransmitAntenna_a->GetNumberOfOverheadAllocations(timeIndex);
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        overheadAllocations += ptrDirectDownlinkAntenna_a->GetNumberOfOverheadAllocations(timeIndex);
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);
            overheadAllocations += ptrSRA->GetNumberOfOverheadAllocations(timeIndex);
        }
    }

    if (ptrTrailVehicle_a != NULL) {
        overheadAllocations += ptrTrailVehicle_a->GetNumberOfOverheadAllocations(timeIndex);
    }

    return(overheadAllocations);
}

inline
double
UserVehicle::GetElevationFactor(const double &elevationAngle,
                                int position) const
{
    return(ptrTimelineCriteria_a->GetElevationFactor(elevationAngle, position));
}

inline
double
UserVehicle::GetFlatScorePerTime() const
{
    return(ptrTimelineCriteria_a->GetFlatScorePerTime());
}

inline
double
UserVehicle::GetMinAscendingOffRegion(const double &longitude) const
{
    return(ptrTimelineCriteria_a->GetMinAscendingOffRegion(longitude));
}

inline
double
UserVehicle::GetMinDescendingOffRegion(const double &longitude) const
{
    return(ptrTimelineCriteria_a->GetMinDescendingOffRegion(longitude));
}

inline
double
UserVehicle::GetMaxAscendingOffRegion(const double &longitude) const
{
    return(ptrTimelineCriteria_a->GetMaxAscendingOffRegion(longitude));
}

inline
double
UserVehicle::GetMaxDescendingOffRegion(const double &longitude) const
{
    return(ptrTimelineCriteria_a->GetMaxDescendingOffRegion(longitude));
}

inline
double
UserVehicle::GetWorthNormalizationX() const
{
    return(worthNormalizationX_a);
}

inline
double
UserVehicle::GetWorthNormalizationY() const
{
    return(worthNormalizationY_a);
}

inline
double
UserVehicle::GetMaximumDataRate() const
{
    double maximumDataRate = 0.0;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        maximumDataRate = ptrUserTransmitAntenna_a->GetDataRate();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        if (ptrDirectDownlinkAntenna_a->GetDataRate() > maximumDataRate) {
            maximumDataRate = ptrDirectDownlinkAntenna_a->GetDataRate();
        }
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);

            if (ptrSRA->GetDataRate() > maximumDataRate) {
                maximumDataRate = ptrSRA->GetDataRate();
            }
        }
    }

    return(maximumDataRate);
}

inline
double
UserVehicle::GetMinimumDataRate() const
{
    double minimumDataRate = DBL_MAX;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        minimumDataRate = ptrUserTransmitAntenna_a->GetDataRate();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        if (ptrDirectDownlinkAntenna_a->GetDataRate() < minimumDataRate) {
            minimumDataRate = ptrDirectDownlinkAntenna_a->GetDataRate();
        }
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);

            if (ptrSRA->GetDataRate() < minimumDataRate) {
                minimumDataRate = ptrSRA->GetDataRate();
            }
        }
    }

    return(minimumDataRate);
}

inline
double
UserVehicle::GetTransmitDataRate() const
{
    double transmitDataRate = 0.0;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        transmitDataRate = ptrUserTransmitAntenna_a->GetDataRate();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        transmitDataRate += ptrDirectDownlinkAntenna_a->GetDataRate();
    }

    if (ptrSelfRelayAntennaList_a != NULL) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);
            transmitDataRate += ptrSRA->GetDataRate();
        }
    }

    return(transmitDataRate);
}

inline
double
UserVehicle::GetBufferMemorySize() const
{
    return(bufferMemorySize_a);
}

inline
double
UserVehicle::GetCollectionEfficiency() const
{
    return(collectionEfficiency_a);
}

inline
double
UserVehicle::GetEffectiveBufferEmptyRate() const
{
    return(effectiveBufferEmptyRate_a);
}

inline
Array<double>*
UserVehicle::GetAzimuthOffsetAngles() const
{
    return(ptrAzimuthOffsetAngles_a);
}

inline
Array<double>*
UserVehicle::GetCableWrapOffsetAngles() const
{
    Array<double> *ptrCableWrapOffsetAngle = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        ptrCableWrapOffsetAngle = ptrUserTransmitAntenna_a->GetCableWrapOffsetAngles();
    }

    return(ptrCableWrapOffsetAngle);
}

inline
bool
UserVehicle::IsTransmitFixedGimbal() const
{
    bool transmitFixedGimbal = false;

    if (ptrUserTransmitAntenna_a != NULL) {
        transmitFixedGimbal = ptrUserTransmitAntenna_a->IsFixedGimbal();
    }

    return(transmitFixedGimbal);
}

inline
bool
UserVehicle::BodyTiltsForCollection() const
{
    return(bodyTiltsForCollection_a);
}

inline
bool
UserVehicle::IsSunBathingVehicle() const
{
    return(sunBatheFlag_a);
}

inline
bool
UserVehicle::IsADirectDownlinkVehicle() const
{
    return(ptrDirectDownlinkAntenna_a != NULL);
}

inline
bool
UserVehicle::IsATransmitSelfRelayingVehicle() const
{
    bool isATranmitSelfRelayingVehicle = false;
    SelfRelayAntenna *ptrSRA = NULL;

    if ( ptrSelfRelayAntennaList_a != NULL ) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (isATranmitSelfRelayingVehicle == false); ++sraIter) {
            ptrSRA = (*sraIter);
            isATranmitSelfRelayingVehicle = ptrSRA->IsTransmittingAntenna();
        }
    }

    return(isATranmitSelfRelayingVehicle);
}

inline
bool
UserVehicle::IsACrosslinkVehicle() const
{
    return(ptrUserTransmitAntenna_a != NULL);
}

inline
bool
UserVehicle::ValidUserVehicle(const string &designator)
{
    bool validUser = false;

    if (ptrIdentificationArray_s != NULL) {
        validUser = ptrIdentificationArray_s->ValueExists(designator);
    }

    return(validUser);
}

inline
bool
UserVehicle::GenerateResourceSpecificTimeline() const
{
    return(ptrTimelineCriteria_a->GenerateResourceSpecificTimeline());
}

inline
bool
UserVehicle::UseDeck() const
{
    return(ptrTimelineCriteria_a->UseDeck());
}

inline
bool
UserVehicle::UseTargetDeck() const
{
    return(ptrTimelineCriteria_a->UseTargetDeck());
}

inline
bool
UserVehicle::UseRequirementDeck() const
{
    return(ptrTimelineCriteria_a->UseRequirementDeck());
}

inline
bool
UserVehicle::UseBothDecks() const
{
    return(ptrTimelineCriteria_a->UseBothDecks());
}

inline
bool
UserVehicle::UseElevationFactors() const
{
    return(ptrTimelineCriteria_a->UseElevationFactors());
}

inline
bool
UserVehicle::ShouldPrintMetricFactors() const
{
    return((GenerateResourceSpecificTimeline() == true) ||
           (UseElevationFactors() == true));
}

inline
bool
UserVehicle::NeedToDetermineRegionData() const
{
    bool needTo = false;

    if (ptrTimelineCriteria_a->GetTimelineGenerationMethod()
            != TimelineCriteria::READ_TIMELINE) {
        needTo = ptrTimelineCriteria_a->UseDeck();
    }
    return(needTo);
}


inline
bool
UserVehicle::IsARevisitVehicle() const
{
    return(minimumProcessingRevisitTime_a > 0);
}

inline
bool
UserVehicle::UseMaximumAllocationsPerRequest() const
{
    return(useMaximumAllocationsPerRequest_a);
}

inline
bool
UserVehicle::IsAMakeBeforeBreakVehicle() const
{
    return( (ptrTrailVehicle_a != NULL) ||
            (GetUserTransmitCapacity() == (int)NUMBER_OF_MBB_VEHICLES) );
}

inline
bool
UserVehicle::IsThereAnyLink(const int &timeIndex) const
{
    bool isThereAnyLink = false;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        isThereAnyLink = ptrUserTransmitAntenna_a->IsThereAnyLink(timeIndex);
    }

    if ((isThereAnyLink == false) && (ptrDirectDownlinkAntenna_a != NULL)) {
        isThereAnyLink = ptrDirectDownlinkAntenna_a->IsThereAnyLink(timeIndex);
    }

    if ((isThereAnyLink == false) && (ptrSelfRelayAntennaList_a != NULL)) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (isThereAnyLink == false); ++sraIter) {
            ptrSRA = (*sraIter);
            isThereAnyLink = ptrSRA->IsThereAnyLink(timeIndex);
        }
    }

    if ((isThereAnyLink == false) && (ptrTrailVehicle_a != NULL)) {
        isThereAnyLink = ptrTrailVehicle_a->IsThereAnyLink(timeIndex);
    }

    return(isThereAnyLink);
}

inline
bool
UserVehicle::IsUserTransmitAllocated(const int &timeIndex) const
{
    bool isAllocated = false;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        isAllocated = ptrUserTransmitAntenna_a->IsAllocated(timeIndex);
    }

    if ((isAllocated == false) && (ptrDirectDownlinkAntenna_a != NULL)) {
        isAllocated = ptrDirectDownlinkAntenna_a->IsAllocated(timeIndex);
    }

    if ((isAllocated == false) && (ptrSelfRelayAntennaList_a != NULL)) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (isAllocated == false); ++sraIter) {
            ptrSRA = (*sraIter);
            isAllocated = ptrSRA->IsAllocated(timeIndex);
        }
    }

    return(isAllocated);
}

inline
bool
UserVehicle::IsAllocatedPreparation(const int &timeIndex) const
{
    bool isAllocatedPreparation = false;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        isAllocatedPreparation = ptrUserTransmitAntenna_a->IsAllocatedTransmitPrep(timeIndex);
    }

    if ((isAllocatedPreparation == false) && (ptrDirectDownlinkAntenna_a != NULL)) {
        isAllocatedPreparation = ptrDirectDownlinkAntenna_a->IsAllocatedTransmitPrep(timeIndex);
    }

    if ((isAllocatedPreparation == false) && (ptrSelfRelayAntennaList_a != NULL)) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (isAllocatedPreparation == false); ++sraIter) {
            ptrSRA = (*sraIter);
            isAllocatedPreparation = ptrSRA->IsAllocatedTransmitPrep(timeIndex);
        }
    }

    if ((isAllocatedPreparation == false) && (ptrTrailVehicle_a != NULL)) {
        isAllocatedPreparation = ptrTrailVehicle_a->IsAllocatedPreparation(timeIndex);
    }

    return(isAllocatedPreparation);
}

inline
bool
UserVehicle::IsAllocatedOverhead(const int &timeIndex) const
{
    bool isAllocatedOverhead = false;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        isAllocatedOverhead = ptrUserTransmitAntenna_a->IsAllocatedOverhead(timeIndex);
    }

    if ((isAllocatedOverhead == false) && (ptrDirectDownlinkAntenna_a != NULL)) {
        isAllocatedOverhead = ptrDirectDownlinkAntenna_a->IsAllocatedOverhead(timeIndex);
    }

    if ((isAllocatedOverhead == false) && (ptrSelfRelayAntennaList_a != NULL)) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (isAllocatedOverhead == false); ++sraIter) {
            ptrSRA = (*sraIter);
            isAllocatedOverhead = ptrSRA->IsAllocatedOverhead(timeIndex);
        }
    }

    if ((isAllocatedOverhead == false) && (ptrTrailVehicle_a != NULL)) {
        isAllocatedOverhead = ptrTrailVehicle_a->IsAllocatedOverhead(timeIndex);
    }

    return(isAllocatedOverhead);
}

inline
bool
UserVehicle::IsAllocatedMission(const int &timeIndex) const
{
    bool isAllocatedMission = false;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        isAllocatedMission = ptrUserTransmitAntenna_a->IsAllocatedMission(timeIndex);
    }

    if ((isAllocatedMission == false) && (ptrDirectDownlinkAntenna_a != NULL)) {
        isAllocatedMission = ptrDirectDownlinkAntenna_a->IsAllocatedMission(timeIndex);
    }

    if ((isAllocatedMission == false) && (ptrSelfRelayAntennaList_a != NULL)) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (isAllocatedMission == false); ++sraIter) {
            ptrSRA = (*sraIter);

            if ( ptrSRA->IsTransmittingAntenna() == true ) {
                isAllocatedMission = ptrSRA->IsAllocatedMission(timeIndex);
            }
        }
    }

    if ((isAllocatedMission == false) && (ptrTrailVehicle_a != NULL)) {
        isAllocatedMission = ptrTrailVehicle_a->IsAllocatedMission(timeIndex);
    }

    return(isAllocatedMission);
}

inline
bool
UserVehicle::IsVehicleAllocatedMission(const int &timeIndex) const
{
    bool isVehicleAllocatedMission = false;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        isVehicleAllocatedMission = ptrUserTransmitAntenna_a->IsAllocatedMission(timeIndex);
    }

    if ((isVehicleAllocatedMission == false) && (ptrDirectDownlinkAntenna_a != NULL)) {
        isVehicleAllocatedMission = ptrDirectDownlinkAntenna_a->IsAllocatedMission(timeIndex);
    }

    if ((isVehicleAllocatedMission == false) && (ptrSelfRelayAntennaList_a != NULL)) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (isVehicleAllocatedMission == false); ++sraIter) {
            ptrSRA = (*sraIter);
            isVehicleAllocatedMission = ptrSRA->IsAllocatedMission(timeIndex);
        }
    }

    return(isVehicleAllocatedMission);
}

inline
bool
UserVehicle::IsAllocatedBuffer(const int &timeIndex) const
{
    bool isAllocatedBuffer = false;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        isAllocatedBuffer = ptrUserTransmitAntenna_a->IsAllocatedBuffer(timeIndex);
    }

    if ((isAllocatedBuffer == false) && (ptrDirectDownlinkAntenna_a != NULL)) {
        isAllocatedBuffer = ptrDirectDownlinkAntenna_a->IsAllocatedBuffer(timeIndex);
    }

    if ((isAllocatedBuffer == false) && (ptrSelfRelayAntennaList_a != NULL)) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (isAllocatedBuffer == false); ++sraIter) {
            ptrSRA = (*sraIter);
            isAllocatedBuffer = ptrSRA->IsAllocatedBuffer(timeIndex);
        }
    }

    if ((isAllocatedBuffer == false) && (ptrTrailVehicle_a != NULL)) {
        isAllocatedBuffer = ptrTrailVehicle_a->IsAllocatedBuffer(timeIndex);
    }

    return(isAllocatedBuffer);
}

inline
bool
UserVehicle::PrecludeReceiver(const string &resourceDesignator) const
{
    bool precludeReceiver = false;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        precludeReceiver = ptrUserTransmitAntenna_a->IsPrecludedReceiver(resourceDesignator);
    }

    if ((precludeReceiver == false) && (ptrDirectDownlinkAntenna_a != NULL)) {
        precludeReceiver = ptrDirectDownlinkAntenna_a->IsPrecludedReceiver(resourceDesignator);
    }

    if ((precludeReceiver == false) && (ptrSelfRelayAntennaList_a != NULL)) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (precludeReceiver == false); ++sraIter) {
            ptrSRA = (*sraIter);
            precludeReceiver = ptrSRA->IsPrecludedReceiver(resourceDesignator);
        }
    }

    if ((precludeReceiver == false) && (ptrTrailVehicle_a != NULL)) {
        precludeReceiver = ptrTrailVehicle_a->PrecludeReceiver(resourceDesignator);
    }

    return(precludeReceiver);
}

inline
bool
UserVehicle::UsingTargetDeck(const string &targetDeckName) const
{
    bool usingTargetDeck = false;

    if (ptrTargetDeckFileName_a != NULL) {
        if (*ptrTargetDeckFileName_a == targetDeckName) {
            usingTargetDeck = true;
        }
    }

    return(usingTargetDeck);
}


inline
bool
UserVehicle::IsConductingMakeBeforeBreak(const int &timeIndex) const
{
    bool isConductingMakeBeforeBreak = false;

    if (ptrTrailVehicle_a != NULL) {
        if ( (IsUserTransmitAllocated(timeIndex) == true) &&
                (ptrTrailVehicle_a->IsUserTransmitAllocated(timeIndex) == true) ) {
            isConductingMakeBeforeBreak = true;
        }
    } else if (GetTotalCapacity() > 1) {
        if ((IsAllocatedMission(timeIndex) == true) && (IsAllocatedOverhead(timeIndex) == true)) {
            isConductingMakeBeforeBreak = true;
        }
    }

    return(isConductingMakeBeforeBreak);
}

inline
char
UserVehicle::GetLossTimeStepID(const int &timeIndex) const
{
    char lossID = BLANK_CHARACTER;

    if (ptrLossTimeStepID_a != NULL) {
        lossID = ptrLossTimeStepID_a->ElementAt(timeIndex);
    }

    return(lossID);
}

inline
string
UserVehicle::GetDesignatorString(const int &index)
{
    string designator = "";

    try {
        designator = ptrIdentificationArray_s->ElementAt(index);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetDesignator(const int&)");
        throw;
    }

    return(designator);
}

inline
string
UserVehicle::GetMakeBeforeBreakDesignator(const int &index)
{
    string designator = "UNKNOWN";

    if (index == (int)LEAD_VEHICLE) {
        designator = "LEAD";
    } else if (index == (int)TRAIL_VEHICLE) {
        designator = "TRAIL";
    }

    return(designator);
}

inline
string
UserVehicle::GetGroupName() const
{
    return(*ptrGroupName_a);
}

inline
string
UserVehicle::GetTargetDeckFileName() const
{
    return(*ptrTargetDeckFileName_a);
}

inline
const Array<string>*
UserVehicle::GetIdentificationArray()
{
    return(ptrIdentificationArray_s);
}

inline
TimelineCriteria::TIMELINE_METHOD
UserVehicle::GetTimelineGenerationMethod() const
{
    return(ptrTimelineCriteria_a->GetTimelineGenerationMethod());
}

inline
shared_ptr<UserVehicle>
UserVehicle::GetTrailVehicle() const
{
    return(ptrTrailVehicle_a);
}

inline
const LinkList<Link>*
UserVehicle::GetTransmitLinksList() const
{
    const LinkList<Link> *ptrLinksList = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        ptrLinksList = ptrUserTransmitAntenna_a->GetLinksList();
    }

    return(ptrLinksList);
}

inline
const LinkList<Link>*
UserVehicle::GetDirectDownlinksList() const
{
    const LinkList<Link> *ptrLinksList = NULL;

    if (ptrDirectDownlinkAntenna_a != NULL) {
        ptrLinksList = ptrDirectDownlinkAntenna_a->GetLinksList();
    }

    return(ptrLinksList);
}

inline
shared_ptr<UserTransmitAntenna>
UserVehicle::GetUserTransmitAntenna() const
{
    return(ptrUserTransmitAntenna_a);
}

inline
shared_ptr<DirectDownlinkAntenna>
UserVehicle::GetDirectDownlinkAntenna() const
{
    return(ptrDirectDownlinkAntenna_a);
}

inline
shared_ptr<MURALTypes::SRAVector>
UserVehicle::GetSelfRelayAntennaList() const
{
    return(ptrSelfRelayAntennaList_a);
}

inline
shared_ptr<SelfRelayAntenna>
UserVehicle::FindSelfRelayAntenna(const string &antennaDesignator) const
{
    shared_ptr<SelfRelayAntenna> ptrSRA       = NULL;
    shared_ptr<SelfRelayAntenna> ptrReturnSRA = NULL;
    MURALTypes::SRAVector::iterator sraIter;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (ptrReturnSRA == NULL); ++sraIter) {
        ptrSRA = (*sraIter);

        if ( ptrSRA->GetAntennaDesignator() == antennaDesignator ) {
            ptrReturnSRA = ptrSRA;
        }
    }

    return(ptrReturnSRA);
}

inline
int
UserVehicle::GetTotalNumberOfTransmitingSelfRelayAntenna(const string receiveTech) const
{
    int totalNumberOfTransmitters = 0;
    MURALTypes::SRAVector::iterator sraIter;
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
        ptrSRA = (*sraIter);

        if ( ptrSRA->IsTransmittingAntenna() == true ) {
            totalNumberOfTransmitters++;
        }
    }

    return(totalNumberOfTransmitters);
}

inline
int
UserVehicle::GetTotalNumberOfReceiveSelfRelayAntenna(const string &frequencyBand) const
{
    int                               totalNumberOfReceivers = 0;
    MURALTypes::SRAVector::iterator   sraIter;
    shared_ptr<SelfRelayAntenna>      ptrSRA = NULL;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
        ptrSRA = (*sraIter);

        if (ptrSRA->IsReceiveAntenna() == true) {
            if (frequencyBand == ptrSRA->GetFrequencyBandString()) {
                ++totalNumberOfReceivers;
            }
        }
    }

    return(totalNumberOfReceivers);
}

//RAGUSA 11/27/2018 - Added Attribute to User Vehicle Class
inline
int
UserVehicle::GetServiceTierDefinition() const
{
	return(serviceTierDefinition_a);
}
//=========================================================

inline
shared_ptr<ValueTimeline>
UserVehicle::GetValueTimeline() const
{
    return(ptrValueTimeline_a);
}

inline
string
UserVehicle::GetClassName()
{
    return(USER_VEHICLE_H);
}

#endif

