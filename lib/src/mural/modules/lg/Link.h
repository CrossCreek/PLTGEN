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
 * Link.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef LINK_H
#define LINK_H "Link"

#include <iostream>
#include <iomanip>

#include "mural/modules/lg/BasicLink.h"
#include "general/time/TimePiece.h"
#include "general/exception/Exception.h"

using namespace std;

class Antenna;
class OverheadData;
class SpaceVector;

/**
 * Inherits from BasicLink.  Holds data regarding the allocation status.  Also holds the
 * ResourcePeriods for the link (RequestResourcePeriods for Crosslinks, DownlinkResourcePeriods
 * for downlinks).  There are also attributes that are needed for determining intervals based
 * on the ResourcePeriods (like aquire and drop time)
 *
 *
 * maximumXLProbabilisticSolutionIndex_s highest solution index for a crosslink
 *                                         in the ProbablisticAllocator (all
 *                                         higher values are supporting links)
 * acquisitionTime_a                  time it takes the two Antenna to acquire
 * droplinkTime_a                     time it takes the two Antenna to droplink
 * makeBeforeBreakDroplinkTime_a      time it takes the two Antenna to droplink
 *                                      when the vehicle is conducting an MBB
 * makeBeforeBreakStartUpTime_a       time it takes the two Antenna to 'start up'
 *                                      when the vehicle is conducting an MBB
 * ptrAllocationStatus_a*             array of ALLOCATION_STATUS for each time
 * probablisticSolutionIndex_a        solution index used by ProbablisticAllocator
 * ptrOverlapTransmitTimeStepIndex_a* array by time index of overlapping transmit
 *                                      time step index ('o') used by Interval
 *                                      Allocator.  if the preparation times are
 *                                      equal this is the general (only) index
 *                                      array
 * ptrOverlapReceiveTimeStepIndex_a*  array by time index of overlapping recieve
 *                                      time step index ('O') used by Interval
 *                                      Allocator
 * ptrNetworkMissionIndexArray_a*     array by pixel index of network mission
 *                                      indices
 * ptrNetworkNarrowbandIndexArray_a*  array by pixel index of network narrowband
 *                                      indices
 * overlappingResourcePeriods_a       incicates if this link has any Resource
 *                                      Periods that overlap
 * widebandTransmission_a             indicates what portion(s) of a link
 *                                      allocation is transmitted via wideband
 *                                      (OVERHEAD refers to acquisition and
 *                                      droplink, COMMAND refers to start up and
 *                                      finish up
 * ptrTransmitAntenna_a*              pointer to the Antenna transmitting
 * ptrReceiveAntenna_a*               pointer to the Antenna receiving
 * ptrResourcePeriodList_a*           list of ResourcePeriod instances based on
 *                                      CSR data (RequestResourcePeriod) or
 *                                      RS-GS data (DownlinkResourcePeriod)
 *
 * @date 12/20/2011
 * @author Brian Fowler
 */
class Link : public BasicLink
{
public:

    enum WIDEBAND_TRANSMISSION {
        UNKNOWN_TRANSMISSION = -1,
        MISSION_ONLY = 0,
        ENTIRE_TRANSMISSION = 1,
        COMMAND_AND_MISSION = 2
    };

    enum ALLOCATION_STATUS {
        INVALID_TIME_STEP = -2,
        NOT_FIXED = -1,
        NO_ALLOCATION = 0,
        MISSION = 1,
        BUFFER = 2,
        STATE_OF_HEALTH = 3,
        NARROWBAND_CONTACT = 4,
        BUFFER_DURING_START_UP = 5,
        ACQUISITION_WIDEBAND = 6,
        START_UP_WIDEBAND = 7,
        FINISH_UP_WIDEBAND = 8,
        DROPLINK_WIDEBAND = 9,
        ACQUISITION_NARROWBAND = 10,
        START_UP_NARROWBAND = 11,
        FINISH_UP_NARROWBAND = 12,
        DROPLINK_NARROWBAND = 13,
        TRANSMIT_ANTENNA_PREP = 14,
        RECEIVE_ANTENNA_PREP = 15,
        BOTH_ANTENNA_PREP = 16
    };

    Link();
    Link(const shared_ptr<Antenna> &ptrFromAntenna,
         const shared_ptr<Antenna> &ptrToAntenna,
         const Array<int> *ptrStatusArray,
         const OverheadData *ptrOverheadData);
    Link(const Link &copyMe);
    virtual ~Link();

    Link&    operator =  (const Link &copyMe);
    bool     operator == (const Link &compareMe) const;

    static string GetWidebandTransmissionString(const WIDEBAND_TRANSMISSION &transmit);

    static WIDEBAND_TRANSMISSION GetWidebandTransmission(const string &value);

	void   SetPrecludedReceiver(const string &receiverDesignator);

    int  GetTransmitNumber() const;
    int  GetReceiveNumber() const;
    int  GetAllocatedCapacity(int timeIndex) const;
    int  GetNumberOfTransmittersModeled() const;
    int  GetNumberOfReceiversModeled() const;

    const int GetReceiveCapacity() const;

    int   GetTransmitPreparationTime() const;
    int   GetReceivePreparationTime() const;
    int   GetMaximumPreparationTime() const;
    int   GetMinimumPreparationTime() const;
    int   GetTransmitIndex() const;
    int   GetReceiveIndex() const;

    double GetSensorDataRate(int timeIndex,
                             int resourceIndex = -1) const;
    double GetTransmitDataRate() const;
    double GetReceiveDataRate() const;
    double GetLinkDataRate() const;

    bool   CheckTransmitCapacityFull(int timeIndex) const;
    bool   CheckReceiveCapacityFull(int timeIndex) const;
    bool   CheckNonDedicatedCapacityFull(int timeIndex) const;
    bool   ConstrainMaximumThroughput() const;
    bool   EqualPreparationTime() const;

    bool   GetNarrowbandNetworkIndex(int pixelIndex,
                                     int &narrowbandIndex) const;
    bool   GetMissionNetworkIndex(int pixelIndex,
                                  int &missionIndex) const;

    bool   IsMultiHopLink() const;
    bool   IsSelfRelayingLink() const;
    bool   IsDirectDownlinkLink() const;
    bool   IsCrosslink() const;
    bool   IsDownlink() const;
    bool   IsPrecluded() const;
    bool   RequiresBuffer() const;
    bool   RequiresBuffer(int timeIndex) const;
    bool   MaximumRelayThroughputProblem(int timeIndex,
                                         double dataRate) const;
    bool   NarrowbandConjunctionProblem(int timeIndex) const;
    bool   WidebandConjunctionProblem(int timeIndex) const;

    char   GetNetworkMissionIndexID() const;
    char   GetNetworkNarrowbandIndexID() const;

    Link*         GetAllocatedDownlink(int timeIndex) const;

    static inline void   SetMaximumXLProbabilisticSolutionIndex(int value);

    static inline int   GetMaximumXLProbabilisticSolutionIndex();

    static inline char   GetNetworkCrosslinkMissionID();
    static inline char   GetNetworkDownlinkMissionID();
    static inline char   GetNetworkDirectDownlinkMissionID();
    static inline char   GetNetworkSelfRelayLinkMissionID();
    static inline char   GetNetworkMultiHopLinkMissionID();

    inline void SetProbabilisticSolutionIndex(int value);
    inline void SetWidebandTransmission(const WIDEBAND_TRANSMISSION &transmit);
    inline void SetNetworkNarrowbandIndexToUnassigned(int pixelIndex) const;
    inline void SetNetworkMissionIndexToUnassigned(int pixelIndex) const;

    inline const int GetAcquisitionTime() const;
    inline const int GetDroplinkTime() const;
    inline const int GetMakeBeforeBreakDroplinkTime() const;
    inline const int GetMinimumDroplinkTime() const;
    inline const int GetMaximumDroplinkTime() const;
    inline const int GetMakeBeforeBreakStartUpTime() const;

    inline int GetOverlapTransmitTimeStepIndex(int timeIndex) const;
    inline int GetOverlapReceiveTimeStepIndex(int timeIndex) const;
    inline int GetNetworkMissionIndex(int pixelIndex) const;
    inline int GetNetworkNarrowbandIndex(int pixelIndex) const;
    inline int GetNetworkWidebandIndex(int pixelIndex) const;

    inline const bool  IsCrosslinkInview(int timeIndex) const;
    inline const bool  IsAllocated(int timeIndex) const;
    inline const bool  IsAllocated(int startTime,
                                   int endTime) const;
    inline const bool  IsAcquiringOrDroplinking(int timeIndex) const;
    inline const bool  IsAllocatedOverhead(int timeIndex) const;
    inline const bool  IsInNarrowbandContact(int timeIndex) const;
    inline const bool  IsAllocatedMission(int timeIndex) const;
    inline const bool  IsAllocatedBuffer(int timeIndex) const;
    inline const bool  IsAllocatedStateOfHealth(int timeIndex) const;
    inline const bool  IsAllocatedWideband(int timeIndex) const;
    inline const bool  IsAllocatedAcquisition(int timeIndex) const;
    inline const bool  IsAllocatedStartUp(int timeIndex) const;
    inline const bool  IsAllocatedFinishUp(int timeIndex) const;
    inline const bool  IsAllocatedDroplink(int timeIndex) const;
    inline const bool  IsAllocatedTransmitPrep(int timeIndex) const;
    inline const bool  IsAllocatedReceivePrep(int timeIndex) const;
    inline const bool  IsAllocatedPreparation(int timeIndex) const;
    inline const bool  IsTransmitAntennaAllocated(int timeIndex) const;
    inline const bool  IsReceiveAntennaAllocated(int timeIndex) const;
    inline bool        AcquisitionIsWideband() const;
    inline bool        StartUpIsWideband() const;
    inline bool        FinishUpIsWideband() const;
    inline bool        DroplinkIsWideband() const;
    inline bool        GetProbabilisticSolutionIndex(int &index) const;
    inline bool        RequiredDownlinkOverhead(int timeIndex) const;
    inline bool        StateOfHealthLinkProblem(int timeIndex) const;
    inline bool        TransmitElevationLinkProblem(int timeIndex) const;
    inline bool        HasOverlappingResourcePeriods() const;
    inline bool        IsMissionWidebandTransmission() const;
    inline bool        IsCommandAndMissionWidebandTransmission() const;
    inline bool        IsEntireWidebandTransmission() const;

    inline WIDEBAND_TRANSMISSION GetWidebandTransmission() const;

    inline ALLOCATION_STATUS GetAllocationStatus(int timeIndex) const;

    //inline const LinkList<ResourcePeriod>* GetResourcePeriodList() const;

protected:

private:

    static inline string GetClassName();

    static int               maximumXLProbabilisticSolutionIndex_s;
    static const char         OVERLAP_TRANSMIT_TIME_STEP_ID_s;
    static const char         OVERLAP_RECEIVE_TIME_STEP_ID_s;
    static const char         NETWORK_CROSSLINK_MISSION_ID_s;
    static const char         NETWORK_DOWNLINK_MISSION_ID_s;
    static const char         NETWORK_DIRECK_DOWNLINK_MISSION_ID_s;
    static const char         NETWORK_SELF_RELAY_LINK_MISSION_ID_s;
    static const char         NETWORK_MULTIHOP_LINK_MISSION_ID_s;

    int                     acquisitionTime_a;
    int                     droplinkTime_a;
    int                     makeBeforeBreakDroplinkTime_a;
    int                     makeBeforeBreakStartUpTime_a;
    Array<int>             *ptrAllocationStatus_a;
    int                      probablisticSolutionIndex_a;
    Array<int>              *ptrOverlapTransmitTimeStepIndex_a;
    Array<int>              *ptrOverlapReceiveTimeStepIndex_a;
    Array<int>              *ptrNetworkMissionIndexArray_a;
    Array<int>              *ptrNetworkNarrowbandIndexArray_a;
    bool                      overlappingResourcePeriods_a;
    WIDEBAND_TRANSMISSION     widebandTransmission_a;
    //LinkList<ResourcePeriod> *ptrResourcePeriodList_a;
};

//inline
//const LinkList<ResourcePeriod>*
//Link::GetResourcePeriodList() const
//{
//    return(ptrResourcePeriodList_a);
//}

inline
int
Link::GetOverlapTransmitTimeStepIndex(int timeIndex) const
{
    int overlapTransmitTimeStepIndex = INVALID_SOLUTION_INDEX;

    if (ptrOverlapTransmitTimeStepIndex_a != NULL) {
        overlapTransmitTimeStepIndex = ptrOverlapTransmitTimeStepIndex_a->ElementAt(timeIndex);
    }

    return(overlapTransmitTimeStepIndex);
}

inline
int
Link::GetOverlapReceiveTimeStepIndex(int timeIndex) const
{
    int overlapReceiveTimeStepIndex = INVALID_SOLUTION_INDEX;

    // if the preparation times are equal the transmit is the general (only) index array
    if (EqualPreparationTime() == true) {
        overlapReceiveTimeStepIndex = GetOverlapTransmitTimeStepIndex(timeIndex);
    } else if (ptrOverlapReceiveTimeStepIndex_a != NULL) {
        overlapReceiveTimeStepIndex = ptrOverlapReceiveTimeStepIndex_a->ElementAt(timeIndex);
    }

    return(overlapReceiveTimeStepIndex);
}

inline
int
Link::GetNetworkMissionIndex(int pixelIndex) const
{
    int networkMissionIndex = INVALID_SOLUTION_INDEX;

    if (ptrNetworkMissionIndexArray_a != NULL) {
        networkMissionIndex = ptrNetworkMissionIndexArray_a->ElementAt(pixelIndex);
    }

    return(networkMissionIndex);
}

inline
int
Link::GetNetworkNarrowbandIndex(int pixelIndex) const
{
    int networkNarrowbandIndex = INVALID_SOLUTION_INDEX;

    if (ptrNetworkNarrowbandIndexArray_a != NULL) {
        networkNarrowbandIndex = ptrNetworkNarrowbandIndexArray_a->ElementAt(pixelIndex);
    }

    return(networkNarrowbandIndex);
}

inline
int
Link::GetNetworkWidebandIndex(int pixelIndex) const
{
    int networkWidebandIndex = INVALID_SOLUTION_INDEX;

    if (widebandTransmission_a == MISSION_ONLY) {
        networkWidebandIndex = GetNetworkMissionIndex(pixelIndex);
    } else {
        networkWidebandIndex = GetNetworkNarrowbandIndex(pixelIndex);
    }

    return(networkWidebandIndex);
}

inline
void
Link::SetNetworkNarrowbandIndexToUnassigned(int pixelIndex) const
{
    if ( ptrNetworkNarrowbandIndexArray_a != NULL ) {
        (*ptrNetworkNarrowbandIndexArray_a)[pixelIndex] = UNASSIGNED_SOLUTION_INDEX;
    }
}

inline
void
Link::SetNetworkMissionIndexToUnassigned(int pixelIndex) const
{
    if ( ptrNetworkMissionIndexArray_a != NULL ) {
        (*ptrNetworkMissionIndexArray_a)[pixelIndex] = UNASSIGNED_SOLUTION_INDEX;
    }
}

inline
bool
Link::HasOverlappingResourcePeriods() const
{
    return(overlappingResourcePeriods_a);
}

inline
const bool
Link::IsCrosslinkInview(int timeIndex) const
{
    bool isCrosslinkInview = false;

    try {
        if ( (ptrInviewStatus_a->at(timeIndex) == LINK) ||
                (ptrInviewStatus_a->at(timeIndex) == STATE_OF_HEALTH_TIME_STEP) ) {
            isCrosslinkInview = true;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "IsCrosslinkInview(int)");
        throw;
    }

    return(isCrosslinkInview);
}

inline
const bool
Link::IsAllocated(int timeIndex) const
{
    bool isAllocated = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if (ptrAllocationStatus_a->ElementAt(timeIndex) != NO_ALLOCATION) {
                isAllocated = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocated(int)");
            throw;
        }
    }

    return(isAllocated);
}

inline
const bool
Link::IsTransmitAntennaAllocated(int timeIndex) const
{
    bool isTransmitAntennaAllocated = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) != NO_ALLOCATION) &&
                    (ptrAllocationStatus_a->ElementAt(timeIndex) != RECEIVE_ANTENNA_PREP) ) {
                isTransmitAntennaAllocated = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsTransmitAntennaAllocated(int)");
            throw;
        }
    }

    return(isTransmitAntennaAllocated);
}

inline
const bool
Link::IsReceiveAntennaAllocated(int timeIndex) const
{
    bool isReceiveAntennaAllocated = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) != NO_ALLOCATION) &&
                    (ptrAllocationStatus_a->ElementAt(timeIndex) != TRANSMIT_ANTENNA_PREP) ) {
                isReceiveAntennaAllocated = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsReceiveAntennaAllocated(int)");
            throw;
        }
    }

    return(isReceiveAntennaAllocated);
}

inline
const bool
Link::IsAllocated(int startTime,
                  int endTime) const
{
    int timeIndex = startTime;
    bool isAllocated = false;

    if (ptrAllocationStatus_a != NULL) {
        isAllocated = true;

        try {
            while ((timeIndex <= endTime) && (isAllocated == true)) {
                if (ptrAllocationStatus_a->ElementAt(timeIndex) == NO_ALLOCATION) {
                    isAllocated = false;
                }

                ++timeIndex;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocated(int, int)");
            throw;
        }
    }

    return(isAllocated);
}

inline
const bool
Link::IsAcquiringOrDroplinking(int timeIndex) const
{
    ALLOCATION_STATUS status = NO_ALLOCATION;

    if (ptrAllocationStatus_a != NULL) {
        try {
            status = GetAllocationStatus(timeIndex);
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAcquiringOrDroplinking(int)");
            throw;
        }
    }

    return( (status == ACQUISITION_WIDEBAND) || (status == ACQUISITION_NARROWBAND) ||
            (status == DROPLINK_WIDEBAND) || (status == DROPLINK_NARROWBAND) );
}

inline
const bool
Link::IsAllocatedOverhead(int timeIndex) const
{
    bool isAllocatedOverhead = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) >= NARROWBAND_CONTACT) &&
                    (ptrAllocationStatus_a->ElementAt(timeIndex) <= DROPLINK_NARROWBAND) ) {
                isAllocatedOverhead = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedOverhead(int)");
            throw;
        }
    }

    return(isAllocatedOverhead);
}

inline
const bool
Link::IsInNarrowbandContact(int timeIndex) const
{
    bool isInNarrowbandContact = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if (ptrAllocationStatus_a->ElementAt(timeIndex) == NARROWBAND_CONTACT) {
                isInNarrowbandContact = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsInNarrowbandContact(int)");
            throw;
        }
    }

    return(isInNarrowbandContact);
}

inline
const bool
Link::IsAllocatedMission(int timeIndex) const
{
    bool isAllocatedMission = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if (ptrAllocationStatus_a->ElementAt(timeIndex) == MISSION) {
                isAllocatedMission = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedMission(int)");
            throw;
        }
    }

    return(isAllocatedMission);
}

inline
const bool
Link::IsAllocatedBuffer(int timeIndex) const
{
    bool isAllocatedBuffer = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) == BUFFER) ||
                    (ptrAllocationStatus_a->ElementAt(timeIndex) == BUFFER_DURING_START_UP) ) {
                isAllocatedBuffer = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedBuffer(int)");
            throw;
        }
    }

    return(isAllocatedBuffer);
}

inline
const bool
Link::IsAllocatedStateOfHealth(int timeIndex) const
{
    bool isAllocatedStateOfHealth = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if (ptrAllocationStatus_a->ElementAt(timeIndex) == STATE_OF_HEALTH) {
                isAllocatedStateOfHealth = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedStateOfHealth(int)");
            throw;
        }
    }

    return(isAllocatedStateOfHealth);
}

inline
const bool
Link::IsAllocatedWideband(int timeIndex) const
{
    bool isAllocatedWideband = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) >= MISSION) &&
                    (ptrAllocationStatus_a->ElementAt(timeIndex) <= DROPLINK_WIDEBAND) ) {
                isAllocatedWideband = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedWideband(int)");
            throw;
        }
    }

    return(isAllocatedWideband);
}

inline
const bool
Link::IsAllocatedAcquisition(int timeIndex) const
{
    bool isAllocatedAcquistion = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) == ACQUISITION_NARROWBAND) ||
                    (ptrAllocationStatus_a->ElementAt(timeIndex) == ACQUISITION_WIDEBAND) ) {
                isAllocatedAcquistion = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedAcquisition(int)");
            throw;
        }
    }

    return(isAllocatedAcquistion);
}

inline
const bool
Link::IsAllocatedStartUp(int timeIndex) const
{
    bool isAllocatedStartUp = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) == START_UP_WIDEBAND) ||
                    (ptrAllocationStatus_a->ElementAt(timeIndex) == START_UP_NARROWBAND) ||
                    (ptrAllocationStatus_a->ElementAt(timeIndex) == BUFFER_DURING_START_UP) ) {
                isAllocatedStartUp = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedStartUp(int)");
            throw;
        }
    }

    return(isAllocatedStartUp);
}

inline
const bool
Link::IsAllocatedFinishUp(int timeIndex) const
{
    bool isAllocatedFinishUp = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) == FINISH_UP_WIDEBAND) ||
                    (ptrAllocationStatus_a->ElementAt(timeIndex) == FINISH_UP_NARROWBAND) ) {
                isAllocatedFinishUp = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedFinishUp(int)");
            throw;
        }
    }

    return(isAllocatedFinishUp);
}

inline
const bool
Link::IsAllocatedDroplink(int timeIndex) const
{
    bool isAllocatedDroplink = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) == DROPLINK_NARROWBAND) ||
                    (ptrAllocationStatus_a->ElementAt(timeIndex) == DROPLINK_WIDEBAND) ) {
                isAllocatedDroplink = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedDroplink(int)");
            throw;
        }
    }

    return(isAllocatedDroplink);
}

inline
const bool
Link::IsAllocatedTransmitPrep(int timeIndex) const
{
    bool isAllocatedTransmitPrep = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) == TRANSMIT_ANTENNA_PREP) ||
                    (ptrAllocationStatus_a->ElementAt(timeIndex) == BOTH_ANTENNA_PREP) ) {
                isAllocatedTransmitPrep = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedTransmitPrep(int)");
            throw;
        }
    }

    return(isAllocatedTransmitPrep);
}

inline
const bool
Link::IsAllocatedReceivePrep(int timeIndex) const
{
    bool isAllocatedReceivePrep = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) == RECEIVE_ANTENNA_PREP) ||
                    (ptrAllocationStatus_a->ElementAt(timeIndex) == BOTH_ANTENNA_PREP) ) {
                isAllocatedReceivePrep = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedReceivePrep(int)");
            throw;
        }
    }

    return(isAllocatedReceivePrep);
}

inline
const bool
Link::IsAllocatedPreparation(int timeIndex) const
{
    bool isAllocatedPreparation = false;

    if (ptrAllocationStatus_a != NULL) {
        try {
            if ( (ptrAllocationStatus_a->ElementAt(timeIndex) == TRANSMIT_ANTENNA_PREP) ||
                    (ptrAllocationStatus_a->ElementAt(timeIndex) == RECEIVE_ANTENNA_PREP) ||
                    (ptrAllocationStatus_a->ElementAt(timeIndex) == BOTH_ANTENNA_PREP) ) {
                isAllocatedPreparation = true;
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "IsAllocatedPreparation(int)");
            throw;
        }
    }

    return(isAllocatedPreparation);
}

inline
bool
Link::IsMissionWidebandTransmission() const
{
    return(widebandTransmission_a == MISSION_ONLY);
}

inline
bool
Link::IsCommandAndMissionWidebandTransmission() const
{
    return(widebandTransmission_a == COMMAND_AND_MISSION);
}

inline
bool
Link::IsEntireWidebandTransmission() const
{
    return(widebandTransmission_a == ENTIRE_TRANSMISSION);
}

inline
Link::WIDEBAND_TRANSMISSION
Link::GetWidebandTransmission() const
{
    return(widebandTransmission_a);
}

inline
bool
Link::AcquisitionIsWideband() const
{
    return(widebandTransmission_a == ENTIRE_TRANSMISSION);
}

inline
bool
Link::StartUpIsWideband() const
{
    return( (widebandTransmission_a == ENTIRE_TRANSMISSION) ||
            (widebandTransmission_a == COMMAND_AND_MISSION) );
}

inline
bool
Link::FinishUpIsWideband() const
{
    return( (widebandTransmission_a == ENTIRE_TRANSMISSION) ||
            (widebandTransmission_a == COMMAND_AND_MISSION) );
}

inline
bool
Link::DroplinkIsWideband() const
{
    return(widebandTransmission_a == ENTIRE_TRANSMISSION);
}

inline
bool
Link::StateOfHealthLinkProblem(int timeIndex) const
{
    return(ptrInviewStatus_a->at(timeIndex) == STATE_OF_HEALTH_TIME_STEP);
}

inline
bool
Link::RequiredDownlinkOverhead(int timeIndex) const
{
    return(ptrInviewStatus_a->at(timeIndex) == REQUIRED_DOWNLINK_OVERHEAD);
}

inline
bool
Link::TransmitElevationLinkProblem(int timeIndex) const
{
    return( (ptrInviewStatus_a->at(timeIndex) == MAX_ELEVATION_TRANSMIT) ||
            (ptrInviewStatus_a->at(timeIndex) == MIN_ELEVATION_TRANSMIT) );
}

inline
Link::ALLOCATION_STATUS
Link::GetAllocationStatus(int timeIndex) const
{
    ALLOCATION_STATUS returnStatus = NO_ALLOCATION;

    if (ptrAllocationStatus_a != NULL) {
        try {
            returnStatus = (ALLOCATION_STATUS)ptrAllocationStatus_a->ElementAt(timeIndex);
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "GetAllocationStatus(int)");
            throw;
        }
    }

    return(returnStatus);
}

inline
const int
Link::GetAcquisitionTime() const
{
    return(acquisitionTime_a);
}

inline
const int
Link::GetDroplinkTime() const
{
    return(droplinkTime_a);
}

inline
const int
Link::GetMakeBeforeBreakDroplinkTime() const
{
    return(makeBeforeBreakDroplinkTime_a);
}

inline
const int
Link::GetMinimumDroplinkTime() const
{
    int minimumDropTime = droplinkTime_a;

    if ( (makeBeforeBreakDroplinkTime_a >= 0) &&
            (makeBeforeBreakDroplinkTime_a < droplinkTime_a) ) {
        minimumDropTime = makeBeforeBreakDroplinkTime_a;
    }

    return(minimumDropTime);
}

inline
const int
Link::GetMaximumDroplinkTime() const
{
    int maximumDropTime = droplinkTime_a;

    if (makeBeforeBreakDroplinkTime_a > droplinkTime_a) {
        maximumDropTime = makeBeforeBreakDroplinkTime_a;
    }

    return(maximumDropTime);
}

inline
const int
Link::GetMakeBeforeBreakStartUpTime() const
{
    return(makeBeforeBreakStartUpTime_a);
}

inline
void
Link::SetWidebandTransmission(const WIDEBAND_TRANSMISSION &transmit)
{
    widebandTransmission_a = transmit;
}

inline
void
Link::SetProbabilisticSolutionIndex(int value)
{
    probablisticSolutionIndex_a = value;
}

inline
bool
Link::GetProbabilisticSolutionIndex(int &index) const
{
    bool status = false;
    index = probablisticSolutionIndex_a;

    if (index > INVALID_SOLUTION_INDEX) {
        status = true;
    }

    return(status);
}

inline
void
Link::SetMaximumXLProbabilisticSolutionIndex(int value)
{
    maximumXLProbabilisticSolutionIndex_s = value;
}

inline
int
Link::GetMaximumXLProbabilisticSolutionIndex()
{
    return(maximumXLProbabilisticSolutionIndex_s);
}

inline
char
Link::GetNetworkCrosslinkMissionID()
{
    return(NETWORK_CROSSLINK_MISSION_ID_s);
}

inline
char
Link::GetNetworkDownlinkMissionID()
{
    return(NETWORK_DOWNLINK_MISSION_ID_s);
}

inline
char
Link::GetNetworkDirectDownlinkMissionID()
{
    return(NETWORK_DIRECK_DOWNLINK_MISSION_ID_s);
}

inline
char
Link::GetNetworkSelfRelayLinkMissionID()
{
    return(NETWORK_SELF_RELAY_LINK_MISSION_ID_s);
}

inline
char
Link::GetNetworkMultiHopLinkMissionID()
{
    return(NETWORK_MULTIHOP_LINK_MISSION_ID_s);
}

inline
string
Link::GetClassName()
{
    return(LINK_H);
}


#endif
