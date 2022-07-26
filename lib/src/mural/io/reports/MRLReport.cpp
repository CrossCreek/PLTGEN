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
 * MRLReport.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include <iomanip>
#include <cmath>

#ifdef PC
#include <Direct.h>   // for getcwd
#include <Process.h>  // for getpid
#include <windows.h>  // needed for typedefs
#include <winbase.h>  // for GetUserName
#else
#include <unistd.h>
#endif

#include "MRLReport.h"
#include "mural/Sun.h"
#include "mural/Region.h"
#include "mural/sensor/Sensor.h"
#include "mural/access/AccessPattern.h"
#include "mural/element/UserVehicle.h"
#include "mural/element/RelaySatellite.h"
#include "mural/element/GroundStation.h"
#include "mural/element/ElementCollection.h"
#include "mural/modules/lg/LinksGenerator.h"
#include "mural/modules/vtg/ValueTimeline.h"
#include "mural/modules/vtg/CrisisData.h"

#include "mural/io/OutputGenerator.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/antenna/RelayPointingAntenna.h"
#include "mural/antenna/UserPointingAntenna.h"
#include "mural/antenna/GroundPointingAntenna.h"
#include "mural/antenna/DirectDownlinkAntenna.h"
#include "mural/antenna/ReceiveFacilityAntenna.h"
#include "mural/modules/ts/TargetDeck.h"

#include "general/array/Array.h"
#include "general/array/Array2D.h"
#include "general/array/ArrayOfLinkLists.h"
#include "general/data/UnitData.h"
#include "general/data/PositionState.h"
#include "general/math/SpaceVector.h"
#include "general/math/SimpleMath.h"
#include "general/time/TimePiece.h"
#include "general/parse/ParseUtility.h"
#include "general/utility/StringEditor.h"
#include "general\utility\Utility.h"

//#include "optimizer/Optimizer.h"

// Static constants
const int     MRLReport::MAXIMUM_PRINT_USERS_s             = 35;

MRLReport::MRLReport(const string &outFile,
                     const shared_ptr<MURALTypes::UserVector> &userList,
                     const shared_ptr<MURALTypes::RelayVector> &relayList,
                     const shared_ptr<MURALTypes::GroundVector> &groundList,
                     const shared_ptr<MURALTypes::ElementCollectionVector> &ptrUserGroupList,
                     const shared_ptr<MURALTypes::ElementCollectionVector> &ptrUserTypeList,
                     const shared_ptr<MURALTypes::ElementCollectionVector> &ptrUserSystemList) :
    outputFileName_a(outFile), ptrUserList_a(userList),
    ptrRelayList_a(relayList),  ptrGroundList_a(groundList),
    ptrUserGroupList_a(ptrUserGroupList), ptrUserTypeList_a(ptrUserTypeList), ptrUserSystemList_a(ptrUserSystemList),
    minimumPrintUserNumber_a(0), maximumPrintUserNumber_a(0)
{
    return;
}

/* ************************************************************************** */

void
MRLReport::CreateReport(const string &executableName)
{
    //RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
    int UserListSize = Utility::CastSizetToInt(ptrUserList_a->size());
    const int       numberOfIterations = ((UserListSize - 1) / MAXIMUM_PRINT_USERS_s) + 1;
    string::size_type extensionPosition = string::npos;
    string            newOutputFileName = "";
    string           *ptrOutputFileName = NULL;
    MURALTypes::UserVector       printUserList;
    shared_ptr<UserVehicle>               ptrUser = NULL;

    try {
        int userIndex = 0;
        int numberOfUsers = 0;
        int iteration = 0;
        MURALTypes::UserVector::iterator userIter;

        for( userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
            ptrUser = (*userIter);
            ++numberOfUsers;

            if (ptrUser->GetTrailVehicle() != NULL) {
                ++numberOfUsers;
            }
        }

        minimumPrintUserNumber_a = 1;
        maximumPrintUserNumber_a = std::min(numberOfUsers, MAXIMUM_PRINT_USERS_s);
        ptrUser = (*ptrUserList_a)[userIndex];

        while (iteration < numberOfIterations) {
            ++iteration;

            if (numberOfIterations == 1) {
                // keep the original file name intact
                ptrOutputFileName = new string(outputFileName_a);
            } else {
                extensionPosition = outputFileName_a.find_last_of(".");

                if (extensionPosition != string::npos) {
                    // strip off the base file name
                    newOutputFileName = outputFileName_a.substr(0, extensionPosition);
                    // add the _n (iteration number)
                    newOutputFileName += ("_" + StringEditor::ConvertToString(iteration));
                    // add the extension back on the end
                    newOutputFileName += outputFileName_a.substr(extensionPosition,
                                         (outputFileName_a.size() - extensionPosition));
                    ptrOutputFileName = new string(newOutputFileName);
                } else {
                    // simply add the _n (iteration number) to the end of the file name
                    ptrOutputFileName = new string(outputFileName_a + "_" + StringEditor::ConvertToString(iteration));
                }
            }

            userIndex = 0;

            while ((userIndex < MAXIMUM_PRINT_USERS_s) && (userIndex < (int)ptrUserList_a->size())) {
                ptrUser = (*ptrUserList_a)[userIndex];
                printUserList.push_back(ptrUser);
                ++userIndex;
            }

            PrintMainOutput(executableName, ptrOutputFileName, printUserList, iteration);
            minimumPrintUserNumber_a += MAXIMUM_PRINT_USERS_s;
            maximumPrintUserNumber_a = std::min<int>(numberOfUsers, maximumPrintUserNumber_a + MAXIMUM_PRINT_USERS_s);
            delete ptrOutputFileName;
            ptrOutputFileName = NULL;
        }
    } catch (Exception *ptrError) {
        if (ptrOutputFileName != NULL) {
            delete ptrOutputFileName;
            ptrOutputFileName = NULL;
        }

        ptrError->AddMethod(GetClassName(), "GenerateMainOutput(const string&)");
        throw;
    }

    return;
}

/* ************************************************************************** */

void
MRLReport::PrintMainOutput(const string &executableName,
                           const string *ptrOutputFileName,
                           MURALTypes::UserVector &printUserList,
                           int iteration)
{
    ofstream outfile;

    try {
        ParseUtility::OpenOutputFile(outfile, ptrOutputFileName);
        CreateRFIdentificationArray();
        PrintOutputHeader(outfile, executableName, iteration);
        PrintInputData(outfile, printUserList);

		if (CrisisData::GetNumberOfCrisisAreas() > 0) {
			//RAGUSA 5/2/2018 - Casting Size_t to int for 64-bit processing
			OutputGenerator::PrintCrisisData(outfile, printUserList, Utility::CastSizetToInt(ptrUserList_a->size()));
		}

        if (MURALInputProcessor::RunLinkPathGenerator() == true) {
            PrintLinkPaths(outfile, printUserList);
        }

        PrintOutputFooter(outfile);
        outfile.close();
    } catch (Exception *ptrError) {
        if (outfile.is_open() != 0) {
            outfile.close();
        }

        ptrError->AddMethod(GetClassName(), "PrintMainOutput(const string&, const string*)");
        throw;
    }

    return;
}

/* ************************************************************************** */

void
MRLReport::CreateRFIdentificationArray()
{
    Array<int>                 *ptrCapacityArray = new Array<int>();
    MURALTypes::GroundVector::iterator      groundIter;
    shared_ptr<GroundStation>   ptrGround = NULL;

    for(groundIter = ptrGroundList_a->begin(); groundIter != ptrGroundList_a->end(); ++groundIter) {
        ptrGround = (*groundIter);
        ptrCapacityArray->AddElement(ptrGround->GetTotalRFCapacity());
    }

    // create the character array that defines the RF identifications ('A', 'B', ...) for
    // output purposes
    ReceiveFacilityAntenna::CreateIdentificationArray(ptrCapacityArray);
    delete ptrCapacityArray;
    ptrCapacityArray = NULL;
    return;
}

/* ************************************************************************** */

void
MRLReport::PrintOutputHeader(ofstream &outfile,
                             const string &executableName,
                             int iteration)
{
    outfile << "  ******* THIS SIMULATION STARTED: ";
    outfile << TimePiece::GetSystemStartTime() << NEW_LINE;
    outfile << " MURAL VERSION :  " << MURAL_VERSION << NEW_LINE;
    outfile << " EXECUTABLE    :  " << executableName << NEW_LINE << NEW_LINE;
    outfile << " MAIN - MURAL OUTPUT FILE" << NEW_LINE << NEW_LINE;
    outfile << " ******************************************************" << NEW_LINE << NEW_LINE;
    outfile << "  THE FOLLOWING LIST OF KEYWORDS WILL HELP IN FINDING" << NEW_LINE;
    outfile << "  CERTAIN SECTIONS IN THE OUTPUT FILE:" << NEW_LINE << NEW_LINE;

    if (CrisisData::GetNumberOfCrisisAreas() > 0) {
        outfile << "   MURALCRI      - CRISIS DATA" << NEW_LINE;
    }

    if ((ptrRevisitGroupList_a != NULL) && (iteration == 1)) {
        outfile << "   MURALRVST     - REVISIT DATA" << NEW_LINE;
    }

    if (MURALInputProcessor::RunLinkPathGenerator() == true) {
        outfile << "   MURALLP       - LINK PATH DATA" << NEW_LINE;
    }

    outfile << "   WARNING       - WARN. MESSAGE" << NEW_LINE;
    outfile << "   ERROR         - ERR. MESSAGE" << NEW_LINE << NEW_LINE;
    outfile << " ******************************************************" << NEW_LINE << NEW_LINE;
    return;
}

/* ************************************************************************** */

void
MRLReport::PrintInputData(ofstream &outfile,
                          MURALTypes::UserVector &printUserList)
{
    const int							MAX_ASSETS_PER_LINE = 10;
    shared_ptr<UserVehicle>				ptrTrailVehicle = NULL;
    shared_ptr<UserVehicle>				ptrUserVehicle = NULL;
    shared_ptr<RelaySatellite>			ptrRelaySatellite = NULL;
    shared_ptr<GroundStation>			ptrGroundStation = NULL;
    MURALTypes::UserVector::iterator	printUserIter;
    MURALTypes::UserVector::iterator	userIter;
    MURALTypes::RelayVector::iterator	relayIter;
    MURALTypes::GroundVector::iterator	groundIter;

    try {
        int assetCount = 0;
        InputProcessor::PrintAttributes(outfile);
        outfile << NEW_LINE;
        outfile << "<ACTIVE_CONSTELLATION_START>" << NEW_LINE;
        assetCount = 0;

        for(userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
            ptrUserVehicle = (*userIter);

            if (assetCount == 0) {
                outfile << "   User Vehicles    : ";
            } else {
                outfile << ", ";
            }

            outfile << ptrUserVehicle->GetDesignator();
            ++assetCount;
            ptrTrailVehicle = ptrUserVehicle->GetTrailVehicle();

            if (ptrTrailVehicle != NULL) {
                outfile << ", " << ptrTrailVehicle->GetDesignator();
                ++assetCount;
            }

            if (assetCount >= MAX_ASSETS_PER_LINE) {
                outfile << NEW_LINE;
                assetCount = 0;
            }
        }

        if (assetCount > 0) {
            outfile << NEW_LINE;
        }

        assetCount = 0;

        for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
            ptrRelaySatellite = (*relayIter);

            if (assetCount == 0) {
                outfile << "   Relay Satellites : ";
            } else {
                outfile << ", ";
            }

            outfile << ptrRelaySatellite->GetDesignator();
            ++assetCount;

            if (assetCount >= MAX_ASSETS_PER_LINE) {
                outfile << NEW_LINE;
                assetCount = 0;
            }
        }

        if (assetCount > 0) {
            outfile << NEW_LINE;
        }

        assetCount = 0;

        for(groundIter = ptrGroundList_a->begin(); groundIter != ptrGroundList_a->end(); ++groundIter) {
            ptrGroundStation = (*groundIter);

            if (assetCount == 0) {
                outfile << "   Ground Stations  : ";
            } else {
                outfile << ", ";
            }

            outfile << ptrGroundStation->GetDesignator();
            ++assetCount;
        }

        if (assetCount > 0) {
            outfile << NEW_LINE;
        }

        outfile << "<ACTIVE_CONSTELLATION_END>" << NEW_LINE;
        outfile << NEW_LINE;
        MURALInputProcessor::PrintAttributes(outfile);
        outfile << NEW_LINE;
        OutputGenerator::PrintAttributes(outfile);
        TimePiece::PrintAttributes(outfile);
        outfile << NEW_LINE;

        if ( (MURALInputProcessor::RunLinkPathGenerator() == true) &&
                (LinksGenerator::CheckAdditionalSeconds() == true) ) {
            LinksGenerator::PrintAttributes(outfile);
            outfile << NEW_LINE;
        }

        if (MURALInputProcessor::RunTimelineGenerator() == true) {
            TargetDeck::PrintAttributes(outfile);
            outfile << NEW_LINE;
        }

        if (CrisisData::GetNumberOfCrisisAreas() > 0) {
            CrisisData::PrintAttributes(outfile);
            outfile << NEW_LINE;
        }

        for(printUserIter = printUserList.begin(); printUserIter != printUserList.end(); ++printUserIter) {
            ptrUserVehicle = (*printUserIter);
            ptrUserVehicle->PrintAttributes(outfile);
            outfile << NEW_LINE;
        }

        for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
            ptrRelaySatellite = (*relayIter);
            ptrRelaySatellite->PrintAttributes(outfile);
            outfile << NEW_LINE;
        }

        for(groundIter = ptrGroundList_a->begin(); groundIter != ptrGroundList_a->end(); ++groundIter) {
            ptrGroundStation = (*groundIter);
            ptrGroundStation->PrintAttributes(outfile);
            outfile << NEW_LINE;
        }

        outfile << NEW_LINE;
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "PrintInputData(ofstream&)");
        throw;
    }

    return;
}

/* ************************************************************************** */

void
MRLReport::PrintLinkPaths(ofstream &outfile,
                          MURALTypes::UserVector &printUserList)
{
    shared_ptr<UserVehicle>                  ptrTrailVehicle = NULL;
    shared_ptr<UserVehicle>                  ptrUser = NULL;
    shared_ptr<RelaySatellite>               ptrRelay = NULL;
    shared_ptr<GroundStation>                ptrGround = NULL;
    MURALTypes::UserVector::iterator         userIter;
    MURALTypes::RelayVector::iterator        relayIter;
    MURALTypes::GroundVector::iterator       groundIter;
    outfile << " *****************************************************************************" << NEW_LINE << NEW_LINE;
    outfile << "                         LINK PATHS" << NEW_LINE << NEW_LINE;
    outfile << "  THIS OUTPUT DISPLAYS ALL POSSIBLE LINK PATHS FOR EACH U/V." << NEW_LINE << NEW_LINE << NEW_LINE;
    outfile << "     TYPE OF RESOURCE    DESIGNATOR    ID CHARACTER" << NEW_LINE;
    outfile << "     ----------------    ----------    ------------" << NEW_LINE;

    try {
        for(userIter = printUserList.begin(); userIter != printUserList.end(); ++userIter) {
            ptrUser = (*userIter);
            outfile << "     User Vehicle        ";
            outfile << setw(10) << ptrUser->GetDesignator();
            outfile << "          " << OutputGenerator::GetUserCharacter(ptrUser->GetNumber()) << NEW_LINE;
            ptrTrailVehicle = ptrUser->GetTrailVehicle();

            if (ptrTrailVehicle != NULL) {
                outfile << "     User Vehicle        ";
                outfile << setw(10) << ptrTrailVehicle->GetDesignator();
                outfile << "          " << OutputGenerator::GetUserCharacter(ptrTrailVehicle->GetNumber()) << NEW_LINE;
            }
        }

        outfile << NEW_LINE;

        for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
            ptrRelay = (*relayIter);
            outfile << "     Relay Satellite     ";
            outfile << setw(10) << ptrRelay->GetDesignator();
            outfile << "          " << OutputGenerator::GetRelayCharacter(ptrRelay->GetNumber()) << NEW_LINE;
        }

        outfile << NEW_LINE;

        for(groundIter = ptrGroundList_a->begin(); groundIter != ptrGroundList_a->end(); ++groundIter) {
            ptrGround = (*groundIter);
            outfile << "     Ground Station      ";
            outfile << setw(10) << ptrGround->GetDesignator();
            outfile << setw(11) << ptrGround->GetNumber() << NEW_LINE;
        }

        outfile << NEW_LINE << NEW_LINE << NEW_LINE;

        for(userIter = printUserList.begin(); userIter != printUserList.end(); ++userIter) {
            ptrUser = (*userIter);
            outfile << " MURALLP" << NEW_LINE << NEW_LINE;
            PrintLinkPaths(outfile, (*ptrUser));
        }

        outfile << NEW_LINE;
    } catch (Exception *ptrError) {
        outfile.flush();
        ptrError->AddMethod(GetClassName(), "PrintLinkPaths(ofstream&, const LinkList<UserVehicle>*)");
        throw;
    }

    return;
}

/* ************************************************************************** */

void
MRLReport::PrintLinkPaths(ofstream &outfile,
                          const UserVehicle &user) const
{
    const int									numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    int											crisisLevel = 0;
    int											spaceIndex = 0;
    int											numberOfSpaces = 0;
    Array<int>									*ptrGroundNumberArray = new Array<int>(numberOfTimeSteps, SHORT_ZERO);
    Array<int>									*ptrRFNumberArray = new Array<int>(numberOfTimeSteps, SHORT_ZERO);
    int											timeIndex = 0;
    int											reportStartTime = TimePiece::GetReportStartIndexTime();
    int											reportEndTime = TimePiece::GetReportEndIndexTime();
    int											resourceIndex = 0;
    int											numberOfValues = 0;
    Array<double>								*ptrDataRateArray = new Array<double>(numberOfTimeSteps, DOUBLE_ZERO);
    Array2D<double>								*ptrScoreProfile = NULL;
    bool										displayDownlinks = false;
    const bool									hasCrisis = CrisisData::HasCrisisArea(user.GetDesignator());
    const bool									hasTransmitSRA = user.HasTransmitSRA();
    string										crisisString = "";
    const Antenna::FREQUENCY_BAND				frequencyBand = user.GetTransmitFrequencyBand();
    Link										*ptrLink = NULL;
    Link										*ptrDownlink = NULL;
    Sensor										*ptrCollectingSensor = NULL;
    shared_ptr<RelaySatellite>					ptrRelay = NULL;
    shared_ptr<GroundStation>					ptrGround = NULL;
    MURALTypes::RelayVector::iterator			relayIter;
    MURALTypes::GroundVector::iterator			groundIter;
    const int									numberOfScoreProfiles = user.GetNumberOfScoreProfiles();
    const shared_ptr<UserTransmitAntenna>		ptrUserTransmitAntenna = user.GetUserTransmitAntenna();
    const shared_ptr<UserVehicle>				ptrTrailVehicle = user.GetTrailVehicle();
    const shared_ptr<ValueTimeline>				ptrValueTimeline = user.GetValueTimeline();

    if (ptrValueTimeline != NULL) {
        numberOfValues = std::max(user.GetNumberOfTimelineResources(), numberOfScoreProfiles);
        ptrScoreProfile = DetermineLinkPathScoreProfile(user, numberOfValues);
    }

    if (reportStartTime == TimePiece::GetStartIndexTime()) {
        reportStartTime = -1;
    }

    if (reportEndTime == TimePiece::GetEndIndexTime()) {
        reportEndTime = -1;
    } else {
        ++reportEndTime;
    }

    numberOfSpaces = PrintLinkPathHeader(outfile, user, numberOfValues, hasCrisis);
    timeIndex = 0;

    while (timeIndex < numberOfTimeSteps) {
        int sensorNumber = 0;
        int valueIndex = 0;
        double dataRate = 0.0;
        char requestedCharacter = ' ';

        if ((timeIndex == reportStartTime) || (timeIndex == reportEndTime)) {
            spaceIndex = 0;

            while (spaceIndex < numberOfSpaces) {
                outfile << "-";
                ++spaceIndex;
            }

            outfile << NEW_LINE;
        }

        TimePiece::PrintIndex(outfile, timeIndex);
        valueIndex = 0;

        while (valueIndex < numberOfValues) {
            outfile << " " << setw(13) << setprecision(3) << ptrScoreProfile->ElementAt(timeIndex, valueIndex);
            ++valueIndex;
        }

        outfile << "             ";

        PrintLinkPathInviews(
            outfile,
            user,
            timeIndex,
            hasTransmitSRA
        );

        if (user.GetTrailVehicle() != NULL) {
            outfile << " || ";
            PrintLinkPathInviews(outfile, (*ptrTrailVehicle), timeIndex, hasTransmitSRA);
        }

        outfile << " " << setw(4) << ptrGroundNumberArray->ElementAt(timeIndex);
        outfile << " " << setw(4) << ptrRFNumberArray->ElementAt(timeIndex);
        outfile << " ";
        Region::PrintFixedWidthRegionNumber(outfile, user.GetOverflownRegion(timeIndex));
        outfile << " " << setw(3) << user.GetRevNumber(timeIndex);
        dataRate = ptrDataRateArray->ElementAt(timeIndex);

        if (dataRate > DOUBLE_ZERO) {
            outfile << setw(6) << setprecision(3) << dataRate;
        } else {
            outfile << "      ";
        }

        if (hasCrisis == true) {
            crisisLevel = user.GetCrisisLevel(timeIndex);
            crisisString = "";
            crisisString += StringEditor::ConvertToString(crisisLevel);
            outfile << setw(5) << crisisString;
        }

        TimePiece::PrintIndex(outfile, timeIndex);

        outfile << NEW_LINE;
        ++timeIndex;
    }

    outfile << NEW_LINE;
    delete ptrGroundNumberArray;
    ptrGroundNumberArray = NULL;
    delete ptrRFNumberArray;
    ptrRFNumberArray = NULL;
    delete ptrDataRateArray;
    ptrDataRateArray = NULL;

    if (ptrScoreProfile != NULL) {
        delete ptrScoreProfile;
        ptrScoreProfile = NULL;
    }

    return;
}

/* ************************************************************************************************ */

int
MRLReport::PrintLinkPathHeader(ofstream &outfile,
                               const UserVehicle &user,
                               const int &numberOfValues,
                               const bool &hasCrisis) const
{
    int										numberOfSpaces = 0;
    Array<int>								*ptrDirectDownlinkArray = NULL;
    Array<int>								*ptrCrosslinkArray = NULL;
    Array<int>								*ptrSelfRelayLinkArray = NULL;
    Array<int>								*ptrTrailDirectDownlinkArray = NULL;
    Array<int>								*ptrTrailCrosslinkArray = NULL;
    Array<int>								*ptrTrailSelfRelayLinkArray = NULL;
    const int								numberOfScoreProfiles = user.GetNumberOfScoreProfiles();
    int										valueIndex     = 0;
    shared_ptr<RelaySatellite>				ptrRelay = NULL;
    shared_ptr<GroundStation>				ptrGround = NULL;
    const shared_ptr<UserTransmitAntenna>	ptrUserTransmitAntenna = user.GetUserTransmitAntenna();
    const shared_ptr<UserVehicle>			ptrTrailVehicle = user.GetTrailVehicle();
    MURALTypes::RelayVector::iterator		relayIter;
    MURALTypes::GroundVector::iterator		groundIter;

    outfile << "USER VEHICLE " << user.GetDesignator() << "  LINK PATHS" << NEW_LINE << NEW_LINE;
    // line 1
    outfile << "       ";
    numberOfSpaces += 7;

    while (valueIndex < numberOfValues) {
        if (numberOfScoreProfiles == 0) {
            outfile << "              ";
        } else {
            outfile << "      ";
            outfile << setw(8) << user.GetResourceDesignator(valueIndex);
        }

        numberOfSpaces += 14;
        ++valueIndex;
    }

    outfile << " REQ         ";
    numberOfSpaces += 13;
    numberOfSpaces += PrintLinkPathResources(
                          outfile,
                          user,
                          ptrDirectDownlinkArray,
                          ptrCrosslinkArray,
                          ptrSelfRelayLinkArray
                      );

    if (ptrTrailVehicle != NULL) {
        outfile << " || ";
        numberOfSpaces += 4;
        numberOfSpaces += PrintLinkPathResources(outfile, (*ptrTrailVehicle), ptrTrailDirectDownlinkArray,
                          ptrTrailCrosslinkArray,
                          ptrTrailSelfRelayLinkArray);
    }

    outfile << "             REGION      DATA";
    numberOfSpaces += 29;

    if (hasCrisis == true) {
        outfile << "  CRI";
        numberOfSpaces += 5;
    }

    outfile << NEW_LINE;
    // line 2
    outfile << "   TIME";
    valueIndex = 0;

    while (valueIndex < numberOfValues) {
        outfile << "     REQ SCORE";
        ++valueIndex;
    }

    outfile << " FLG   LOSS  ";
    PrintLinkPathResourceNumbers(outfile, user, ptrDirectDownlinkArray, ptrCrosslinkArray, ptrSelfRelayLinkArray);

    if (ptrTrailVehicle != NULL) {
        outfile << " || ";
        PrintLinkPathResourceNumbers(outfile, (*ptrTrailVehicle), ptrTrailDirectDownlinkArray,
                                     ptrTrailCrosslinkArray, ptrTrailSelfRelayLinkArray);
    }

    outfile << "  G/S  R/F     OVER REV  RATE";

    if (hasCrisis == true) {
        outfile << "  LEV";
    }

    outfile << "   TIME";
    numberOfSpaces += 7;
    outfile << NEW_LINE;
    // line 3
    outfile << "       ";
    valueIndex = 0;

    while (valueIndex < numberOfValues) {
        outfile << "              ";
        ++valueIndex;
    }

    outfile << "             ";
    PrintLinkPathUnderline(outfile, ptrDirectDownlinkArray, ptrCrosslinkArray, ptrSelfRelayLinkArray);

    if (ptrTrailVehicle != NULL) {
        outfile << " || ";
        PrintLinkPathUnderline(outfile, ptrTrailDirectDownlinkArray, ptrTrailCrosslinkArray, ptrTrailSelfRelayLinkArray);
    }

    outfile << NEW_LINE;

    if (ptrDirectDownlinkArray != NULL) {
        delete ptrDirectDownlinkArray;
        ptrDirectDownlinkArray = NULL;
    }

    if (ptrCrosslinkArray != NULL) {
        delete ptrCrosslinkArray;
        ptrCrosslinkArray = NULL;
    }

    if (ptrSelfRelayLinkArray != NULL) {
        delete ptrSelfRelayLinkArray;
        ptrSelfRelayLinkArray = NULL;
    }

    if (ptrTrailDirectDownlinkArray != NULL) {
        delete ptrTrailDirectDownlinkArray;
        ptrTrailDirectDownlinkArray = NULL;
    }

    if (ptrTrailCrosslinkArray != NULL) {
        delete ptrTrailCrosslinkArray;
        ptrTrailCrosslinkArray = NULL;
    }

    if (ptrTrailSelfRelayLinkArray != NULL) {
        delete ptrTrailSelfRelayLinkArray;
        ptrTrailSelfRelayLinkArray = NULL;
    }

    return(numberOfSpaces);
}

/* *********************************************************************************** */

void
MRLReport::PrintLinkPathResourceNumbers(ofstream &outfile,
                                        const UserVehicle &user,
                                        const Array<int> *ptrDirectDownlinkSpaces,
                                        const Array<int> *ptrCrosslinkSpaces,
                                        const Array<int> *ptrSelfRelayLinkSpaces) const
{
    int									resourceSpaces = 0;
    int									resourceIndex = 0;
    const int							myUserIndex = user.GetIndex();
    shared_ptr<UserVehicle>				ptrSelfUser = NULL;
    shared_ptr<RelaySatellite>			ptrRelay = NULL;
    shared_ptr<GroundStation>			ptrGround = NULL;
    shared_ptr<SelfRelayAntenna>		ptrTransmitSRA = NULL;
    shared_ptr<MURALTypes::SRAVector>	ptrSRAList = NULL;
    MURALTypes::SRAVector::iterator		sraIter;
    outfile.unsetf(ios::right);
    outfile.setf(ios::left);

    if (ptrDirectDownlinkSpaces != NULL) {
        MURALTypes::GroundVector::iterator groundIter;

        for( groundIter = ptrGroundList_a->begin(); groundIter != ptrGroundList_a->end(); ++groundIter) {
            ptrGround = (*groundIter);

            if (user.GetDirectDownlink(ptrGround->GetDesignator()) != NULL) {
                resourceSpaces = ptrDirectDownlinkSpaces->ElementAt(resourceIndex);
                outfile << setw(resourceSpaces) << ptrGround->GetNumber();
                ++resourceIndex;
            }
        }

        if ((ptrCrosslinkSpaces != NULL) || (ptrSelfRelayLinkSpaces != NULL)) {
            outfile << " | ";
        }
    }

    if (ptrCrosslinkSpaces != NULL) {
        resourceIndex = 0;
        MURALTypes::RelayVector::iterator relayIter;

        for( relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
            ptrRelay = (*relayIter);

            if (user.GetCrosslink(ptrRelay->GetDesignator()) != NULL) {
                resourceSpaces = ptrCrosslinkSpaces->ElementAt(resourceIndex);
                outfile << setw(resourceSpaces) << ptrRelay->GetNumber();
                ++resourceIndex;
            }
        }

        if (ptrSelfRelayLinkSpaces != NULL) {
            outfile << " | ";
        }
    }

    if (ptrSelfRelayLinkSpaces != NULL) {
        bool firstTransmittingAntenna;
        int numberOfReceiveSRAIndex = 0;
        ptrSRAList = user.GetSelfRelayAntennaList();
        firstTransmittingAntenna = true;

        for(sraIter = ptrSRAList->begin(); sraIter != ptrSRAList->end(); ++sraIter) {
            ptrTransmitSRA = (*sraIter);

            if (ptrTransmitSRA->IsTransmittingAntenna() == true) {
                if (firstTransmittingAntenna == false) {
                    outfile << " | ";
                }

                firstTransmittingAntenna = false;
                resourceIndex = 0;
                MURALTypes::UserVector::iterator selfUserIter;

                for(selfUserIter = ptrUserList_a->begin(); selfUserIter != ptrUserList_a->end(); ++selfUserIter) {
                    ptrSelfUser = (*selfUserIter);

                    if ((ptrSelfUser->GetIndex() != myUserIndex) &&
                            (ptrTransmitSRA->GetLinkTo(ptrSelfUser->GetDesignator()) != NULL)) {
                        numberOfReceiveSRAIndex = ptrSelfUser->GetTotalNumberOfReceiveSelfRelayAntenna(ptrTransmitSRA->GetFrequencyBandString());

                        while (numberOfReceiveSRAIndex > 0) {
                            resourceSpaces = ptrSelfRelayLinkSpaces->ElementAt(resourceIndex);

                            if (numberOfReceiveSRAIndex == ptrSelfUser->GetTotalNumberOfReceiveSelfRelayAntenna(ptrTransmitSRA->GetFrequencyBandString()) ) {
                                outfile << setw(resourceSpaces) << ptrSelfUser->GetNumber();
                            } else {
                                outfile << setw(resourceSpaces) << " ";
                            }

                            ++resourceIndex;
                            --numberOfReceiveSRAIndex;
                        }
                    }
                }
            }
        }
    }

    outfile.unsetf(ios::left);
    outfile.setf(ios::right);
    return;
}

/* ********************************************************************************** */

int
MRLReport::PrintLinkPathResources(ofstream &outfile,
                                  const UserVehicle &user,
                                  Array<int> *&ptrDirectDownlinkSpaces,
                                  Array<int> *&ptrCrosslinkSpaces,
                                  Array<int> *&ptrSelfRelayLinkSpaces) const
{
    int									numberOfSpaces = 0;
    int									resourceSpaces = 0;
    int									resourceNumber = 0;
    int									spaceIndex = 0;
    int									totalSpaces = 0;
    const int							myUserIndex = user.GetIndex();
    const bool							hasTransmitSRA = user.HasTransmitSRA();
    shared_ptr<UserVehicle>				ptrSelfUser = NULL;
    shared_ptr<RelaySatellite>			ptrRelay = NULL;
    shared_ptr<GroundStation>			ptrGround = NULL;
    shared_ptr<SelfRelayAntenna>		ptrTransmitSRA = NULL;
    shared_ptr<DirectDownlinkAntenna>	ptrDirectDownlinkAntenna = user.GetDirectDownlinkAntenna();
    shared_ptr<UserTransmitAntenna>		ptrUserTransmitAntenna = user.GetUserTransmitAntenna();
    shared_ptr<MURALTypes::SRAVector>	ptrSRAList = NULL;
    MURALTypes::SRAVector::iterator		sraIter;
    outfile.unsetf(ios::right);
    outfile.setf(ios::left);

    if (ptrDirectDownlinkAntenna != NULL) {
        int numberOfRFs = 0;
        ptrDirectDownlinkSpaces = new Array<int>();
        MURALTypes::GroundVector::iterator groundIter;

        for(groundIter = ptrGroundList_a->begin(); groundIter != ptrGroundList_a->end(); ++groundIter) {
            ptrGround = (*groundIter);

            if (user.GetDirectDownlink(ptrGround->GetDesignator()) != NULL) {
                numberOfRFs = (int)ptrGround->GetNumberOfRFs();
                resourceSpaces = (4 * numberOfRFs) - 1;
                resourceNumber = ptrGround->GetNumber();

                if (resourceNumber >= 10) {
                    resourceSpaces += numberOfRFs;
                }

                ptrDirectDownlinkSpaces->AddElement(resourceSpaces);
                numberOfSpaces += resourceSpaces;
            }
        }

        outfile << "GS ";
        spaceIndex = 3;
        totalSpaces = ptrDirectDownlinkSpaces->GetSum();

        while (spaceIndex < totalSpaces) {
            outfile << " ";
            ++spaceIndex;
        }

        if ((ptrUserTransmitAntenna != NULL) || (hasTransmitSRA == true)) {
            numberOfSpaces += 3;
            outfile << " | ";
        }
    }

    if (ptrUserTransmitAntenna != NULL) {
        int numberOfUPAs = 0;
        ptrCrosslinkSpaces = new Array<int>();
        MURALTypes::RelayVector::iterator relayIter;

        for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
            ptrRelay = (*relayIter);

            if (user.GetCrosslink(ptrRelay->GetDesignator()) != NULL) {
                numberOfUPAs = (int)ptrRelay->GetNumberOfUPAs();
                resourceSpaces = (4 * numberOfUPAs) - 1;
                resourceNumber = ptrRelay->GetNumber();

                if (resourceNumber >= 10) {
                    resourceSpaces += numberOfUPAs;
                }

                ptrCrosslinkSpaces->AddElement(resourceSpaces);
                numberOfSpaces += resourceSpaces;
            }
        }

        outfile << "RS ";
        spaceIndex = 3;
        totalSpaces = ptrCrosslinkSpaces->GetSum();

        while (spaceIndex < totalSpaces) {
            outfile << " ";
            ++spaceIndex;
        }

        if (hasTransmitSRA == true) {
            numberOfSpaces += 3;
            outfile << " | ";
        }
    }

    if (hasTransmitSRA == true) {
        int selfRelaySpaces = 0;
        int numberOfReceiveSRAIndex = 0;
        int totalPrintedTransmitSRA = 0;
        ptrSRAList = user.GetSelfRelayAntennaList();
        ptrSelfRelayLinkSpaces = new Array<int>();

        for(sraIter = ptrSRAList->begin(); sraIter != ptrSRAList->end(); ++sraIter) {
            ptrTransmitSRA = (*sraIter);

            if (ptrTransmitSRA->IsTransmittingAntenna() == true) {
                selfRelaySpaces = 0;

                if ( totalPrintedTransmitSRA > 0 ) {
                    numberOfSpaces += 3;
                    outfile << " | ";
                }

                MURALTypes::UserVector::iterator selfUserIter;

                for(selfUserIter = ptrUserList_a->begin(); selfUserIter != ptrUserList_a->end(); ++selfUserIter) {
                    ptrSelfUser = (*selfUserIter);

                    if ((ptrSelfUser->GetIndex() != myUserIndex) &&
                            (ptrTransmitSRA->GetLinkTo(ptrSelfUser->GetDesignator()) != NULL)) {
                        numberOfReceiveSRAIndex = ptrSelfUser->GetTotalNumberOfReceiveSelfRelayAntenna(ptrTransmitSRA->GetFrequencyBandString());

                        while (numberOfReceiveSRAIndex > 0) {
                            resourceSpaces = 3;
                            resourceNumber = ptrSelfUser->GetNumber();

                            if (resourceNumber >= 10) {
                                ++resourceSpaces;
                            }

                            ptrSelfRelayLinkSpaces->AddElement(resourceSpaces);
                            numberOfSpaces += resourceSpaces;
                            selfRelaySpaces += resourceSpaces;
                            --numberOfReceiveSRAIndex;
                        }
                    }
                }

                outfile << "UV ";
                spaceIndex = 3;

                while (spaceIndex < selfRelaySpaces) {
                    outfile << " ";
                    ++spaceIndex;
                }

                totalPrintedTransmitSRA++;
            }
        }
    }

    outfile.unsetf(ios::left);
    outfile.setf(ios::right);
    return(numberOfSpaces);
}

/* ********************************************************************************** */

void
MRLReport::PrintLinkPathUnderline(ofstream &outfile,
                                  const Array<int> *ptrDirectDownlinkSpaces,
                                  const Array<int> *ptrCrosslinkSpaces,
                                  const Array<int> *ptrSelfRelayLinkSpaces) const
{
    int spaceIndex = 0;
    int totalSpaces = 0;

    if (ptrDirectDownlinkSpaces != NULL) {
        spaceIndex = 0;
        totalSpaces = ptrDirectDownlinkSpaces->GetSum();

        while (spaceIndex < totalSpaces) {
            outfile << "-";
            ++spaceIndex;
        }

        if ((ptrCrosslinkSpaces != NULL) || (ptrSelfRelayLinkSpaces != NULL)) {
            outfile << "-|-";
        }
    }

    if (ptrCrosslinkSpaces != NULL) {
        spaceIndex = 0;
        totalSpaces = ptrCrosslinkSpaces->GetSum();

        while (spaceIndex < totalSpaces) {
            outfile << "-";
            ++spaceIndex;
        }

        if (ptrSelfRelayLinkSpaces != NULL) {
            outfile << "-|-";
        }
    }

    if (ptrSelfRelayLinkSpaces != NULL) {
        spaceIndex = 0;
        totalSpaces = ptrSelfRelayLinkSpaces->GetSum();

        while (spaceIndex < totalSpaces) {
            outfile << "-";
            ++spaceIndex;
        }
    }

    return;
}

/* ******************************************************************************* */

Array2D<double>*
MRLReport::DetermineLinkPathScoreProfile(const UserVehicle &user, const int &numberOfValues) const
{
    Array2D<int>                              *ptrValueStatus = NULL;
    Array2D<double>                           *ptrScoreProfile = NULL;
    const int                                  numberOfScoreProfiles = user.GetNumberOfScoreProfiles();
    const int                                  numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const shared_ptr<ValueTimeline>            ptrValueTimeline = user.GetValueTimeline();

    try {
        int timeIndex = 0;
        int endTimeIndex = 0;
        int valueIndex = 0;
        ptrScoreProfile = new Array2D<double>(numberOfTimeSteps, numberOfValues, 0.0);
        ptrValueStatus = new Array2D<int>(numberOfTimeSteps, numberOfValues, 0);

        // now use the timeline score for all non-requested time steps
        timeIndex = 0;

        while (timeIndex < numberOfTimeSteps) {
            valueIndex = 0;

            while (valueIndex < numberOfValues) {
                if (ptrValueStatus->ElementAt(timeIndex, valueIndex) == 0) {
                    (*ptrScoreProfile)[timeIndex][valueIndex] = ptrValueTimeline->GetScore(timeIndex, valueIndex);
                }

                ++valueIndex;
            }

            ++timeIndex;
        }

        delete ptrValueStatus;
        ptrValueStatus = NULL;
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "DetermineLinkPathScoreProfile(const int&)");
        throw;
    }

    return(ptrScoreProfile);
}

/* *********************************************************************************************** */
void
MRLReport::PrintLinkPathInviews(ofstream &outfile,
                                const UserVehicle &user,
                                const int &timeIndex,
                                const bool &hasTransmitSRA) const
{
    const int							myUserIndex = user.GetIndex();
    char								allocationCharacter = BLANK_CHARACTER;
    string								resourceDesignator = "";
    string								antennaDesignator = "";
    Link								*ptrLink = NULL;
    shared_ptr<ReceiveFacilityAntenna>	ptrRF = NULL;
    shared_ptr<UserPointingAntenna>		ptrUPA = NULL;
    shared_ptr<RelaySatellite>			ptrRelay = NULL;
    shared_ptr<GroundStation>			ptrGround = NULL;
    shared_ptr<UserVehicle>				ptrSelfUser = NULL;
    shared_ptr<SelfRelayAntenna>		ptrTransmitSRA = NULL;
    shared_ptr<SelfRelayAntenna>		ptrReceiveSRA = NULL;
    shared_ptr<DirectDownlinkAntenna>	ptrDirectDownlinkAntenna = user.GetDirectDownlinkAntenna();
    shared_ptr<UserTransmitAntenna>		ptrUserTransmitAntenna = user.GetUserTransmitAntenna();
    shared_ptr<MURALTypes::RFAVector>	ptrRFList = NULL;
    shared_ptr<MURALTypes::UPAVector>	ptrUPAList = NULL;
    shared_ptr<MURALTypes::SRAVector>	ptrTransmitSRAList = NULL;
    shared_ptr<MURALTypes::SRAVector>	ptrReceiveSRAList = NULL;
    MURALTypes::RFAVector::iterator		rfaIter;
    MURALTypes::UPAVector::iterator		upaIter;
    MURALTypes::SRAVector::iterator		transmitSRAIter;
    MURALTypes::SRAVector::iterator		receiveSRAIter;

    try {
        int inviewNumber = 0;
        int resourceNumber = 0;

        if (ptrDirectDownlinkAntenna != NULL) {
            MURALTypes::GroundVector::iterator groundIter;

            for(groundIter = ptrGroundList_a->begin(); groundIter != ptrGroundList_a->end(); ++groundIter) {
                ptrGround = (*groundIter);
                resourceDesignator = ptrGround->GetDesignator();

                if (user.GetDirectDownlink(resourceDesignator) != NULL) {
                    resourceNumber = ptrGround->GetNumber();
                    ptrRFList = ptrGround->GetRFList();

                    for(rfaIter = ptrRFList->begin(); rfaIter != ptrRFList->end(); ++rfaIter) {
                        ptrRF = (*rfaIter);
                        ptrLink = user.GetDirectDownlink(resourceDesignator, ptrRF->GetAntennaDesignator());

                        if (ptrLink != NULL) {
                            inviewNumber = 0;
                            allocationCharacter = BLANK_CHARACTER;

                            if (ptrLink->IsAllocatedTransmitPrep(timeIndex) == true) {
                                allocationCharacter = USER_PREP_CHARACTER;
                            }

                            if (ptrLink->IsInview(timeIndex) == true) {
                                inviewNumber = resourceNumber;
                            }

                            outfile << inviewNumber << allocationCharacter;

                            if ((resourceNumber >= 10) && (inviewNumber == 0)) {
                                outfile << " ";
                            }
                        } else {
                            outfile << "- ";

                            if (resourceNumber >= 10) {
                                outfile << " ";
                            }
                        }

                        // if the there is another iteration
                        if ((rfaIter + 1) != ptrRFList->end()) {
                            outfile << ": ";
                        } else {
                            outfile << " ";
                        }
                    }
                }
            }

            if ((ptrUserTransmitAntenna != NULL) || (hasTransmitSRA == true)) {
                outfile << " | ";
            }
        }

        if (ptrUserTransmitAntenna != NULL) {
            MURALTypes::RelayVector::iterator relayIter;

            for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
                ptrRelay = (*relayIter);
                resourceDesignator = ptrRelay->GetDesignator();

                if (user.GetCrosslink(resourceDesignator) != NULL) {
                    resourceNumber = ptrRelay->GetNumber();
                    ptrUPAList = ptrRelay->GetUserPointingAntennaList();

                    for(upaIter = ptrUPAList->begin(); upaIter != ptrUPAList->end(); ++upaIter) {
                        ptrUPA = (*upaIter);
                        ptrLink = user.GetCrosslink(resourceDesignator, ptrUPA->GetAntennaDesignator());

                        if (ptrLink != NULL) {
                            inviewNumber = 0;
                            allocationCharacter = BLANK_CHARACTER;


                            if (ptrLink->IsInview(timeIndex) == true) {
                                inviewNumber = resourceNumber;
                            }

                            if (ptrRelay->IsRequiredDownlinkOverheadTime(timeIndex) == true) {
                                outfile << REQUIRED_DOWNLINK_OVERHEAD_CHARACTER;
                                inviewNumber = 0;
                            } else if (ptrLink->StateOfHealthLinkProblem(timeIndex) == true) {
                                outfile << STATE_OF_HEALTH_CHARACTER;
                                inviewNumber = 0;
                            } else {
                                outfile << inviewNumber;
                            }

                            outfile << allocationCharacter;

                            if ((resourceNumber >= 10) && (inviewNumber == 0)) {
                                outfile << " ";
                            }
                        } else {
                            outfile << "- ";

                            if (resourceNumber >= 10) {
                                outfile << " ";
                            }
                        }

                        // if the there is another iteration
                        if ((upaIter + 1) != ptrUPAList->end() ) {
                            outfile << ": ";
                        } else {
                            outfile << " ";
                        }
                    }
                }
            }

            if (hasTransmitSRA == true) {
                outfile << " | ";
            }
        }

        if (hasTransmitSRA == true) {
            int totalReceiveSRA = 0;
            int totalPrintedTransmitSRA = 0;
            ptrTransmitSRAList = user.GetSelfRelayAntennaList();
            // Loop on the transmit SRA;s
            totalPrintedTransmitSRA = 0;

            for(transmitSRAIter = ptrTransmitSRAList->begin(); transmitSRAIter != ptrTransmitSRAList->end(); ++transmitSRAIter) {
                ptrTransmitSRA = (*transmitSRAIter);

                if (ptrTransmitSRA->IsTransmittingAntenna() == true) {
                    if (totalPrintedTransmitSRA > 0 ) {
                        outfile << " | ";
                    }

                    MURALTypes::UserVector::iterator selfUserIter;

                    for(selfUserIter = ptrUserList_a->begin(); selfUserIter != ptrUserList_a->end(); ++selfUserIter) {
                        ptrSelfUser = (*selfUserIter);

                        if (ptrSelfUser->GetIndex() != myUserIndex) {
                            totalReceiveSRA    = ptrSelfUser->GetTotalNumberOfReceiveSelfRelayAntenna(ptrTransmitSRA->GetFrequencyBandString());
                            resourceDesignator = ptrSelfUser->GetDesignator();
                            // Loop on the receive SRA's
                            ptrReceiveSRAList = ptrSelfUser->GetSelfRelayAntennaList();

                            for(receiveSRAIter = ptrReceiveSRAList->begin(); receiveSRAIter != ptrReceiveSRAList->end(); ++receiveSRAIter) {
                                ptrReceiveSRA = (*receiveSRAIter);

                                if (ptrReceiveSRA->IsReceiveAntenna() == true) {
                                    antennaDesignator = ptrReceiveSRA->GetAntennaDesignator();
                                    ptrLink = ptrTransmitSRA->GetLinkTo(resourceDesignator, antennaDesignator);

                                    if (ptrLink != NULL) {
                                        resourceNumber = ptrSelfUser->GetNumber();
                                        inviewNumber = 0;
                                        allocationCharacter = BLANK_CHARACTER;

                                        if (ptrLink->IsAllocatedTransmitPrep(timeIndex) == true) {
                                            allocationCharacter = USER_PREP_CHARACTER;
                                        }

                                        if (ptrLink->IsInview(timeIndex) == true) {
                                            inviewNumber = resourceNumber;
                                        }

                                        outfile << inviewNumber;
                                        outfile << allocationCharacter;

                                        if ((resourceNumber >= 10) && (inviewNumber == 0)) {
                                            outfile << " ";
                                        }

                                        if (totalReceiveSRA > 1) {
                                            totalReceiveSRA--;
                                            outfile << ": ";
                                        } else {
                                            outfile << " ";
                                        }
                                    } //END: if ((ptrLink = ptrTransmitSRA->GetLinkTo(resourceDesignator, antennaDesignator)) != NULL)
                                } //END: if (ptrReceiveSRA->IsReceiveAntenna() == true)
                            } //END: for(receiveSRAIter = ptrReceiveSRAList->begin(); receiveSRAIter != ptrReceiveSRAList->end(); ++receiveSRAIter)
                        } //END: if (ptrSelfUser->GetIndex() != myUserIndex)
                    } //END: for(selfUserIter = ptrUserList_a->begin(); selfUserIter != ptrUserList_a->end(); ++selfUserIter)

                    totalPrintedTransmitSRA++;
                } //END: if (ptrTransmitSRA->IsTransmittingAntenna() == true)
            } //END: for(transmitSRAIter = ptrTransmitSRAList->begin(); transmitSRAIter != ptrTransmitSRAList->end(); ++transmitSRAIter)
        } //END: if (hasTransmitSRA == true)
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "PrintLinkPathInviews(ofstream&, const int&, ...)");
        throw;
    }

    return;
}

/* ************************************************************************** */

void
MRLReport::PrintForwardResourceActivityData(ofstream &outfile, MURALTypes::UserVector &printUserList)
{
    int									reportStartTime = TimePiece::GetReportStartIndexTime();
    int									reportEndTime = TimePiece::GetReportEndIndexTime();
    const int							numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    string								separator = "-------------";
    Link								*ptrDownlink = NULL;
    shared_ptr<UserVehicle>				ptrUser = NULL;
    shared_ptr<UserVehicle>				ptrTrailVehicle = NULL;
    shared_ptr<RelaySatellite>			ptrRelay = NULL;
    shared_ptr<GroundStation>			ptrGround = NULL;
    MURALTypes::UserVector::iterator	userIter;
    MURALTypes::RelayVector::iterator	relayIter;
    MURALTypes::GroundVector::iterator	groundIter;

    try {
        int capacityIndex = 0;
        int totalCapacity = 0;
        int timeIndex = 0;

        if (reportStartTime == TimePiece::GetStartIndexTime()) {
            reportStartTime = -1;
        }

        if (reportEndTime == TimePiece::GetEndIndexTime()) {
            reportEndTime = -1;
        } else {
            ++reportEndTime;
        }

        outfile << " *****************************************************************************";
        outfile << NEW_LINE << NEW_LINE;
        outfile << " MURALFORWARD" << NEW_LINE << NEW_LINE;
        outfile << "              FORWARD PATH OF RESOURCE ACTIVITY DATA" << NEW_LINE << NEW_LINE << NEW_LINE << NEW_LINE;
        outfile << "  The output displays the forward resource activity time for each" << NEW_LINE;
        outfile << "user to relay (or ground station) and for each relay to ground" << NEW_LINE;
        outfile << "station/RF.  The relay (or ground station) is displayed under" << NEW_LINE;
        outfile << "the user and the ground station/RF is displayed under the relay" << NEW_LINE;
        outfile << "to which it is linked.  The total time each user is being used" << NEW_LINE;
        outfile << "includes overhead (acquisition, start-up, droplink) and allocation" << NEW_LINE;
        outfile << "time.  Downlink overhead is displayed by a '#'." << NEW_LINE << NEW_LINE << NEW_LINE;
        outfile << "     TYPE OF RESOURCE    DESIGNATOR    ID CHARACTER" << NEW_LINE;
        outfile << "     ----------------    ----------    ------------" << NEW_LINE;

        for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
            ptrRelay = (*relayIter);
            outfile << "     Relay Satellite     ";
            outfile << setw(10) << ptrRelay->GetDesignator();
            outfile << "          " << OutputGenerator::GetRelayCharacter(ptrRelay->GetNumber()) << NEW_LINE;
        }

        for(groundIter = ptrGroundList_a->begin(); groundIter != ptrGroundList_a->end(); ++groundIter) {
            int groundIndex = 0;
            int rfIndex = 0;
            int numberOfRFs = 0;
            ptrGround = (*groundIter);
            outfile << "     Ground Station      ";
            outfile << setw(10) << ptrGround->GetDesignator() << "       ";
            groundIndex = ptrGround->GetIndex();
            rfIndex = 0;
            numberOfRFs = ptrGround->GetTotalRFCapacity();
            outfile << ReceiveFacilityAntenna::GetIdentification(groundIndex, rfIndex);
            ++rfIndex;

            while (rfIndex < numberOfRFs) {
                outfile << ", " << ReceiveFacilityAntenna::GetIdentification(groundIndex, rfIndex);
                ++rfIndex;
            }

            outfile << NEW_LINE;
        }

        outfile << NEW_LINE << NEW_LINE << NEW_LINE;
        // print table header
        outfile << "         ";

        for(userIter = printUserList.begin(); userIter != printUserList.end(); ++userIter) {
            ptrUser = (*userIter);
            outfile << " " << setw(8) << ptrUser->GetDesignator();
            separator += "---------";
        }

        outfile << "  | ";

        for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
            ptrRelay = (*relayIter);

            if (ptrRelay->IsADownlinkRelay() == true) {
                outfile << " " << setw(8) << ptrRelay->GetDesignator();
                separator += "---------";
            }
        }

        outfile << NEW_LINE;
        outfile << "   TIME  ";

        for(userIter = printUserList.begin(); userIter != printUserList.end(); ++userIter) {
            ptrUser = (*userIter);
            capacityIndex = 9;
            totalCapacity = ptrUser->GetTotalCapacity();
            ptrTrailVehicle = ptrUser->GetTrailVehicle();

            if (ptrTrailVehicle != NULL) {
                totalCapacity += ptrTrailVehicle->GetTotalCapacity();
            }

            while (capacityIndex > totalCapacity) {
                outfile << " ";
                --capacityIndex;
            }

            capacityIndex = 0;

            while (capacityIndex < totalCapacity) {
                ++capacityIndex;
                outfile << capacityIndex;
            }
        }

        outfile << "  | ";

        for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
            ptrRelay = (*relayIter);

            if (ptrRelay->IsADownlinkRelay() == true) {
                capacityIndex = 9;
                totalCapacity = ptrRelay->GetGroundPointingAntennaCapacity();

                while (capacityIndex > totalCapacity) {
                    outfile << " ";
                    --capacityIndex;
                }

                capacityIndex = 0;

                while (capacityIndex < totalCapacity) {
                    ++capacityIndex;
                    outfile << capacityIndex;
                }
            }
        }

        outfile << NEW_LINE;

        while (timeIndex < numberOfTimeSteps) {
            if ((timeIndex == reportStartTime) || (timeIndex == reportEndTime)) {
                outfile << separator << NEW_LINE;
            }

            TimePiece::PrintIndex(outfile, timeIndex);
            outfile << "  ";

            for(userIter = printUserList.begin(); userIter != printUserList.end(); ++userIter) {
                ptrUser = (*userIter);
                PrintUserTransmitAntennaUsage(outfile, (*ptrUser), timeIndex);
            }

            outfile << "  | ";

            for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
                ptrRelay = (*relayIter);

                if (ptrRelay->IsADownlinkRelay() == true) {
                    ptrRelay->PrintGroundPointingAntennaUsage(outfile, timeIndex);
                }
            }

            outfile << NEW_LINE;
            ++timeIndex;
        }

        outfile << NEW_LINE << NEW_LINE;
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "PrintForwardResourceActivityData(ofstream&, const LinkList<UserVehicle>*)");
        throw;
    }

    return;
}

/* ************************************************************************** */

void
MRLReport::PrintUserTransmitAntennaUsage(ofstream &outfile,
        const UserVehicle &user,
        const int &timeIndex) const
{
    int						capacityIndex = 9;
    int						totalCapacity = 0;
    const int				userCapacity = user.GetTotalCapacity();
    shared_ptr<UserVehicle>	ptrTrailVehicle = user.GetTrailVehicle();
    totalCapacity = userCapacity;

    if (ptrTrailVehicle != NULL) {
        totalCapacity += ptrTrailVehicle->GetTotalCapacity();
    }

    while (capacityIndex > totalCapacity) {
        outfile << " ";
        --capacityIndex;
    }

    capacityIndex = 0;

    while (capacityIndex < userCapacity) {
        outfile << user.GetResourceCharacter(timeIndex, capacityIndex);
        ++capacityIndex;
    }

    if (ptrTrailVehicle != NULL) {
        while (capacityIndex < totalCapacity) {
            outfile << ptrTrailVehicle->GetResourceCharacter(timeIndex, (capacityIndex - userCapacity));
            ++capacityIndex;
        }
    }

    return;
}

/* ************************************************************************** */

void
MRLReport::PrintReturnResourceActivityData(ofstream &outfile, MURALTypes::UserVector &printUserList)
{
    int									reportStartTime = TimePiece::GetReportStartIndexTime();
    int									reportEndTime = TimePiece::GetReportEndIndexTime();
    const int							numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    string								separator = "---------";
    shared_ptr<UserVehicle>				ptrUser = NULL;
    shared_ptr<UserVehicle>				ptrTrailVehicle = NULL;
    shared_ptr<RelaySatellite>			ptrRelay = NULL;
    shared_ptr<GroundStation>			ptrGround = NULL;
    MURALTypes::UserVector::iterator	userIter;
    MURALTypes::RelayVector::iterator	relayIter;
    MURALTypes::GroundVector::iterator	groundIter;

    try {
        int capacityIndex = 0;
        int totalCapacity = 0;
        int timeIndex = 0;

        if (reportStartTime == TimePiece::GetStartIndexTime()) {
            reportStartTime = -1;
        }

        if (reportEndTime == TimePiece::GetEndIndexTime()) {
            reportEndTime = -1;
        } else {
            ++reportEndTime;
        }

        outfile << " *****************************************************************************";
        outfile << NEW_LINE << NEW_LINE;
        outfile << " MURALRETURN" << NEW_LINE << NEW_LINE;
        outfile << "              RETURN PATH OF RESOURCE ACTIVITY DATA" << NEW_LINE << NEW_LINE << NEW_LINE << NEW_LINE;
        outfile << "  The output displays the return resource activity time for each" << NEW_LINE;
        outfile << "relay to user and for each ground station/RF to relay (or user)." << NEW_LINE;
        outfile << "The user is displayed under the relay and the relay (or user) is" << NEW_LINE;
        outfile << "displayed under the ground station/RF to which it is linked.  The" << NEW_LINE;
        outfile << "total time each user is being used includes overhead (acquisition," << NEW_LINE;
        outfile << "start-up, droplink) and allocation time.  Downlink overhead is" << NEW_LINE;
        outfile << "displayed by a '#'." << NEW_LINE << NEW_LINE << NEW_LINE;
        outfile << "     TYPE OF RESOURCE    DESIGNATOR    ID CHARACTER" << NEW_LINE;
        outfile << "     ----------------    ----------    ------------" << NEW_LINE;

        for(userIter = printUserList.begin(); userIter != printUserList.end(); ++userIter) {
            ptrUser = (*userIter);
            outfile << "     User Vehicle        ";
            outfile << setw(10) << ptrUser->GetDesignator();
            outfile << "          " << OutputGenerator::GetUserCharacter(ptrUser->GetNumber()) << NEW_LINE;
            ptrTrailVehicle = ptrUser->GetTrailVehicle();

            if (ptrTrailVehicle != NULL) {
                outfile << "     User Vehicle        ";
                outfile << setw(10) << ptrTrailVehicle->GetDesignator();
                outfile << "          " << OutputGenerator::GetUserCharacter(ptrTrailVehicle->GetNumber()) << NEW_LINE;
            }
        }

        outfile << NEW_LINE;

        for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
            ptrRelay = (*relayIter);
            outfile << "     Relay Satellite     ";
            outfile << setw(10) << ptrRelay->GetDesignator();
            outfile << "          " << OutputGenerator::GetRelayCharacter(ptrRelay->GetNumber()) << NEW_LINE;
        }

        outfile << NEW_LINE << NEW_LINE << NEW_LINE;
        // print table header
        outfile << "         ";

        for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
            ptrRelay = (*relayIter);
            outfile << " " << setw(8) << ptrRelay->GetDesignator();
            separator += "---------";
        }

        for(groundIter = ptrGroundList_a->begin(); groundIter != ptrGroundList_a->end(); ++groundIter) {
            ptrGround = (*groundIter);
            outfile << "  | ";
            outfile << " " << setw(8) << ptrGround->GetDesignator();
            separator += "-------------";
        }

        outfile << NEW_LINE;
        outfile << "   TIME  ";

        for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
            ptrRelay = (*relayIter);
            capacityIndex = 9;
            totalCapacity = ptrRelay->GetUserPointingAntennaCapacity();

            while (capacityIndex > totalCapacity) {
                outfile << " ";
                --capacityIndex;
            }

            capacityIndex = 0;

            while (capacityIndex < totalCapacity) {
                ++capacityIndex;
                outfile << capacityIndex;
            }
        }

        for(groundIter = ptrGroundList_a->begin(); groundIter != ptrGroundList_a->end(); ++groundIter) {
            int groundIndex = 0;
            ptrGround = (*groundIter);
            outfile << "  | ";
            capacityIndex = 9;
            totalCapacity = ptrGround->GetTotalRFCapacity();

            while (capacityIndex > totalCapacity) {
                outfile << " ";
                --capacityIndex;
            }

            groundIndex = ptrGround->GetIndex();
            capacityIndex = 0;

            while (capacityIndex < totalCapacity) {
                outfile << ReceiveFacilityAntenna::GetIdentification(groundIndex, capacityIndex);
                ++capacityIndex;
            }
        }

        outfile << NEW_LINE;

        while (timeIndex < numberOfTimeSteps) {
            if ((timeIndex == reportStartTime) || (timeIndex == reportEndTime)) {
                outfile << separator << NEW_LINE;
            }

            TimePiece::PrintIndex(outfile, timeIndex);
            outfile << "  ";

            for(relayIter = ptrRelayList_a->begin(); relayIter != ptrRelayList_a->end(); ++relayIter) {
                ptrRelay = (*relayIter);
                capacityIndex = 9;
                totalCapacity = ptrRelay->GetUserPointingAntennaCapacity();

                while (capacityIndex > totalCapacity) {
                    outfile << " ";
                    --capacityIndex;
                }

                ptrRelay->PrintUserPointingAntennaUsage(outfile, timeIndex);
            }

            for(groundIter = ptrGroundList_a->begin(); groundIter != ptrGroundList_a->end(); ++groundIter) {
                ptrGround = (*groundIter);
                outfile << "  | ";
                capacityIndex = 9;
                totalCapacity = ptrGround->GetTotalRFCapacity();

                while (capacityIndex > totalCapacity) {
                    outfile << " ";
                    --capacityIndex;
                }

            }

            outfile << NEW_LINE;
            ++timeIndex;
        }

        outfile << NEW_LINE << NEW_LINE;
    } catch (Exception *ptrError) {
        outfile.flush();
        ptrError->AddMethod(GetClassName(),
                            "PrintForwardResourceActivityData(ofstream&, const LinkList<UserVehicle>*)");
        throw;
    }

    return;
}

/* ************************************************************************** */

void
MRLReport::PrintMakeBeforeBreakData(ofstream &outfile, MURALTypes::UserVector &printUserList)
{
    int									numberOfMBBUsers = 0;
    int									totalTimeMBB = 0;
    int									timeIndex = TimePiece::GetReportStartIndexTime();
    const int							reportEndTimeIndex = TimePiece::GetReportEndIndexTime();
    bool								printedHeader = false;
    MURALTypes::UserVector::iterator	userIter;
    shared_ptr<UserVehicle>				ptrUser = NULL;

    for(userIter = ptrUserList_a->begin(); userIter != ptrUserList_a->end(); ++userIter) {
        ptrUser = (*userIter);

        if (ptrUser->IsAMakeBeforeBreakVehicle() == true) {
            if (printedHeader == false) {
                outfile << " *****************************************************************************";
                outfile << NEW_LINE << NEW_LINE;
                outfile << " MURALMBB" << NEW_LINE << NEW_LINE;
                outfile << "             MAKE-BEFORE-BREAK STATISTICS" << NEW_LINE << NEW_LINE;
                printedHeader = true;
            }

            totalTimeMBB += ptrUser->PrintMakeBeforeBreaks(outfile);
            ++numberOfMBBUsers;
        }
    }

    if (ptrUserList_a->size() <= MAXIMUM_PRINT_USERS_s) {
        outfile << NEW_LINE << " Total Make-Before-Break Time        : ";
        TimePiece::PrintDuration(outfile, totalTimeMBB);
        outfile << NEW_LINE << NEW_LINE;

        if (numberOfMBBUsers > 1) {
            int totalSimultaneousMBBTime = 0;

            while (timeIndex <= reportEndTimeIndex) {
                int simultaneousMBBs = 0;
                simultaneousMBBs = 0;
                userIter = ptrUserList_a->begin();

                while ((userIter != ptrUserList_a->end()) && (simultaneousMBBs < 2)) {
                    ptrUser = (*userIter);

                    if (ptrUser->IsConductingMakeBeforeBreak(timeIndex) == true) {
                        ++simultaneousMBBs;
                    }

                    ++userIter;
                }

                if (simultaneousMBBs > 1) {
                    ++totalSimultaneousMBBTime;
                }

                ++timeIndex;
            }

            outfile << " Simultaneous Make-Before-Break Time : ";
            TimePiece::PrintDuration(outfile, totalSimultaneousMBBTime);
            outfile << NEW_LINE << NEW_LINE;
        }
    }

    return;
}

/* ************************************************************************************* */
void
MRLReport::PrintOutputFooter(ofstream &outfile)
{
    bool	loginStatus = false;
    char	currentDirectory[STRING_SIZE] = "";
    string	hostName;
    outfile << " *****************************************************************************" << NEW_LINE << NEW_LINE;
    outfile << "             HISTORY OF THE RUN" << NEW_LINE << NEW_LINE;
#ifdef PC
    TCHAR login[STRING_SIZE];
    DWORD loginNameSize = sizeof(login);
    TCHAR host[MAX_COMPUTERNAME_LENGTH];
    DWORD hostNameSize = sizeof(host);

    if (GetUserName(login, &loginNameSize)) {
        loginStatus = true;
    }

    if (GetComputerName(host, &hostNameSize)) {
        hostName = host;
    } else {
        hostName = "UNKNOWN PC";
    }

#else
    char *login = NULL;
    char  name[STRING_SIZE];
    login = getlogin();

    if (login != NULL) {
        loginStatus = true;
    }

    if (gethostname(name, STRING_SIZE) == 0) {
        hostName = string(name);
    } else {
        hostName = "UNKNOWN SUN";
    }

#endif

    if (loginStatus == true) {
        outfile << " ANALYST             :   " << login << NEW_LINE;
    }

    outfile << " WORKSTATION         :   " << hostName << NEW_LINE;
    outfile << " PROCESS ID          :   " << getpid() << NEW_LINE;

    if (getcwd(currentDirectory, STRING_SIZE) != NULL) {
        outfile << " DIRECTORY           :   " << currentDirectory << NEW_LINE << NEW_LINE;
    }

    outfile << " SIMULATION STARTED  :   " << TimePiece::GetSystemStartTime();
    outfile << " SIMULATION ENDED    :   " << TimePiece::GetSystemCurrentTime();
    outfile << " SIMULATION DURATION : ~ " << TimePiece::GetSystemDuration() << " MINUTE(S)" << NEW_LINE;
    return;
}

MRLReport::~MRLReport()
{
    // Do nothing
}
