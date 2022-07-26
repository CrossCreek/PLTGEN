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
 * TimelineGenerator.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "TimelineGenerator.h"
#include "CrisisData.h"
#include "MultipleRequirement.h"
#include "RegionData.h"
#include "Requirement.h"
#include "mural/Sun.h"
#include "TCARegionData.h"
#include "ValueTimelineElement.h"
#include "RequirementMap.h"
#include "mural/element/UserVehicle.h"
#include "mural/element/DataProviderCollection.h"
#include "mural/io/reports/FieldOfRegardRIVReport.h"
#include "mural/io/FileNames.h"
#include "mural/modules/ts/TargetDeck.h"

#include "mural/modules/pg/Revolution.h"
#include "mural/modules/ts/Target.h"
#include "mural/sensor/Sensor.h"
#include "mural/access/AccessPattern.h"
#include "mural/Region.h"
#include "mural/Earth.h"

#include "general/exception/InputException.h"
#include "general/array/ListIterator.h"
#include "general/array/Array3D.h"
#include "general/data/UnitData.h"
#include "general/data/PositionState.h"
#include "general/data/TimePeriod.h"
#include "general/math/CoordinateFrame.h"
#include "general/parse/ParseUtility.h"
#include "general/utility/StringEditor.h"

const int                  TimelineGenerator::CRISIS_MISSION_NUMBER_s           = 10;
shared_ptr<RequirementMap> TimelineGenerator::requirementMap_s                  = shared_ptr<RequirementMap>(new RequirementMap());

TimelineGenerator::TimelineGenerator()
{
    return;
}

TimelineGenerator::TimelineGenerator(const TimelineGenerator &copyMe)
{
    return;
}

TimelineGenerator&
TimelineGenerator::operator = (const TimelineGenerator &copyMe)
{
    return(*this);
}

bool
TimelineGenerator::operator == (const TimelineGenerator &compareMe) const
{
    return(false);
}

bool
TimelineGenerator::operator != (const TimelineGenerator &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
TimelineGenerator::GenerateTimelines(const shared_ptr<DataProviderCollection> &ptrDataProviderCollection)
{
    const int                    numberOfTargetDecks = TargetDeck::GetNumberOfTargetDecks();
    Array<string>                *ptrRetrieveUserArray = NULL;
    ofstream                      valtLogFile;
    shared_ptr<TimelineCriteria>  ptrTimelineCriteria = NULL;
    ptrDataProviderCollection_a = ptrDataProviderCollection;

    try {
        shared_ptr<MURALTypes::UserVector> ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();
        shared_ptr<UserVehicle>            ptrUserVehicle = NULL;
        MURALTypes::UserVector::iterator userIter;

        //CPPCHECK_FIX Performance - prefix iterator change
        for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
            ptrUserVehicle = (*userIter);
            ptrTimelineCriteria = ptrUserVehicle->GetTimelineCriteria();

            // Set up the target/requirement deck names here (moved off sensor to decouple)
            if (ptrTimelineCriteria->GetTypeOfDeck() != TimelineCriteria::NO_DECK ) {
                SetupDeckFileNames(*ptrUserVehicle);
            }
        }

        if (FileNames::GetFileList(FileNames::REQUIREMENT_DECK) != NULL) {
            RetrieveRequirementDeckData(numberOfTargetDecks);
        }

        if (numberOfTargetDecks > 0) {
            BuildRequirementsFromTargets(numberOfTargetDecks);
        }

        if (FileNames::HasFileNameDefined(FileNames::VALT_DEBUGFILE) == true) {
            ParseUtility::OpenOutputFile(valtLogFile, FileNames::GetFileName(FileNames::VALT_DEBUGFILE));
            valtLogFile << "                                         TARGET";
            valtLogFile << "             CALCD              REQD" << NEW_LINE;
            valtLogFile << "    USER   TIME  SEN   REGION  SUN ANG     ELEV";
            valtLogFile << "  RNG/SQU  QUALITY  MISSION  QUALITY    PRI   TTC        SCORE  STATUS" << NEW_LINE;
        }

        // Generate Timelines
        //CPPCHECK_FIX Performance - prefix iterator change
        for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
            ptrUserVehicle = (*userIter);
            DetermineBasicTimelineElements(ptrUserVehicle, valtLogFile);

            if (ptrUserVehicle->GetTimelineGenerationMethod()
                    == TimelineCriteria::READ_TIMELINE) {
                if (ptrRetrieveUserArray == NULL) {
                    ptrRetrieveUserArray = new Array<string>();
                }

                ptrRetrieveUserArray->AddElement(ptrUserVehicle->GetDesignator());
            } else if (ptrUserVehicle->GetTimelineGenerationMethod()
                       == TimelineCriteria::MOST_VALUABLE_REGION) {
                cout << "TimelineGenerator: Generating Most Valuable Region Timeline For "
                     << ptrUserVehicle->GetDesignator() << NEW_LINE;
                GenerateMostValuableRegionTimeline(ptrUserVehicle, valtLogFile);
            } else if (ptrUserVehicle->GetTimelineGenerationMethod()
                       == TimelineCriteria::TIME_CLOSEST_APPROACH) {
                cout << "TimelineGenerator: Generating Time of Closest Approach Timeline For "
                     << ptrUserVehicle->GetDesignator() << NEW_LINE;
                GenerateTimeOfClosestApproachTimeline(ptrUserVehicle, valtLogFile);
            } else if (ptrUserVehicle->GetTimelineGenerationMethod()
                       == TimelineCriteria::NOMINAL_REGION) {
                cout << "TimelineGenerator: Generating Nominal Region Timeline For "
                     << ptrUserVehicle->GetDesignator() << NEW_LINE;
                GenerateNominalRequestRegionTimeline(ptrUserVehicle);
            }

            ptrUserVehicle->DetermineMaximumTimelineScore();
        }

        if (ptrRetrieveUserArray != NULL) {
            cout << "TimelineGenerator: Retrieving Timeline(s) For ";

            if (ptrRetrieveUserArray->Size() == UserVehicle::GetUserVehicleCount()) {
                cout << "All User Vehicles";
            } else {
                ptrRetrieveUserArray->Print(cout);
            }

            cout << NEW_LINE;
            RetrieveValueTimelineData(ptrRetrieveUserArray);
            delete ptrRetrieveUserArray;
            ptrRetrieveUserArray = NULL;
        }

        // Create/Write out the FOR Regions Inview file (if specified).
        if (FileNames::HasFileNameDefined(FileNames::FOR_REGIONS_INVIEW_DEBUGFILE) == true) {
            FieldOfRegardRIVReport forRIVreporter(FileNames::GetFileName(FileNames::FOR_REGIONS_INVIEW_DEBUGFILE), ptrUserList);
            forRIVreporter.CreateReport();
        }

        if (FileNames::HasFileNameDefined(FileNames::VALT_DEBUGFILE) == true) {
            valtLogFile.close();
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GenerateTimeline(LinkList<DataProvider>*)");
        throw;
    }

    return;
}

void
TimelineGenerator::RetrieveRequirementDeckData(int numberOfTargetDecks) const
{
    int        region = 0;
    int        subRegion = 0;
    int        missionNumber = 0;
    int        qualityLevel = 0;
    int        priorityLevel = 0;
    int        subPriority = 0;
    int        numberOfPoints = 0;
    int         requirementDeckIndex = 0;
    int         sensorIndex = 0;
    int         missionIndex = 0;
    int         qualityIndex = 0;
    int         priorityIndex = 0;
    const int   numberOfRequirementDecks = FileNames::GetFileListSize(FileNames::REQUIREMENT_DECK);
    const int   numberOfDecks = numberOfTargetDecks + numberOfRequirementDecks;
    const int   numberOfSensors = Sensor::GetNumberOfDeckSensors();
    double        regionNumber = 0.0;
    double        score = 0.0;
    double        area = 0.0;
    Array<bool> *ptrActiveSensor = new Array<bool>(numberOfSensors, false);
    string       inputString = "";
    string       errorString = "";
    string       requirementDeckFileName = "";
    string       fullRequirementDeckFileName = "";
    Sensor      *ptrSensor = NULL;
    Region      *ptrRegion = NULL;
    shared_ptr<MURALTypes::UserVector>  ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();
    shared_ptr<UserVehicle>             ptrUser = NULL;
    shared_ptr<MURALTypes::FileList>    ptrRequirementDeckList = FileNames::GetFileList(FileNames::REQUIREMENT_DECK);
    MURALTypes::FileList::iterator reqDeckIter;

    for(reqDeckIter = ptrRequirementDeckList->begin(); reqDeckIter != ptrRequirementDeckList->end(); ++reqDeckIter) {
        int numberOfResources = 0;
        ifstream requirementDeckFile;
        requirementDeckFileName = *(*reqDeckIter);
        fullRequirementDeckFileName = *(TargetDeck::GetDeckFileBaseDirectory()) + requirementDeckFileName;
        cout << "TimelineGenerator: Retrieving Requirement Deck Data From "
             << fullRequirementDeckFileName << NEW_LINE;
        ParseUtility::OpenInputFile(requirementDeckFile, fullRequirementDeckFileName);
        numberOfResources = 0;
        MURALTypes::UserVector::iterator userIter;

        for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
            ptrUser = (*userIter);

            if (ptrUser->UseRequirementDeck() == true) {
                ListIterator<Sensor> sensorListIter(ptrUser->GetSensorList());
                ptrSensor = sensorListIter.First();

                while (ptrSensor != NULL) {
                    if (ptrSensor->UsingRequirementDeck(requirementDeckFileName) == true) {
                        (*ptrActiveSensor)[ptrSensor->GetSensorIndex()] = true;
                        numberOfResources = std::max(numberOfResources,
                                                     ptrUser->GetNumberOfTimelineResources());
                    }

                    ptrSensor = sensorListIter.Next();
                }
            }
        }

        // get the header line first
        getline(requirementDeckFile, inputString, NEW_LINE);

        while (getline(requirementDeckFile, inputString, NEW_LINE)) {
            istringstream inputStream(inputString);
            inputStream >> region >> subRegion >> missionNumber >> qualityLevel
                        >> priorityLevel >> subPriority >> score >> area >> numberOfPoints;
            regionNumber = Region::GetFullRegionNumber(region, subRegion);
            ptrRegion = Earth::GetRegion(regionNumber);

            if (ptrRegion == NULL) {
                errorString = "Could not find Region "
                              + StringEditor::ConvertToString(regionNumber);
            } else {
                missionIndex = Target::ConvertMissionToIndex(missionNumber);

                if (missionIndex < 0) {
                    errorString = "Invalid mission number: "
                                  + StringEditor::ConvertToString(missionNumber);
                } else {
                    qualityIndex = Target::ConvertQualityToIndex(qualityLevel);

                    if (qualityIndex < 0) {
                        errorString = "Invalid quality level: "
                                      + StringEditor::ConvertToString(qualityLevel);
                    } else  {
                        priorityIndex = Target::ConvertPriorityToIndex(priorityLevel);

                        if (priorityIndex < 0) {
                            errorString = "Invalid priority level: "
                                          + StringEditor::ConvertToString(priorityLevel);
                        }
                    }
                }
            }

            if (errorString.size() > 0) {
                delete ptrActiveSensor;
                throw new InputException(GetClassName(),
                                         "RetrieveRequirementDeckData(LinkList<UserVehicle>*)", errorString);
            }

            sensorIndex = 0;

            while (sensorIndex < numberOfSensors) {
                if (ptrActiveSensor->ElementAt(sensorIndex) == true) {
                    try {
                        requirementMap_s->UpdateRequirementData(ptrRegion->GetFullRegionNumber(),
                                                                requirementDeckIndex, sensorIndex,
                                                                missionIndex, qualityIndex, priorityIndex,
                                                                subPriority, score, area, numberOfPoints,
                                                                numberOfDecks, numberOfResources);
                    } catch (Exception *ptrError) {
                        delete ptrActiveSensor;
                        ptrError->AddMethod(GetClassName(),
                                            "RetrieveRequirementDeckData(LinkList<UserVehicle>*)");
                        throw;
                    }
                }

                ++sensorIndex;
            }
        }

        ptrActiveSensor->ResetValues(false);
        requirementDeckFile.close();
        ++requirementDeckIndex;
    }

    delete ptrActiveSensor;
    ptrActiveSensor = NULL;
    return;
}

void
TimelineGenerator::BuildRequirementsFromTargets(int numberOfTargetDecks)
{
    const int        numberOfRequirementDecks = FileNames::GetFileListSize(FileNames::REQUIREMENT_DECK);
    const int        numberOfDecks = numberOfTargetDecks + numberOfRequirementDecks;
    const int        numberOfSensors = Sensor::GetNumberOfTargetSensors();
    const bool        printDeckAsRequirements = TargetDeck::PrintDeckAsRequirements();
    Array2D<bool>    *ptrActiveRequirements = new Array2D<bool>(numberOfSensors, numberOfDecks, false);
    string            deckFileName = "";
    string            targetDeckFileName = "";
    string            fullTargetDeckFileName = "";
    const string              requirementExtension = "_Requirement.deck";
    const string              targetExtension = "_Target.deck";
    shared_ptr<string>  ptrRequirementDeckName = NULL;
    Sensor           *ptrSensor = NULL;
    TargetDeck       *ptrTargetDeck = NULL;
    shared_ptr<MURALTypes::UserVector> ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();
    shared_ptr<UserVehicle>            ptrUser = NULL;
    shared_ptr<TimelineCriteria>  ptrTimelineCriteria = NULL;

    try {
        int targetDeckIndex = 0;
        int deckIndex = 0;
        int sensorIndex = 0;

        while (targetDeckIndex < numberOfTargetDecks) {
            int numberOfResources = 0;
            targetDeckFileName = TargetDeck::GetTargetDeckFileName(targetDeckIndex);
            fullTargetDeckFileName = *(TargetDeck::GetDeckFileBaseDirectory()) + targetDeckFileName;
            cout << "TimelineGenerator: Building Requirements From Target Deck "
                 << fullTargetDeckFileName << NEW_LINE;
            numberOfResources = 0;
            MURALTypes::UserVector::iterator userIter;

            for( userIter = ptrUserList->begin(); userIter != ptrUserList->end(); ++userIter) {
                ptrUser = (*userIter);
                ptrTimelineCriteria = ptrUser->GetTimelineCriteria();

                if (ptrUser->UsingTargetDeck(targetDeckFileName) == true && ptrTimelineCriteria->UseTargetDeck()) {
                    ListIterator<Sensor> sensorListIter(ptrUser->GetSensorList());
                    ptrSensor = sensorListIter.First();

                    while (ptrSensor != NULL) {
                        ptrRequirementDeckName = ptrSensor->GetRequirementDeckFileName();

                        if ((ptrRequirementDeckName != NULL) && ptrTimelineCriteria->UseRequirementDeck()) {
                            deckIndex = FileNames::FindIndexInList(FileNames::REQUIREMENT_DECK, *ptrRequirementDeckName);
                        } else {
                            deckIndex = numberOfRequirementDecks + targetDeckIndex;
                        }

                        (*ptrActiveRequirements)[ptrSensor->GetSensorIndex()][deckIndex] = true;
                        ptrSensor = sensorListIter.Next();
                    }

                    numberOfResources = std::max(numberOfResources,
                                                 ptrUser->GetNumberOfTimelineResources());
                }
            }

            ptrTargetDeck = new TargetDeck(targetDeckIndex, ptrActiveRequirements,
                                           numberOfResources);

            // if targets are to later be scheduled, then add the deck to
            // the target schedule (else delete it)
            //if (TargetDeck::ScheduleTargets() == true) {
            //    TargetScheduler::AddTargetDeck(ptrTargetDeck);
            //} else {
                delete ptrTargetDeck;
            //}

            ptrTargetDeck = NULL;

            if (printDeckAsRequirements == true) {
                // this is for the BOTH option (target and requirement decks combined)
                shared_ptr<MURALTypes::FileList> ptrDeckList = FileNames::GetFileList(FileNames::REQUIREMENT_DECK);
                MURALTypes::FileList::iterator reqDeckIter;

                for(reqDeckIter = ptrDeckList->begin(); reqDeckIter != ptrDeckList->end(); ++reqDeckIter) {
                    deckFileName = GetBaseDeckFileName(*(*reqDeckIter), requirementExtension);
                    sensorIndex = 0;

                    while (sensorIndex < numberOfSensors) {
                        if (ptrActiveRequirements->ElementAt(sensorIndex, deckIndex) == true) {
                            stringstream outStream;
                            outStream << deckFileName;
                            outStream << "_" << Sensor::GetSensorTypeString(sensorIndex);
                            outStream << "_" << TimePiece::GetStartMonth();
                            outStream << "_" << TimePiece::GetStartDay();
                            outStream << requirementExtension;
                            PrintRequirementDeck(outStream.str(), deckIndex, sensorIndex);
                        }

                        ++sensorIndex;
                    }

                    ++deckIndex;
                }

                // this is for the TARGET option (using only a target deck)
                deckIndex = numberOfRequirementDecks + targetDeckIndex;
                deckFileName = GetBaseDeckFileName(targetDeckFileName, targetExtension);
                sensorIndex = 0;

                while (sensorIndex < numberOfSensors) {
                    if (ptrActiveRequirements->ElementAt(sensorIndex, deckIndex) == true) {
                        stringstream outStream;
                        outStream << deckFileName;
                        outStream << "_" << Sensor::GetSensorTypeString(sensorIndex);
                        outStream << "_" << TimePiece::GetStartMonth();
                        outStream << "_" << TimePiece::GetStartDay();
                        outStream << requirementExtension;
                        PrintRequirementDeck(outStream.str(), deckIndex, sensorIndex);
                    }

                    ++sensorIndex;
                }
            }

            ptrActiveRequirements->ResetValues(false);
            ++targetDeckIndex;
        }

        delete ptrActiveRequirements;
        ptrActiveRequirements = NULL;
    } catch (Exception *ptrError) {
        delete ptrActiveRequirements;
        ptrActiveRequirements = NULL;
        ptrError->AddMethod(GetClassName(),
                            "BuildRequirementsFromTargets(LinkList<UserVehicle>*, int)");
        throw;
    }

    return;
}

/* ****************************************************************************************** */

void TimelineGenerator::SetupDeckFileNames(const UserVehicle &ptrUser) const
{
    ListIterator<Sensor> sensorIter = ListIterator<Sensor>(ptrUser.GetSensorList());
    Sensor *ptrSensor = sensorIter.First();

    while( ptrSensor != NULL ) {
        shared_ptr<string> ptrRequirementDeckFileName = ptrSensor->GetRequirementDeckFileName();
        string *ptrFullFileName = NULL;

        if (ptrRequirementDeckFileName == NULL) {
            throw new InputException(GetClassName(), "SetupDeckFileNames()",
                                     ("Missing 'Requirement Deck File Name' input for sensor " + ptrSensor->GetSensorName()));
        }

        try {
            ptrFullFileName = new string(*(TargetDeck::GetDeckFileBaseDirectory())
                                         + (*ptrRequirementDeckFileName));
            ParseUtility::VerifyInputFile(ptrFullFileName);
            FileNames::AddFileToList(FileNames::REQUIREMENT_DECK, *ptrRequirementDeckFileName);
            delete ptrFullFileName;
            ptrFullFileName = NULL;
        } catch (Exception *ptrError) {
            delete ptrFullFileName;
            ptrFullFileName = NULL;
            ptrError->AddMethod(GetClassName(), "SetupDeckFileNames()");
            throw;
        }

        ptrSensor = sensorIter.Next();
    }
}

/* ****************************************************************************************** */

string
TimelineGenerator::GetBaseDeckFileName(const string &fullDeckFileName,
                                       const string &standardExtension)
{
    string::size_type startPosition = string::npos;
    string::size_type endPosition;
    string            baseDeckFileName = "";
    endPosition = fullDeckFileName.size();
    // strip off any directory path to get just the file name
    startPosition = fullDeckFileName.find_last_of("/");

    if (startPosition != string::npos) {
        baseDeckFileName = fullDeckFileName.substr((startPosition + 1),
                           (endPosition - startPosition));
    } else {
        startPosition = fullDeckFileName.find_last_of("\\");

        if (startPosition != string::npos) {
            baseDeckFileName = fullDeckFileName.substr((startPosition + 1),
                               (endPosition - startPosition));
        } else {
            baseDeckFileName = fullDeckFileName;
        }
    }

    startPosition = 0;
    // strip off the standard '_Target.deck' extension (if it exists)
    endPosition = baseDeckFileName.find_last_of(standardExtension);

    if (endPosition != string::npos) {
        baseDeckFileName = baseDeckFileName.substr(startPosition,
                           (endPosition - standardExtension.size() + 1));
    }
    // strip off ANY extension (if one exists)
    else {
        endPosition = baseDeckFileName.find_last_of(".");

        if (endPosition != string::npos) {
            baseDeckFileName = baseDeckFileName.substr(startPosition, endPosition);
        }
    }

    return(baseDeckFileName);
}

void
TimelineGenerator::PrintRequirementDeck(const string &outputFileName,

                                        int deckIndex,
                                        int sensorIndex)
{
    string    *ptrDeckFileName = NULL;
    ofstream   outfile;

    try {
        ptrDeckFileName = new string(outputFileName);
        ParseUtility::OpenOutputFile(outfile, ptrDeckFileName);
        outfile << "  RGN  SR  MSN  QUAL   PRI  SUB            SCORE               AREA  PTS";
        outfile << NEW_LINE;
        requirementMap_s->PrintRequirements(deckIndex, sensorIndex, outfile);
        delete ptrDeckFileName;
        ptrDeckFileName = NULL;
        outfile.close();
    } catch (Exception *ptrError) {
        delete ptrDeckFileName;
        ptrDeckFileName = NULL;
        ptrError->AddMethod(GetClassName(),
                            "PrintRequirementDeck(const string&, int, int)");
        throw;
    }

    return;
}

void
TimelineGenerator::DetermineBasicTimelineElements(const shared_ptr<UserVehicle> &ptrUserVehicle,
        ofstream &valtLogFile)
{
    const int                                        numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const int                                        numberOfResources = ptrUserVehicle->GetNumberOfTimelineResources();
    const int                                        numberOfMissions = Target::GetNumberOfMissions();
    const double                                     quarterCircle = UnitData::GetStoreQuarterCircle();
    string                                           sensorName = "";
    string											 errorString = "";
    const string                                     userDesignator = ptrUserVehicle->GetDesignator();
    bool                                             scheduleTargets = TargetDeck::ScheduleTargets();
    const bool                                       determineRegionData = ptrUserVehicle->NeedToDetermineRegionData();
    Array<bool>                                     *ptrValidMission = new Array<bool>(numberOfMissions, false);
    SpaceVector                                      vehiclePosition;
    SpaceVector                                      vehicleVelocity;
    SpaceVector                                      sunPosition;
    SpaceVector                                      targetPosition;
    SpaceVector                                      targetPositionNegated;
    SpaceVector                                      targetToVehiclePosition;
    SpaceVector                                      targetToSun;
    AccessPattern                                   *ptrAccessPattern = NULL;
    CrisisArea                                      *ptrCrisisArea = NULL;
    Region                                          *ptrRegion = NULL;
    RegionData                                      *ptrRegionData = NULL;
    LinkList<Region>                                *ptrRegionsInViewList = new LinkList<Region>();
    //RAGUSA 1/10/2018 - List of Regions that are Inview in the Crisis Region Access Pattern at each time index
    LinkList<Region>                                *ptrCrisisPatternInViewList = new LinkList<Region>();
    LinkList<RegionData>                            *ptrRegionDataList = NULL;
    Sensor                                          *ptrSensor = NULL;
    ListIterator<Sensor>                             sensorListIter(ptrUserVehicle->GetSensorList());
    ValueTimelineElement                            *ptrTimelineElement = NULL;
    shared_ptr<Requirement>                          ptrRequirement = NULL;
    shared_ptr<MURALTypes::RequirementVector>        requirementVector = NULL;
    MURALTypes::RequirementVector::iterator          requirementIter;

    try {
        int calculatedQuality = 0;
        int requiredQuality = 0;
        int deckIndex = 0;
        int sensorIndex = 0;
        int timeIndex = 0;
        int missionIndex = 0;
        double regionNumber = 0.0;
        double sunAngle = 0.0;
        double targetElevationAngle = 0.0;
        double qualityParameter = 0.0;
        bool foundValidRequirement = false;
        bool spectralSensor = false;
        bool firstRegion = false;
        bool firstRequirement = false;
        //RAGUSA 1/5/2018 - Default Requirement Parameters. Only change if absolutely necessary-
        //these values are currently validated to work as of 1/18/2018===========================================================
        int defaultQualityIndex = 1;     //Quality Index; Can be 0-100 - set low so requirement is not constraining
        int defaultPriorityIndex = 50;   //Priority to meet this requirement can be 0-100 - set to 50 for 'average' priority
        int defaultSubPriorityIndex = 0; //SubPriority - set to 0, rarley used
        double defaultRegionScore = 0.0; //Region Score - set to 0.0 for crisis region to work, but will never be selected
        double defaultRegionArea = 2400; //Surface Area of Region [(input distance unit)^2] - set to 2400, the max region area
        int defaultNumberOfTargets = 0;  //Number of Targets in the Region - set to 0 because no defined targets
        //=======================================================================================================================

        // if targets are to later be scheduled but this user does not use a target
        // deck, then reset the flag so unnecessary data is not kept
        if ((scheduleTargets == true) && (ptrUserVehicle->UseTargetDeck() == false)) {
            scheduleTargets = false;
        }

        while (timeIndex < numberOfTimeSteps) {
            vehiclePosition = ptrUserVehicle->GetPosition(timeIndex);

            if (determineRegionData == true) {
                if (ValidTimeStep(ptrUserVehicle, timeIndex, valtLogFile) == true) {
                    ptrRegionDataList = new LinkList<RegionData>();
                    vehicleVelocity = ptrUserVehicle->GetVelocity(timeIndex);
                    sunPosition = Sun::GetPosition(timeIndex);
                    // Create a position state for arguements
                    PositionState positionState(vehiclePosition, vehicleVelocity);
                    ptrSensor = sensorListIter.First();

                    while (ptrSensor != NULL) {
                        deckIndex = ptrUserVehicle->GetDeckIndex(ptrSensor);
                        sensorIndex = ptrSensor->GetSensorIndex();
                        sensorName = ptrSensor->GetSensorName();
                        spectralSensor = Sensor::IsSpectralSensor(sensorIndex);
                        ptrAccessPattern = ptrSensor->GetAccessPattern();

                        /*RAGUSA 1/9/2018 - If this user's sensor uses CRISIS region Mode, then need to create two access pattern lists:
                        		1. The Access Pattern defined by the CRISIS Missions cone angle inputs
                        		2. The Access Pattern defined by the Default cone angle inputs
                          Now, Crisis Region Mode activates when the first list has *at least* 1 Crisis Region defined in the CrisisArea Block.
                          If Crisis Region Mode is not used by this user/sensor - then use LARGEST_PATTERN (same as prior to V11.0).
                          Also added exception handling if there is no mission pattern on the sensors of the crisis vehicle. MURAL requires
                          that input to use crisis region.*/
                        if (CrisisData::HasCrisisArea(userDesignator) == true) {
                            if ((ptrAccessPattern->GetOuterConeElevationAngle(AccessPattern::CRISIS_PATTERN, "CRISIS") != UNDEFINED_ANGLE) &&
                                    ((ptrAccessPattern->GetInnerConeElevationAngle(AccessPattern::DEFAULT_PATTERN) != UNDEFINED_ANGLE) ==
                                     (ptrAccessPattern->GetInnerConeElevationAngle(AccessPattern::CRISIS_PATTERN, "CRISIS") != UNDEFINED_ANGLE))) {
                                ptrAccessPattern->GetAllContainedRegions(ptrCrisisPatternInViewList,vehiclePosition, vehicleVelocity,
                                        AccessPattern::CRISIS_PATTERN, "CRISIS");
                                ptrAccessPattern->GetAllContainedRegions(ptrRegionsInViewList,vehiclePosition, vehicleVelocity,
                                        AccessPattern::DEFAULT_PATTERN);

                                //RAGUSA 1/10/2018 - If in Crisis Region Mode, then find the most valuable (highest multiplier)
                                //crisis area in-view at this timestep. We do not care about the specific region anymore,
                                //just the whole area itself (for the multiplier).
                                if (ptrCrisisPatternInViewList != NULL) {
                                    ptrCrisisArea = GetMostValuableCrisisAreaInview(ptrCrisisPatternInViewList, userDesignator);
                                    //OLD:ptrCrisisArea = CrisisData::GetCrisisArea(userDesignator, crisisRegionNumber);
                                }
                            } else {
                                errorString += "User Vehicle: " + userDesignator + " is designated for a crisis area but does not"
                                               " have the correct CRISIS mission pattern defined.\n"
                                               "	 Note: All sensors for this vehicle must have a mission pattern ";
                                throw new InputException(GetClassName(),
                                                         "DetermineBasicTimelineElements(const shared_ptr<UserVehicle> &ptrUserVehicle,ofstream"
                                                         "&valtLogFile)", errorString);
                            }
                        } else {
                            //============================================================================================================================================*/
                            ptrAccessPattern->GetAllContainedRegions(ptrRegionsInViewList,
                                    vehiclePosition, vehicleVelocity,
                                    AccessPattern::LARGEST_PATTERN);
                        }

                        firstRegion = true;
                        ptrRegion = ptrRegionsInViewList->Retrieve();

                        while (ptrRegion != NULL) {
                            if (ValidRegion(ptrUserVehicle, ptrSensor, ptrRegion, timeIndex, valtLogFile) == true) {
                                regionNumber = ptrRegion->GetFullRegionNumber();
                                targetPosition = ptrRegion->GetRegionCenter(timeIndex);
                                targetPositionNegated = targetPosition.Negate();
                                targetToSun = sunPosition - targetPosition;
                                // compute sun elevation angle
                                sunAngle = targetToSun.GetSeparationAngle(targetPositionNegated) - quarterCircle;

                                if (ptrSensor->MeetsSunAngleRequirement(sunAngle) == true) {
                                    firstRequirement = true;
                                    targetToVehiclePosition = vehiclePosition - targetPosition;
                                    // compute target elevation angle which is the angle between the plane
                                    // tangent to the center of the target (region center) and the UV.
                                    targetElevationAngle = quarterCircle
                                                           - targetPosition.GetSeparationAngle(targetToVehiclePosition);
                                    // determine which mission(s) in this region are collectable by this sensor,
                                    // since the largest access pattern was used to determine the regions in view
                                    missionIndex = 0;

                                    while (missionIndex < numberOfMissions) {
                                        (*ptrValidMission)[missionIndex] = ptrAccessPattern->InsideAccessPattern(Target::GetMissionString(missionIndex),
                                                                           targetPosition, vehiclePosition, vehicleVelocity);
                                        ++missionIndex;
                                    }

                                    //RAGUSA 1/12/2018 - No mod here. But every region data at a CRISIS timestep will get the same CrisisArea object.
                                    //This isn't how it used to be where it would only get the object if it is defined in the
                                    //area. Now, every timestep with a crisis area in-view ALL of the regions get the crisisArea and
                                    //later the MVR of these regions with be able to give the crisis area multiplier to boost it's score.
                                    ptrRegionData = new RegionData(ptrRegion, ptrSensor,  ptrValidMission,
                                                                   scheduleTargets, ptrCrisisArea);
                                    calculatedQuality = 0;
                                    foundValidRequirement = false;
                                    requirementVector = requirementMap_s->GetRequirementList(ptrRegion->GetFullRegionNumber(), deckIndex, sensorIndex);

                                    if (requirementVector != NULL) {
                                        for(requirementIter = requirementVector->begin(); requirementIter != requirementVector->end(); ++requirementIter) {
                                            ptrRequirement = (*requirementIter);
                                            missionIndex = ptrRequirement->GetMissionIndex();
                                            // calculate the quality level (NIIRS for VIS/IR, NRIS for SAR/SSAR, GSD for Spectral)
                                            calculatedQuality = ptrSensor->CalculateQualityLevel(targetElevationAngle,
                                                                ptrRequirement->GetMissionString(), targetPosition, positionState);
                                            // get the requirement's required quality
                                            requiredQuality = ptrRequirement->GetQualityLevel(sensorIndex);

                                            // first check that the required quality is within the actual (calculated) quality
                                            if ( ((spectralSensor == false) && (calculatedQuality >= requiredQuality)) ||
                                                    ((spectralSensor == true) && (calculatedQuality <= requiredQuality)) ) {
                                                // check that the sensor's resource weight for this requirement is valid
                                                if (ptrRequirement->GetScore(ptrSensor) > 0.0) {
                                                    // check that this requirement's mission type is inside the access pattern
                                                    if (ptrValidMission->ElementAt(ptrRequirement->GetMissionIndex()) == true) {
                                                        foundValidRequirement = true;
                                                        ptrRegionData->AddRequirement(ptrRequirement);

                                                        if (scheduleTargets == true) {
                                                            ptrRegionData->SetHighestQualityLevel(sensorIndex, calculatedQuality,
                                                                                                  missionIndex);
                                                        }

                                                        if (valtLogFile) {
                                                            // calculate quality parameter (range for Optical and squint for SAR/SSAR)
                                                            shared_ptr<IQE> ptrIQE = ptrSensor->GetIQE();
                                                            qualityParameter = ptrIQE->GetDebugQualityParameter(targetPosition, positionState);
                                                            PrintTimelineDebug(firstRegion, userDesignator, timeIndex, sensorName,
                                                                               firstRequirement, regionNumber, sunAngle, targetElevationAngle,
                                                                               ptrSensor, qualityParameter, calculatedQuality, ptrCrisisArea,
                                                                               valtLogFile, ptrRequirement);
                                                        }
                                                    }
                                                }
                                            } else {
                                                ptrRequirement = NULL;
                                            }
                                        }
                                    }

                                    if ((valtLogFile) && (ptrCrisisArea != NULL) && (foundValidRequirement == false)) {
                                        PrintTimelineDebug(firstRegion, userDesignator, timeIndex, sensorName, firstRequirement,
                                                           regionNumber, sunAngle, targetElevationAngle, ptrSensor,
                                                           qualityParameter, calculatedQuality, ptrCrisisArea, valtLogFile);
                                    }

                                    if (ptrRegionData->IsValid() == true) {
                                        /*RAGUSA 1/5/2018 - Added control stucture to check if a crisis region has no requirements list
                                         (this -used to- cause a major error in the MVR process). Uses constructor call to Requirement class to create a
                                         "placeholder" requirement here to avoid a NULL pointer exception. ========================================*/
                                        if (ptrRegionData->IsCrisisRegion() == true && ptrRegionData->GetRequirementList() == NULL) {
                                            //Default requirement constructor call**********
                                            ptrRequirement = (shared_ptr<Requirement>) new Requirement(sensorIndex,
                                                             missionIndex,
                                                             defaultQualityIndex,
                                                             defaultPriorityIndex,
                                                             defaultSubPriorityIndex,
                                                             defaultRegionScore,
                                                             defaultRegionArea,
                                                             defaultNumberOfTargets,
                                                             numberOfResources);
                                            //**********************************************
                                            ptrRegionData->AddRequirement(ptrRequirement);
                                            ptrRegionDataList->Append(ptrRegionData);
                                            //Add warning statement that a "default requirement" was placed in the ptrRequirement address
                                            //so the analyst knows what happened
                                            cout << "***WARNING*** User Vehicle: " << userDesignator << " Time Index: "<< timeIndex+1 <<" Region: " << regionNumber <<
                                                 " No defined requirements list! A default "
                                                 "requirement was applied as a placeholder." << NEW_LINE;
                                        } else {
                                            ptrRegionDataList->Append(ptrRegionData);
                                        }

                                        //==========================================================================================================
                                    } else {
                                        delete ptrRegionData;
                                    }

                                    ptrRegionData = NULL;
                                }
                            }

                            ptrRegion = ptrRegionsInViewList->Retrieve();
                        }

                        ptrSensor = sensorListIter.Next();
                    }
                }
            }

            ptrTimelineElement = new ValueTimelineElement(GetOverflownRegion(vehiclePosition),
                    numberOfResources, ptrRegionDataList);

            if (ptrRegionDataList != NULL) {
                ptrRegionDataList->RemoveAll();
                delete ptrRegionDataList;
                ptrRegionDataList = NULL;
            }

            ptrUserVehicle->AddValueTimelineElement(timeIndex, ptrTimelineElement);
            ptrTimelineElement = NULL;
            //Clear out regions lists for next timestep
            ptrRegionsInViewList->RemoveAll();
            ptrCrisisPatternInViewList->RemoveAll();
            ++timeIndex;
        }
    } catch (Exception *ptrError) {
        delete ptrValidMission;
        ptrValidMission = NULL;
        ptrRegionsInViewList->RemoveAll();
        ptrCrisisPatternInViewList->RemoveAll();
        delete ptrRegionsInViewList;
        ptrRegionsInViewList = NULL;
        delete ptrCrisisPatternInViewList;
        ptrCrisisPatternInViewList = NULL;
        ptrError->AddMethod(GetClassName(),
                            "DetermineBasicTimelineElements(UserVehicle*, ofstream&, ofstream&)");
        throw;
    }

    delete ptrValidMission;
    ptrValidMission = NULL;
    delete ptrRegionsInViewList;
    ptrRegionsInViewList = NULL;
    delete ptrCrisisPatternInViewList;
    ptrCrisisPatternInViewList = NULL;

    return;
}

/*RAGUSA 1/10/2018 - New Method to find any crisis areas in the CRISIS mission pattern in-view list.
  Just need *at least* 1 crisis region in the list to turn make this time step use crisis region. If more than
  one area is in the pattern, then the area with the highest score multiplier (higher priority essentially)
  is chosen as the crisis area for this timestep. If a specific region is found in two crisis areas with
  the same score multiplier - then the *most valuable (based on score multiplier)* one will 'win out'*/
CrisisArea*
TimelineGenerator::GetMostValuableCrisisAreaInview(LinkList<Region> *ptrRegionList, string userDesignator)
{
    int crisisRegionNumber = 0;
    Region *ptrRegion = NULL;
    CrisisArea *ptrCrisisArea = NULL;
    CrisisArea *ptrMostValuableCrisisArea = NULL;
    double greatestScoreMultiplier = -1.0;
    double scoreMultiplier = 0.0;
    ptrRegion = ptrRegionList->Retrieve();

    while (ptrRegion != NULL) {
        ptrCrisisArea = CrisisData::GetCrisisArea(userDesignator, ptrRegion->GetRegionNumber());

        if (ptrCrisisArea != NULL) {
            scoreMultiplier = ptrCrisisArea->GetTimelineScoreMutliplier(userDesignator);

            if (scoreMultiplier > greatestScoreMultiplier) {
                greatestScoreMultiplier = scoreMultiplier;
                ptrMostValuableCrisisArea = ptrCrisisArea;
            }
        }

        ptrRegion = ptrRegionList->Retrieve();
    }

	delete ptrRegion;
    ptrRegion = NULL;
    //delete ptrCrisisArea;
	//ptrCrisisArea = NULL;
	
    return(ptrMostValuableCrisisArea);
}
//====================================================================================================================

void
TimelineGenerator::RetrieveValueTimelineData(Array<string> *ptrRetrieveUserArray)
{
    const int     numberOfUsers = ptrRetrieveUserArray->Size();
    Array<double> *ptrResourceFactors = NULL;
    Array2D<bool> *ptrRetrievedTime = new Array2D<bool>(numberOfUsers, TimePiece::GetNumberOfTimeSteps(), false);
    string         inputString = "";
    string         userDesignator = "";
    string         errorString = "";
    ifstream       timelineFile;
    ValueTimelineElement *ptrTimelineElement = NULL;
    shared_ptr<MURALTypes::UserVector> ptrUserList = ptrDataProviderCollection_a->GetUserListPtr();
    shared_ptr<UserVehicle> ptrUserVehicle = NULL;
    int userIndex = 0;

    try {
        int crisisLevel = 0;
        int sensorNumber = 0;
        int missionNumber = 0;
        int subPriorityLevel = 0;
        int indexTime = 0;
        int resourceIndex = 0;
        int numberOfResources = 0;
        double overRegion = 0.0;
        double score = 0.0;
        double imagedRegion = 0.0;
        double collectionTime = 0.0;
        double maxBitsCollected = 0.0;
        double inputTime = 0.0;
        ParseUtility::OpenInputFile(timelineFile, FileNames::GetFileName(FileNames::TIMELINE_INPUTFILE));
        // get the header line first
        getline(timelineFile, inputString, NEW_LINE);

        while (getline(timelineFile, inputString, NEW_LINE) && (errorString.size() == 0)) {
            istringstream  inputStream(inputString);
            inputStream >> userDesignator;
            ptrUserVehicle = ptrDataProviderCollection_a->FindUser(userDesignator);
            userIndex = ptrRetrieveUserArray->FindIndex(userDesignator);

            if ((userIndex >= 0) && (ptrUserVehicle != NULL)) {
                inputStream >> inputTime;

                if (TimePiece::ConvertToStoreTimeIndex(inputTime, indexTime) == true) {
                    if (ptrRetrievedTime->ElementAt(userIndex, indexTime) == true) {
                        errorString = "Multiple inputs for User " + userDesignator;
                        errorString += " at time step ";
                        errorString += StringEditor::ConvertToString(TimePiece::GetTimeStep(indexTime));
                    } else {
                        ptrTimelineElement = ptrUserVehicle->GetValueTimelineElement(indexTime);

                        if (ptrTimelineElement != NULL) {
                            inputStream >> overRegion >> score >> crisisLevel;
                            ptrTimelineElement->SetScore(score);
                            ptrTimelineElement->SetCrisisLevel(crisisLevel);
                            numberOfResources = ptrUserVehicle->GetNumberOfTimelineResources();
                            resourceIndex = 0;

                            while (resourceIndex < numberOfResources) {
                                inputStream >> imagedRegion;
                                ptrTimelineElement->SetImagedRegion(imagedRegion, resourceIndex);
                                ++resourceIndex;
                            }

                            resourceIndex = 0;

                            while (resourceIndex < numberOfResources) {
                                inputStream >> sensorNumber;
                                ptrTimelineElement->SetSensorNumber(sensorNumber, resourceIndex);
                                ++resourceIndex;
                            }

                            resourceIndex = 0;

                            while (resourceIndex < numberOfResources) {
                                inputStream >> missionNumber;
                                ptrTimelineElement->SetMissionNumber(missionNumber, resourceIndex);
                                ++resourceIndex;
                            }

                            resourceIndex = 0;

                            while (resourceIndex < numberOfResources) {
                                inputStream >> collectionTime;
                                ptrTimelineElement->SetCollectionTime(collectionTime, resourceIndex);
                                ++resourceIndex;
                            }

                            resourceIndex = 0;

                            while (resourceIndex < numberOfResources) {
                                inputStream >> maxBitsCollected;
                                ptrTimelineElement->SetMaxPossibleBitsCollected(maxBitsCollected, resourceIndex);
                                ++resourceIndex;
                            }

                            resourceIndex = 0;

                            while (resourceIndex < numberOfResources) {
                                inputStream >> subPriorityLevel;
                                ptrTimelineElement->SetSubPriorityLevel(subPriorityLevel, resourceIndex);
                                ++resourceIndex;
                            }

                            if (ptrUserVehicle->GenerateResourceSpecificTimeline() == true) {
                                ptrResourceFactors = new Array<double>(numberOfResources, 0.0);
                                resourceIndex = 0;

                                while (resourceIndex < numberOfResources) {
                                    inputStream >> (*ptrResourceFactors)[resourceIndex];
                                    ++resourceIndex;
                                }

                                ptrTimelineElement->SetResourceFactors(ptrResourceFactors);
                                ptrResourceFactors = NULL;
                            }

                            (*ptrRetrievedTime)[userIndex][indexTime] = true;
                        }
                    }
                }
            }
        }

        timelineFile.close();
    } catch (Exception *ptrError) {
        delete ptrRetrievedTime;
        ptrRetrievedTime = NULL;
        ptrError->AddMethod(GetClassName(), "RetrieveValueTimelineData(UserVehicle*)");
        throw;
    }

    if (errorString.size() == 0) {
        userIndex = 0;

        while (userIndex < numberOfUsers) {
            if (ptrRetrievedTime->ValueExists(userIndex, false) == true) {
                if (errorString.size() == 0) {
                    errorString = "Not enough Value Timeline entries for "
                                  + ptrRetrieveUserArray->ElementAt(userIndex);
                } else {
                    errorString += (", " + ptrRetrieveUserArray->ElementAt(userIndex));
                }
            }

            ++userIndex;
        }
    }

    delete ptrRetrievedTime;
    ptrRetrievedTime = NULL;

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "RetrieveValueTimelineData(Array<string>*, ArrayOfPointers<UserVehicle>*)",
                                 errorString);
    }

    return;
}

void
TimelineGenerator::PrintTimelineDebug(bool &firstRegion,
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
                                      const shared_ptr<Requirement> &ptrRequirement)
{
    const double fullTimeStep = (double)TimePiece::GetSecondsPerTimeStep();

    if (firstRegion == true) {
        valtLogFile << setw(8) << userDesignator;
        TimePiece::PrintIndex(valtLogFile, timeIndex);
        valtLogFile << setw(5) << sensorName;
        valtLogFile << " ";
        firstRegion = false;
    } else {
        valtLogFile << "                     ";
    }

    if (firstRequirement == true) {
        Region::PrintFixedWidthRegionNumber(valtLogFile, regionNumber);
        valtLogFile << " ";
        valtLogFile << setw(8) << setprecision(3)
                    << UnitData::GetOutputAngle(sunAngle);
        valtLogFile << " " << setw(8) << setprecision(3)
                    << UnitData::GetOutputAngle(targetElevationAngle);
        valtLogFile << " " << setw(8) << setprecision(3)
                    << qualityParameter;
        firstRequirement = false;
    } else {
        valtLogFile << "                                   ";
    }

    if (ptrRequirement != NULL) {
        valtLogFile << " " << setw(8) << calculatedQuality << "  ";
        valtLogFile << setw(7) << ptrRequirement->GetMissionString();
        valtLogFile << " " << setw(8)
                    << ptrRequirement->GetQualityLevel(ptrSensor->GetSensorIndex());
        valtLogFile << " " << setw(6)
                    << ptrRequirement->GetPriorityLevel();
        valtLogFile << " " << setw(5) << setprecision(1)
                    << ptrRequirement->GetAchievableCollectionTime(ptrSensor, fullTimeStep);
        valtLogFile << " " << setw(12) << setprecision(3)
                    << ptrRequirement->GetAchievableScore(ptrSensor, fullTimeStep);
        valtLogFile << "  VALID_REQT";

        if (ptrCrisisArea != NULL) {
            valtLogFile << " *";
        }
    } else {
        valtLogFile << "                                    CRISIS_REQT";
    }

    valtLogFile << NEW_LINE;
    return;
}

void
TimelineGenerator::GenerateMostValuableRegionTimeline(const shared_ptr<UserVehicle> &ptrUserVehicle,
        ofstream &valtLogFile)
{
    int                   timeIndex = 0;
    int                   resourceIndex = 0;
    const int             numberOfResources = ptrUserVehicle->GetNumberOfTimelineResources();
    const int             numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    double                regionScore = 0.0;
    double                regionFactor = 0.0;
    double                mostValuableRegionScore = 0.0;
    const double          fullTimeStep = (double)TimePiece::GetSecondsPerTimeStep();
    Array<double>        *ptrMostValuableResourceScore = NULL;
    Array<double>        *ptrResourceFactors = NULL;
    //RAGUSA 1/11/2018 - For re-implementation of Crisis Region Mode==========================
    int                   crisisRegionLevel = 0;
    /*Don't need these anymore because we're not making crisis regions
    the MVR automatically - you'll see commented out lines elsewhere
    in this method for the same reason
    bool                  foundCrisisRegion = false;
    Array<bool>           *ptrFoundCrisisRegion = NULL;
    =========================================================================================*/
    const bool            resourceSpecificTimeline = ptrUserVehicle->GenerateResourceSpecificTimeline();
    string                 statusString = "TIMELINE_MVR";
    const string           userDesignator = ptrUserVehicle->GetDesignator();
    RegionData                  *ptrRegionData = NULL;
    RegionData                  *ptrMostValuableRegionData = NULL;
    ValueTimelineElement        *ptrTimelineElement = NULL;
    LinkList<RegionData>        *ptrRegionDataList = NULL;
    ArrayOfPointers<RegionData> *ptrMostValuableResourceData = NULL;
    ArrayOfPointers<Link>       *ptrLinkArray = NULL;

    if (resourceSpecificTimeline == true) {
        ptrMostValuableResourceScore = new Array<double>(numberOfResources, 0.0);
        //ptrFoundCrisisRegion = new Array<bool>(numberOfResources, false);
        ptrMostValuableResourceData = new ArrayOfPointers<RegionData>(numberOfResources);
        ptrLinkArray = new ArrayOfPointers<Link>(numberOfResources);

        while (resourceIndex < numberOfResources) {
            ptrLinkArray->AddElement(resourceIndex,
                                     ptrUserVehicle->GetLinkTo(resourceIndex));
            ++resourceIndex;
        }
    }

    while (timeIndex < numberOfTimeSteps) {
        ptrRegionDataList = ptrUserVehicle->GetTimelineRegionDataList(timeIndex);

        if (ptrUserVehicle->NeedToResetTimelineRequirements(timeIndex) == true) {
            ResetRequirements(ptrUserVehicle);
        }

        ptrTimelineElement = ptrUserVehicle->GetValueTimelineElement(timeIndex);
        ptrRegionDataList = ptrUserVehicle->GetTimelineRegionDataList(timeIndex);

        if (ptrRegionDataList != NULL) {
            ListIterator<RegionData> regionDataListIter(ptrRegionDataList);
            mostValuableRegionScore = 0.0;
            ptrMostValuableRegionData = NULL;
            //foundCrisisRegion = false;

            if (resourceSpecificTimeline == true) {
                ptrMostValuableResourceScore->ResetValues(0.0);
                ptrResourceFactors = new Array<double>(numberOfResources, 0.0);
                //ptrFoundCrisisRegion->ResetValues(false);
                ptrMostValuableResourceData->RemoveAll();
            }

            ptrRegionData = regionDataListIter.First();

			while (ptrRegionData != NULL) {
				regionScore = ptrRegionData->GetAchievableScore(fullTimeStep);
				//RAGUSA - Don't need this
				//crisisRegionAtTimeStep = ptrRegionData->IsCrisisRegion();

				if (IsMostValuableRegion(regionScore, mostValuableRegionScore) == true) {
					if (ptrMostValuableRegionData != NULL) {
						ptrMostValuableRegionData->RemoveMostValuableRequirements();
					}

					mostValuableRegionScore = regionScore;
					ptrMostValuableRegionData = ptrRegionData;
				}
				else {
					ptrRegionData->RemoveMostValuableRequirements();
				}

				if (resourceSpecificTimeline == true) {
					resourceIndex = 0;

					while (resourceIndex < numberOfResources) {
						regionFactor = FactorWhenPointingAtRegion((*ptrLinkArray)[resourceIndex],
							ptrUserVehicle, ptrRegionData->GetRegion(),
							timeIndex, regionScore, valtLogFile);

						if (regionFactor > 0.0) {
							regionScore = ptrRegionData->GetAchievableScore(fullTimeStep, resourceIndex, numberOfResources)
								* regionFactor;

							if (IsMostValuableRegion(regionScore, ptrMostValuableResourceScore->ElementAt(resourceIndex)) == true) {
								if (ptrMostValuableResourceData->ElementAt(resourceIndex) != NULL) {
									(*ptrMostValuableResourceData)[resourceIndex]
										->RemoveMostValuableRequirements(resourceIndex);
								}

								(*ptrMostValuableResourceScore)[resourceIndex] = regionScore;
								ptrMostValuableResourceData->AddElement(resourceIndex, ptrRegionData);
							}
							else {
								ptrRegionData->RemoveMostValuableRequirements(resourceIndex);
							}

							//if (crisisRegionAtTimeStep == true) {
							//    (*ptrFoundCrisisRegion)[resourceIndex] = true;
							//}
						}
						++resourceIndex;
					}
				}
				ptrRegionData = regionDataListIter.Next();
			}
            if (ptrMostValuableRegionData != NULL) {
                /*RAGUSA 1/11/2018 - MVR may not be part of crisis area, but still want it
                 on this timestep if a crisis area is in-view at the same time - level will have highest crisis region multiplier.
                 Now applied the multiplier here because it should be multiplied by timestep.
                 OLD:ptrTimelineElement->SetCrisisLevel(ptrMostValuableRegionData->GetCrisisLevel());*/
                crisisRegionLevel = ptrMostValuableRegionData->GetCrisisLevel();
                ptrTimelineElement->SetScore(mostValuableRegionScore);
                ptrTimelineElement->SetCrisisLevel(crisisRegionLevel);
                //=================================================================================================================
                ptrTimelineElement->SetMostValuableRegionData(ptrMostValuableRegionData);
                resourceIndex = 0;

                if (resourceSpecificTimeline == false) {
                    if (valtLogFile) {
                        PrintMostValuableRegionDebug(userDesignator, timeIndex, ptrMostValuableRegionData,
                                                     statusString, valtLogFile);
                    }

                    UpdateTimelineElement(ptrTimelineElement, ptrMostValuableRegionData);
                } else {
                    while (resourceIndex < numberOfResources) {
                        if (mostValuableRegionScore > 0.0) {
                            (*ptrResourceFactors)[resourceIndex] = ptrMostValuableResourceScore->ElementAt(resourceIndex)
                                                                   / mostValuableRegionScore;
                        }

                        ptrMostValuableRegionData = (*ptrMostValuableResourceData)[resourceIndex];

                        if (ptrMostValuableRegionData != NULL) {
                            if (valtLogFile) {
                                statusString = "TIMELINE_" + (ptrLinkArray->ElementAt(resourceIndex))->GetReceiveDesignator();
                                PrintMostValuableRegionDebug(userDesignator, timeIndex, ptrMostValuableRegionData,
                                                             statusString, valtLogFile, resourceIndex);
                            }

                            UpdateTimelineElement(ptrTimelineElement, ptrMostValuableRegionData, resourceIndex);
                        }

                        ++resourceIndex;
                    }
                }
            }
        }

        if (ptrResourceFactors != NULL) {
            ptrTimelineElement->SetResourceFactors(ptrResourceFactors);
            ptrResourceFactors = NULL;
        }

        ++timeIndex;
    }

    ResetRequirements(ptrUserVehicle);

    if (resourceSpecificTimeline == true) {
        delete ptrMostValuableResourceScore;
        ptrMostValuableResourceScore = NULL;
        //delete ptrFoundCrisisRegion;
        //ptrFoundCrisisRegion = NULL;
        delete ptrMostValuableResourceData;
        ptrMostValuableResourceData = NULL;
        delete ptrLinkArray;
        ptrLinkArray = NULL;
    }

    return;
}

void
TimelineGenerator::PrintMostValuableRegionDebug(const string &userDesignator,
        int timeIndex,
        RegionData *ptrMostValuableRegionData,
        const string &statusString,
        ofstream &valtLogFile,
        int resourceIndex,
        double resourceFactor)
{
    valtLogFile << setw(8) << userDesignator;
    TimePiece::PrintIndex(valtLogFile, timeIndex);
    ptrMostValuableRegionData->PrintDebug(statusString, resourceIndex,
                                          resourceFactor, valtLogFile);
    return;
}

void
TimelineGenerator::UpdateTimelineElement(ValueTimelineElement *ptrElement,
        RegionData *ptrMostValuableRegionData,
        int resourceIndex)
{
    double timeToCollect;
    const double sensorDataRate = ptrMostValuableRegionData->GetSensorDataRate();
    ptrElement->SetImagedRegion(ptrMostValuableRegionData->GetRegionNumber(), resourceIndex);
    ptrElement->SetSensorNumber(ptrMostValuableRegionData->GetSensorNumber(), resourceIndex);
    ptrElement->SetMissionNumber(ptrMostValuableRegionData->GetMostValuableMissionNumber(resourceIndex),
                                 resourceIndex);
    ptrElement->SetSubPriorityLevel(ptrMostValuableRegionData->GetMostValuableSubPriLevel(resourceIndex),
                                    resourceIndex);
    timeToCollect = ptrMostValuableRegionData->UpdateMostValuableRequirements(resourceIndex);
    ptrElement->SetCollectionTime(timeToCollect, resourceIndex);
    ptrElement->SetMaxPossibleBitsCollected((timeToCollect * sensorDataRate), resourceIndex);
    return;
}

void
TimelineGenerator::GenerateTimeOfClosestApproachTimeline(const shared_ptr<UserVehicle> &ptrUserVehicle,
        ofstream &valtLogFile)
{
    const int                       numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    const int                       numberOfSensors = ptrUserVehicle->GetNumberOfSensors();
    const double                      fullTimeStep = (double)TimePiece::GetSecondsPerTimeStep();
    RegionData                      *ptrRegionData = NULL;
    TCARegionData                   *ptrTCARegionData = NULL;
    SpaceVector                      vehiclePosition;
    SpaceVector                      regionCenterToUser;
    ListIterator<Sensor>             sensorIter(ptrUserVehicle->GetSensorList());
    Sensor                          *ptrSensor = sensorIter.First();
    ArrayOfLinkLists<TCARegionData> *ptrTCADataListArray
        = new ArrayOfLinkLists<TCARegionData>(numberOfSensors);

    try {
        int sensorIndex = 0;
        double regionNumber = 0.0;
        double totalRegionScore = 0.0;
        double timeToCollect = 0.0;
        //RAGUSA 1/12/2018
        int crisisLevel = 0;

        while (ptrSensor != NULL) {
            int sensorNumber = 0;
            int timeIndex = 0;
            sensorNumber = ptrSensor->GetSensorNumber();
            timeIndex = 0;

            while (timeIndex < numberOfTimeSteps) {
                ListIterator<RegionData> regionDataListIter(ptrUserVehicle->GetTimelineRegionDataList(timeIndex));
                vehiclePosition = ptrUserVehicle->GetPosition(timeIndex);
                ptrRegionData = regionDataListIter.First();

                while (ptrRegionData != NULL) {
                    if (ptrRegionData->GetSensorNumber() == sensorNumber) {
                        totalRegionScore = ptrRegionData->GetTotalAchievableScore(fullTimeStep);

                        if (totalRegionScore > 0.0) {
                            regionNumber = ptrRegionData->GetRegionNumber();
                            timeToCollect = ptrRegionData->GetTotalAchievableTTC(fullTimeStep);
                            crisisLevel = ptrRegionData->GetCrisisLevel();
                            ptrTCARegionData = FindTCARegionData(ptrTCADataListArray->LinkListAt(sensorIndex),
                                                                 regionNumber);

                            if (ptrTCARegionData == NULL) {
                                ptrTCARegionData = new TCARegionData(regionNumber, crisisLevel);
                                ptrTCADataListArray->AddObject(ptrTCARegionData, sensorIndex, (int)regionNumber);
                            }

                            regionCenterToUser = vehiclePosition - ptrRegionData->GetRegionPosition(timeIndex);
                            ptrTCARegionData->AddTimeStepData(timeIndex, regionCenterToUser.GetMagnitude(),
                                                              totalRegionScore, timeToCollect);
                        }
                    }

                    ptrRegionData = regionDataListIter.Next();
                }

                ++timeIndex;
            }

            ++sensorIndex;
            ptrSensor = sensorIter.Next();
        }

        BuildTCATimeline(ptrUserVehicle, ptrTCADataListArray, valtLogFile);
    } catch (Exception *ptrError) {
        ptrTCADataListArray->Delete();
        delete ptrTCADataListArray;
        ptrTCADataListArray = NULL;
        ptrError->AddMethod(GetClassName(), "GenerateTimeOfClosestApproachTimeline(UserVehicle*)");
        throw;
    }

    ptrTCADataListArray->Delete();
    delete ptrTCADataListArray;
    ptrTCADataListArray = NULL;
    return;
}

void
TimelineGenerator::GenerateNominalRequestRegionTimeline(const shared_ptr<UserVehicle> &ptrUserVehicle)
{
    const int    numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    double        currentLatitude = 0.0;
    Array<bool>  *ptrTimeStepOnFlag = new Array<bool>(numberOfTimeSteps, false);
    Revolution   *ptrRevolution = NULL;
    LinkList<Revolution> *ptrRevList = NULL;

    if (ptrUserVehicle->UsesSouthPointBoundary() == true) {
        ptrRevList = new LinkList<Revolution>(*(ptrUserVehicle->GetRevolutionList()));
    } else {
        ptrRevList = ptrUserVehicle->GenerateSouthPointRevolutionList();
    }

    ptrRevolution = ptrRevList->Retrieve();

    while (ptrRevolution != NULL) {
        int timeIndex = 0;
        int revolutionEndTime = 0;
        double previousLatitude;
        double ascendingZeroLongitude;
        double minimumAscendingLatitude;
        double maximumAscendingLatitude;
        double maximumDescendingLatitude;
        double minimumDescendingLatitude;
        timeIndex = ptrRevolution->GetStartTime();
        revolutionEndTime = ptrRevolution->GetEndTime();
        ascendingZeroLongitude = ptrRevolution->GetAscendingZeroCrossingLongitude();
        minimumAscendingLatitude = ptrUserVehicle->GetMinAscendingOffRegion(ascendingZeroLongitude);
        maximumAscendingLatitude = ptrUserVehicle->GetMaxAscendingOffRegion(ascendingZeroLongitude);
        maximumDescendingLatitude = ptrUserVehicle->GetMaxDescendingOffRegion(ascendingZeroLongitude);
        minimumDescendingLatitude = ptrUserVehicle->GetMinDescendingOffRegion(ascendingZeroLongitude);
        previousLatitude = ptrUserVehicle->GetLatitude(timeIndex);

        while (timeIndex <= revolutionEndTime) {
            currentLatitude = ptrUserVehicle->GetLatitude(timeIndex);

            if (currentLatitude >= previousLatitude) {
                if ( (currentLatitude > minimumAscendingLatitude) &&
                        (currentLatitude < maximumAscendingLatitude) ) {
                    (*ptrTimeStepOnFlag)[timeIndex] = true;
                }
            } else if ( (currentLatitude < maximumDescendingLatitude) &&
                        (currentLatitude > minimumDescendingLatitude) ) {
                (*ptrTimeStepOnFlag)[timeIndex] = true;
            }

            previousLatitude = currentLatitude;
            ++timeIndex;
        }

        delete ptrRevolution;
        ptrRevolution = NULL;
        ptrRevolution = ptrRevList->Retrieve();
    }

    delete ptrRevList;
    ptrRevList = NULL;
    DetermineNominalRequestRegionElements(ptrUserVehicle, ptrTimeStepOnFlag);
    delete ptrTimeStepOnFlag;
    ptrTimeStepOnFlag = NULL;
    return;
}

void
TimelineGenerator::BuildTCATimeline(const shared_ptr<UserVehicle> &ptrUserVehicle,
                                    ArrayOfLinkLists<TCARegionData> *ptrTCADataListArray,
                                    ofstream &valtLogFile)
{
    int          crisisID = 0;
    const int    subPriorityLevel = 0;
    int           sensorIndex = 0;
    int           timeIndex = 0;
    int           instanceOfTCA = 0;
    int           numberOfTCAs = 0;
    const int     resourceIndex = -1;
    const int     numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    double          regionNumber = 0.0;
    double         maximumBits = 0.0;
    const double   maxCollectionDataRate = ptrUserVehicle->GetMaximumSensorDataRate();
    string         sensorName = "";
    const string   userDesignator = ptrUserVehicle->GetDesignator();
    Array<int>   *ptrTCATimeIndexArray = NULL;
    Array<double>  *ptrImagedRegion = new Array<double>(numberOfTimeSteps, 0.0);
    Array<int>  *ptrSensorNumber = new Array<int>(numberOfTimeSteps, 0);
    Array<int>  *ptrMissionNumber = new Array<int>(numberOfTimeSteps, 0);
    Array<double> *ptrTotalScore = new Array<double>(numberOfTimeSteps, 0.0);
    Array<double> *ptrTotalTTC = new Array<double>(numberOfTimeSteps, 0.0);
    Array<int>  *ptrCrisisAreaNumber = new Array<int>(numberOfTimeSteps, 0);
    CrisisArea           *ptrCrisisArea = NULL;
    TCARegionData        *ptrTCARegionData = NULL;
    ValueTimelineElement *ptrTimelineElement = NULL;
    ListIterator<Sensor>  sensorListIter(ptrUserVehicle->GetSensorList());
    Sensor               *ptrSensor = sensorListIter.First();

    if (valtLogFile) {
        valtLogFile << " USER   SEN    REGION  INVIEW PERIODS (TCA)" << NEW_LINE;
    }

    while (ptrSensor != NULL) {
        int sensorNumber = 0;
        ListIterator<TCARegionData> dataListIter(ptrTCADataListArray->LinkListAt(sensorIndex));
        sensorNumber = ptrSensor->GetSensorNumber();
        sensorName = ptrSensor->GetSensorName();
        ptrTCARegionData = dataListIter.First();

        while (ptrTCARegionData != NULL) {
            regionNumber = ptrTCARegionData->GetRegionNumber();

            if (valtLogFile) {
                valtLogFile << setw(8) << userDesignator;
                valtLogFile << " " << setw(5) << sensorName << " ";
                Region::PrintFixedWidthRegionNumber(valtLogFile, regionNumber);
                valtLogFile << " ";
            }

            //ptrCrisisArea = CrisisData::GetCrisisArea(userDesignator, regionNumber);
            crisisID = ptrTCARegionData->GetCrisisRegionLevel();
            //if (ptrCrisisArea == NULL) {
            ptrTCATimeIndexArray = ptrTCARegionData->DetermineTCATimeSteps();

            if (ptrTCATimeIndexArray != NULL) {
                instanceOfTCA = 0;
                numberOfTCAs = ptrTCATimeIndexArray->Size();

                while (instanceOfTCA < numberOfTCAs) {
                    timeIndex = ptrTCATimeIndexArray->ElementAt(instanceOfTCA);
                    (*ptrSensorNumber)[timeIndex] = sensorNumber;
                    (*ptrTotalScore)[timeIndex] += ptrTCARegionData->GetScore(timeIndex);
                    (*ptrTotalTTC)[timeIndex] += ptrTCARegionData->GetTimeToCollect(timeIndex);

                    if (crisisID > 0) {
                        (*ptrMissionNumber)[timeIndex] = CRISIS_MISSION_NUMBER_s;
                        (*ptrCrisisAreaNumber)[timeIndex] = crisisID;
                    }

                    ++instanceOfTCA;
                }

                delete ptrTCATimeIndexArray;
                ptrTCATimeIndexArray = NULL;
            }

            if (valtLogFile) {
                ptrTCARegionData->PrintTCAPeriods(valtLogFile);
                valtLogFile << NEW_LINE;
            }

            if ((crisisID > 0) && (valtLogFile)) {
                valtLogFile << " Crisis Times:";
            }

            if (valtLogFile) {
                valtLogFile << " " << TimePiece::GetTimeStep(timeIndex) << NEW_LINE;
            }

            /*RAGUSA 1/15/2018 - Removes case if there is a crisis region on the TCARegionData.
              The removed code used to skip finding the TCA regions and always go after the
              crisis region instead for this timeline element - we do not want this.
              Instead, we choose the TCA and then if a crisis area is in-view, we want this
              timeIndex to get the crisis level (multiplier) - but it gets applied to the TCA region's score.
              This seems odd because TCA works differently than MVR and REGION, but it is done to remain consistent.
            } else {
              //  crisisID = ptrCrisisArea->GetIdentificationNumber();
                if (valtLogFile) {
                    valtLogFile << " Crisis Times:";
                }

                timeIndex = 0;

                while (timeIndex < numberOfTimeSteps) {
                    if (ptrTCARegionData->GetDistance(timeIndex) > 0.0) {
                        if (ptrTCARegionData->GetScore(timeIndex) > ptrTotalScore->ElementAt(timeIndex)) {
                            (*ptrImagedRegion)[timeIndex] = regionNumber;
                            (*ptrSensorNumber)[timeIndex] = sensorNumber;
                            (*ptrMissionNumber)[timeIndex] = CRISIS_MISSION_NUMBER_s;
                            (*ptrTotalScore)[timeIndex] = ptrTCARegionData->GetScore(timeIndex);
                            (*ptrTotalTTC)[timeIndex] = ptrTCARegionData->GetTimeToCollect(timeIndex);
                            (*ptrCrisisAreaNumber)[timeIndex] = crisisID;
                        }

                        if (valtLogFile) {
                            valtLogFile << " " << TimePiece::GetTimeStep(timeIndex);
                        }
                    }

                    ++timeIndex;
                }

            	if (valtLogFile) {
            		valtLogFile << NEW_LINE;
            	}
            }*/
            //=================================================================================================
            ptrTCARegionData = dataListIter.Next();
        }

        ++sensorIndex;
        ptrSensor = sensorListIter.Next();
    }

    timeIndex = 0;

    while (timeIndex < numberOfTimeSteps) {
        ptrTimelineElement = ptrUserVehicle->GetValueTimelineElement(timeIndex);

        if (ptrTimelineElement != NULL) {
            maximumBits = ptrTotalTTC->ElementAt(timeIndex) * maxCollectionDataRate;

            //RAGUSA 1/17/2018 - Once a region has been chosen, if CRISIS is on, then apply the multiplier
            //to that element's score=======================================================================
            if (ptrCrisisAreaNumber->ElementAt(timeIndex) > 0) {
                ptrTimelineElement->SetScore(ptrTotalScore->ElementAt(timeIndex)
                                             * CrisisData::GetTimelineScoreMultiplier(ptrCrisisAreaNumber->ElementAt(timeIndex), userDesignator));
            } else {
                ptrTimelineElement->SetScore(ptrTotalScore->ElementAt(timeIndex));
            }

            //==============================================================================================
            ptrTimelineElement->SetCrisisLevel(ptrCrisisAreaNumber->ElementAt(timeIndex));
            ptrTimelineElement->SetImagedRegion(ptrImagedRegion->ElementAt(timeIndex), resourceIndex);
            ptrTimelineElement->SetSensorNumber(ptrSensorNumber->ElementAt(timeIndex), resourceIndex);
            ptrTimelineElement->SetMissionNumber(ptrMissionNumber->ElementAt(timeIndex), resourceIndex);
            ptrTimelineElement->SetCollectionTime(ptrTotalTTC->ElementAt(timeIndex), resourceIndex);
            ptrTimelineElement->SetMaxPossibleBitsCollected(maximumBits, resourceIndex);
            ptrTimelineElement->SetSubPriorityLevel(subPriorityLevel, resourceIndex);
            //ptrTimelineElement->SetMostValuableRegionData(ptrMostValuableRegionData);
        }

        ++timeIndex;
    }

    delete ptrImagedRegion;
    ptrImagedRegion = NULL;
    delete ptrSensorNumber;
    ptrSensorNumber = NULL;
    delete ptrMissionNumber;
    ptrMissionNumber = NULL;
    delete ptrTotalScore;
    ptrTotalScore = NULL;
    delete ptrTotalTTC;
    ptrTotalTTC = NULL;
    delete ptrCrisisAreaNumber;
    ptrCrisisAreaNumber = NULL;
    return;
}

void
TimelineGenerator::DetermineNominalRequestRegionElements(const shared_ptr<UserVehicle> &ptrUserVehicle,
        const Array<bool> *ptrTimeStepOnFlag)
{
    int                 sensorNumber = 0;
    //int                 level = 0; Don't need currently - RAGUSA
    int                 crisisLevel = 0;
    int                 sensorCrisisLevel = 0;
    int                 missionNumber = 0;
    int                 sensorMissionNumber = 0;
    int                 subPriorityLevel = 0;
    int                 sensorSubPriorityLevel = 0;
    int                  timeIndex = 0;
    const int            resourceIndex = -1;
    const int            numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    double                 timeStepScore = 0.0;
    double                 sensorScore = 0.0;
    double                 timeToCollect = 0.0;
    double                 sensorTTC = 0.0;
    double                 regionNumber = 0.0;
    double                 sensorRegionNumber = 0.0;
    double                 sensorWeightFactor = 0.0;
    const double           fullTimeStep = (double)TimePiece::GetSecondsPerTimeStep();
    const double           flatScorePerTime = (double)ptrUserVehicle->GetFlatScorePerTime();
    int                    crisisFlag = 0;
    ValueTimelineElement *ptrTimelineElement = NULL;
    RegionData           *ptrRegionData = NULL;
    //RAGUSA 1/29/2018 - Added to correctly set the ptrMostValubleRegion_a in collectionElement
    RegionData           *ptrTimeStepMVRegionData = NULL;
    RegionData           *ptrSensorMVRegionData = NULL;
    //=========================================================================================
    Sensor               *ptrHighestSensor = NULL;
    Sensor               *ptrSensor = NULL;
    LinkList<RegionData> *ptrRegionDataList = NULL;
    ListIterator<Sensor>  sensorListIter(ptrUserVehicle->GetSensorList());

    while (timeIndex < numberOfTimeSteps) {
        if (ptrTimeStepOnFlag->ElementAt(timeIndex) == true) {
            ptrTimelineElement = ptrUserVehicle->GetValueTimelineElement(timeIndex);
            timeStepScore = 0.0;
            timeToCollect = 0.0;
            crisisLevel = 0;
            missionNumber = 0;
            regionNumber = 0.0;
            subPriorityLevel = 0;
            ptrHighestSensor = NULL;
            ptrRegionDataList = ptrUserVehicle->GetTimelineRegionDataList(timeIndex);

            if (ptrRegionDataList != NULL) {
                ListIterator<RegionData> regionDataListIter(ptrRegionDataList);
                ptrSensor = sensorListIter.First();

                while (ptrSensor != NULL) {
                    sensorScore = 0.0;
                    sensorTTC = 0.0;
                    sensorCrisisLevel = 0;
                    sensorMissionNumber = 0;
                    sensorRegionNumber = 0.0;
                    sensorSubPriorityLevel = SHRT_MAX;
                    sensorNumber = ptrSensor->GetSensorNumber();
                    sensorWeightFactor = ptrSensor->GetSensorWeightFactor();
                    ptrRegionData = regionDataListIter.First();
                    crisisFlag = 0;

                    while (ptrRegionData != NULL) {
                        if (ptrRegionData->GetSensorNumber() == sensorNumber) {
                            sensorScore += (ptrRegionData->GetTotalAchievableScore(fullTimeStep)/sensorWeightFactor);
                            sensorTTC += ptrRegionData->GetTotalAchievableTTC(fullTimeStep);
                            sensorSubPriorityLevel = std::min(sensorSubPriorityLevel,
                                                              ptrRegionData->GetSubPriorityLevel());
                            /*RAGUSA 1/12/2018 - Rewritten for Crisis Region Mode. If/when a region is chosen as MVR, if it has a CrisisArea,
                              then the crisis level will be applied to the timelineElement. If a crisis area is in-view at this timestep,
                              then every region on the sensor should have it in their RegionData object. This is however sensor specific so
                              it's not guaranteed the crisis region will be applied for this timestep. Also note the most valuable
                              crisis area was determined in the DetermineBasicTimelineElements() method.*/
                            sensorCrisisLevel = ptrRegionData->GetCrisisLevel();
                            sensorRegionNumber = ptrRegionData->GetRegionNumber();
                            ptrSensorMVRegionData = ptrRegionData;

                            if (ptrRegionDataList->Size() == 1) {
                                sensorMissionNumber = ptrRegionData->GetMissionNumber();
                            }
                        }

                        ptrRegionData = regionDataListIter.Next();
                    }

                    if (sensorScore > timeStepScore) {
                        timeStepScore = sensorScore;
                        timeToCollect = std::min(sensorTTC, fullTimeStep);
                        //Whatever region ends up MVR then it's crisis level is used for the timelineElement
                        crisisLevel = sensorCrisisLevel;
                        missionNumber = sensorMissionNumber;
                        regionNumber = sensorRegionNumber;
                        subPriorityLevel = sensorSubPriorityLevel;
                        ptrHighestSensor = ptrSensor;
                        ptrTimeStepMVRegionData = ptrSensorMVRegionData;
                    }

                    //=========================================================================================================================
                    ptrSensor = sensorListIter.Next();
                }
            } else if (flatScorePerTime > 0.0) {
                timeStepScore = flatScorePerTime;
                ptrHighestSensor = sensorListIter.First();
                timeToCollect = fullTimeStep;
            }

            //RAGUSA 1/17/2018 - Once a region has been chosen, if CRISIS is on, then apply the multiplier
            //to that element's score==============================================================================
            if (crisisLevel > 0) {
                ptrTimelineElement->SetScore(timeStepScore
                                             * CrisisData::GetTimelineScoreMultiplier(crisisLevel, ptrUserVehicle->GetDesignator()));
                ptrTimelineElement->SetMostValuableRegionData(ptrTimeStepMVRegionData);
            } else {
                ptrTimelineElement->SetScore(timeStepScore);
            }

            //=====================================================================================================
            ptrTimelineElement->SetCollectionTime(timeToCollect, resourceIndex);
            ptrTimelineElement->SetCrisisLevel(crisisLevel);
            ptrTimelineElement->SetMissionNumber(missionNumber, resourceIndex);
            ptrTimelineElement->SetImagedRegion(regionNumber, resourceIndex);
            ptrTimelineElement->SetSubPriorityLevel(subPriorityLevel, resourceIndex);

            if (ptrHighestSensor != NULL) {
                ptrTimelineElement->SetSensorNumber(ptrHighestSensor->GetSensorNumber(), resourceIndex);
                ptrTimelineElement->SetMaxPossibleBitsCollected((timeToCollect * ptrHighestSensor->GetDataRate()),
                        resourceIndex);
            }
        }

        ++timeIndex;
    }

    return;
}

TCARegionData*
TimelineGenerator::FindTCARegionData(const LinkList<TCARegionData> *ptrTCADataList,
                                     double regionNumber) const
{
    bool                        foundRegion = false;
    ListIterator<TCARegionData> dataListIter(ptrTCADataList);
    TCARegionData              *ptrTCARegionData = dataListIter.First();

    while ((ptrTCARegionData != NULL) && (foundRegion == false)) {
        if (ptrTCARegionData->GetRegionNumber() == regionNumber) {
            foundRegion = true;
        } else {
            ptrTCARegionData = dataListIter.Next();
        }
    }

    return(ptrTCARegionData);
}

void
TimelineGenerator::ResetRequirements(const shared_ptr<UserVehicle> &ptrUserVehicle)
{
    int                 deckIndex = -1;
    Array<bool>         *ptrResetSensor = NULL;
    ListIterator<Sensor> sensorListIter(ptrUserVehicle->GetSensorList());
    Sensor              *ptrSensor = sensorListIter.First();

    if (ptrUserVehicle->UseRequirementDeck() == true) {
        ptrResetSensor = new Array<bool>(Sensor::GetNumberOfDeckSensors(), false);

        while (ptrSensor != NULL) {
            deckIndex = ptrUserVehicle->GetDeckIndex(ptrSensor);
            (*ptrResetSensor)[ptrSensor->GetSensorIndex()] = true;
            requirementMap_s->ResetScoreAndArea(deckIndex, ptrResetSensor);
            ptrResetSensor->ResetValues(false);
            ptrSensor = sensorListIter.Next();
        }
    } else {
        deckIndex = ptrUserVehicle->GetDeckIndex(ptrSensor);
        ptrResetSensor = new Array<bool>(Sensor::GetNumberOfTargetSensors(), false);

        while (ptrSensor != NULL) {
            (*ptrResetSensor)[ptrSensor->GetSensorIndex()] = true;
            ptrSensor = sensorListIter.Next();
        }

        requirementMap_s->ResetScoreAndArea(deckIndex, ptrResetSensor);
    }

    delete ptrResetSensor;
    ptrResetSensor = NULL;
    return;
}

double
TimelineGenerator::GetOverflownRegion(const SpaceVector &vehiclePosition)
{
    double   overflownRegionNumber = 0;
    Region *ptrRegion = Earth::GetRegion(vehiclePosition);

    if (ptrRegion != NULL) {
        overflownRegionNumber = ptrRegion->GetFullRegionNumber();
    }

    return(overflownRegionNumber);
}

ArrayOfPointers<CollectionElement>*
TimelineGenerator::CreateCollectionElements(ArrayOfLinkLists<RegionData> *ptrArrayOfRegionDataLists,
        const shared_ptr<UserVehicle> &ptrUserVehicle,
        const shared_ptr<vector<double>> &ptrSolutionArray,
        double integrality,
        const bool &updateRequirements,
        const bool &planCollections,
        ofstream &mvrDebugFile)
{
    Array<int>         *ptrPointsCollected = NULL;
    const int            numberOfTimeSteps = TimePiece::GetNumberOfTimeSteps();
    Array<double>         *ptrAreaCollected = NULL;
    Array<double>        *ptrResourceFactorArray = NULL;
    const string          userDesignator = ptrUserVehicle->GetDesignator();
    RegionData           *ptrRegionData = NULL;
    RegionData           *ptrMostValuableRegionData = NULL;
    CollectionElement    *ptrCollectionElement = NULL;
    LinkList<RegionData> *ptrRegionDataList = NULL;
    ArrayOfPointers<CollectionElement> *ptrCollectionElementArray
        = new ArrayOfPointers<CollectionElement>(numberOfTimeSteps);

    try {
        int solutionIndex = 0;
        int timeIndex = 0;
        double timeToCollect = 0.0;
        double requirementScore = 0.0;
        //RAGUSA 1/31/2018
        int crisisLevel = 0;

        if (planCollections == true) {
            ptrPointsCollected = new Array<int>(Target::GetNumberOfPriorities(), SHORT_ZERO);
            ptrAreaCollected = new Array<double>(Target::GetNumberOfPriorities(), DOUBLE_ZERO);
        }

        while (timeIndex < numberOfTimeSteps) {
            ptrRegionDataList = (*ptrArrayOfRegionDataLists)[timeIndex];

            if (ptrRegionDataList != NULL) {
                ListIterator<RegionData> regionDataListIter(ptrRegionDataList);
                ptrRegionData = regionDataListIter.First();

                while (ptrRegionData != NULL) {
                    solutionIndex = ptrRegionData->GetSolutionIndex();

                    // determine if this region data was the one picked for this time step
                    if (ptrSolutionArray->at(solutionIndex) > integrality) {
                        // should only have one region data picked per time step
                        if (ptrMostValuableRegionData != NULL) {
                            throw new InputException(GetClassName(),
                                                     "CreateCollectionElements(ArrayOfLinkLists<RegionData>*, const UserVehicle*, ...)",
                                                     ("Chose multiple Regions at time step "
                                                      + StringEditor::ConvertToString(TimePiece::GetTimeStep(timeIndex))));
                        }

                        // if this is the most valuable region data, save the pointer to be set later
                        ptrMostValuableRegionData = ptrRegionData;
                        //RAGUSA 1/31/2018 - Need to add score multiplier for the MVR process becuase it creates new collectionElements
                        crisisLevel = ptrRegionData->GetCrisisLevel();

                        if (crisisLevel > 0) {
                            requirementScore = (double)ptrRegionData->GetAchievableScore(ptrSolutionArray) *
                                               CrisisData::GetTimelineScoreMultiplier(crisisLevel,userDesignator);
                        } else {
                            requirementScore = (double)ptrRegionData->GetAchievableScore(ptrSolutionArray);
                        }

                        //=================================================================================================================
                        if (planCollections == true) {
                            ptrRegionData->GetAchievableCollection(ptrSolutionArray, ptrPointsCollected,
                                                                   ptrAreaCollected);
                        }

                        timeToCollect = ptrRegionData->GetTimeToCollect(ptrSolutionArray, updateRequirements);

                        if (mvrDebugFile) {
                            mvrDebugFile << setw(8) << userDesignator << " ";
                            TimePiece::PrintIndex(mvrDebugFile, timeIndex);
                            Region::PrintFixedWidthRegionNumber(mvrDebugFile, ptrRegionData->GetRegionNumber());
                            mvrDebugFile << " " << setw(12) << setprecision(3) << requirementScore;
                            mvrDebugFile << " " << setw(6) << setprecision(2) << timeToCollect;

                            if (ptrRegionData->GetCrisisLevel() > 0) {
                                mvrDebugFile << "  *";
                            }

                            mvrDebugFile << NEW_LINE;
                        }
                    }

                    ptrRegionData = regionDataListIter.Next();
                }

                if (ptrMostValuableRegionData != NULL) {
                    ptrCollectionElement = new CollectionElement(requirementScore, ptrRegionDataList,
                            //RAGUSA 1/30/2018
                            ptrMostValuableRegionData, ptrUserVehicle->GetCrisisLevel(timeIndex), ptrResourceFactorArray,
                            ptrPointsCollected, ptrAreaCollected);
                    ptrCollectionElementArray->AddElement(timeIndex, ptrCollectionElement);
                    ptrCollectionElement = NULL;
                    ptrMostValuableRegionData = NULL;
                }

                if (planCollections == true) {
                    ptrPointsCollected->ResetValues(SHORT_ZERO);
                    ptrAreaCollected->ResetValues(DOUBLE_ZERO);
                }
            }

            ++timeIndex;
        }

        if (planCollections == true) {
            delete ptrPointsCollected;
            ptrPointsCollected = NULL;
            delete ptrAreaCollected;
            ptrAreaCollected = NULL;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "CreateCollectionElements(ArrayOfLinkLists<RegionData>*, const UserVehicle*, ...)");
        throw;
    }

    return(ptrCollectionElementArray);
}

bool
TimelineGenerator::LinkWhenPointingAtRegion(Link *ptrTransmitLink,
        const shared_ptr<UserVehicle> &ptrUserVehicle,
        int timeIndex,
        double regionNumber,
        double azimuthOffsetAngle,
        double cableWrapOffsetAngle,
        double additionalSeconds)
{
    double                                azimuthAngle = 0.0;
    double                                elevationAngle = 0.0;
    double                                minElevationAngle = -UNDEFINED_ANGLE;
    double                                maxElevationAngle = UNDEFINED_ANGLE;
    double                                gimbalStopAngle = 0.0;
    const double                          halfCircle = UnitData::GetStoreHalfCircle();
    bool                                  linkWhenPointingAtRegion = true;
    const string                          receiveDesignator = ptrTransmitLink->GetReceiveDesignator();
    const shared_ptr<UserTransmitAntenna> ptrUserAntenna = ptrUserVehicle->GetUserTransmitAntenna();
    ptrUserAntenna->DetermineAzElAngles(timeIndex,
                                        ptrTransmitLink->GetLineOfSightVector(timeIndex),
                                        additionalSeconds,
                                        azimuthAngle, elevationAngle, regionNumber,
                                        azimuthOffsetAngle);
    gimbalStopAngle = azimuthAngle - cableWrapOffsetAngle;

    if (gimbalStopAngle < -halfCircle) {
        gimbalStopAngle += (2.0 * halfCircle);
    }

    if (ptrUserAntenna->HasAzimuthBasedElevationAngles() == false) {
        // use the resource specific min/max elevation angles
        minElevationAngle = ptrUserAntenna->GetMinimumElevationAngle(receiveDesignator);
        maxElevationAngle = ptrUserAntenna->GetMaximumElevationAngle(receiveDesignator);
    } else {
        // use the azimuth based elevation angles
        ptrUserAntenna->GetAzimuthBasedElevationAngles(azimuthAngle,
                ptrTransmitLink->GetReceiveDesignator(),
                minElevationAngle, maxElevationAngle);
    }

    // there are three checks performed to see if there is a link to the resource when
    // a 'tilting' vehicle is pointing at a region (checks not performed during link
    // generation since the region pointing at is unknown at that time)
    //  1) maximum elevation (due to keyhole region)
    //  2) minimum elevation (due to obscuration)
    //  3) gimbal stop region

    if ( (elevationAngle < minElevationAngle) || (elevationAngle > maxElevationAngle) ||
            (ptrUserAntenna->OutsideOfGimbalStop(gimbalStopAngle) == true) ) {
        linkWhenPointingAtRegion = false;
    }

    return(linkWhenPointingAtRegion);
}

double
TimelineGenerator::FactorWhenPointingAtRegion(Link *ptrTransmitLink,
        const shared_ptr<UserVehicle> &ptrUserVehicle,
        Region *ptrRegion,
        int timeIndex,
        double regionScore,
        ofstream &valtLogFile,
        double azimuthOffsetAngle,
        double cableWrapOffsetAngle)
{
    return(0.0);
}

bool
TimelineGenerator::ValidTimeStep(const shared_ptr<UserVehicle> &ptrUserVehicle,
                                 int timeIndex,
                                 ofstream &valtLogFile)
{
    return(true);
}

bool
TimelineGenerator::ValidRegion(const shared_ptr<UserVehicle> &ptrUserVehicle,
                               Sensor *ptrSensor,
                               Region *ptrRegion,
                               int timeIndex,
                               ofstream &valtLogFile)
{
    return(true);
}

/* ********************************************************************************************* */

TimelineGenerator::~TimelineGenerator()
{
    return;
}
