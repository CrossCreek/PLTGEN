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
 * Link.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include <algorithm>

#include "Link.h"
#include "OverheadData.h"
#include "mural/antenna/Antenna.h"
#include "mural/element/DataProvider.h"
#include "mural/element/RelaySatellite.h"
#include "mural/io/MURALInputProcessor.h"

#include "general/array/ListIterator.h"
#include "general/exception/UpdateException.h"
#include "general/math/SpaceVector.h"
#include "general/utility/StringEditor.h"
#include "general\utility\Utility.h"

int         Link::maximumXLProbabilisticSolutionIndex_s = -1;
const char   Link::OVERLAP_TRANSMIT_TIME_STEP_ID_s       = 'o';
const char   Link::OVERLAP_RECEIVE_TIME_STEP_ID_s        = 'O';
const char   Link::NETWORK_CROSSLINK_MISSION_ID_s        = 'X';
const char   Link::NETWORK_DOWNLINK_MISSION_ID_s         = 'H';
const char   Link::NETWORK_DIRECK_DOWNLINK_MISSION_ID_s  = 'D';
const char   Link::NETWORK_SELF_RELAY_LINK_MISSION_ID_s  = 'S';
const char   Link::NETWORK_MULTIHOP_LINK_MISSION_ID_s    = 'M';

Link::Link()
    :	BasicLink(),
      acquisitionTime_a(-1), droplinkTime_a(-1), makeBeforeBreakDroplinkTime_a(-1),
      makeBeforeBreakStartUpTime_a(-1), widebandTransmission_a(MISSION_ONLY),
      ptrOverlapTransmitTimeStepIndex_a(NULL), ptrOverlapReceiveTimeStepIndex_a(NULL),
      ptrNetworkMissionIndexArray_a(NULL), ptrNetworkNarrowbandIndexArray_a(NULL),
	  overlappingResourcePeriods_a(false), probablisticSolutionIndex_a(-1)
{
    return;
}

Link::Link(const shared_ptr<Antenna> &ptrFromAntenna,
           const shared_ptr<Antenna> &ptrToAntenna,
           const Array<int> *ptrStatusArray,
           const OverheadData *ptrOverheadData)
    :	BasicLink(ptrFromAntenna, ptrToAntenna, ptrStatusArray),
      droplinkTime_a(ptrOverheadData->GetDroplinkTime()),
      makeBeforeBreakDroplinkTime_a(ptrOverheadData->GetMakeBeforeBreakDroplinkTime()),
      makeBeforeBreakStartUpTime_a(ptrOverheadData->GetMakeBeforeBreakStartUpTime()),
      widebandTransmission_a(MISSION_ONLY),
      ptrOverlapTransmitTimeStepIndex_a(NULL), ptrOverlapReceiveTimeStepIndex_a(NULL),
      ptrNetworkMissionIndexArray_a(NULL),
      ptrNetworkNarrowbandIndexArray_a(NULL), overlappingResourcePeriods_a(false),
      ptrAllocationStatus_a(NULL),probablisticSolutionIndex_a(-1)
{
    return;
}

Link::Link(const Link &copyMe)
    :	BasicLink(copyMe),
      droplinkTime_a(copyMe.droplinkTime_a),
      makeBeforeBreakDroplinkTime_a(copyMe.makeBeforeBreakDroplinkTime_a),
      makeBeforeBreakStartUpTime_a(copyMe.makeBeforeBreakStartUpTime_a),
      ptrOverlapTransmitTimeStepIndex_a(NULL), ptrOverlapReceiveTimeStepIndex_a(NULL),
      ptrNetworkMissionIndexArray_a(NULL),
      ptrNetworkNarrowbandIndexArray_a(NULL),
      overlappingResourcePeriods_a(copyMe.overlappingResourcePeriods_a),
      widebandTransmission_a(copyMe.widebandTransmission_a),
      probablisticSolutionIndex_a(copyMe.probablisticSolutionIndex_a)
{
	return;
}

Link&
Link::operator = (const Link &copyMe)
{
    droplinkTime_a                    = copyMe.droplinkTime_a;
    makeBeforeBreakDroplinkTime_a     = copyMe.makeBeforeBreakDroplinkTime_a;
    makeBeforeBreakStartUpTime_a      = copyMe.makeBeforeBreakStartUpTime_a;
    overlappingResourcePeriods_a      = copyMe.overlappingResourcePeriods_a;
    widebandTransmission_a            = copyMe.widebandTransmission_a;
    ptrTransmitAntenna_a              = copyMe.ptrTransmitAntenna_a;
    ptrReceiveAntenna_a               = copyMe.ptrReceiveAntenna_a;
    ptrInviewStatus_a                 = NULL;
    ptrOverlapTransmitTimeStepIndex_a = NULL;
    ptrOverlapReceiveTimeStepIndex_a  = NULL;

    try {
        if (copyMe.ptrInviewStatus_a != NULL) {
            //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
            int statusSize = Utility::CastSizetToInt(copyMe.ptrInviewStatus_a->size());
            ptrInviewStatus_a = shared_ptr<vector<int>>(new vector<int>(statusSize));

            for (int index = 0; index < statusSize; index++) {
                (*ptrInviewStatus_a)[index] = (*copyMe.ptrInviewStatus_a)[index];
            }
        }

    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "operator = (const Link&)");
        throw;
    }

    return(*this);
}

bool
Link::operator == (const Link &compareMe) const
{
    return( (droplinkTime_a == compareMe.droplinkTime_a) &&
            (makeBeforeBreakDroplinkTime_a == compareMe.makeBeforeBreakDroplinkTime_a) &&
            (makeBeforeBreakStartUpTime_a == compareMe.makeBeforeBreakStartUpTime_a) &&
            (overlappingResourcePeriods_a == compareMe.overlappingResourcePeriods_a) &&
            (widebandTransmission_a == compareMe.widebandTransmission_a) &&
            (*ptrInviewStatus_a == *(compareMe.ptrInviewStatus_a)) &&
            (*ptrTransmitAntenna_a == *(compareMe.ptrTransmitAntenna_a)) &&
            (*ptrReceiveAntenna_a == *(compareMe.ptrReceiveAntenna_a)) );
}

bool
Link::IsMultiHopLink() const
{
    bool         multiHopLink       = false;
    const string transmitDesignator = GetTransmitDesignator();
    const string receiveDesignator  = GetReceiveDesignator();

    if ( (MURALInputProcessor::ValidRelaySatellite(transmitDesignator) == true) &&
            (MURALInputProcessor::ValidRelaySatellite(receiveDesignator) == true) ) {
        multiHopLink = true;
    }

    return(multiHopLink);
}

bool
Link::IsSelfRelayingLink() const
{
    bool         selfRelayLink  = false;
    const string transmitDesignator = GetTransmitDesignator();
    const string receiveDesignator  = GetReceiveDesignator();

    if ( (MURALInputProcessor::ValidUserVehicle(transmitDesignator) == true) &&
            (MURALInputProcessor::ValidUserVehicle(receiveDesignator) == true) ) {
        selfRelayLink = true;
    }

    return(selfRelayLink);
}

bool
Link::IsDirectDownlinkLink() const
{
    bool         directDownLink  = false;
    const string transmitDesignator = GetTransmitDesignator();
    const string receiveDesignator  = GetReceiveDesignator();

    if ( (MURALInputProcessor::ValidUserVehicle(transmitDesignator) == true) &&
            (MURALInputProcessor::ValidGroundStation(receiveDesignator) == true) ) {
        directDownLink = true;
    }

    return(directDownLink);
}

bool
Link::IsCrosslink() const
{
    bool         crosslink  = false;
    const string transmitDesignator = GetTransmitDesignator();
    const string receiveDesignator  = GetReceiveDesignator();

    if ( (MURALInputProcessor::ValidUserVehicle(transmitDesignator) == true) &&
            (MURALInputProcessor::ValidRelaySatellite(receiveDesignator) == true) ) {
        crosslink = true;
    }

    return(crosslink);
}

bool
Link::IsDownlink() const
{
    bool         crosslink  = false;
    const string transmitDesignator = GetTransmitDesignator();
    const string receiveDesignator  = GetReceiveDesignator();

    if ( (MURALInputProcessor::ValidRelaySatellite(transmitDesignator) == true) &&
            (MURALInputProcessor::ValidGroundStation(receiveDesignator) == true) ) {
        crosslink = true;
    }

    return(crosslink);
}

bool
Link::IsPrecluded() const
{
    bool   precluded = false;
    string receiveDesignator = GetReceiveDesignator();

    if (ptrTransmitAntenna_a != NULL) {
        precluded = ptrTransmitAntenna_a->IsPrecludedReceiver(receiveDesignator);
    }

    return(precluded);
}

void
Link::SetPrecludedReceiver(const string &receiverDesignator)
{
    ptrTransmitAntenna_a->SetPrecludedReceiver(receiverDesignator);
}

int
Link::GetTransmitPreparationTime() const
{
    return(ptrTransmitAntenna_a->GetPreparationTime());
}

int
Link::GetReceivePreparationTime() const
{
    return(ptrReceiveAntenna_a->GetPreparationTime());
}

int
Link::GetMaximumPreparationTime() const
{
    return(
              std::max(
                  ptrTransmitAntenna_a->GetPreparationTime(),
                  ptrReceiveAntenna_a->GetPreparationTime()
              )
          );
}

int
Link::GetMinimumPreparationTime() const
{
    return(
              std::min(
                  ptrTransmitAntenna_a->GetPreparationTime(),
                  ptrReceiveAntenna_a->GetPreparationTime()
              )
          );
}

bool
Link::EqualPreparationTime() const
{
    return(ptrTransmitAntenna_a->GetPreparationTime() == ptrReceiveAntenna_a->GetPreparationTime());
}

int
Link::GetTransmitIndex() const
{
    return((int)ptrTransmitAntenna_a->GetDataProviderIndex());
}

int
Link::GetReceiveIndex() const
{
    return((int)ptrReceiveAntenna_a->GetDataProviderIndex());
}

int
Link::GetTransmitNumber() const
{
    return(ptrTransmitAntenna_a->GetDataProviderNumber());
}

int
Link::GetReceiveNumber() const
{
    return(ptrReceiveAntenna_a->GetDataProviderNumber());
}

int
Link::GetAllocatedCapacity(int timeIndex) const
{
    return(ptrReceiveAntenna_a->GetAllocatedCapacity(timeIndex, GetTransmitNumber()));
}

char
Link::GetNetworkMissionIndexID() const
{
    char         networkIndexID = BLANK_CHARACTER;
    const string transmitDesignator = GetTransmitDesignator();
    const string receiveDesignator = GetReceiveDesignator();

    if (MURALInputProcessor::ValidUserVehicle(transmitDesignator) == true) {
        if (MURALInputProcessor::ValidRelaySatellite(receiveDesignator) == true) {
            networkIndexID = NETWORK_CROSSLINK_MISSION_ID_s;
        } else if (MURALInputProcessor::ValidGroundStation(receiveDesignator) == true) {
            networkIndexID = NETWORK_DIRECK_DOWNLINK_MISSION_ID_s;
        } else {
            networkIndexID = NETWORK_SELF_RELAY_LINK_MISSION_ID_s;
        }
    } else if (MURALInputProcessor::ValidGroundStation(receiveDesignator) == true) {
        networkIndexID = NETWORK_DOWNLINK_MISSION_ID_s;
    } else {
        networkIndexID = NETWORK_MULTIHOP_LINK_MISSION_ID_s;
    }

    return(networkIndexID);
}

char
Link::GetNetworkNarrowbandIndexID() const
{
    return(tolower(GetNetworkMissionIndexID()));
}

bool
Link::MaximumRelayThroughputProblem(int timeIndex,
                                    double dataRate) const
{
    return(ptrReceiveAntenna_a->MaximumRelayThroughputProblem(timeIndex, dataRate));
}

bool
Link::NarrowbandConjunctionProblem(int timeIndex) const
{
    bool   problem = false;
    const ConjunctionPeriod::CONJUNCTION_TYPE type = ConjunctionPeriod::NB;

    if (GetReceiveProvider()->GetConjunctionPeriodList(type) != NULL) {
        problem = GetReceiveProvider()->ConjunctionProblem(
                      type,
                      timeIndex,
                      GetTransmitDesignator()
                  );
    }

    return(problem);
}

bool
Link::WidebandConjunctionProblem(int timeIndex) const
{
    bool   problem = false;
    const ConjunctionPeriod::CONJUNCTION_TYPE type = ConjunctionPeriod::WB;

    if (GetReceiveProvider()->GetConjunctionPeriodList(type) != NULL) {
        problem = GetReceiveProvider()->ConjunctionProblem(
                      type,
                      timeIndex,
                      GetTransmitDesignator()
                  );
    }

    return(problem);
}

int
Link::GetNumberOfTransmittersModeled() const
{
    return(ptrTransmitAntenna_a->GetNumberOfAssetsModeled());
}

int
Link::GetNumberOfReceiversModeled() const
{
    return(ptrReceiveAntenna_a->GetNumberOfAssetsModeled());
}

double
Link::GetSensorDataRate(int timeIndex,
                        int resourceIndex) const
{
    return(ptrTransmitAntenna_a->GetTransmitDataRate(timeIndex, resourceIndex));
}

double
Link::GetTransmitDataRate() const
{
    return(ptrTransmitAntenna_a->GetDataRate());
}

double
Link::GetReceiveDataRate() const
{
    return(ptrReceiveAntenna_a->GetDataRate());
}

double
Link::GetLinkDataRate() const
{
    return(
              std::min(
                  ptrTransmitAntenna_a->GetDataRate(),
                  ptrReceiveAntenna_a->GetDataRate()
              )
          );
}

const int
Link::GetReceiveCapacity() const
{
    return( ptrReceiveAntenna_a->GetCapacity() );
}

bool
Link::GetNarrowbandNetworkIndex(int pixelIndex,
                                int &narrowbandIndex) const
{
    narrowbandIndex = INVALID_SOLUTION_INDEX;

    if ((ptrNetworkNarrowbandIndexArray_a != NULL) && (pixelIndex >= 0)) {
        if (pixelIndex < ptrNetworkNarrowbandIndexArray_a->Size()) {
            narrowbandIndex = ptrNetworkNarrowbandIndexArray_a->ElementAt(pixelIndex);
        }
    }

    return(narrowbandIndex >= 0);
}

bool
Link::GetMissionNetworkIndex(int pixelIndex,
                             int &missionIndex) const
{
    missionIndex = INVALID_SOLUTION_INDEX;

    if ((ptrNetworkMissionIndexArray_a != NULL) && (pixelIndex >= 0)) {
        if (pixelIndex < ptrNetworkNarrowbandIndexArray_a->Size()) {
            missionIndex = ptrNetworkMissionIndexArray_a->ElementAt(pixelIndex);
        }
    }

    return(missionIndex >= 0);
}

bool
Link::RequiresBuffer() const
{
    return(ptrTransmitAntenna_a->GetMaximumCollectionDataRate() > GetLinkDataRate());
}

bool
Link::RequiresBuffer(int timeIndex) const
{
    return(ptrTransmitAntenna_a->GetCollectionDataRate(timeIndex) > GetLinkDataRate());
}

bool
Link::CheckTransmitCapacityFull(int timeIndex) const
{
    return(ptrTransmitAntenna_a->CheckTransmitCapacityFull(timeIndex));
}

bool
Link::CheckReceiveCapacityFull(int timeIndex) const
{
    return(ptrReceiveAntenna_a->CheckReceiveCapacityFull(timeIndex));
}

bool
Link::CheckNonDedicatedCapacityFull(int timeIndex) const
{
    return(
              ptrReceiveAntenna_a->CheckNonDedicatedCapacityFull(
                  timeIndex,
                  GetTransmitDesignator()
              )
          );
}

bool
Link::ConstrainMaximumThroughput() const
{
    return(ptrReceiveAntenna_a->ConstrainMaximumThroughput());
}

Link*
Link::GetAllocatedDownlink(int timeIndex) const
{
    return(ptrReceiveAntenna_a->GetAllocatedDownlink(timeIndex));
}

Link::WIDEBAND_TRANSMISSION
Link::GetWidebandTransmission(const string &value)
{
    WIDEBAND_TRANSMISSION returnTransmission = UNKNOWN_TRANSMISSION;

    if (value == "MISSION") {
        returnTransmission = MISSION_ONLY;
    } else if (value == "CM") {
        returnTransmission = COMMAND_AND_MISSION;
    } else if (value == "ENTIRE") {
        returnTransmission = ENTIRE_TRANSMISSION;
    }

    return(returnTransmission);
}

string
Link::GetWidebandTransmissionString(const WIDEBAND_TRANSMISSION &transmit)
{
    string  transmissionString = "UNKNOWN";

    if (transmit == MISSION_ONLY) {
        transmissionString = "MISSION";
    } else if (transmit == ENTIRE_TRANSMISSION) {
        transmissionString = "ENTIRE";
    } else if (transmit == COMMAND_AND_MISSION) {
        transmissionString = "CM";
    }

    return(transmissionString);
}

Link::~Link()
{
    if (ptrAllocationStatus_a != NULL) {
        delete ptrAllocationStatus_a;
        ptrAllocationStatus_a = NULL;
    }

    if (ptrOverlapTransmitTimeStepIndex_a != NULL) {
        delete ptrOverlapTransmitTimeStepIndex_a;
        ptrOverlapTransmitTimeStepIndex_a = NULL;
    }

    if (ptrOverlapReceiveTimeStepIndex_a != NULL) {
        delete ptrOverlapReceiveTimeStepIndex_a;
        ptrOverlapReceiveTimeStepIndex_a = NULL;
    }

    if (ptrNetworkMissionIndexArray_a != NULL) {
        delete ptrNetworkMissionIndexArray_a;
        ptrNetworkMissionIndexArray_a = NULL;
    }

    if (ptrNetworkNarrowbandIndexArray_a != NULL) {
        delete ptrNetworkNarrowbandIndexArray_a;
        ptrNetworkNarrowbandIndexArray_a = NULL;
    }
}
