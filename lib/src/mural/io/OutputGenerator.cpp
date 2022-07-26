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
 * OutputGenerator.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include "OutputGenerator.h"
#include "FileNames.h"
#include "MURALInputProcessor.h"
#include "mural/CollectionElement.h"
#include "mural/modules/vtg/CrisisData.h"
#include "mural/Earth.h"
#include "mural/Sun.h"
#include "mural/Region.h"
#include "mural/antenna/GroundPointingAntenna.h"
#include "mural/antenna/RelayPointingAntenna.h"
#include "mural/sensor/Sensor.h"
#include "mural/element/ElementCollection.h"
#include "mural/element/GroundStation.h"
#include "mural/element/RelaySatellite.h"
#include "mural/element/UserVehicle.h"
#include "mural/element/DataProviderCollection.h"
#include "mural/io/reports/VehiclePositionReport.h"
#include "mural/io/reports/RevolutionReport.h"
#include "mural/io/reports/LinkReport.h"
#include "mural/io/reports/ConjunctionReport.h"
#include "mural/io/reports/RegionInViewReport.h"
#include "mural/io/reports/TimelineReport.h"
#include "mural/modules/pg/Orbit.h"
#include "mural/modules/ts/TargetDeck.h"
#include "mural/modules/ts/Target.h"
#include "general/array/Array2D.h"
#include "general/array/ListIterator.h"
#include "general/data/UnitData.h"
#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"
#include "general/utility/StringEditor.h"
#include "general/utility/Utility.h"
#include "general/parse/ParseUtility.h"
#include "general/exception/OutputFileException.h"
#include "general\utility\Utility.h"

//------------------------------------------------------------------------------
//
//  Method Name: OutputGenerator::OutputGenerator
//
//  Purpose:
//    Constructor for the OutputGenerator class.
//
//  Calling Method:
//     OutputGenerator();
//
//  Calling Parameters
//  Parameters      i/o  description
//  ----------      ---  -----------
//
//  Returns:
//     None
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

OutputGenerator::OutputGenerator()
    : ptrUserGroupList_a(NULL), ptrUserTypeList_a(NULL), ptrUserSystemList_a(NULL)
{
    return;
}

OutputGenerator::OutputGenerator(const shared_ptr<DataProviderCollection> &ptrDataProviderCollection)
    : ptrUserGroupList_a(NULL), ptrUserTypeList_a(NULL), ptrUserSystemList_a(NULL)
{
    SetupDataProviderLists(ptrDataProviderCollection);
    return;
}

OutputGenerator::OutputGenerator(const OutputGenerator &copyMe)
    : ptrUserGroupList_a(NULL), ptrUserTypeList_a(NULL), ptrUserSystemList_a(NULL)
{
    return;
}

OutputGenerator&
OutputGenerator::operator = (const OutputGenerator &copyMe)
{
    return(*this);
}

bool
OutputGenerator::operator == (const OutputGenerator &compareMe) const
{
    return(true);
}

bool
OutputGenerator::operator != (const OutputGenerator &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
OutputGenerator::SetupDataProviderLists(const shared_ptr<DataProviderCollection> &ptrDataProviderCollection)
{
    int                                 setIndex = 0;
    int                                 numberOfGroups = 0;
    Array<string>                      *ptrUserGroupArray = new Array<string>();
    Array<string>                      *ptrUserTypeArray = new Array<string>();
    Array<string>                      *ptrUserSystemArray = new Array<string>();
    shared_ptr<UserVehicle>             ptrUser = NULL;
    MURALTypes::UserVector::iterator    userIter;
    ptrDataProviderCollection_a = ptrDataProviderCollection;
    shared_ptr<MURALTypes::UserVector>  ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();

    for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
        ptrUser = (*userIter);

        if (ptrUser->GetGroupName() != ptrUser->GetType()) {
            ptrUserGroupArray->AddUniqueElement(ptrUser->GetGroupName());
        }

        ptrUserTypeArray->AddUniqueElement(ptrUser->GetType());
        ptrUserSystemArray->AddUniqueElement(ptrUser->GetSystemType());
    }

    numberOfGroups = ptrUserGroupArray->Size();

    if (numberOfGroups > 0) {
        ptrUserGroupList_a = shared_ptr<MURALTypes::ElementCollectionVector>(new MURALTypes::ElementCollectionVector(numberOfGroups));
    }

    ptrUserTypeList_a = shared_ptr<MURALTypes::ElementCollectionVector>(new MURALTypes::ElementCollectionVector(ptrUserTypeArray->Size()));
    ptrUserSystemList_a = shared_ptr<MURALTypes::ElementCollectionVector>(new MURALTypes::ElementCollectionVector(ptrUserSystemArray->Size()));

    for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
        ptrUser = (*userIter);

        if (ptrUserGroupList_a != NULL) {
            setIndex = ptrUserGroupArray->FindIndex(ptrUser->GetGroupName());

            if (setIndex >= 0) {
                if ((*ptrUserGroupList_a)[setIndex] == NULL) {
                    (*ptrUserGroupList_a)[setIndex] = shared_ptr<ElementCollection>(new ElementCollection(ptrUser->GetGroupName()));
                }

                ((*ptrUserGroupList_a)[setIndex])->AddElement(ptrUser);
            }
        }

        setIndex = ptrUserTypeArray->FindIndex(ptrUser->GetType());

        if (setIndex >= 0) {
            if ((*ptrUserTypeList_a)[setIndex] == NULL) {
                (*ptrUserTypeList_a)[setIndex] = shared_ptr<ElementCollection>(new ElementCollection(ptrUser->GetType()));
            }

            ((*ptrUserTypeList_a)[setIndex])->AddElement(ptrUser);
        }

        setIndex = ptrUserSystemArray->FindIndex(ptrUser->GetSystemType());

        if (setIndex >= 0) {
            if ((*ptrUserSystemList_a)[setIndex] == NULL) {
                (*ptrUserSystemList_a)[setIndex] = shared_ptr<ElementCollection>(new ElementCollection(ptrUser->GetSystemType()));
            }

            ((*ptrUserSystemList_a)[setIndex])->AddElement(ptrUser);
        }
    }

    delete ptrUserGroupArray;
    ptrUserGroupArray = NULL;
    delete ptrUserTypeArray;
    ptrUserTypeArray = NULL;
    delete ptrUserSystemArray;
    ptrUserSystemArray = NULL;
    return;
}

void
OutputGenerator::ExtractOutputParameterInfo(const shared_ptr<InputDataElement> &ptrElement)
{
    int								index = 0;
    int								numberOfValues = 0;
    string							errorString = "";
    shared_ptr<RevisitGroup>		ptrRevisitGroup = NULL;
    SATSIMData						*ptrSATSIMData = NULL;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrElement->GetInputDataList();
    InputDataElement::InputDataList::iterator	inputDataIter;
    shared_ptr<InputData>						ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Output Angle Units") == true) {
            UnitData::SetOutputAngleUnits(
                UnitData::GetAngleUnits(ptrInputData->GetStringValue())
            );
        } else if (ptrInputData->LabelContainsString("Output Distance Units") == true) {
            UnitData::SetOutputDistanceUnits(
                UnitData::GetDistanceUnits(ptrInputData->GetStringValue())
            );
        } else if (ptrInputData->LabelContainsString("Output Time Units") == true) {
            UnitData::SetOutputTimeUnits(
                UnitData::GetTimeUnits(ptrInputData->GetStringValue())
            );
        } else if (ptrInputData->LabelContainsString("Output Reference Frame") == true) {
            UnitData::SetOutputReferenceFrame(
                UnitData::GetReferenceFrame(ptrInputData->GetStringValue())
            );
        }
    }

    if (UnitData::GetOutputAngleUnits() == UNKNOWN_ANGLE_UNIT) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Output Angle Units' input in OUTPUT_PARAMETERS block";
    }

    if (UnitData::GetOutputDistanceUnits() == UNKNOWN_DISTANCE_UNIT) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Output Distance Units' input in OUTPUT_PARAMETERS block";
    }

    if (UnitData::GetOutputTimeUnits() == UNKNOWN_TIME_UNIT) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Output Time Units' input in OUTPUT_PARAMETERS block";
    }

    if (UnitData::GetOutputReferenceFrame() == UNKNOWN_FRAME) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Output Reference Frame' input in OUTPUT_PARAMETERS block";
    }

    if (errorString.size() > 0) {
        throw new InputException(
            GetClassName(),
            "ExtractOutputParameterInfo()",
            errorString
        );
    }

    return;
}

void
OutputGenerator::SetOutputFileNames(const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    string                  errorString = "";
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>   ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Main Output File") == true) {
            FileNames::SetFileName(FileNames::MAIN_MRL_OUTPUTFILE, ptrInputData->GetStringValue());
        }

        if (ptrInputData->LabelContainsString("Data Provider Position Output File") == true) {
            FileNames::SetFileName(FileNames::POSITION_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Revolution Times Output File") == true) {
            FileNames::SetFileName(FileNames::REV_TIME_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Revolution GPL Output File") == true) {
            FileNames::SetFileName(FileNames::REV_GPL_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Updated Orbit Parameters Output File") == true) {
            FileNames::SetFileName(FileNames::UPDATE_ORBIT_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Crosslink Output File") == true) {
            FileNames::SetFileName(FileNames::CROSSLINK_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Downlink Output File") == true) {
            FileNames::SetFileName(FileNames::DOWNLINK_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Downlink Direct Output File") == true) {
            FileNames::SetFileName(FileNames::DIRECTDOWNLINK_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Multihop Link Output File") == true) {
            FileNames::SetFileName(FileNames::MULTIHOP_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Self Relay Link Output File") == true) {
            FileNames::SetFileName(FileNames::SELFRELAY_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("UPA Conjunction Output File") == true) {
            FileNames::SetFileName(FileNames::UPA_CONJ_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("RF Conjunction Output File") == true) {
            FileNames::SetFileName(FileNames::RF_CONJ_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Value Timeline Output File") == true) {
            FileNames::SetFileName(FileNames::VALUE_TIMELINE_OUTPUTFILE, ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Regions In View Output File") == true) {
			FileNames::SetFileName(FileNames::RIV_OUTPUTFILE, ptrInputData->GetStringValue());
		}
    } //END: for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end();++inputDataIter)

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "SetOutputFileNames(InputDataElement*)",
                                 errorString);
    }

    return;
} //END: OutputGenerator::SetOutputFileNames(const shared_ptr<InputDataElement> &ptrInputDataElement)

void
OutputGenerator::PrintAttributes(ofstream &outfile)
{
    MURALTypes::RevisitVector::iterator revisitIter;
    MURALTypes::SatsimDataVector::iterator satsimIter;
    shared_ptr<RevisitGroup> ptrRevisitGroup = NULL;
    outfile << "<OUTPUT_PARAMETERS_START>" << NEW_LINE;
    outfile << "   Output Angle Units (DEG/RAD/GRAD)          : ";
    outfile << UnitData::GetAngleUnitString(UnitData::GetOutputAngleUnits()) << NEW_LINE;
    outfile << "   Output Distance Units (M/KM/FT/MI/NMI)     : ";
    outfile << UnitData::GetDistanceUnitString(UnitData::GetOutputDistanceUnits()) << NEW_LINE;
    outfile << "   Output Time Units (HR/MN/SEC/TS)           : ";
    outfile << UnitData::GetTimeUnitString(UnitData::GetOutputTimeUnits()) << NEW_LINE;
    outfile << "   Output Reference Frame (ECF/ECI/LLA)       : ";
    outfile << UnitData::GetReferenceFrameString(UnitData::GetOutputReferenceFrame()) << NEW_LINE;

    outfile << "<OUTPUT_PARAMETERS_END>" << NEW_LINE;
    outfile << NEW_LINE;
    return;
} //END: OutputGenerator::PrintAttributes(ofstream &outfile)

void
OutputGenerator::PrintOutputFiles(ofstream &outfile)
{
    outfile << "<OUTPUT_FILE_NAMES_START>" << NEW_LINE;

    if (FileNames::HasFileNameDefined(FileNames::MAIN_MRL_OUTPUTFILE) == true ) {
        outfile << "  Main Output File                      : " << FileNames::GetFileName(FileNames::MAIN_MRL_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::POSITION_OUTPUTFILE) == true ) {
        outfile << "  Data Provider Position Output File    : " << FileNames::GetFileName(FileNames::POSITION_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::REV_TIME_OUTPUTFILE) == true ) {
        outfile << "  Revolution Times Output File          : " << FileNames::GetFileName(FileNames::REV_TIME_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::REV_GPL_OUTPUTFILE) == true ) {
        outfile << "  Revolution GPL Output File            : " << FileNames::GetFileName(FileNames::REV_GPL_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::UPDATE_ORBIT_OUTPUTFILE) == true ) {
        outfile << "  Updated Orbit Parameters Output File  : " << FileNames::GetFileName(FileNames::UPDATE_ORBIT_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::CROSSLINK_OUTPUTFILE) == true ) {
        outfile << "  Crosslink Output File                 : " << FileNames::GetFileName(FileNames::CROSSLINK_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::DOWNLINK_OUTPUTFILE) == true ) {
        outfile << "  Downlink Output File                  : " << FileNames::GetFileName(FileNames::DOWNLINK_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::DIRECTDOWNLINK_OUTPUTFILE) == true ) {
        outfile << "  Downlink Direct Output File           : " << FileNames::GetFileName(FileNames::DIRECTDOWNLINK_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::MULTIHOP_OUTPUTFILE) == true ) {
        outfile << "  Multihop Link Output File             : " << FileNames::GetFileName(FileNames::MULTIHOP_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::SELFRELAY_OUTPUTFILE) == true ) {
        outfile << "  Self Relay Link Output File           : " << FileNames::GetFileName(FileNames::SELFRELAY_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::UPA_CONJ_OUTPUTFILE) == true ) {
        outfile << "  UPA Conjunction Output File           : " << FileNames::GetFileName(FileNames::UPA_CONJ_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::RF_CONJ_OUTPUTFILE) == true ) {
        outfile << "  RF Conjunction Output File            : " << FileNames::GetFileName(FileNames::RF_CONJ_OUTPUTFILE) << NEW_LINE;
    }

    if (FileNames::HasFileNameDefined(FileNames::VALUE_TIMELINE_OUTPUTFILE) == true ) {
        outfile << "  Value Timeline Output File            : " << FileNames::GetFileName(FileNames::VALUE_TIMELINE_OUTPUTFILE) << NEW_LINE;
    }

	if (FileNames::HasFileNameDefined(FileNames::RIV_OUTPUTFILE) == true) {
		outfile << "  Regions In View Output File           : " << FileNames::GetFileName(FileNames::RIV_OUTPUTFILE) << NEW_LINE;
	}

    outfile << "<OUTPUT_FILE_NAMES_END>" << NEW_LINE;
    return;
} //END: OutputGenerator::PrintOutputFiles(ofstream &outfile)

/* *************************************************************************************** */

void
OutputGenerator::GenerateOrbitOutput()
{
    GenerateVehiclePositionOutputFile();
    GenerateRevolutionTimeOutputFile();
    GenerateUpdatedOrbitOutputFile();

    return;
}

/* *************************************************************************************** */

void
OutputGenerator::GenerateVehiclePositionOutputFile()
{
    shared_ptr<UserVehicle>    ptrUserVehicle = NULL;
    shared_ptr<RelaySatellite> ptrRelaySatellite = NULL;
    shared_ptr<GroundStation>  ptrGroundStation = NULL;
    shared_ptr<UserVehicle>    ptrTrailVehicle = NULL;
    shared_ptr<MURALTypes::ElementVector> ptrElementList = shared_ptr<MURALTypes::ElementVector>(new MURALTypes::ElementVector());
    shared_ptr<MURALTypes::UserVector> ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();
    shared_ptr<MURALTypes::RelayVector> ptrRelayList = ptrDataProviderCollection_a->GetRelayListPtr();
    shared_ptr<MURALTypes::GroundVector> ptrGroundList = ptrDataProviderCollection_a->GetGroundListPtr();
    MURALTypes::UserVector::iterator userIter;
    MURALTypes::RelayVector::iterator relayIter;
    MURALTypes::GroundVector::iterator groundIter;

    if (FileNames::HasFileNameDefined(FileNames::POSITION_OUTPUTFILE) == true) {
        // Add all users to the element list
        for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
            ptrUserVehicle = (*userIter);
            ptrElementList->push_back(ptrUserVehicle);
            ptrTrailVehicle = ptrUserVehicle->GetTrailVehicle();

            if (ptrTrailVehicle != NULL) {
                ptrElementList->push_back(ptrTrailVehicle);
            }
        }

        // add all relays
        for( relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
            ptrRelaySatellite = (*relayIter);
            ptrElementList->push_back(ptrRelaySatellite);
        }

        // add all grounds
        for( groundIter = ptrGroundList->begin(); groundIter != ptrGroundList->end(); ++groundIter) {
            ptrGroundStation = (*groundIter);
            ptrElementList->push_back(ptrGroundStation);
        }

        // Create the report
        VehiclePositionReport vehiclePositionReport(FileNames::GetFileName(FileNames::POSITION_OUTPUTFILE), ptrElementList);
        vehiclePositionReport.CreateReport();
    }

    return;
} //END: OutputGenerator::GenerateVehiclePositionOutputFile()

/* *************************************************************************************** */

void
OutputGenerator::GenerateRevolutionTimeOutputFile()
{
    shared_ptr<MURALTypes::UserVector> ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();

    if (FileNames::HasFileNameDefined(FileNames::REV_TIME_OUTPUTFILE ) == true) {
        RevolutionReport revReport(FileNames::GetFileName(FileNames::REV_TIME_OUTPUTFILE), ptrUserList);
        revReport.CreateReport();
    }

    if (FileNames::HasFileNameDefined(FileNames::REV_GPL_OUTPUTFILE ) == true != NULL) {
        RevolutionReport revReport(FileNames::GetFileName(FileNames::REV_GPL_OUTPUTFILE), ptrUserList);
        revReport.CreateGPLReport();
    }

    return;
}

/* *************************************************************************************** */

void
OutputGenerator::GenerateUpdatedOrbitOutputFile()
{
    ofstream        outfile;
    shared_ptr<UserVehicle>    ptrUserVehicle = NULL;
    shared_ptr<RelaySatellite> ptrRelaySatellite = NULL;
    shared_ptr<MURALTypes::UserVector> ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();
    shared_ptr<MURALTypes::RelayVector> ptrRelayList = ptrDataProviderCollection_a->GetRelayListPtr();
    MURALTypes::UserVector::iterator userIter;
    MURALTypes::RelayVector::iterator relayIter;

    if (FileNames::HasFileNameDefined(FileNames::UPDATE_ORBIT_OUTPUTFILE) == true) {
        ParseUtility::OpenOutputFile(outfile, FileNames::GetFileName(FileNames::UPDATE_ORBIT_OUTPUTFILE));

        for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
            ptrUserVehicle = (*userIter);
            ptrUserVehicle->PrintUpdatedOrbitFile(outfile);
        }

        for( relayIter = ptrRelayList->begin(); relayIter != ptrRelayList->end(); ++relayIter) {
            ptrRelaySatellite = (*relayIter);
            ptrRelaySatellite->PrintUpdatedOrbitFile(outfile);
        }

        outfile.close();
    }

    return;
}

/* *************************************************************************************** */

void
OutputGenerator::GenerateLinkOutput()
{
    GenerateCrosslinkOutputFile();
    GenerateDownlinkOutputFile();
    GenerateDirectDownlinkOutputFile();
    GenerateMultihopLinkOutputFile();
    GenerateSelfRelayLinkOutputFile();
    GenerateUPAConjunctionOutputFile();
    GenerateRFConjunctionOutputFile();
    return;
}

/* *************************************************************************************** */

void
OutputGenerator::GenerateCrosslinkOutputFile()
{
    shared_ptr<MURALTypes::UserVector> ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();

    if (FileNames::HasFileNameDefined(FileNames::CROSSLINK_OUTPUTFILE) == true) {
        LinkReport linkReport(FileNames::GetFileName(FileNames::CROSSLINK_OUTPUTFILE));
        linkReport.CreateCrosslinkReport(ptrUserList);
    }

    return;
}

/* *************************************************************************************** */

void
OutputGenerator::GenerateDownlinkOutputFile()
{
    shared_ptr<MURALTypes::RelayVector> ptrRelayList = ptrDataProviderCollection_a->GetRelayListPtr();

    if (FileNames::HasFileNameDefined(FileNames::DOWNLINK_OUTPUTFILE) == true) {
        LinkReport linkReport(FileNames::GetFileName(FileNames::DOWNLINK_OUTPUTFILE));
        linkReport.CreateDownlinkReport(ptrRelayList);
    }

    return;
}

/* *************************************************************************************** */

void
OutputGenerator::GenerateDirectDownlinkOutputFile()
{
    shared_ptr<MURALTypes::UserVector> ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();

    if (FileNames::HasFileNameDefined(FileNames::DIRECTDOWNLINK_OUTPUTFILE) == true) {
        LinkReport linkReport(FileNames::GetFileName(FileNames::DIRECTDOWNLINK_OUTPUTFILE));
        linkReport.CreateDirectDownlinkReport(ptrUserList);
    }

    return;
}

/* *************************************************************************************** */

void
OutputGenerator::GenerateMultihopLinkOutputFile()
{
    shared_ptr<MURALTypes::RelayVector> ptrRelayList = ptrDataProviderCollection_a->GetRelayListPtr();

    if (FileNames::HasFileNameDefined(FileNames::MULTIHOP_OUTPUTFILE) == true) {
        LinkReport linkReport(FileNames::GetFileName(FileNames::MULTIHOP_OUTPUTFILE));
        linkReport.CreateRelayMultiHopReport(ptrRelayList);
    }

    return;
}

/* *************************************************************************************** */

void
OutputGenerator::GenerateSelfRelayLinkOutputFile()
{
    shared_ptr<MURALTypes::UserVector> ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();

    if (FileNames::HasFileNameDefined(FileNames::SELFRELAY_OUTPUTFILE) == true) {
        LinkReport linkReport(FileNames::GetFileName(FileNames::SELFRELAY_OUTPUTFILE));
        linkReport.CreateUserSelfRelayReport(ptrUserList);
    }

    return;
}

/* *************************************************************************************** */

void
OutputGenerator::GenerateUPAConjunctionOutputFile()
{
    shared_ptr<MURALTypes::RelayVector> relayList = ptrDataProviderCollection_a->GetRelayListPtr();

    if (FileNames::HasFileNameDefined(FileNames::UPA_CONJ_OUTPUTFILE) == true) {
        ConjunctionReport conjReport(FileNames::GetFileName(FileNames::UPA_CONJ_OUTPUTFILE));
        conjReport.CreateUPAConjReport(relayList);
    }

    return;
}

void
OutputGenerator::GenerateRFConjunctionOutputFile()
{
    shared_ptr<MURALTypes::GroundVector> groundList = ptrDataProviderCollection_a->GetGroundListPtr();

    if (FileNames::HasFileNameDefined(FileNames::RF_CONJ_OUTPUTFILE) == true) {
        ConjunctionReport conjReport(FileNames::GetFileName(FileNames::RF_CONJ_OUTPUTFILE));
        conjReport.CreateRFConjReport(groundList);
    }

    return;
}

void
OutputGenerator::GenerateValueTimelineOutput()
{
    GenerateValueTimelineOutputFile();
    return;
}

void
OutputGenerator::GenerateValueTimelineOutputFile()
{
    shared_ptr<MURALTypes::UserVector> ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();

    if (FileNames::HasFileNameDefined(FileNames::VALUE_TIMELINE_OUTPUTFILE) == true) {
        TimelineReport timelineReport(FileNames::GetFileName(FileNames::VALUE_TIMELINE_OUTPUTFILE), ptrUserList);
        timelineReport.CreateReport();
    }
}

void
OutputGenerator::GenerateRegionsInViewOutputFile()
{
    shared_ptr<MURALTypes::UserVector> ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();

    if (FileNames::HasFileNameDefined(FileNames::RIV_OUTPUTFILE) == true) {
        RegionInViewReport regionInViewReport(FileNames::GetFileName(FileNames::RIV_OUTPUTFILE), ptrUserList);
        regionInViewReport.CreateReport();
    }
}

void
OutputGenerator::GenerateMainOutput(const string &executableName)
{
    MRLReport mrlReport(
        FileNames::GetFileName(FileNames::MAIN_MRL_OUTPUTFILE),
        ptrDataProviderCollection_a->GetUserListPtr(),
        ptrDataProviderCollection_a->GetRelayListPtr(),
        ptrDataProviderCollection_a->GetGroundListPtr(),
        ptrUserGroupList_a,
        ptrUserTypeList_a,
        ptrUserSystemList_a
    );
    mrlReport.CreateReport(executableName);
    return;
} //END: OutputGenerator::GenerateMainOutput

void
OutputGenerator::PrintCrisisData(ofstream &outfile,MURALTypes::UserVector &printUserList,
	                             int userVehicleCount)
{
	int									inview = 0;
	int									overallInview = 0;
	string								crisisLabel = "";
	string								userDesignator = "";
	ListIterator<CrisisArea>			crisisAreaListIter(CrisisData::GetCrisisAreaList());
	CrisisArea							*ptrCrisisArea = crisisAreaListIter.First();
	shared_ptr<UserVehicle>				ptrUser = NULL;
	MURALTypes::UserVector::iterator	printUserIter;

	outfile << " *****************************************************************************";
	outfile << NEW_LINE << NEW_LINE;
	outfile << " MURALCRI" << NEW_LINE << NEW_LINE;
	outfile << "          CRISIS DATA SUMMARY" << NEW_LINE << NEW_LINE;
	outfile << NEW_LINE << " ID    LABEL          USER       INVIEW";
	outfile << NEW_LINE;
	outfile << " --    -----          ----       ------";
	outfile << NEW_LINE;

	while (ptrCrisisArea != NULL) {
		int crisisNumber = 0;
		int totalInview = 0;
		bool firstUserPrinted;
		crisisNumber = ptrCrisisArea->GetIdentificationNumber();
		outfile << setw(3) << crisisNumber << "    ";
		crisisLabel = ptrCrisisArea->GetIdentificationLabel();
		outfile.unsetf(ios::right);
		outfile.setf(ios::left);
		outfile << setw(12) << crisisLabel;
		outfile.unsetf(ios::left);
		outfile.setf(ios::right);
		outfile << " ";
		totalInview = 0;
		firstUserPrinted = false;

		for (printUserIter = printUserList.begin(); printUserIter != printUserList.end(); ++printUserIter) {
			ptrUser = (*printUserIter);
			userDesignator = ptrUser->GetDesignator();

			if (ptrCrisisArea->ContainsUserVehicle(userDesignator) == true) {
				if (firstUserPrinted == false) {
					firstUserPrinted = true;
				}
				else {
					outfile << "                    ";
				}

				outfile << "  ";
				outfile.unsetf(ios::right);
				outfile.setf(ios::left);
				outfile << setw(8) << userDesignator;
				outfile.unsetf(ios::left);
				outfile.setf(ios::right);
				outfile << " ";
				ptrUser->GetCrisisData(crisisNumber, inview);
				PrintCrisisDataNumbers(outfile, inview);

				outfile << NEW_LINE;
				totalInview += inview;
			}
		} //END: for( printUserIter = printUserList.begin(); printUserIter != printUserList.end();++printUserIter)

		outfile << "                       Total   ";
		PrintCrisisDataNumbers(outfile, totalInview);
		//=================================================================================================================
		outfile << NEW_LINE << NEW_LINE;
		overallInview += totalInview;
		ptrCrisisArea = crisisAreaListIter.Next();
	} //END: while (ptrCrisisArea != NULL)

	outfile << NEW_LINE << NEW_LINE;
	outfile << "   Total Overall Crisis Data ";
	PrintCrisisDataNumbers(outfile, overallInview);
	outfile << NEW_LINE;
	
	return;
} //END: OutputGenerator::PrintCrisisData

void
OutputGenerator::PrintCrisisDataNumbers(ofstream &outfile,int inview)
{
	outfile << " ";
	TimePiece::PrintDuration(outfile, inview);
	return;
} //END: OutputGenerator::PrintCrisisDataNumbers
/* ********************************************************************************** */

OutputGenerator::~OutputGenerator()
{
    return;
}
