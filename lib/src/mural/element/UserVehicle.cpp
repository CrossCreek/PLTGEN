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
 * UserVehicle.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "UserVehicle.h"
#include "RelaySatellite.h"
#include "GroundStation.h"
#include "mural/CollectionElement.h"
#include "mural/Earth.h"
#include "mural/Region.h"
#include "mural/antenna/DirectDownlinkAntenna.h"
#include "mural/antenna/ReceiveFacilityAntenna.h"
#include "mural/antenna/SelfRelayAntenna.h"
#include "mural/antenna/UserPointingAntenna.h"
#include "mural/coorframe/ElementCoordinateFrame.h"
#include "mural/sensor/Sensor.h"
#include "mural/io/FileNames.h"
#include "mural/io/OutputGenerator.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/modules/pg/Revolution.h"
#include "mural/modules/lg/Link.h"
#include "mural/modules/lg/LinkCriteria.h"
#include "mural/modules/lg/LinksGenerator.h"
#include "mural/modules/lg/OverheadData.h"
#include "mural/modules/vtg/RegionData.h"
#include "mural/modules/vtg/TimelineGenerator.h"
#include "mural/modules/vtg/ValueTimeline.h"
#include "mural/modules/vtg/CrisisData.h"
#include "mural/modules/ts/TargetDeck.h"
#include "mural/modules/ts/Target.h"
#include "mural/modules/ts/TargetRegion.h"

#include "general/array/Array2D.h"
#include "general/array/ListIterator.h"
#include "general/exception/InputException.h"
#include "general/exception/UpdateException.h"
#include "general/parse/ParseUtility.h"
#include "general/data/UnitData.h"
#include "general/data/PositionState.h"
#include "general/math/CoordinateFrame.h"
#include "general/math/SimpleMath.h"
#include "general/utility/StringEditor.h"

bool           UserVehicle::anyDirectDownlinkUsers_s         = false;
bool           UserVehicle::anyInAndOutOfContactUsers_s      = false;
bool           UserVehicle::anyMakeBeforeBreakUsers_s        = false;
bool           UserVehicle::anyGenerateDumpUsers_s           = false;
const char     UserVehicle::REAL_TIME_REQUEST_ID_s           = 'R';
const char     UserVehicle::FLOATING_REQUEST_ID_s            = 'W';
const char     UserVehicle::STORE_REQUEST_ID_s               = 'S';
const char     UserVehicle::OVERLAPPING_REQUEST_ID_s         = 'O';
const char     UserVehicle::REAL_TIME_LOSS_ID_s              = 'L';
const char     UserVehicle::FLOATING_LOSS_ID_s               = 'F';
const char     UserVehicle::VISIBILITY_LOSS_ID_s             = 'V';
const char     UserVehicle::HANDOVER_LOSS_ID_s               = 'H';
const char     UserVehicle::NARROWBAND_CONJUNCTION_LOSS_ID_s = 'j';
const char     UserVehicle::WIDEBAND_CONJUNCTION_LOSS_ID_s   = 'J';
const char     UserVehicle::TYPE_NARROWBAND_LOSS_ID_s        = 't';
const char     UserVehicle::TYPE_WIDEBAND_LOSS_ID_s          = 'T';
const char     UserVehicle::SYSTEM_NARROWBAND_LOSS_ID_s      = 's';
const char     UserVehicle::SYSTEM_WIDEBAND_LOSS_ID_s        = 'S';
const char     UserVehicle::STATE_OF_HEALTH_LOSS_ID_s        = 'X';
const char     UserVehicle::MAKE_BEFORE_BREAK_ID_s           = 'B';
const char     UserVehicle::DIRECT_DOWNLINK_TIME_STEP_ID_s   = 'd';
Array<string>* UserVehicle::ptrIdentificationArray_s         = NULL;


UserVehicle::UserVehicle(shared_ptr<UserVehicle> ptrTrailVehicle)
    : DataProvider(), vehiclePriority_a(0), minimumProcessingRevisitTime_a(0),
      multipleRequestDurations_a(false), numberOfScoreProfiles_a(0),
      dailyVehicleDutyCycle_a(0), serviceTierDefinition_a(1), ptrRollingRevDutyCycle_a(NULL),
      bodyTiltsForCollection_a(false),
      collectionEfficiency_a(1.0), bufferMemorySize_a(0.0),
      effectiveBufferEmptyRate_a(0.0),
      worthNormalizationX_a(0.0), worthNormalizationY_a(0.0),
      sunBatheFlag_a(false),
      useMaximumAllocationsPerRequest_a(false),
      ptrGroupName_a(NULL), ptrTargetDeckFileName_a(NULL),
      ptrAzimuthOffsetAngles_a(NULL),
      ptrTimelineCriteria_a(NULL),
      ptrUserTransmitAntenna_a(NULL), ptrDirectDownlinkAntenna_a(NULL),
      ptrSelfRelayAntennaList_a(NULL),
      ptrValueTimeline_a(NULL), ptrUserType_a(NULL), ptrUserSystem_a(NULL),
      ptrAllocationStartsPerRev_a(NULL), ptrLossTimeStepID_a(NULL),
      ptrTrailVehicle_a(ptrTrailVehicle)
{
    elementType_a = USER_VEHICLE;
    // Init the sra list to avoid NULL checks
    ptrSelfRelayAntennaList_a = shared_ptr<MURALTypes::SRAVector>(new MURALTypes::SRAVector());
    return;
}

UserVehicle&
UserVehicle::operator = (const UserVehicle &copyMe)
{
    DataProvider::operator=(copyMe);
    vehiclePriority_a                 = copyMe.vehiclePriority_a;
    minimumProcessingRevisitTime_a    = copyMe.minimumProcessingRevisitTime_a;
    multipleRequestDurations_a        = copyMe.multipleRequestDurations_a;
    numberOfScoreProfiles_a           = copyMe.numberOfScoreProfiles_a;
    bodyTiltsForCollection_a          = copyMe.bodyTiltsForCollection_a;
    collectionEfficiency_a            = copyMe.collectionEfficiency_a;
    bufferMemorySize_a                = copyMe.bufferMemorySize_a;
	serviceTierDefinition_a           = copyMe.serviceTierDefinition_a;
    effectiveBufferEmptyRate_a        = copyMe.effectiveBufferEmptyRate_a;
    worthNormalizationX_a             = copyMe.worthNormalizationX_a;
    worthNormalizationY_a             = copyMe.worthNormalizationY_a;
    sunBatheFlag_a                    = copyMe.sunBatheFlag_a;
    useMaximumAllocationsPerRequest_a = copyMe.useMaximumAllocationsPerRequest_a;
    ptrUserType_a                     = copyMe.ptrUserType_a;
    ptrUserSystem_a                   = copyMe.ptrUserSystem_a;
    ptrAllocationStartsPerRev_a       = copyMe.ptrAllocationStartsPerRev_a;
    ptrTrailVehicle_a                 = copyMe.ptrTrailVehicle_a;
    ptrUserTransmitAntenna_a          = copyMe.ptrUserTransmitAntenna_a;
    ptrDirectDownlinkAntenna_a        = copyMe.ptrDirectDownlinkAntenna_a;
    ptrGroupName_a                    = NULL;
    ptrTargetDeckFileName_a           = NULL;
    ptrValueTimeline_a                = NULL;
    ptrRollingRevDutyCycle_a          = NULL;

    if (copyMe.ptrGroupName_a != NULL) {
        ptrGroupName_a = shared_ptr<string>(new string(*(copyMe.ptrGroupName_a)));
    }

    if (copyMe.ptrTargetDeckFileName_a != NULL) {
        ptrTargetDeckFileName_a = shared_ptr<string>(new string(*(copyMe.ptrTargetDeckFileName_a)));
    }

    if (copyMe.ptrValueTimeline_a != NULL) {
        ptrValueTimeline_a = copyMe.ptrValueTimeline_a;
    }

    if (copyMe.ptrRollingRevDutyCycle_a != NULL) {
        ptrRollingRevDutyCycle_a = new Array<int>(*(copyMe.ptrRollingRevDutyCycle_a));
    }

    if (copyMe.ptrSelfRelayAntennaList_a != NULL) {
        ptrSelfRelayAntennaList_a = copyMe.ptrSelfRelayAntennaList_a;
    }
    return(*this);
}

bool
UserVehicle::operator == (const UserVehicle &compareMe) const
{
    return( (DataProvider::operator == (compareMe)) &&
            (*ptrGroupName_a == *(compareMe.ptrGroupName_a)) &&
            (worthNormalizationX_a == compareMe.worthNormalizationX_a) &&
            (worthNormalizationY_a == compareMe.worthNormalizationY_a) &&
            (sunBatheFlag_a == compareMe.sunBatheFlag_a) &&
            (vehiclePriority_a == compareMe.vehiclePriority_a) &&
            (minimumProcessingRevisitTime_a == compareMe.minimumProcessingRevisitTime_a) &&
            (multipleRequestDurations_a == compareMe.multipleRequestDurations_a) &&
            (numberOfScoreProfiles_a == compareMe.numberOfScoreProfiles_a)) &&
		    (serviceTierDefinition_a == compareMe.serviceTierDefinition_a);
}

bool
UserVehicle::operator != (const UserVehicle &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
UserVehicle::ResetIdentification()
{
    if (ptrIdentificationArray_s != NULL) {
        delete ptrIdentificationArray_s;
        ptrIdentificationArray_s = NULL;
    }
}

void
UserVehicle::SetIdentification(const Array<string> *ptrIdentification)
{
    int       vehicleIndex = 0;
    const int numberOfVehicles = ptrIdentification->Size();
    string     designator = "";

    if (ptrIdentificationArray_s == NULL) {
        ptrIdentificationArray_s = new Array<string>();
    }

    while (vehicleIndex < numberOfVehicles) {
        designator = ptrIdentification->ElementAt(vehicleIndex);

        if (ptrIdentificationArray_s->ValueExists(designator) == true) {
            throw new InputException(GetClassName(),
                                     "SetIdentification(const Array<string>*)",
                                     "Can not have multiple User Vehicles with the same Designator!");
        }

        ptrIdentificationArray_s->AddElement(designator);
        ++vehicleIndex;
    }

    return;
}

void
UserVehicle::RemoveTrailVehicleIdentification(Array<string> *ptrTrailDesignatorArray)
{
    const int arraySize = ptrTrailDesignatorArray->Size();
    string     trailDesignator = "";

    if (ptrIdentificationArray_s != NULL) {
        int arrayIndex = 0;

        while (arrayIndex < arraySize) {
            trailDesignator = ptrTrailDesignatorArray->ElementAt(arrayIndex);
            ptrIdentificationArray_s->RemoveElement(trailDesignator);
            ++arrayIndex;
        }
    }

    return;
}

void
UserVehicle::SortIdentificationArray()
{
    Array<string> *ptrSortedIdentificationArray;

    if(ptrIdentificationArray_s != NULL) {
        int arrayIndex = 0;
        const int arraySize = ptrIdentificationArray_s->Size();
        ptrSortedIdentificationArray = new Array<string>();

        while (arrayIndex < arraySize) {
            ptrSortedIdentificationArray->InsertElement(ptrIdentificationArray_s->ElementAt(arrayIndex));
            ++arrayIndex;
        }

        delete ptrIdentificationArray_s;
        ptrIdentificationArray_s = ptrSortedIdentificationArray;
        ptrSortedIdentificationArray = NULL;
    }
}

void
UserVehicle::VerifyTargetDeck()
{
    string *ptrFullFileName = NULL;

    if (ptrTargetDeckFileName_a == NULL) {
        throw new InputException(GetClassName(), "VerifyTargetDeck()",
                                 "Missing 'Target Deck File Name' input");
    }

    try {
        ptrFullFileName = new string(*(TargetDeck::GetDeckFileBaseDirectory())
                                     + (*ptrTargetDeckFileName_a));
        ParseUtility::VerifyInputFile(ptrFullFileName);
        TargetDeck::AddTargetDeckFileName(*ptrTargetDeckFileName_a);
        delete ptrFullFileName;
        ptrFullFileName = NULL;
    } catch (Exception *ptrError) {
        delete ptrFullFileName;
        ptrFullFileName = NULL;
        ptrError->AddMethod(GetClassName(), "VerifyTargetDeck()");
        throw;
    }

    return;
}

void
UserVehicle::PrintAttributes(ostream &outfile) const
{
    int            arrayIndex = 0;
    SelfRelayAntenna *ptrSRA = NULL;
    const TIME_UNIT inputTimeUnits = UnitData::GetInputTimeUnits();
    outfile << "<USER_VEHICLE_OBJECT_START>" << NEW_LINE;
    PrintAllNames(outfile);

    if (GetGroupName() != GetType()) {
        outfile << "   Data Provider Group                  : ";
        outfile << GetGroupName() << NEW_LINE;
    }

    if (ptrTrailVehicle_a != NULL) {
        outfile << "   Trail Vehicle Designator             : ";
        outfile << ptrTrailVehicle_a->GetDesignator() << NEW_LINE;
    }

    outfile.unsetf(ios::right);
    outfile.setf(ios::left);
    outfile << "   Body Tilts For Collection            : ";
    outfile << StringEditor::ConvertToString(bodyTiltsForCollection_a) << NEW_LINE;

    if (ptrTargetDeckFileName_a != NULL) {
        outfile << "   Target Deck File Name                : ";
        outfile << *ptrTargetDeckFileName_a << NEW_LINE;
    }

	//RAGUSA 11/27/2018 - Writeout Service Tier Input
	if (serviceTierDefinition_a > 0) {
		outfile << "   Service Tier Definition                : ";
		outfile << serviceTierDefinition_a << NEW_LINE;
	}

    outfile << "   Daily Vehicle Duty Cycle             : ";
    TimePiece::PrintDuration(outfile, dailyVehicleDutyCycle_a, inputTimeUnits);
    outfile << NEW_LINE;

    if (ptrRollingRevDutyCycle_a != NULL) {
        arrayIndex = ptrRollingRevDutyCycle_a->Size() - 1;

        while (arrayIndex >= 0) {
            int dutyCycle = 0;
            dutyCycle = ptrRollingRevDutyCycle_a->ElementAt(arrayIndex);

            if (dutyCycle > 0) {
                outfile << "   Consecutive Rev Duty Cycle [" << (arrayIndex + 1) << "]";

                if (arrayIndex < 9) {
                    outfile << " ";
                }

                outfile << "      : ";
                TimePiece::PrintDuration(outfile, dutyCycle, inputTimeUnits);
                outfile << NEW_LINE;
            }

            --arrayIndex;
        }
    }

    outfile << "   Collection Efficiency                : ";
    outfile << collectionEfficiency_a << NEW_LINE;

    if (bufferMemorySize_a > 0.0) {
        outfile << "   Buffer Memory Size (Gbits)           : ";
        outfile << bufferMemorySize_a << NEW_LINE;
        outfile << "   Effective Buffer Empty Rate (Gbps)   : ";
        outfile << effectiveBufferEmptyRate_a << NEW_LINE;
    }

    if (worthNormalizationX_a > 0.0) {
        outfile << "   Worth Normalization X Factor         : ";
        outfile << setprecision(6) << worthNormalizationX_a << NEW_LINE;
        outfile << "   Worth Normalization Y Factor         : ";
        outfile << setprecision(6) << worthNormalizationY_a << NEW_LINE;
    }

    if (ptrAzimuthOffsetAngles_a != NULL) {
        arrayIndex = 0;
        outfile << "   Azimuth Offset Angles                : ";
        outfile << UnitData::GetInputAngle(ptrAzimuthOffsetAngles_a->ElementAt(arrayIndex));
        ++arrayIndex;

        while (arrayIndex < ptrAzimuthOffsetAngles_a->Size()) {
            outfile << ", " << UnitData::GetInputAngle(ptrAzimuthOffsetAngles_a->ElementAt(arrayIndex));
            ++arrayIndex;
        }

        outfile << NEW_LINE;
    }

    outfile << "   Vehicle Priority                     : ";
    outfile << vehiclePriority_a << NEW_LINE;

    if (minimumProcessingRevisitTime_a > 0) {
        outfile << "   Minimum Processing Revisit Time    : ";
        TimePiece::PrintDuration(outfile, (int)minimumProcessingRevisitTime_a, inputTimeUnits);
        outfile  << NEW_LINE;
    }

    if (sunBatheFlag_a == true) {
        outfile << "   Sun Bathe                            : true" << NEW_LINE;
    }

    outfile.unsetf(ios::left);
    outfile.setf(ios::right);

    if (ptrUserTransmitAntenna_a != NULL) {
        outfile << NEW_LINE;
        ptrUserTransmitAntenna_a->PrintAttributes(outfile);
    }

    if (ptrSelfRelayAntennaList_a->size() > 0) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);
            outfile << NEW_LINE;
            ptrSRA->PrintAttributes(outfile);
        }
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        outfile << NEW_LINE;
        ptrDirectDownlinkAntenna_a->PrintAttributes(outfile);
    }

    DataProvider::PrintAttributes(outfile);

    if (ptrTimelineCriteria_a != NULL) {
        outfile << NEW_LINE;
        ptrTimelineCriteria_a->PrintAttributes(outfile);
    }

    outfile << "<USER_VEHICLE_OBJECT_END>" << NEW_LINE;

    if (ptrTrailVehicle_a != NULL) {
        outfile << NEW_LINE;
        ptrTrailVehicle_a->PrintAttributes(outfile);
    }

    return;
}

void
UserVehicle::UpdateRollingRevs()
{
    const int           numberOfRevs = (int)GetNumberOfRevs();
    Array<int>         *ptrTempRollingRevs = NULL;
    ListIterator<Sensor> sensorIter(ptrSensorList_a);
    Sensor              *ptrSensor = sensorIter.First();

    if (ptrRollingRevDutyCycle_a != NULL) {
        int revIndex = 0;

        if (ptrRollingRevDutyCycle_a->Size() > numberOfRevs) {
            ptrTempRollingRevs = new Array<int>(*ptrRollingRevDutyCycle_a);
            delete ptrRollingRevDutyCycle_a;
            ptrRollingRevDutyCycle_a = new Array<int>(numberOfRevs);
            revIndex = 0;

            while (revIndex < numberOfRevs) {
                (*ptrRollingRevDutyCycle_a)[revIndex] = ptrTempRollingRevs->ElementAt(revIndex);
                ++revIndex;
            }

            delete ptrTempRollingRevs;
            ptrTempRollingRevs = NULL;
        }

        revIndex = ptrRollingRevDutyCycle_a->Size() - 1;

        while (revIndex > 0) {
            if (ptrRollingRevDutyCycle_a->ElementAt(revIndex) <= ptrRollingRevDutyCycle_a->ElementAt(revIndex - 1)) {
                (*ptrRollingRevDutyCycle_a)[revIndex] = LONG_ZERO;
            }

            --revIndex;
        }
    }

    while (ptrSensor != NULL) {
        ptrSensor->UpdateRollingRevs(numberOfRevs);
        ptrSensor = sensorIter.Next();
    }

    return;
}

int
UserVehicle::GetDeckIndex(const Sensor *ptrSensor) const
{
    int deckIndex = -1;

    if (ptrTimelineCriteria_a->UseRequirementDeck() == true) {
        if (ptrSensor == NULL) {
            throw new NullPointerException(GetClassName(), "GetDeckIndex(const Sensor*)", "ptrSensor");
        }

        deckIndex = FileNames::FindIndexInList(FileNames::REQUIREMENT_DECK, *(ptrSensor->GetRequirementDeckFileName()));
    } else if (ptrTargetDeckFileName_a != NULL) {
        int numberOfRequirementDecks = FileNames::GetFileListSize(FileNames::REQUIREMENT_DECK);
        deckIndex = numberOfRequirementDecks + TargetDeck::GetTargetDeckIndex(*ptrTargetDeckFileName_a);
    }

    return(deckIndex);
}

void
UserVehicle::AddValueTimelineElement(const int &timeIndex,
                                     ValueTimelineElement *ptrNewElement)
{
    if (ptrValueTimeline_a == NULL) {
        ptrValueTimeline_a = shared_ptr<ValueTimeline>(new ValueTimeline());
    }

    ptrValueTimeline_a->AddValueTimelineElement(timeIndex, ptrNewElement);
    return;
}


double
UserVehicle::CalculateElevationAngle(const int &timeIndex,
                                     const int &resourceIndex) const
{
    double  regionImaging = 0.0;
    double elevationAngle = UNDEFINED_ANGLE;

    if (ptrValueTimeline_a != NULL) {
        regionImaging = ptrValueTimeline_a->GetImagedRegion(timeIndex, resourceIndex);
    }

    elevationAngle = CalculateElevationAngle(timeIndex, resourceIndex, regionImaging);
    return(elevationAngle);
}

double
UserVehicle::CalculateElevationAngle(const int &timeIndex,
                                     const int &resourceIndex,
                                     const double &regionImaging) const
{
    double           elevationAngle = UNDEFINED_ANGLE;
    SpaceVector      lineOfSightVector;
    shared_ptr<CoordinateFrame> ptrVehicleFrame = GetCoordinateFrame(timeIndex, 0.0, regionImaging);
    const Link      *ptrTransmitLink = GetLinkTo(resourceIndex);

    if (ptrVehicleFrame != NULL) {
        if (ptrTransmitLink != NULL) {
            lineOfSightVector = ptrTransmitLink->GetLineOfSightVector(timeIndex);
            elevationAngle = lineOfSightVector.GetSeparationAngle(ptrVehicleFrame->GetZAxis());
        }
    }

    return(elevationAngle);
}

void
UserVehicle::CheckTransmitLinkStatus()
{
    bool  foundValidLink = false;
    Link *ptrLink = NULL;
    SelfRelayAntenna *ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        ListIterator<Link> crosslinksListIter(ptrUserTransmitAntenna_a->GetLinksList());
        ptrLink = crosslinksListIter.First();

        while (ptrLink != NULL) {
            if (ptrLink->IsPrecluded() == false) {
                foundValidLink = true;
                ptrLink = NULL;
            } else {
                ptrLink = crosslinksListIter.Next();
            }
        }

        if (foundValidLink == false) {
            throw new InputException(GetClassName(), "CheckTransmitLinkStatus()",
                                     (GetDesignator() + " has no valid link for user transmit antenna"));
        }
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        ListIterator<Link> directDownlinksListIter(ptrDirectDownlinkAntenna_a->GetLinksList());
        foundValidLink = false;
        ptrLink = directDownlinksListIter.First();

        while (ptrLink != NULL) {
            if (ptrLink->IsPrecluded() == false) {
                foundValidLink = true;
                ptrLink = NULL;
            } else {
                ptrLink = directDownlinksListIter.Next();
            }
        }

        if (foundValidLink == false) {
            throw new InputException(GetClassName(), "CheckTransmitLinkStatus()",
                                     (GetDesignator() + " has no valid link for direct downlink antenna"));
        }
    }

    if (ptrSelfRelayAntennaList_a->size() > 0) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);

            if ( ptrSRA->IsTransmittingAntenna() == true ) {
                ListIterator<Link> selfRelayLinksListIter(ptrSRA->GetLinksList());
                foundValidLink = false;
                ptrLink = selfRelayLinksListIter.First();

                while (ptrLink != NULL) {
                    if (ptrLink->IsPrecluded() == false) {
                        foundValidLink = true;
                        ptrLink = NULL;
                    } else {
                        ptrLink = selfRelayLinksListIter.Next();
                    }
                }
            }
        }

        if (foundValidLink == false) {
            throw new InputException(GetClassName(), "CheckTransmitLinkStatus()",
                                     (GetDesignator() + " has no valid link for self relay antenna"));
        }
    }

    return;
}

bool
UserVehicle::HasTransmitSRA() const
{
    bool                            hasTransmitSRA = false;
    MURALTypes::SRAVector::iterator sraIter;
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (hasTransmitSRA == false); ++sraIter) {
        ptrSRA = (*sraIter);
        hasTransmitSRA = ptrSRA->IsTransmittingAntenna();
    }

    return(hasTransmitSRA);
}

bool
UserVehicle::HasReceiveSRA() const
{
    bool                           hasReceiveSRA = false;
    MURALTypes::SRAVector::iterator sraIter;
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (hasReceiveSRA == false); ++sraIter) {
        ptrSRA = (*sraIter);
        hasReceiveSRA = ptrSRA->IsReceiveAntenna();
    }

    return(hasReceiveSRA);
}

int
UserVehicle::GetNumberOfTimelineResources() const
{
    int numberOfTimelineResources = 1;

    if (GenerateResourceSpecificTimeline() == true) {
        numberOfTimelineResources = GetNumberOfResources();
    }

    return(numberOfTimelineResources);
}

void
UserVehicle::SetWidebandTransmission(const Link::WIDEBAND_TRANSMISSION &transmit) const
{
    ListIterator<Link> linksListIter(GetTransmitLinksList());
    Link              *ptrLink = linksListIter.First();

    while (ptrLink != NULL) {
        ptrLink->SetWidebandTransmission(transmit);
        ptrLink = linksListIter.Next();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        ListIterator<Link> directDownlinksListIter(ptrDirectDownlinkAntenna_a->GetLinksList());
        ptrLink = directDownlinksListIter.First();

        while (ptrLink != NULL) {
            ptrLink->SetWidebandTransmission(transmit);
            ptrLink = directDownlinksListIter.Next();
        }
    }

    if (ptrSelfRelayAntennaList_a->size() > 0) {
        MURALTypes::SRAVector::iterator sraIter;
        shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);

            if ( ptrSRA->IsTransmittingAntenna() == true ) {
                ListIterator<Link> selfRelayLinksListIter(ptrSRA->GetLinksList());
                ptrLink = selfRelayLinksListIter.First();

                while (ptrLink != NULL) {
                    ptrLink->SetWidebandTransmission(transmit);
                    ptrLink = selfRelayLinksListIter.Next();
                }
            }
        }
    }

    return;
}

bool
UserVehicle::PossibleWidebandConjunctionProblem(const int &startTimeIndex,
        const int &endTimeIndex) const
{
    const int         startCheckTime = std::max(startTimeIndex, TimePiece::GetStartIndexTime());
    const int         endCheckTime = std::min(endTimeIndex, TimePiece::GetEndIndexTime());
    bool               possibleProblem = false;
    ListIterator<Link> crosslinksListIter(GetTransmitLinksList());
    Link              *ptrLink = crosslinksListIter.First();

    while ((ptrLink != NULL) && (possibleProblem == false)) {
        int checkTime = 0;
        checkTime = startCheckTime;

        while ((checkTime <= endCheckTime) && (possibleProblem == false)) {
            if ( (ptrLink->IsInview(checkTime) == true) &&
                    ((ptrLink->IsAllocated(checkTime) == true) ||
                     (ptrLink->CheckReceiveCapacityFull(checkTime) == false)) ) {
                possibleProblem = ptrLink->WidebandConjunctionProblem(checkTime);
            }

            ++checkTime;
        }

        ptrLink = crosslinksListIter.Next();
    }

    if ((possibleProblem == false) && (ptrTrailVehicle_a != NULL)) {
        possibleProblem = ptrTrailVehicle_a->PossibleWidebandConjunctionProblem(startTimeIndex, endTimeIndex);
    }

    return(possibleProblem);
}

bool
UserVehicle::PossibleNarrowbandConjunctionProblem(const int &timeIndex) const
{
    bool               possibleProblem = false;
    ListIterator<Link> crosslinksListIter(GetTransmitLinksList());
    Link              *ptrLink = crosslinksListIter.First();

    while ((ptrLink != NULL) && (possibleProblem == false)) {
        if ( (ptrLink->IsInview(timeIndex) == true) &&
                (ptrLink->CheckReceiveCapacityFull(timeIndex) == false) ) {
            possibleProblem = ptrLink->NarrowbandConjunctionProblem(timeIndex);
        }

        ptrLink = crosslinksListIter.Next();
    }

    if ((possibleProblem == false) && (ptrTrailVehicle_a != NULL)) {
        possibleProblem = ptrTrailVehicle_a->PossibleNarrowbandConjunctionProblem(timeIndex);
    }

    return(possibleProblem);
}

bool
UserVehicle::PossibleStateOfHealthProblem(const int &timeIndex) const
{
    bool               possibleProblem = false;
    ListIterator<Link> crosslinksListIter(GetTransmitLinksList());
    Link              *ptrLink = crosslinksListIter.First();

    // only have to check crosslinks since SOH is a relay issue
    while ((ptrLink != NULL) && (possibleProblem == false)) {
        possibleProblem = ptrLink->StateOfHealthLinkProblem(timeIndex);
        ptrLink = crosslinksListIter.Next();
    }

    if ((possibleProblem == false) && (ptrTrailVehicle_a != NULL)) {
        possibleProblem = ptrTrailVehicle_a->PossibleStateOfHealthProblem(timeIndex);
    }

    return(possibleProblem);
}

double
UserVehicle::GetMaximumCollectionDataRate(const int &timeIndex) const
{
    double      maxCollectionDataRate = 0.0;
    Sensor     *ptrSensor = NULL;

    try {
        int sensorNumber = 0;
        int resourceIndex = -1;

        if (numberOfScoreProfiles_a == 0) {
            sensorNumber = ptrValueTimeline_a->GetSensorNumber(timeIndex, resourceIndex);

            if (sensorNumber > 0) {
                ptrSensor = GetSensor(sensorNumber);

                if (ptrSensor != NULL) {
                    maxCollectionDataRate = ptrSensor->GetDataRate();
                }
            }
        } else {
            resourceIndex = 0;

            while (resourceIndex < numberOfScoreProfiles_a) {
                sensorNumber = ptrValueTimeline_a->GetSensorNumber(timeIndex, resourceIndex);

                if (sensorNumber > 0) {
                    ptrSensor = GetSensor(sensorNumber);

                    if (ptrSensor != NULL) {
                        maxCollectionDataRate = std::max(maxCollectionDataRate,
                                                         ptrSensor->GetDataRate());
                    }
                }

                ++resourceIndex;
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetMaximumCollectionDataRate(const int&)");
        throw;
    }

    return(maxCollectionDataRate);
}

double
UserVehicle::GetMaximumCollectionDataRate(const int &startTimeIndex,
        const int &endTimeIndex) const
{
    int   timeIndex = startTimeIndex;
    double maxCollectionDataRate = 0.0;

    try {
        while (timeIndex <= endTimeIndex) {
            maxCollectionDataRate = std::max(GetMaximumCollectionDataRate(timeIndex),
                                             maxCollectionDataRate);
            ++timeIndex;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetMaximumCollectionDataRate(const int&, const int&)");
        throw;
    }

    return(maxCollectionDataRate);
}

Sensor*
UserVehicle::GetSensorCollecting(const int &timeIndex,
                                 int resourceIndex) const
{
    const int sensorNumber = GetTimelineSensorNumber(timeIndex, resourceIndex);
    Sensor     *ptrSensor = NULL;

    if (sensorNumber > 0) {
        ptrSensor = GetSensor(sensorNumber);
    }

    return(ptrSensor);
}

double
UserVehicle::GetMaximumSensorCollectingDataRate(const int &startTime,
        const int &endTime,
        int resourceIndex) const
{
    int    timeIndex = startTime;
    double  maxDataRate = 0.0;
    Sensor *ptrSensor = NULL;

    while (timeIndex <= endTime) {
        ptrSensor = GetSensorCollecting(timeIndex, resourceIndex);

        if (ptrSensor != NULL) {
            maxDataRate = std::max(maxDataRate, ptrSensor->GetDataRate());
        }

        ++timeIndex;
    }

    return(maxDataRate);
}

double
UserVehicle::GetMaximumTransmitDataRate() const
{
    double                       maximumTransmitDataRate = 0.0;
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        maximumTransmitDataRate = ptrUserTransmitAntenna_a->GetDataRate()
                                  * (double)ptrUserTransmitAntenna_a->GetCapacity();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        maximumTransmitDataRate += (ptrDirectDownlinkAntenna_a->GetDataRate()
                                    * (double)ptrDirectDownlinkAntenna_a->GetCapacity());
    }

    MURALTypes::SRAVector::iterator sraIter;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
        ptrSRA = (*sraIter);

        if (ptrSRA->IsTransmittingAntenna() == true) {
            maximumTransmitDataRate += (ptrSRA->GetDataRate()
                                        * (double)ptrSRA->GetCapacity());
        }
    }

    return(maximumTransmitDataRate);
}

double
UserVehicle::GetMaximumReceiveDataRate() const
{
    double                         maximumReceiveDataRate = 0.0;
    shared_ptr<SelfRelayAntenna>   ptrSRA = NULL;
    MURALTypes::SRAVector::iterator sraIter;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
        ptrSRA = (*sraIter);

        if (ptrSRA->IsReceiveAntenna() == true) {
            maximumReceiveDataRate += (ptrSRA->GetDataRate()
                                       * (double)ptrSRA->GetCapacity());
        }
    }

    return(maximumReceiveDataRate);
}

int
UserVehicle::GetDutyCycle(int revIndex) const
{
    int   dutyCycle = 0;

    if (revIndex < 0) {
        double dailyFactor;
        dailyFactor = (TimePiece::GetReportableTimeSteps()
                       * TimePiece::GetSecondsPerTimeStep()) / SECONDS_PER_DAY;
        dutyCycle = (int)((double)dailyVehicleDutyCycle_a * dailyFactor);
    } else if (ptrRollingRevDutyCycle_a != NULL) {
        if (revIndex < ptrRollingRevDutyCycle_a->Size()) {
            dutyCycle = ptrRollingRevDutyCycle_a->ElementAt(revIndex);
        }
    }

    return(dutyCycle);
}

bool
UserVehicle::GetResourceInview(const int &timeIndex) const
{
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;
    bool resourceInview = false;

    if (ptrUserTransmitAntenna_a != NULL) {
        resourceInview = ptrUserTransmitAntenna_a->IsThereAnyLink(timeIndex);
    }

    if ((ptrDirectDownlinkAntenna_a != NULL) && (resourceInview == false)) {
        resourceInview = ptrDirectDownlinkAntenna_a->IsThereAnyLink(timeIndex);
    }

    if ((ptrSelfRelayAntennaList_a->size() > 0) && (resourceInview == false)) {
        MURALTypes::SRAVector::iterator sraIter;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (resourceInview == false); ++sraIter) {
            ptrSRA = (*sraIter);

            if ( ptrSRA->IsTransmittingAntenna() == true ) {
                resourceInview = ptrSRA->IsThereAnyLink(timeIndex);
            }
        }
    }

    if ((resourceInview == false) && (ptrTrailVehicle_a != NULL)) {
        resourceInview = ptrTrailVehicle_a->GetResourceInview(timeIndex);
    }

    return(resourceInview);
}

int
UserVehicle::GetNumberOfResources() const
{
    //RAGUSA 11/28/2017 - Changed to include direct users, also includes crosslink & direct combined users
    int numberOfResources = 0;

    if (IsACrosslinkVehicle() == true) {
        numberOfResources = (int)RelaySatellite::GetRelaySatelliteCount();
    }

    if (IsADirectDownlinkVehicle() == true) {
        numberOfResources += (int)GroundStation::GetGroundStationCount();
    }

    //*****************************************************************************************************
    return(numberOfResources);
}

bool
UserVehicle::ConvertResourceDesignatorToIndex(const string &resourceString,
        int &resourceIndex) const
{
    bool validResource;
    int numberOfRelays = RelaySatellite::GetRelaySatelliteCount();
    // Assume that the Crosslinks are the lower indexes for now
    validResource = RelaySatellite::ConvertDesignatorToIndex(resourceString, resourceIndex);

    if (validResource == false) {
        // Increment past the relay indices to the DirectDownlink indexes
        resourceIndex += numberOfRelays;
        validResource = GroundStation::ConvertDesignatorToIndex(resourceString, resourceIndex);
    }

    return(validResource);
}

string
UserVehicle::GetResourceDesignator(const int &resourceIndex) const
{
    int       useIndex = resourceIndex;
    int       numberOfRelays = (int)RelaySatellite::GetRelaySatelliteCount();
    int       numberOfGroundStations = (int)GroundStation::GetGroundStationCount();
    string     resourceDesignator = "";

    /*RAGUSA 11/28/2017 - Added to include direct only users, if the user has no
      UTA for communicating to relays then relays are not a valid resource.
      The same is true for crosslink only users (groundstations are not valid resources),
      the other cases are combined UTA & DDL and Self-relay*/
    if (IsACrosslinkVehicle() == false) {
        numberOfRelays = 0;
    }

    if (IsADirectDownlinkVehicle() == false) {
        numberOfGroundStations = 0;
    }

    //***********************************************************************************
    // assume that the crosslinks are the lower indices for now
    if (useIndex < numberOfRelays) {
        resourceDesignator = RelaySatellite::GetDesignatorString(useIndex);
    } else {
        // convert it for the ground station array
        useIndex -= numberOfRelays;

        if (useIndex < numberOfGroundStations) {
            resourceDesignator = GroundStation::GetDesignatorString(useIndex);
        } else {
            useIndex -= numberOfGroundStations;

            if (useIndex < GetUserVehicleCount()) {
                resourceDesignator = GetDesignatorString(useIndex);
            }
        }
    }

    return(resourceDesignator);
}

double
UserVehicle::GetAvailableBitsInDuration(const int &duration) const
{
    double availableBits = 0.0;

    if ( effectiveBufferEmptyRate_a > 0.0 ) {
        availableBits = duration * effectiveBufferEmptyRate_a * TimePiece::GetSecondsPerTimeStep();
    }

    availableBits = std::min(bufferMemorySize_a, availableBits);
    return( availableBits );
}

void
UserVehicle::AddRevisitInviewTime(Array3D<bool> *ptrInviewTime) const
{
    int           timeIndex = TimePiece::GetReportStartIndexTime();
    int           endTimeIndex = TimePiece::GetReportEndIndexTime();
    int           regionIndex = 0;
    const int     NUMBER_OF_REGIONS = Region::GetNumberOfRegions();
    const int     NUMBER_OF_SUB_REGIONS = Earth::GetSubRegionsPerRegion();
    const int     NUMBER_OF_TIME_STEPS = TimePiece::GetNumberOfTimeSteps();
    Array<bool>   *ptrNewTimeStepArray = NULL;
    Array3D<bool> *ptrUserInviewTime = new Array3D<bool>(NUMBER_OF_REGIONS, NUMBER_OF_SUB_REGIONS,
            TimePiece::GetNumberOfTimeSteps(), false);
    TimePeriod                  *ptrInviewPeriod = NULL;
    LinkList<TimePeriod>        *ptrInviewPeriodList = NULL;

        AddRevisitTimes(ptrUserInviewTime, timeIndex, endTimeIndex,
                        NUMBER_OF_SUB_REGIONS, NUMBER_OF_TIME_STEPS);

    while (regionIndex < NUMBER_OF_REGIONS) {
        int subRegionIndex = 0;
        subRegionIndex = 0;

        while (subRegionIndex < NUMBER_OF_SUB_REGIONS) {
            if (ptrUserInviewTime->ElementAt(regionIndex, subRegionIndex) != NULL) {
                if ((ptrInviewPeriodList
                        = TimePeriod::GroupIntoPeriods(ptrUserInviewTime->ElementAt(regionIndex, subRegionIndex))) != NULL) {
                    if (ptrInviewTime->ElementAt(regionIndex, subRegionIndex) == NULL) {
                        ptrNewTimeStepArray = new Array<bool>(NUMBER_OF_TIME_STEPS, false);
                        ptrInviewTime->AddArray(regionIndex, subRegionIndex, ptrNewTimeStepArray);
                        ptrNewTimeStepArray = NULL;
                    }

                    //JOE:
                    ptrInviewPeriod = ptrInviewPeriodList->Retrieve();

                    while (ptrInviewPeriod != NULL) {
                        if (ptrInviewPeriod->GetDuration() >= (int)minimumProcessingRevisitTime_a) {
                            timeIndex = ptrInviewPeriod->GetStartTime();
                            endTimeIndex = ptrInviewPeriod->GetEndTime();

                            while (timeIndex <= endTimeIndex) {
                                (*ptrInviewTime)[regionIndex][subRegionIndex][timeIndex] = true;
                                ++timeIndex;
                            }
                        }

                        delete ptrInviewPeriod;
                        ptrInviewPeriod = NULL;
                        ptrInviewPeriod = ptrInviewPeriodList->Retrieve();
                    }

                    delete ptrInviewPeriodList;
                    ptrInviewPeriodList = NULL;
                }
            }

            ++subRegionIndex;
        }

        ++regionIndex;
    }
}

void
UserVehicle::AddRevisitTimes(Array3D<bool> *ptrUserInviewTime,
                             const int &startTimeIndex,
                             const int &endTimeIndex,
                             const int &numberOfSubRegions,
                             const int &numberOfTimeSteps) const
{
    int                 regionIndex = 0;
    int                 subRegionIndex = 0;
    int                 timeIndex = startTimeIndex;
    Array<bool>         *ptrNewTimeStepArray = NULL;
    Sensor              *ptrSensor = NULL;
    Region              *ptrRegion = NULL;
    LinkList<Region>    *ptrRegionsInViewList = new LinkList<Region>();
    ListIterator<Sensor> sensorListIter(ptrSensorList_a);

    while (timeIndex <= endTimeIndex) {
        ptrSensor = sensorListIter.First();

        while (ptrSensor != NULL) { //includes all sensors
            (ptrSensor->GetAccessPattern())->GetAllContainedRegions(ptrRegionsInViewList,
                    GetPosition(timeIndex), GetVelocity(timeIndex),
                    AccessPattern::REVISIT);
            //JOE:
            ptrRegion = ptrRegionsInViewList->Retrieve();

            while (ptrRegion != NULL) {
                // check for match to excluded regions Array HERE
                if (ptrRegion->IsRevisitRegion() == true) {
                    regionIndex = ptrRegion->GetRegionNumber() - 1;

                    if (numberOfSubRegions > 1) {
                        subRegionIndex = (int)ptrRegion->GetSubRegionNumber() - 1;
                    }

                    if (ptrUserInviewTime->ElementAt(regionIndex, subRegionIndex) == NULL) {
                        ptrNewTimeStepArray = new Array<bool>(numberOfTimeSteps, false);
                        ptrUserInviewTime->AddArray(regionIndex, subRegionIndex, ptrNewTimeStepArray);
                        ptrNewTimeStepArray = NULL;
                    }

                    (*ptrUserInviewTime)[regionIndex][subRegionIndex][timeIndex] = true;
                }

                ptrRegion = ptrRegionsInViewList->Retrieve();
            }

            ptrSensor = sensorListIter.Next();
        }

        ++timeIndex;
    }

    delete ptrRegionsInViewList;
    ptrRegionsInViewList = NULL;
}

void
UserVehicle::GetCrisisData(const int &crisisNumber,int &timeInview) const
{
	int       timeIndex = 0;
	int       endTimeIndex = 0;
	const int reportStartTime = TimePiece::GetReportStartIndexTime();
	const int reportEndTime = TimePiece::GetReportEndIndexTime();
	timeInview = 0;
	timeIndex = reportStartTime;

	while (timeIndex <= reportEndTime) {
		if (ptrValueTimeline_a->GetCrisisLevel(timeIndex) == crisisNumber) {
			++timeInview;
		}
		++timeIndex;
	}
	return;
}

char
UserVehicle::GetResourceCharacter(const int &timeIndex,
                                  const int &capacityIndex) const
{
    int       useCapacityIndex = capacityIndex;
    const int transmitCapacity = GetUserTransmitCapacity();
    const int directDownlinkCapacity = GetDirectDownlinkCapacity();
    char        resourceCharacter = EMPTY_CAPACITY_CHARACTER;
    string      resourceDesignator = "";
    Link       *ptrLink = NULL;
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

    try {
        int resourceID = 0;
        int rfIndex = 0;
        int capacityNumber = 0;
        bool isCrosslink = false;
        bool isDirectDownlink = false;

        if (useCapacityIndex < transmitCapacity) {
            isCrosslink = true;
            resourceID = ptrUserTransmitAntenna_a->GetAllocatedAssetNumber(timeIndex, useCapacityIndex);
        } else {
            useCapacityIndex -= transmitCapacity;

            if (useCapacityIndex < directDownlinkCapacity) {
                isDirectDownlink = true;
                resourceID = ptrDirectDownlinkAntenna_a->GetAllocatedAssetNumber(timeIndex, useCapacityIndex);
            } else {
                bool isSelfRelayLink = false;
                int selfRelayCapacity = 0;
                useCapacityIndex -= directDownlinkCapacity;
                MURALTypes::SRAVector::iterator sraIter;

                for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (isSelfRelayLink == false); ++sraIter) {
                    ptrSRA = (*sraIter);

                    if (ptrSRA->IsTransmittingAntenna() == true) {
                        selfRelayCapacity = ptrSRA->GetCapacity();

                        if (useCapacityIndex < selfRelayCapacity) {
                            isSelfRelayLink = true;
                            resourceID = ptrSRA->GetAllocatedAssetNumber(timeIndex, useCapacityIndex);
                        } else {
                            resourceID = ERROR_ASSIGNMENT;
                        }

                        useCapacityIndex -= selfRelayCapacity;
                    }
                }
            }
        }

        if (resourceID == ERROR_ASSIGNMENT) {
            resourceCharacter = ERROR_CHARACTER;
        } else if (resourceID != 0) {
            resourceID = (int)SimpleMath::Abs(resourceID);

            if (isCrosslink == true) {
                resourceCharacter = OutputGenerator::GetRelayCharacter(resourceID);

                if (RelaySatellite::ConvertIndexToDesignator(((int)resourceID - 1), resourceDesignator) == true) {
                    ptrLink = GetAllocatedCrosslink(resourceDesignator, timeIndex);
                }
            } else if (isDirectDownlink == true) {
                resourceCharacter = resourceID + '@';

                if (GroundStation::ConvertIndexToDesignator(((int)resourceID - 1), resourceDesignator) == true) {
                    ptrLink = GetAllocatedDirectDownlink(resourceDesignator, timeIndex);
                }

                // We want the RF ID for direct downlink
                capacityNumber = ptrLink->GetReceiveAntenna()->GetAllocatedCapacity(timeIndex, GetNumber());

                if (capacityNumber > 0) {
                    rfIndex           = (int)(capacityNumber - 1);
                    resourceCharacter = ReceiveFacilityAntenna::GetIdentification((int)resourceID - 1, rfIndex);
                }
            } else {
                resourceCharacter = resourceID + '@';

                if (ConvertIndexToDesignator(((int)resourceID - 1), resourceDesignator) == true) {
                    ptrLink = GetAllocatedTransmitSelfRelayLink(resourceDesignator, timeIndex);
                }
            }

            if (ptrLink != NULL) {
                if (ptrLink->IsAllocatedTransmitPrep(timeIndex) == true) {
                    resourceCharacter = USER_PREP_CHARACTER;
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "");
        throw;
    }

    return(resourceCharacter);
}

int
UserVehicle::PrintMakeBeforeBreaks(ofstream &outfile) const
{
    int        numberOfMakeBeforeBreaks = 0;
    int         totalTimeMBB = 0;
    int         timeIndex = TimePiece::GetReportStartIndexTime();
    const int   reportEndTimeIndex = TimePiece::GetReportEndIndexTime();
    bool         withinMBBPeriod = false;
    const string userDesignator = GetDesignator();

    while (timeIndex <= reportEndTimeIndex) {
        if (IsConductingMakeBeforeBreak(timeIndex) == true) {
            ++totalTimeMBB;

            if (withinMBBPeriod == false) {
                ++numberOfMakeBeforeBreaks;
                withinMBBPeriod = true;
            }
        } else if (withinMBBPeriod == true) {
            withinMBBPeriod = false;
        }

        ++timeIndex;
    }

    outfile << "USER VEHICLE ";
    outfile << setw(9) << userDesignator;
    outfile << " ";
    outfile << " : Conducted " << setw(3) << numberOfMakeBeforeBreaks;
    outfile << " Make-Before-Breaks, Total Time Of ";
    TimePiece::PrintDuration(outfile, totalTimeMBB);
    outfile << NEW_LINE;
    return(totalTimeMBB);
}

Link*
UserVehicle::GetLinkTo(const string &resourceDesignator) const
{
    Link *ptrLink = NULL;

    if (RelaySatellite::ValidRelaySatellite(resourceDesignator) == true) {
        ptrLink = GetCrosslink(resourceDesignator);
    } else if (GroundStation::ValidGroundStation(resourceDesignator) == true) {
        ptrLink = GetDirectDownlink(resourceDesignator);
    } else if (ValidUserVehicle(resourceDesignator) == true) {
        ptrLink = GetTransmitSelfRelayLink(resourceDesignator);
    }

    return(ptrLink);
}

Link*
UserVehicle::GetLinkTo(const int &resourceIndex) const
{
    const string resourceDesignator = GetResourceDesignator(resourceIndex);
    return(GetLinkTo(resourceDesignator));
}

Link*
UserVehicle::GetAllocatedCrosslink(const string &relayDesignator,
                                   const int &timeIndex) const
{
    Link *ptrLink = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        ptrLink = ptrUserTransmitAntenna_a->GetAllocatedLinkTo(relayDesignator, timeIndex);
    }

    return(ptrLink);
}

Link*
UserVehicle::GetCrosslink(const string &relayDesignator) const
{
    Link *ptrLink = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        ptrLink = ptrUserTransmitAntenna_a->GetLinkTo(relayDesignator);
    }

    return(ptrLink);
}

Link*
UserVehicle::GetCrosslink(const string &relayDesignator,
                          const string &antennaDesignator) const
{
    Link *ptrLink = NULL;

    if (ptrUserTransmitAntenna_a != NULL) {
        ptrLink = ptrUserTransmitAntenna_a->GetLinkTo(relayDesignator, antennaDesignator);
    }

    return(ptrLink);
}

Link*
UserVehicle::GetAllocatedDirectDownlink(const string &groundDesignator,
                                        const int &timeIndex) const
{
    Link *ptrLink = NULL;

    if (ptrDirectDownlinkAntenna_a != NULL) {
        ptrLink = ptrDirectDownlinkAntenna_a->GetAllocatedLinkTo(groundDesignator, timeIndex);
    }

    return(ptrLink);
}

Link*
UserVehicle::GetDirectDownlink(const string &groundDesignator) const
{
    Link *ptrLink = NULL;

    if (ptrDirectDownlinkAntenna_a != NULL) {
        ptrLink = ptrDirectDownlinkAntenna_a->GetLinkTo(groundDesignator);
    }

    return(ptrLink);
}

Link*
UserVehicle::GetDirectDownlink(const string &groundDesignator,
                               const string &antennaDesignator) const
{
    Link *ptrLink = NULL;

    if (ptrDirectDownlinkAntenna_a != NULL) {
        ptrLink = ptrDirectDownlinkAntenna_a->GetLinkTo(groundDesignator, antennaDesignator);
    }

    return(ptrLink);
}

Link*
UserVehicle::GetTransmitSelfRelayLink(const string &userDesignator,
                                      const string &antennaDesignator) const
{
    Link                          *ptrLink = NULL;
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;
    MURALTypes::SRAVector::iterator sraIter;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (ptrLink == NULL); ++sraIter) {
        ptrSRA = (*sraIter);

        if (ptrSRA->IsTransmittingAntenna() == true) {
            ptrLink = ptrSRA->GetLinkTo(userDesignator, antennaDesignator);
        }
    }

    return(ptrLink);
}

Link*
UserVehicle::GetReceiveSelfRelayLink(const string &userDesignator,
                                     const string &antennaDesignator) const
{
    Link                        *ptrLink = NULL;
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;
    MURALTypes::SRAVector::iterator sraIter;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (ptrLink == NULL); ++sraIter) {
        ptrSRA = (*sraIter);

        if (ptrSRA->IsReceiveAntenna() == true) {
            ptrLink = ptrSRA->GetLinkTo(userDesignator, antennaDesignator);
        }
    }

    return(ptrLink);
}

Link*
UserVehicle::GetAllocatedTransmitSelfRelayLink(const string &userDesignator,
        const int &timeIndex) const
{
    Link                          *ptrLink = NULL;
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;
    MURALTypes::SRAVector::iterator sraIter;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (ptrLink == NULL); ++sraIter) {
        ptrSRA = (*sraIter);

        if (ptrSRA->IsTransmittingAntenna() == true) {
            ptrLink = ptrSRA->GetAllocatedLinkTo(userDesignator, timeIndex);
        }
    }

    return(ptrLink);
}

Link*
UserVehicle::GetTransmitSelfRelayLink(const string &userDesignator) const
{
    Link                          *ptrLink = NULL;
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;
    MURALTypes::SRAVector::iterator sraIter;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (ptrLink == NULL); ++sraIter) {
        ptrSRA = (*sraIter);

        if (ptrSRA->IsTransmittingAntenna() == true) {
            ptrLink = ptrSRA->GetLinkTo(userDesignator);
        }
    }

    return(ptrLink);
}

Link*
UserVehicle::GetReceiveSelfRelayLink(const string &userDesignator) const
{
    Link                          *ptrLink = NULL;
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;
    MURALTypes::SRAVector::iterator sraIter;

    for( sraIter = ptrSelfRelayAntennaList_a->begin(); (sraIter != ptrSelfRelayAntennaList_a->end()) && (ptrLink == NULL); ++sraIter) {
        ptrSRA = (*sraIter);

        if (ptrSRA->IsReceiveAntenna() == true) {
            ptrLink = ptrSRA->GetLinkTo(userDesignator);
        }
    }

    return(ptrLink);
}

LinkList<Link>*
UserVehicle::GetAllTransmitLinks() const
{
    const string       userDesignator = GetDesignator();
    LinkList<Link>    *ptrAllUserLinksList = new LinkList<Link>();
    ListIterator<Link> crosslinksListIter(GetTransmitLinksList());
    Link              *ptrLink = crosslinksListIter.First();
    shared_ptr<SelfRelayAntenna> ptrSRA = NULL;

    while (ptrLink != NULL) {
        ptrAllUserLinksList->Append(ptrLink);
        ptrLink = crosslinksListIter.Next();
    }

    if (ptrDirectDownlinkAntenna_a != NULL) {
        ListIterator<Link> directDownlinksListIter(ptrDirectDownlinkAntenna_a->GetLinksList());
        ptrLink = directDownlinksListIter.First();

        while (ptrLink != NULL) {
            ptrAllUserLinksList->Append(ptrLink);
            ptrLink = directDownlinksListIter.Next();
        }
    }

    if (ptrSelfRelayAntennaList_a->size() > 0) {
        MURALTypes::SRAVector::iterator sraIter;

        for( sraIter = ptrSelfRelayAntennaList_a->begin(); sraIter != ptrSelfRelayAntennaList_a->end(); ++sraIter) {
            ptrSRA = (*sraIter);

            if (ptrSRA->IsTransmittingAntenna() == true) {
                ListIterator<Link> selfRelayLinksListIter(ptrSRA->GetLinksList());
                ptrLink = selfRelayLinksListIter.First();

                while (ptrLink != NULL) {
                    if (ptrLink->GetTransmitDesignator() == userDesignator) {
                        ptrAllUserLinksList->Append(ptrLink);
                    }

                    ptrLink = selfRelayLinksListIter.Next();
                }
            }
        }
    }

    return(ptrAllUserLinksList);
}

void
UserVehicle::ReportScheduleBurdenNumbers(ofstream &outfile,
        const int &necessaryOverhead,
        const int &excessOverhead)
{
    double efficiency = 0.0;
    double burden = 0.0;

    if ((necessaryOverhead > 0) || (excessOverhead > 0)) {
        efficiency = ((double)necessaryOverhead /
                      (double)(necessaryOverhead + excessOverhead)) * 100.0;
    }

    if (necessaryOverhead > 0) {
        burden = (double)excessOverhead / (double)necessaryOverhead;
    }

    outfile << setw(7) << necessaryOverhead;
    outfile << " " << setw(12) << excessOverhead;
    outfile << " " << setw(13) << setprecision(1) << efficiency;
    outfile << " " << setw(12) << setprecision(2) << burden << NEW_LINE;
    return;
}

double
UserVehicle::GetMinimumCrossLinkDataRate() const
{
    double             minimumLinkDataRate = DBL_MAX;
    ListIterator<Link> linksListIter(GetTransmitLinksList());
    Link              *ptrLink = linksListIter.First();

    while (ptrLink != NULL) {
        if (PrecludeReceiver(ptrLink->GetReceiveDesignator()) == false) {
            minimumLinkDataRate = std::min(minimumLinkDataRate,
                                           ptrLink->GetReceiveDataRate());
            minimumLinkDataRate = std::min(minimumLinkDataRate,
                                           ptrLink->GetTransmitDataRate());
        }

        ptrLink = linksListIter.Next();
    }

    return(minimumLinkDataRate);
}

//RAGUSA 11/21/2017 - Added method to include direct downlinks as possible minimum link rate
//
double
UserVehicle::GetMinimumDirectDownLinkDataRate() const
{
    double             minimumLinkDataRate = DBL_MAX;
    {
        ListIterator<Link> directDownlinksListIter(GetDirectDownlinksList());
        Link              *ptrdirectLink = directDownlinksListIter.First();

        while (ptrdirectLink != NULL) {
            if (PrecludeReceiver(ptrdirectLink->GetReceiveDesignator()) == false) {
                minimumLinkDataRate = std::min(minimumLinkDataRate,
                                               ptrdirectLink->GetReceiveDataRate());
                minimumLinkDataRate = std::min(minimumLinkDataRate,
                                               ptrdirectLink->GetTransmitDataRate());
            }

            ptrdirectLink = directDownlinksListIter.Next();
        }
    }
    return(minimumLinkDataRate);
}
//*********************************************************************************************

bool
UserVehicle::ContainsRequirementDeck(const string &deckName) const
{
    bool                 containsRequirementDeck = false;
    ListIterator<Sensor> sensorListIter(ptrSensorList_a);
    Sensor              *ptrSensor = sensorListIter.First();

    if (ptrTimelineCriteria_a->UseRequirementDeck() == true) {
        while ((ptrSensor != NULL) && (containsRequirementDeck == false)) {
            if (*(ptrSensor->GetRequirementDeckFileName()) == deckName) {
                containsRequirementDeck = true;
            } else {
                ptrSensor = sensorListIter.Next();
            }
        }
    }

    return(containsRequirementDeck);
}

bool
UserVehicle::UseDutyCycle() const
{
    bool    useDutyCycle = false;
    Sensor *ptrSensor = NULL;

    if (dailyVehicleDutyCycle_a > 0) {
        useDutyCycle = true;
    } else {
        ListIterator<Sensor> sensorListIter(ptrSensorList_a);
        ptrSensor = sensorListIter.First();

        while ((ptrSensor != NULL) && (useDutyCycle == false)) {
            useDutyCycle = ptrSensor->UseDutyCycle();

            if (useDutyCycle == false) {
                ptrSensor = sensorListIter.Next();
            }
        }
    }

    return(useDutyCycle);
}

bool
UserVehicle::UseRollingRevs() const
{
    bool    useRollingRevs = false;
    Sensor *ptrSensor = NULL;

    if (ptrRollingRevDutyCycle_a != NULL) {
        useRollingRevs = true;
    } else {
        ListIterator<Sensor> sensorListIter(ptrSensorList_a);
        ptrSensor = sensorListIter.First();

        while ((ptrSensor != NULL) && (useRollingRevs == false)) {
            useRollingRevs = ptrSensor->UseRollingRevs();
            ptrSensor = sensorListIter.Next();;
        }
    }

    return(useRollingRevs);
}

bool
UserVehicle::ConvertDesignatorToIndex(const string &userDesignator,
                                      int &index)
{
    index = -1;

    if (ptrIdentificationArray_s != NULL) {
        index = ptrIdentificationArray_s->FindIndex(userDesignator);
    }

    return(index >= 0);
}

bool
UserVehicle::ConvertIndexToDesignator(const int &index,
                                      string &userDesignator)
{
    bool validDesignator = false;

    if (ptrIdentificationArray_s != NULL) {
        if ((index >= 0) && (index < ptrIdentificationArray_s->Size())) {
            userDesignator = ptrIdentificationArray_s->ElementAt(index);
            validDesignator = true;
        }
    }

    return(validDesignator);
}



bool
UserVehicle::NeedToResetTimelineRequirements(const int &timeIndex) const
{
    const int checkTimeIndex = timeIndex - TimePiece::GetReportStartIndexTime();
    const int timeToCollectReset = ptrTimelineCriteria_a->GetTimeToCollectReset();
    bool       resetRequirements = false;

    if ( (checkTimeIndex == 0) ||
            ((checkTimeIndex % timeToCollectReset) == 0) ) {
        resetRequirements = true;
    }

    return(resetRequirements);
}

double
UserVehicle::GetMinimumOuterConeElevation() const
{
    double               minimumAngle = 0.00;
    ListIterator<Sensor> sensorIter(ptrSensorList_a);
    Sensor              *ptrSensor = sensorIter.First();

    while (ptrSensor != NULL) {
        minimumAngle = std::max(minimumAngle,
                                ptrSensor->GetOuterConeElevationAngle(AccessPattern::LARGEST_PATTERN));
        ptrSensor = sensorIter.Next();
    }

    return(minimumAngle);
}

double
UserVehicle::ConvertScoreToWorth(const double &score) const
{
    double worth = score;

    if (worthNormalizationX_a > 0.0) {
        try {
            worth = worthNormalizationX_a * SimpleMath::Power(score, worthNormalizationY_a);
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(), "ConvertScoreToWorth(const double&)");
            throw;
        }
    }

    return(worth);
}

ValueTimelineElement*
UserVehicle::GetValueTimelineElement(const int &timeIndex) const
{
    ValueTimelineElement *ptrElement = NULL;

    if (ptrValueTimeline_a != NULL) {
        ptrElement = ptrValueTimeline_a->GetValueTimelineElement(timeIndex);
    }

    return(ptrElement);
}

const Antenna::FREQUENCY_BAND
UserVehicle::GetDirectDownlinkFrequencyBand() const
{
    Antenna::FREQUENCY_BAND frequencyBand = Antenna::UNKNOWN_BAND;

    if (ptrDirectDownlinkAntenna_a != NULL) {
        frequencyBand = ptrDirectDownlinkAntenna_a->GetFrequencyBand();
    }

    return(frequencyBand);
}

const Antenna::FREQUENCY_BAND
UserVehicle::GetTransmitFrequencyBand() const
{
    Antenna::FREQUENCY_BAND frequencyBand = Antenna::UNKNOWN_BAND;

    if (ptrUserTransmitAntenna_a != NULL) {
        frequencyBand = ptrUserTransmitAntenna_a->GetFrequencyBand();
    }

    return(frequencyBand);
}

void
UserVehicle::SetNumberOfScoreProfiles(const int &numberOfProfiles)
{
    numberOfScoreProfiles_a = std::max(numberOfScoreProfiles_a, numberOfProfiles);
}

void
UserVehicle::DetermineMaximumTimelineScore()
{
    ptrValueTimeline_a->DetermineMaximumTimelineScore();
}

void
UserVehicle::RemoveRegionData(const int &timeIndex) const
{
    ptrValueTimeline_a->RemoveRegionData(timeIndex);
}

int
UserVehicle::GetTimelineSensorNumber(const int &timeIndex,
                                     int resourceIndex) const
{
    int sensorNumber = 0;

    if (ptrValueTimeline_a != NULL) {
        if (resourceIndex >= 0) {
            sensorNumber = ptrValueTimeline_a->GetSensorNumber(timeIndex, resourceIndex);
        } else {
            sensorNumber = ptrValueTimeline_a->GetHighestScoreSensorNumber(timeIndex);
        }
    }

    return(sensorNumber);
}

double
UserVehicle::GetImagedRegion(const int &timeIndex,
                             int resourceIndex) const
{
    double imagedRegion = 0.0;

    if (ptrValueTimeline_a != NULL) {
        imagedRegion = ptrValueTimeline_a->GetImagedRegion(timeIndex, resourceIndex);
    }

    return(imagedRegion);
}

double
UserVehicle::GetOverflownRegion(const int &timeIndex) const
{
    double overflownRegion = 0.0;

    if (ptrValueTimeline_a != NULL) {
        overflownRegion = ptrValueTimeline_a->GetOverflownRegion(timeIndex);
    }

    return(overflownRegion);
}

int
UserVehicle::GetMissionNumber(const int &timeIndex,
                              const int &resourceIndex) const
{
    int missionNumber = 0;

    if (ptrValueTimeline_a != NULL) {
        missionNumber = ptrValueTimeline_a->GetMissionNumber(timeIndex, resourceIndex);
    }

    return(missionNumber);
}

int
UserVehicle::GetSubPriorityLevel(const int &timeIndex,
                                 const int &resourceIndex) const
{
    int subPriority = 0;

    if (ptrValueTimeline_a != NULL) {
        subPriority = ptrValueTimeline_a->GetSubPriorityLevel(timeIndex, resourceIndex);
    }

    return(subPriority);
}

int
UserVehicle::GetCrisisLevel(const int &timeIndex) const
{
    int crisisLevel = 0;

    if (ptrValueTimeline_a != NULL) {
        crisisLevel = ptrValueTimeline_a->GetCrisisLevel(timeIndex);
    }

    return(crisisLevel);
}


double
UserVehicle::GetMaxPossibleBitsCollected(const int &timeIndex,
        const int &resourceIndex) const
{
    double maxPossibleBits = 0.0;

    if (ptrValueTimeline_a != NULL) {
        maxPossibleBits = ptrValueTimeline_a->GetMaxPossibleBitsCollected(timeIndex,
                          resourceIndex);
    }

    return(maxPossibleBits);
}

double
UserVehicle::GetEstimatedBitsCollected(const int &startTime,
                                       const int &endTime,
                                       int resourceIndex) const
{
    int   timeIndex = startTime;
    double estimatedBitsCollected = 0.0;

    if (ptrValueTimeline_a != NULL) {
        while (timeIndex <= endTime) {
            if (resourceIndex < 0) {
                estimatedBitsCollected += ptrValueTimeline_a->GetMaximumMaxPossibleBitsCollected(timeIndex);
            } else {
                estimatedBitsCollected += ptrValueTimeline_a->GetMaxPossibleBitsCollected(timeIndex,
                                          resourceIndex);
            }

            ++timeIndex;
        }

        estimatedBitsCollected *= collectionEfficiency_a;
    }

    return(estimatedBitsCollected);
}

double
UserVehicle::GetScore(const int &timeIndex,
                      int resourceIndex) const
{
    double score = 0.0;

    if (ptrValueTimeline_a != NULL) {
        score = ptrValueTimeline_a->GetScore(timeIndex, resourceIndex);
    }

    return(score);
}

double
UserVehicle::GetObjectiveValueScore(const int &timeIndex,
                                    int resourceIndex) const
{
    double score = 0.0;

    if (ptrValueTimeline_a != NULL) {
        score = ptrValueTimeline_a->GetObjectiveValueScore(timeIndex,
                GetDesignator(), resourceIndex);
    }

    return(score);
}

double
UserVehicle::GetMaximumTimelineScore() const
{
    double maximumScore = 0.0;

    if (ptrValueTimeline_a != NULL) {
        maximumScore = ptrValueTimeline_a->GetMaximumTimelineScore();
    }

    return(maximumScore);
}

double
UserVehicle::GetCollectionTime(const int &timeIndex,
                               int resourceIndex) const
{
    double collectionTime = 0.0;

    if (ptrValueTimeline_a != NULL) {
        collectionTime = ptrValueTimeline_a->GetCollectionTime(timeIndex, resourceIndex);
    }

    return(collectionTime);
}

double
UserVehicle::GetResourceFactor(const int &timeIndex,
                               const int &resourceIndex) const
{
    double resourceFactor = 1.0;

    if (ptrValueTimeline_a != NULL) {
        resourceFactor = ptrValueTimeline_a->GetResourceFactor(timeIndex, resourceIndex);
    }

    return(resourceFactor);
}

RegionData*
UserVehicle::GetTimelineMostValuableRegionData(const int &timeIndex) const
{
    return(ptrValueTimeline_a->GetMostValuableRegionData(timeIndex));
}

LinkList<RegionData>*
UserVehicle::GetTimelineRegionDataList(const int &timeIndex) const
{
    return(ptrValueTimeline_a->GetRegionDataList(timeIndex));
}

/* ******************************************************************** */

shared_ptr<string>
UserVehicle::GetGroupNamePtr() const
{
    return ptrGroupName_a;
}

/* ******************************************************************** */

shared_ptr<string>
UserVehicle::GetTargetDeckFileNamePtr() const
{
    return ptrTargetDeckFileName_a;
}

/* ******************************************************************** */

shared_ptr<TimelineCriteria>
UserVehicle::GetTimelineCriteria() const
{
    return (ptrTimelineCriteria_a);
}

/* ******************************************************************** */

bool
UserVehicle::HasMultipleRequestDurations() const
{
    return multipleRequestDurations_a;
}

/* ******************************************************************** */

void
UserVehicle::SetGroupName(shared_ptr<string> ptrGroup)
{
    ptrGroupName_a = ptrGroup;
}

/* ******************************************************************** */

void
UserVehicle::SetBodyTiltsForCollection(const bool &bodyTiltsForCollection)
{
    bodyTiltsForCollection_a = bodyTiltsForCollection;
}

/* ******************************************************************** */

void
UserVehicle::SetTargetDeckFileName(shared_ptr<string> ptrTargetDeck)
{
    ptrTargetDeckFileName_a = ptrTargetDeck;
}

/* ******************************************************************** */

//RAGUSA 11/27/218 - New Attribute to User Vehicle
void
UserVehicle::SetServiceTierDefinition(int serviceDefinition)
{
	serviceTierDefinition_a = serviceDefinition;
}

/* ******************************************************************** */

void
UserVehicle::SetDailyVehicleDutyCycle(int dailyVehicleDutyCycle)
{
    dailyVehicleDutyCycle_a = dailyVehicleDutyCycle;
}

/* ******************************************************************** */

void
UserVehicle::SetRollingRevDutyCycle(Array<int> *ptrRollingRev)
{
    ptrRollingRevDutyCycle_a = ptrRollingRev;
}

/* ******************************************************************** */

void
UserVehicle::SetCollectionEfficiency(double collectionEfficiency)
{
    collectionEfficiency_a = collectionEfficiency;
}

/* ******************************************************************** */

void
UserVehicle::SetBufferMemorySize(double bufferMemorySize)
{
    bufferMemorySize_a = bufferMemorySize;
}

/* ******************************************************************** */

void
UserVehicle::SetEffectiveBufferEmptyRate(double effectiveBufferEmptyRate)
{
    effectiveBufferEmptyRate_a = effectiveBufferEmptyRate;
}

/* ******************************************************************** */

void
UserVehicle::SetWorthNormCoeffX(double worthNormX)
{
    worthNormalizationX_a = worthNormX;
}

/* ******************************************************************** */

void
UserVehicle::SetWorthNormCoeffY(double worthNormY)
{
    worthNormalizationY_a = worthNormY;
}

/* ******************************************************************** */

void
UserVehicle::SetAzimuthOffsetAngles(Array<double> *ptrAzimuthOffsetAngles)
{
    ptrAzimuthOffsetAngles_a = ptrAzimuthOffsetAngles;
}

/* ******************************************************************** */

void
UserVehicle::SetVehiclePriority(int vehiclePriority)
{
    vehiclePriority_a = vehiclePriority;
}

/* ******************************************************************** */

void
UserVehicle::SetMinimumProcessingRevisitTime(int minimumProcessingRevisitTime)
{
    minimumProcessingRevisitTime_a = minimumProcessingRevisitTime;
}

/* ******************************************************************** */

void
UserVehicle::SetSunBatheFlag(bool sunBatheFlag)
{
    sunBatheFlag_a = sunBatheFlag;
}

/* ******************************************************************** */

void
UserVehicle::SetUserTransmitAntenna(const shared_ptr<UserTransmitAntenna> &ptrUTA)
{
    ptrUserTransmitAntenna_a = ptrUTA;
}

/* ******************************************************************** */

void
UserVehicle::SetDirectDownlinkAntenna(const shared_ptr<DirectDownlinkAntenna> &ptrDDA)
{
    ptrDirectDownlinkAntenna_a = ptrDDA;
}

/* ******************************************************************** */

void
UserVehicle::SetSelfRelayAntennaList(const shared_ptr<MURALTypes::SRAVector> &ptrSelfRelayAntennaList)
{
    ptrSelfRelayAntennaList_a = ptrSelfRelayAntennaList;
}

/* ******************************************************************** */

void
UserVehicle::SetTimelineCriteria(const shared_ptr<TimelineCriteria> &ptrTimelineCriteria)
{
    ptrTimelineCriteria_a = ptrTimelineCriteria;
}

/* ******************************************************************** */

UserVehicle::~UserVehicle()
{
    if (ptrIdentificationArray_s != NULL) {
        ptrIdentificationArray_s->RemoveElement(GetDesignator());

        if (ptrIdentificationArray_s->Size() == 0) {
            delete ptrIdentificationArray_s;
            ptrIdentificationArray_s = NULL;
        }
    }

    if (ptrRollingRevDutyCycle_a != NULL) {
        delete ptrRollingRevDutyCycle_a;
        ptrRollingRevDutyCycle_a = NULL;
    }

    if (ptrLossTimeStepID_a != NULL) {
        delete ptrLossTimeStepID_a;
        ptrLossTimeStepID_a = NULL;
    }

    if (ptrAzimuthOffsetAngles_a != NULL) {
        delete ptrAzimuthOffsetAngles_a;
        ptrAzimuthOffsetAngles_a = NULL;
    }

    return;
}
