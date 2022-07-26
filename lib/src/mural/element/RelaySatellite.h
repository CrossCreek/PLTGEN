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
 * RelaySatellite.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#ifndef RELAY_SATELLITE_H
#define RELAY_SATELLITE_H "RelaySatellite"

#include "DataProvider.h"
#include "mural/antenna/Antenna.h"
#include "general\utility\Utility.h"

using namespace std;

class CoordinateFrame;
class GroundPointingAntenna;
class RelayPointingAntenna;
class UserPointingAntenna;

/**
 * This is a child of DataProvider which represnts a relay satellite.  It holds
 * UserPointingAntennas (UPAs) and GroundPointingAntennas (GPAs) which are the "bridge"
 * of the user->relay->ground allocation algorithm.
 *
 * This can also have RelayPointingAntennas for NetworkAllocation algoritms but these
 * are not in the standard MURAL Run.
 *
 * Class Attribute Descriptions
 *
 * Name                            Description (units)
 * -------------                   ---------------------------------------------
 * anyStateOfHealthRelays_s        indicates if any relay in the constellation
 *                                   has state of health constraints
 * RELAY_USE_TIME_STEP_ID_s        identifier for relay use ('r') indices
 * ptrIdentificationArray_s*       static array of relay designators
 * probablisticMaximumThroughput_a throughput determined by  the
 *                                   ProbablisticAllocator
 * bodyTiltAngle_a                 angle the relay body tilts forward (if any)
 * ptrAllocationCriteria_a*        pointer to criteria for resource allocator
 * ptrUPAList_a*                   list of UserPointingAntenna instances
 * ptrGPAList_a*                   list of GroundPointingAntenna instances
 * ptrRPAList_a*                   list of RelayPointingAntenna instances
 * ptrUserConstraintList_a*        list of constraint columns used by the
 *                                   ProbablisticAllocator for gateway relay
 *                                   throughput constraints
 * ptrStateOfHealthPeriodList_a*   list of TimePeriod(s) when a state of health
 *                                   (rise or set) downlink allocation must
 *                                   be made to the designated ground station
 *
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class RelaySatellite : public DataProvider
{
public:

    RelaySatellite();
    virtual ~RelaySatellite();

    RelaySatellite& operator =  (const RelaySatellite &copyMe);
    bool            operator == (const RelaySatellite &compareMe) const;

    static bool   anyStateOfHealthRelays_s;

    static void   ResetIdentification();
    static void   SetIdentification(const Array<string> *ptrIdentification);
    static void   SortIdentificationArray();

    static bool   ConvertDesignatorToIndex(const string &relayDesignator,
                                           int &index);
    static bool   ConvertIndexToDesignator(int index,
                                           string &relayDesignator);

    static string GetDesignatorString(int index);

    static inline int  GetRelaySatelliteCount();

    static inline bool   ValidRelaySatellite(const string &designator);

    static inline const Array<string>* GetIdentificationArray();

    /**
     * Override of virtual method on DataProvider to return the UPA list, (the
     * list to perform conjunction on)
     *
     * @return AntennaVector
     */
    shared_ptr<MURALTypes::AntennaVector> GetConjunctionAntennaList();

    void   AddObjectiveValues(const shared_ptr<vector<double>> &ptrObjectiveArray) const;
    void   AddUserConstraint(int index, double value);
    void   CheckTransmitLinkStatus();
    void   DeleteUserConstraintList();
    void   DetermineUtilizationNumbers(int &utilizedMiss,
                                       int &utilizedDiss,
                                       int &utilizedPrepTimeIC,
                                       int &utilizedPrepTimeOOC,
                                       int &utilizedOverhead,
                                       int &utilizedUserOverhead,
                                       int &stateOfHealth,
                                       int &availableMiss,
                                       int &availableDiss,
                                       const bool &countDissTime) const;

    void   PrintAttributes(ostream &outfile) const;
    void   PrintGroundPointingAntennaUsage(ofstream &outfile,
                                           int timeIndex) const;
    void   PrintUserPointingAntennaUsage(ofstream &outfile,
                                         int timeIndex) const;
    void   PrintUserPointingAntennaUsage(ofstream &outfile,
                                         int timeIndex,
                                         const Antenna::FREQUENCY_BAND &frequencyBand) const;
    void   PrintExcessTime(ofstream &outfile,
                           const bool &countNarrowbandDownlinkTime) const;

    void   ResetForAnotherPass();
    void   SetMinimumNumberOfTransmitChannels(const bool &constrainMaximumThroughput);

    int  GetGroundPointingAntennaCapacity() const;
    int  GetTransmitRelayPointingAntennaCapacity() const;
    int  GetMinimumDownlinkAcquisitionTime() const;
    int  GetMinimumDownlinkDroplinkTime() const;
    int  GetMinimumNumberOfHops(Array<int> *ptrVisitedRelayIndicies = NULL) const;
    int  GetTotalTransmitCapacity() const;
    int  GetUserPointingAntennaCapacity() const;

    int   GetMaximumPreparationTimeUPA() const;
    int   GetNumberOfReceiveAntenna() const;
    int   GetTotalNumberOfTransmitChannels() const;

    double GetMaximumTransmitDataRate() const;
    double GetMaximumReceiveDataRate() const;
    double GetTotalAllocatedDataRate(int timeIndex) const;

    bool   DownlinkingNarrowband(int timeIndex) const;
    bool   DownlinkingWideband(int timeIndex) const;
    bool   DownlinkIsAllocatedAcquisition(int timeIndex) const;
    bool   DownlinkIsAllocatedDroplink(int timeIndex) const;
    bool   DownlinkIsAllocatedStateOfHealth(int timeIndex) const;
    bool   DownlinkIsAllocated(int timeIndex) const;
    bool   HasConjunctionGPA() const;
    bool   HasReceiveRPA() const;
    bool   HasTransmitRPA() const;
    bool   HasDownlink(int timeIndex) const;
    bool   HasAnyGroundStationInView(int timeIndex) const;
    bool   HasAnyResouceInView(int timeIndex,
                               Array<string> *ptrCheckedRelayArray) const;
    bool   HasAnyUserInView(int timeIndex) const;
    bool   HasUserAllocated(int timeIndex) const;
    bool   IsAllocatedStateOfHealth(int timeIndex) const;
    bool   IsHighlyElliptical() const;
    bool   IsRequiredDownlinkOverheadTime(int timeIndex) const;
    bool   MakingContactForDiss(int timeIndex) const;

    Array<double>* GetChannelDataRates() const;

    Array<bool>*   GetRequiredDownlinkOverheadTimeArray() const;

    shared_ptr<GroundPointingAntenna> GetGPA(const string &antennaDesignator) const;
    shared_ptr<UserPointingAntenna>   GetUPA(const string &antennaDesignator, int &upaIndex) const;

    Link*         GetAllocatedDownlink(int timeIndex) const;

    LinkList<Link>* GetCrosslinks(const string &userDesignator) const;

    inline void   SetProbabilisticMaximumThroughput(int newThroughput);

    inline int   GetIndex() const;


    inline int  GetNumberOfGPAs() const;

    inline int  GetNumberOfUPAs() const;
    inline int   GetProbabilisticMaximumThroughput() const;

    inline double GetBodyTiltAngle() const;

    inline bool   IsADownlinkRelay() const;
    inline bool   IsAMultihopRelay() const;
    inline bool   IsAGatewayRelay() const;
    inline bool   IsARemoteRelay() const;

    inline shared_ptr<MURALTypes::GPAVector>        GetGroundPointingAntennaList() const;
    inline shared_ptr<MURALTypes::RPAVector>        GetRelayPointingAntennaList() const;
    inline shared_ptr<MURALTypes::UPAVector>        GetUserPointingAntennaList() const;

    inline LinkList<TimePeriod>*            GetStateOfHealthPeriodList() const;
    /**
     * Setters
     */
    void SetBodyTiltAngle(double bodyTiltAngle);
    void SetUPAList(const shared_ptr<MURALTypes::UPAVector> &ptrUPAList);
    void SetRPAList(const shared_ptr<MURALTypes::RPAVector> &ptrRPAList);
    void SetGPAList(const shared_ptr<MURALTypes::GPAVector> &ptrGPAList);
protected:

private:

    static inline string GetClassName();

    static const char     RELAY_USE_TIME_STEP_ID_s;
    static Array<string> *ptrIdentificationArray_s;

    int                                      probablisticMaximumThroughput_a;
    double                                   bodyTiltAngle_a;
    LinkList<TimePeriod>                    *ptrStateOfHealthPeriodList_a;

    shared_ptr<MURALTypes::UPAVector>   ptrUPAList_a;
    shared_ptr<MURALTypes::RPAVector>   ptrRPAList_a;
    shared_ptr<MURALTypes::GPAVector>   ptrGPAList_a;
};

inline

int
RelaySatellite::GetNumberOfGPAs() const
{
    int numberOfGPAs = 0;

    if (ptrGPAList_a != NULL) {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        numberOfGPAs = Utility::CastSizetToInt(ptrGPAList_a->size());
    }

    return(numberOfGPAs);
}

inline

int
RelaySatellite::GetNumberOfUPAs() const
{
    int numberOfUPAs = 0;

    if (ptrUPAList_a != NULL) {
        numberOfUPAs = (int) ptrUPAList_a->size();
    }

    return((int) numberOfUPAs);
}

inline
int
RelaySatellite::GetIndex() const
{
    return(ptrIdentificationArray_s->FindIndex(GetDesignator()));
}

inline
bool
RelaySatellite::IsADownlinkRelay() const
{
    return(ptrGPAList_a != NULL);
}

inline
bool
RelaySatellite::IsAMultihopRelay() const
{
    return(ptrRPAList_a != NULL);
}

inline
bool
RelaySatellite::IsAGatewayRelay() const
{
    return((ptrGPAList_a != NULL) && (HasReceiveRPA() == true));
}

inline
bool
RelaySatellite::IsARemoteRelay() const
{
    return((ptrRPAList_a != NULL) && (ptrGPAList_a == NULL));
}

inline
shared_ptr<MURALTypes::GPAVector>
RelaySatellite::GetGroundPointingAntennaList() const
{
    return(ptrGPAList_a);
}

inline
shared_ptr<MURALTypes::UPAVector>
RelaySatellite::GetUserPointingAntennaList() const
{
    return(ptrUPAList_a);
}

inline
shared_ptr<MURALTypes::RPAVector>
RelaySatellite::GetRelayPointingAntennaList() const
{
    return(ptrRPAList_a);
}

inline
int
RelaySatellite::GetRelaySatelliteCount()
{
    int numberOfRelays = 0;

    if (ptrIdentificationArray_s != NULL) {
        numberOfRelays = ptrIdentificationArray_s->Size();
    }

    return(numberOfRelays);
}

inline
bool
RelaySatellite::ValidRelaySatellite(const string &designator)
{
    bool validRelay = false;

    if (ptrIdentificationArray_s != NULL) {
        validRelay = ptrIdentificationArray_s->ValueExists(designator);
    }

    return(validRelay);
}

inline
const Array<string>*
RelaySatellite::GetIdentificationArray()
{
    return(ptrIdentificationArray_s);
}

inline
LinkList<TimePeriod>*
RelaySatellite::GetStateOfHealthPeriodList() const
{
    return(ptrStateOfHealthPeriodList_a);
}

inline
int
RelaySatellite::GetProbabilisticMaximumThroughput() const
{
    return(probablisticMaximumThroughput_a);
}

inline
double
RelaySatellite::GetBodyTiltAngle() const
{
    return(bodyTiltAngle_a);
}

inline
void
RelaySatellite::SetProbabilisticMaximumThroughput(int newThroughput)
{
    probablisticMaximumThroughput_a = newThroughput;
}

inline
string
RelaySatellite::GetClassName()
{
    return(RELAY_SATELLITE_H);
}

#endif
