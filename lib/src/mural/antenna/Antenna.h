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
 * Antenna.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#ifndef ANTENNA_H
#define ANTENNA_H "Antenna"

#include <iostream>
#include <fstream>
#include <string>

#include "AntennaBase.h"
#include "mural/modules/lg/Link.h"

#include "general/exception/InputException.h"
#include "general/array/Array2D.h"
#include "general/utility/Constant.h"
#include "general/array/LinkList.h"
#include "general/data/DefaultedData.h"
#include "general\utility\Utility.h"
#include "mural/MURALTypes.h"

using namespace std;

class AzimuthAngleData;
class CoordinateFrame;
class InputData;
class InputDataElement;
class Link;
class LinkConstraintData;
class SpaceVector;
class AntennaCoordinateFrame;
class Element;

/**
 * Antenna is the child to AntennaBase.  Where AntennaBase contains geometric
 *     methods, Antenna contains common resource allocation methods
 *     (index assignment, constraint generation, etc.)
 *
 * Antenna is the parent class created and inherited by one of the
 *     following three child classes: UserAntenna, RelayAntenna, or
 *     ReceiveFacilityAntenna.  This maintains the data that is common to all
 *     three children.
 *
 * Class Attribute Descriptions
 *
 * Name                             Description (units)
 * -------------                    --------------------------------------------
 * frequencyBand_a                  frequency band at which this antenna
 *                                    transmits or receives data (X-band,
 *                                    S-band, W-band, Kt-band, etc...)
 * capacity_a                       number of antenna of this type
 * preferencedCapacity_a            the capacity to be assigned the majority of
 *                                    the allocated time (0 indicates not used)
 * previousCapacity_a               the total capacity of identical type antenna
 *                                    coming before this instance
 * ptrCapacityArray_a               array of capacities at each time step used
 *                                    to store outages
 * ptrAllocatedAssetArray_a         array (time x capacity) of the asset number
 *                                    allocated to this antenna
 * preparationTime_a                amount of time for the antenna to slew and
 *                                    prepare to make acquisition (may be done
 *                                    out of view)
 * dataRate_a                       rate at which data is tranmitted or received
 *
 * minimumGimbalStopAngle_a         minimum azimuth angle (X-Y plane) the
 *                                    antenna may travel
 * maximumGimbalStopAngle_a         maximum azimuth angle (X-Y plane) the
 *                                    antenna may travel
 * antennaDesignator_a              name associated with this antenna
 * ptrDedicatedTransmitterArray_a   array of dedicated tranmsitter designators
 * ptrPrecludedReceiverArray_a      array of precluded receiver designators
 * ptrAzimuthAngleDataList_a        link list of AzimuthAngleData instances
 * ptrLinksList_a*                  link list of Link instances
 * ptrOutageTimePeriodList_a        link list of OutageTimePeriod instances
 *
 * ptrLinkConstraintData_a          object which holds the LinkConstraintData instances.  It is
 *                                  a DefaultedData object which holds an instance for the defaul
 *                                  link parameters and any link parameters specific to
 *                                  a given designator
 *
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class Antenna : public AntennaBase
{
public:

    enum FREQUENCY_BAND {
        UNKNOWN_BAND = -1,
        X_BAND = 0,
        MA_X_BAND = 1,
        S_BAND = 2,
        MA_S_BAND = 3,
        V_BAND = 4,
        K_BAND = 5,
        MA_K_BAND = 6,
        KU_BAND = 7,
        MA_KU_BAND = 8,
        KA_BAND = 9,
        MA_KA_BAND = 10,
        UHF = 11,
        VHF = 12,
        EHF = 13,
        LASER = 14,
        MA_LASER = 15,
        NUMBER_OF_FREQUENCY_BANDS = 16
    };

    Antenna();
    Antenna(shared_ptr<Element> ptrElement,
            int previousCapacity);
    virtual ~Antenna();

    Antenna& operator =  (const Antenna &copyMe);
    bool     operator == (const Antenna &compareMe);
    bool     operator != (const Antenna &compareMe);

    void   GetAzimuthBasedElevationAngles(double azimuthAngle,
                                          const string &designator,
                                          double &minimumElevationAngle,
                                          double &maximumElevationAngle) const;

    void   MakeBestFitAllocatedAssetAssignments(bool assetIsTransmitter = false,
            int switchThreshold = 0);

    void   PrintAttributes(ostream &outfile) const;

    bool   CheckCapacityEmpty(int timeIndex) const;

    bool   CheckCapacityFull(int timeIndex) const;

    bool   CheckCapacityFull(int startTime, int endTime) const;

    bool   CheckNonDedicatedCapacityFull(int timeIndex,
                                         const string &transmitDesignator) const;

    bool   CheckReceiveCapacityFull(int timeIndex) const;

    bool   CheckTransmitCapacityFull(int timeIndex) const;

    bool   FindContactStart(int &timeIndex,
                            int &startContact) const;

    bool   FindLossInContact(int &timeIndex,
                             int &endPreviousContact,
                             int &startCurrentContact) const;

    static inline bool IsConjunctionFrequency(const FREQUENCY_BAND &frequency);

    // Getters
    bool   IsRequiredDownlinkOverheadTime(int timeIndex) const;
	bool   IsAnOutageTimeStep(int timeIndex,
		                      const string &designatorString) const;
    bool   IsThereAnInterval(int timeIndex) const;
    bool   IsThereAnyLink(int timeIndex) const;
    bool   IsThereALinkTo(const string &receiveDesignator,
                          int timeIndex) const;
    bool   IsThereALinkFrom(const string &transmitDesignator,
                            int timeIndex) const;
    bool   IsAllocated(int timeIndex) const;
    bool   IsAllocatedTransmitPrep(int timeIndex) const;
    bool   IsAllocatedReceivePrep(int timeIndex) const;
    bool   IsAllocatedPreparation(int timeIndex) const;
    bool   IsAllocatedNonPrepTime(int timeIndex) const;
    bool   IsAllocatedOverhead(int timeIndex) const;
    bool   IsAllocatedMission(int timeIndex) const;
    bool   IsInNarrowbandContact(int timeIndex) const;
    bool   IsAllocatedBuffer(int timeIndex) const;
    bool   IsAllocatedAcquisition(int timeIndex) const;
    bool   IsAllocatedDroplink(int timeIndex) const;
    bool   IsAllocatedStateOfHealth(int timeIndex) const;
    bool   IsAllocatedWideband(int timeIndex) const;
    bool   ShouldCheckConjunction(const string &transmitDesignator,
                                  int timeIndex) const;
    bool   IsDedicatedTransmitter(const string &transmitterDesignator) const;
    bool   IsPrecludedReceiver(const string &receiverDesignator) const;

    int  GetPreferencedCapacity() const;
    int  GetMinimumCapacity() const;
    int  GetMinimumCapacity(int startTime,
                            int endTime) const;
    int  GetMaximumCapacity(int startTime,
                            int endTime) const;
    int  GetMinimumAcquisitionTime() const;
    int  GetMaximumAcquisitionTime() const;
    int  GetMinimumDroplinkTime() const;
    int  GetMaximumDroplinkTime() const;
    int  GetMaximumOverheadTime() const;
    int  GetAllocatedAssetNumber(int timeIndex,
                                 int capacityIndex) const;
    int  GetNumberOfAllocatedAssets(int timeIndex) const;
    int  GetNumberOfReceivePrepAllocations(int timeIndex) const;
    int  GetNumberOfOverheadAllocations(int timeIndex) const;
    int  GetNumberOfAllocatedTransmitPrepIC(int timeIndex) const;
    int  GetNumberOfAllocatedTransmitPrepOOC(int timeIndex) const;
    double GetMaximumElevationAngle(const string &designator) const;
    double GetMinimumElevationAngle(const string &designator) const;
    double GetMinimumGrazingAltitude(const string &designator) const;
    double GetMaximumRange(const string &designator) const;
    double GetMinimumRange(const string &designator) const;
    double GetMaximumSunSeparationAngle(const string &designator) const;
    double GetMinimumSunSeparationAngle(const string &designator) const;

    Link*  GetAllocatedLinkTo(const string &receiveDesignator,
                              int timeIndex) const;
    Link*  GetLinkTo(const string &receiveDesignator) const;
    Link*  GetLinkTo(const string &receiveDesignator,
                     const string &antennaDesignator) const;
    Link*  GetLinkFrom(const string &transmitDesignator) const;
    Link*  GetLinkFrom(const string &transmitDesignator,
                       const string &antennaDesignator) const;
    Link*  GetAllocatedLink(int timeIndex) const;

    Array<string>* GetMultiLinksArray() const;
    shared_ptr<MURALTypes::AzimuthAngleDataVector> GetAzimuthAngleDataList() const;

    inline int GetCapacity() const;
    inline int GetCapacity(int timeIndex) const;

    inline int GetNumberOfDedicatedTransmitters() const;

    inline int GetNumberOfPrecludedReceivers() const;
    inline int GetNumberOfLinks() const;
    inline int GetAllocatedCapacity(int timeIndex,
                                    int assetNumber) const;

    inline int   GetPreparationTime() const;

    inline double GetDataRate() const;

    inline bool   OutsideOfGimbalStop(double gimbalStopAngle) const;
    inline bool   HasPreferencedCapacity() const;
    inline bool   HasAzimuthBasedElevationAngles() const;
    inline bool   IsConjunctionAntenna() const;
    inline bool   IsNonFullyDedicatedTransmitter(int timeIndex,
            const string &transmitterDesignator) const;

    inline FREQUENCY_BAND GetFrequencyBand() const;
    inline string GetFrequencyBandString() const;
    inline string GetBasicFrequencyString() const;

    inline const shared_ptr<vector<string>> GetDedicatedTransmitterArray() const;
    inline const shared_ptr<vector<string>> GetPrecludedReceiverArray() const;

    inline const MURALTypes::LinkConstraintDataPtr             GetLinkConstraintData(const string &designator) const;
    inline const bool                                          HasLinkConstraintData(const string &designator) const;
    inline const LinkList<Link>*                               GetLinksList() const;

    /**
     * Method cast the element on Antenna base to a DataProvider
     * (for MURAL compatability)
     *
     * @return shared_ptr<DataProvider>
     */
    shared_ptr<DataProvider> GetDataProvider() const;

    // Setters
    void SetFrequencyBand(const FREQUENCY_BAND &frequency);
    void SetCapacity(int capacity);
    void SetPreferencedCapacity(int preferencedCapacity);
    void SetPreparationTime(int preparationTime);
    void SetDataRate(double dataRate);
    void SetMaximumGimbalStopAngle(double maximumGimbalStopAngle);
    void SetMinimumGimbalStopAngle(double minimumGimbalStopAngle);
    void SetAzimuthAngleDataList(const shared_ptr<MURALTypes::AzimuthAngleDataVector> &ptrAzimuthAngleDataList);
    void SetDedicatedTransmitterArray(const shared_ptr<vector<string>> &ptrDedicatedTransmitterArray);
    void SetPrecludedReceiver(const string &receiverDesignator);
    void SetPrecludedReceiverArray(const shared_ptr<vector<string>> &ptrPrecludedReceiverArray);

    /**
     * Sets the default Link Constraint data on the unique DefaultData object the antenna owns
     */
    void SetDefaultLinkConstraintData(const shared_ptr<LinkConstraintData> &ptrDefaultData);

    /**
     * Sets the specific designator Link Constraint data on the unique DefaultData object the antenna owns
     */
    void SetSpecificLinkConstraintData(const string &designator, const shared_ptr<LinkConstraintData> &ptrSpecificData);

    inline void  AppendLink(Link *ptrLink);

    // Virtual stuff
    virtual inline void DetermineTransmitLink(const string &transmitDesignator);

    virtual inline bool AcquisitionIsWideband() const;
    virtual inline bool StartUpIsWideband() const;
    virtual inline bool DroplinkIsWideband() const;
    virtual inline bool IsPrecludedUser(const string &userDesignator) const;

    virtual inline double GetTransmitDataRate(int timeIndex,
            int resourceIndex) const;

    virtual inline bool IsTransmittingAntenna() const;
    virtual inline bool IsReceiveAntenna() const;

    virtual void   BuybackAllocatedTimeStep(int timeIndex) const;

    virtual void   SetAllocationStatus(int startAllocation,
                                       int endAllocation);

    virtual int  GetNumberOfAssetsModeled() const;
    virtual int  GetDataProviderIndex() const;
    virtual int  GetDataProviderNumber() const;

    virtual double GetCollectionDataRate(int timeIndex) const;
    virtual double GetMaximumCollectionDataRate() const;

    virtual bool   ConstrainMaximumThroughput() const;
    virtual bool   MaximumRelayThroughputProblem(int timeIndex,
            double dataRate) const;
    virtual bool   PrecludeSimultaneousAcquisitions() const;
    virtual bool   UseTransmitDataRateInThroughput() const;

    virtual char   GetIdentification(int capacityNumber) const;

    virtual Link*         GetAllocatedDownlink(int timeIndex) const;

    virtual LinkList<Link>* GetCrosslinks(const string &userDesignator) const;

protected:

    FREQUENCY_BAND                frequencyBand_a;
    int                           capacity_a;
    int                           preferencedCapacity_a;
    int                           previousCapacity_a;
    int                           preparationTime_a;
    double                        dataRate_a;
    double                        minimumGimbalStopAngle_a;
    double                        maximumGimbalStopAngle_a;
    shared_ptr<vector<int>>       ptrCapacityArray_a;
    shared_ptr<vector<string>>    ptrDedicatedTransmitterArray_a;
    shared_ptr<vector<string>>    ptrPrecludedReceiverArray_a;

    shared_ptr<MURALTypes::AzimuthAngleDataVector>    ptrAzimuthAngleDataList_a;

    unique_ptr<DefaultedData<MURALTypes::LinkConstraintDataPtr>> ptrLinkConstraintData_a;

    LinkList<Link>               *ptrLinksList_a;
    Array2D<int>                 *ptrAllocatedAssetArray_a;

private:

    static inline string GetClassName();

    static const char             ASSIGNMENT_INTERVAL_ID_s;
};

inline
bool
Antenna::AcquisitionIsWideband() const
{
    return(false);
}

inline
bool
Antenna::StartUpIsWideband() const
{
    return(false);
}

inline
bool
Antenna::DroplinkIsWideband() const
{
    return(false);
}

inline
bool
Antenna::IsPrecludedUser(const string &userDesignator) const
{
    return(false);
}

inline
void
Antenna::DetermineTransmitLink(const string &transmitDesignator)
{
    return;
}

inline
void
Antenna::AppendLink(Link *ptrLink)
{
    if (ptrLinksList_a == NULL) {
        ptrLinksList_a = new LinkList<Link>();
    }

    ptrLinksList_a->Append(ptrLink);
    DetermineTransmitLink(ptrLink->GetTransmitDesignator());
}

inline
string
Antenna::GetFrequencyBandString() const
{
    string returnString = "UNKNOWN_BAND";

    if (frequencyBand_a == X_BAND) {
        returnString = "X_BAND";
    } else if (frequencyBand_a == MA_X_BAND) {
        returnString = "MA_X_BAND";
    } else if (frequencyBand_a == S_BAND) {
        returnString = "S_BAND";
    } else if (frequencyBand_a == MA_S_BAND) {
        returnString = "MA_S_BAND";
    } else if (frequencyBand_a == V_BAND) {
        returnString = "V_BAND";
    } else if (frequencyBand_a == K_BAND) {
        returnString = "K_BAND";
    } else if (frequencyBand_a == MA_K_BAND) {
        returnString = "MA_K_BAND";
    } else if (frequencyBand_a == KU_BAND) {
        returnString = "KU_BAND";
    } else if (frequencyBand_a == MA_KU_BAND) {
        returnString = "MA_KU_BAND";
    } else if (frequencyBand_a == KA_BAND) {
        returnString = "KA_BAND";
    } else if (frequencyBand_a == MA_KA_BAND) {
        returnString = "MA_KA_BAND";
    } else if (frequencyBand_a == UHF) {
        returnString = "UHF";
    } else if (frequencyBand_a == VHF) {
        returnString = "VHF";
    } else if (frequencyBand_a == EHF) {
        returnString = "EHF";
    } else if (frequencyBand_a == LASER) {
        returnString = "LASER";
    } else if (frequencyBand_a == MA_LASER) {
        returnString = "MA_LASER";
    }

    return(returnString);
}

inline
string
Antenna::GetBasicFrequencyString() const
{
    string returnString = "NA";

    // it is either in the radio frequency (RF) spectrum or Laser Communication (LC)
    if (frequencyBand_a >= LASER) {
        returnString = "LC";
    } else if (frequencyBand_a >= X_BAND) {
        returnString = "RF";
    }

    return(returnString);
}

inline
bool
Antenna::IsConjunctionFrequency(const FREQUENCY_BAND &frequency)
{
    return((frequency != LASER) && (frequency != MA_LASER));
}

inline
const shared_ptr<vector<string>>
                              Antenna::GetDedicatedTransmitterArray() const
{
    return(ptrDedicatedTransmitterArray_a);
}

inline
const shared_ptr<vector<string>>
                              Antenna::GetPrecludedReceiverArray() const
{
    return(ptrPrecludedReceiverArray_a);
}

inline
const MURALTypes::LinkConstraintDataPtr
Antenna::GetLinkConstraintData(const string &designator) const
{
    return(ptrLinkConstraintData_a->GetValue(designator));
}

inline
const bool
Antenna::HasLinkConstraintData(const string &designator) const
{
    return(ptrLinkConstraintData_a->HasSpecificKey(designator));
}

inline
const LinkList<Link>*
Antenna::GetLinksList() const
{
    return(ptrLinksList_a);
}

inline

int
Antenna::GetNumberOfDedicatedTransmitters() const
{
    int dedicatedTransmitters = 0;

    if (ptrDedicatedTransmitterArray_a != NULL) {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        dedicatedTransmitters = Utility::CastSizetToInt(ptrDedicatedTransmitterArray_a->size());
    }

    return(dedicatedTransmitters);
}

inline

int
Antenna::GetNumberOfPrecludedReceivers() const
{
    int precludedReceivers = 0;

    if (ptrPrecludedReceiverArray_a != NULL) {
        //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
        precludedReceivers = Utility::CastSizetToInt(ptrPrecludedReceiverArray_a->size());
    }

    return(precludedReceivers);
}

inline
bool
Antenna::HasPreferencedCapacity() const
{
    return((preferencedCapacity_a > 0) && (preferencedCapacity_a <= capacity_a));
}

inline
bool
Antenna::HasAzimuthBasedElevationAngles() const
{
    return(ptrAzimuthAngleDataList_a != NULL);
}

inline
bool
Antenna::IsConjunctionAntenna() const
{
    return(IsConjunctionFrequency(frequencyBand_a));
}

inline
bool
Antenna::IsNonFullyDedicatedTransmitter(int timeIndex,
                                        const string &transmitterDesignator) const
{
    bool nonFullyDedicatedTransmitter = false;

    if ((GetNumberOfDedicatedTransmitters() == GetCapacity(timeIndex)) &&
            (IsDedicatedTransmitter(transmitterDesignator) == false)) {
        nonFullyDedicatedTransmitter = true;
    }

    return(nonFullyDedicatedTransmitter);
}

inline
bool
Antenna::OutsideOfGimbalStop(double gimbalStopAngle) const
{
    bool outsideOfGimbalStop = false;

    if ((minimumGimbalStopAngle_a != UNDEFINED_ANGLE) &&
            (gimbalStopAngle < minimumGimbalStopAngle_a)) {
        outsideOfGimbalStop = true;
    } else if ((maximumGimbalStopAngle_a != UNDEFINED_ANGLE) &&
               (gimbalStopAngle > maximumGimbalStopAngle_a)) {
        outsideOfGimbalStop = true;
    }

    return(outsideOfGimbalStop);
}

inline
int
Antenna::GetNumberOfLinks() const
{
    int numberOfLinks = 0;

    if (ptrLinksList_a != NULL) {
        numberOfLinks = ptrLinksList_a->Size();
    }

    return(numberOfLinks);
}

inline
int
Antenna::GetAllocatedCapacity(int timeIndex,
                              int assetNumber) const
{
    int allocatedCapacity = 0;

    if (ptrAllocatedAssetArray_a != NULL) {
        int capacityIndex = 0;
        capacityIndex = ptrAllocatedAssetArray_a->FindIndex(timeIndex, assetNumber);

        if (capacityIndex >= 0) {
            allocatedCapacity = capacityIndex + 1;
        } else {
            capacityIndex = ptrAllocatedAssetArray_a->FindIndex(timeIndex, -assetNumber);

            if (capacityIndex >= 0) {
                allocatedCapacity = capacityIndex + 1;
            }
        }

        if (allocatedCapacity > 0) {
            allocatedCapacity += previousCapacity_a;
        }
    }

    return(allocatedCapacity);
}

inline
int
Antenna::GetPreparationTime() const
{
    return(preparationTime_a);
}

inline
double
Antenna::GetDataRate() const
{
    return(dataRate_a);
}

inline
double
Antenna::GetTransmitDataRate(int timeIndex,
                             int resourceIndex) const
{
    return(dataRate_a);
}

inline
bool
Antenna::IsTransmittingAntenna() const
{
    return(false);
}

inline
bool
Antenna::IsReceiveAntenna() const
{
    return(false);
}

inline
int
Antenna::GetCapacity() const
{
    return(capacity_a);
}

inline
int
Antenna::GetCapacity(int timeIndex) const
{
    int returnCapacity = capacity_a;

    if (ptrCapacityArray_a != NULL) {
        returnCapacity = ptrCapacityArray_a->at(timeIndex);
    }

    return(returnCapacity);
}

inline
Antenna::FREQUENCY_BAND
Antenna::GetFrequencyBand() const
{
    return(frequencyBand_a);
}

inline
string
Antenna::GetClassName()
{
    return(ANTENNA_H);
}

#endif
