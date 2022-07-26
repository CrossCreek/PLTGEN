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
 * MURALInputProcessor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <memory>

#include "MURALInputProcessor.h"
#include "FileNames.h"
#include "mural/Region.h"
#include "mural/Earth.h"
#include "OutputGenerator.h"
#include "extract/DataProviderExtractor.h"
#include "extract/MURALTimePieceExtractor.h"
#include "mural/element/UserVehicle.h"
#include "mural/element/GroundStation.h"
#include "mural/element/RelaySatellite.h"
#include "mural/element/DataProviderCollection.h"
#include "mural/io/parse/OPSStateVectorParser.h"
#include "mural/modules/pg/PositionGenerator.h"
#include "mural/modules/lg/LinksGenerator.h"
#include "mural/modules/vtg/CrisisData.h"
#include "mural/modules/vtg/Requirement.h"
#include "mural/modules/vtg/ValueTimelineGenerator.h"
#include "mural/modules/ts/TargetDeck.h"
#include "general/array/ListIterator.h"
#include "general/math/SimpleMath.h"
#include "general/utility/StringEditor.h"
#include "general/parse/ParseUtility.h"
#include "general/time/TimePiece.h"

// From geneng
#include "GenUtility.h"

bool                  MURALInputProcessor::jplHeaderDefined_s = false;
MURALInputProcessor::RUN_MODULE MURALInputProcessor::lastModuleToComplete_s = UNKNOWN_RUN_MODULE;

//------------------------------------------------------------------------------
//
//  Method Name: MURALInputProcessor::MURALInputProcessor
//
//  Purpose:
//    Default constructor for the MURALInputProcessor class.
//
//  Calling Method:
//     MURALInputProcessor();
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

MURALInputProcessor::MURALInputProcessor()
    : InputProcessor()
{
    throw new InputException(GetClassName(), "MURALInputProcessor()",
                             "Can NOT use default constructor");
}

//------------------------------------------------------------------------------
//
//  Method Name: MURALInputProcessor::MURALInputProcessor
//
//  Purpose:
//    Alternate constructor for the MURALInputProcessor class.
//
//  Calling Method:
//     MURALInputProcessor(const string&);
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

MURALInputProcessor::MURALInputProcessor(const string &mainInputFileName)
    : InputProcessor(mainInputFileName)
{
    if (ptrInputDataFileNameElement_s == NULL) {
        throw new InputException(GetClassName(), "MURALInputProcessor(const string*)",
                                 "Must have at least one Input Data File");
    }

    if (ptrInputDataElementList_s == NULL) {
        throw new InputException(GetClassName(), "MURALInputProcessor(const string*)",
                                 "Did not find any data in Input Data File(s)");
    }

    try {
        // need to extract active constellation and input parameters first
        ExtractInputRunVariables();
        // now set the rest of the static (miscellaneous) data
        ExtractGlobalRunVariables();

        if (ptrInputFileNameElement_s != NULL) {
            SetInputFileNames();
        }

        if (ptrOutputFileNameElement_s != NULL) {
            OutputGenerator::SetOutputFileNames(ptrOutputFileNameElement_s);
        }

        if (ptrDebugFileNameElement_s != NULL) {
            if ( ptrDebugFileNameElement_s->GetNumberOfInputData() > 0 ) {
                SetDebugFileNames();
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "MURALInputProcessor(const string*)");
        throw;
    }

    return;
}

MURALInputProcessor::MURALInputProcessor(const MURALInputProcessor &copyMe)
    : InputProcessor(copyMe)
{
    return;
}

MURALInputProcessor&
MURALInputProcessor::operator = (const MURALInputProcessor &copyMe)
{
    InputProcessor::operator = (copyMe);
    return(*this);
}

bool
MURALInputProcessor::operator == (const MURALInputProcessor &compareMe) const
{
    return(InputProcessor::operator == (compareMe));
}

void
MURALInputProcessor::PrintAttributes(ostream &outfile)
{
    outfile << "<INPUT_PARAMETERS_START>" << NEW_LINE;
    outfile << "   Last Module To Complete (PG/LPG/VTG) : ";

    if (lastModuleToComplete_s == POSITION_GENERATOR) {
        outfile << "PG" << NEW_LINE;
    } else if (lastModuleToComplete_s == LINK_PATH_GENERATOR) {
        outfile << "LPG" << NEW_LINE;
    } else {
        outfile << "VTG" << NEW_LINE;
    }

    outfile << "   Use Sub Regions For Requirements           : ";
    outfile << StringEditor::ConvertToString(Region::UsingSubRegions()) << NEW_LINE;
    outfile << "   Input Angle Units (DEG/RAD/GRAD)           : ";
    outfile << UnitData::GetAngleUnitString( UnitData::GetInputAngleUnits() ) << NEW_LINE;
    outfile << "   Input Distance Units (M/KM/FT/MI/NMI)      : ";
    outfile << UnitData::GetDistanceUnitString( UnitData::GetInputDistanceUnits() ) << NEW_LINE;
    outfile << "   Input Time Units (HR/MN/SEC/TS)            : ";
    outfile << UnitData::GetTimeUnitString( UnitData::GetInputTimeUnits() ) << NEW_LINE;
    outfile << "   Input Reference Frame (ECF/ECI/LLA)        : ";
    outfile << UnitData::GetReferenceFrameString( UnitData::GetInputReferenceFrame() ) << NEW_LINE;
    outfile << "<INPUT_PARAMETERS_END>" << NEW_LINE;
    return;
}

void
MURALInputProcessor::ExtractInputRunVariables()
{
    bool                            sortDataProviders = false;
    string                          errorString = "";
    shared_ptr<InputData>            ptrInputData = NULL;
    shared_ptr<InputDataElement>     ptrInputDataElement = NULL;
    InputDataElement::InputDataElementList::iterator inputDataElementIter;

    for( inputDataElementIter = ptrInputDataElementList_s->begin(); inputDataElementIter != ptrInputDataElementList_s->end(); ++inputDataElementIter) {
        ptrInputDataElement = (*inputDataElementIter);

        if (ptrInputDataElement->BlockTagContainsString("ACTIVE_CONSTELLATION") == true) {
            shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
            InputDataElement::InputDataList::iterator inputDataIter;

            for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
                ptrInputData = (*inputDataIter);

                if (ptrInputData->LabelContainsString("User Vehicles") == true) {
                    UserVehicle::SetIdentification(ptrInputData->GetValueArray());
                } else if (ptrInputData->LabelContainsString("Relay Satellites") == true) {
                    RelaySatellite::SetIdentification(ptrInputData->GetValueArray());
                } else if (ptrInputData->LabelContainsString("Ground Stations") == true) {
                    GroundStation::SetIdentification(ptrInputData->GetValueArray());
                } else if (ptrInputData->LabelContainsString("Sort Data Providers") == true) {
                    sortDataProviders = ptrInputData->GetBoolValue();
                }
            }
        } else if (ptrInputDataElement->BlockTagContainsString("INPUT_PARAMETERS") == true) {
            shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
            InputDataElement::InputDataList::iterator inputDataIter;

            for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
                ptrInputData = (*inputDataIter);

                if (ptrInputData->LabelContainsString("Last Module To Complete") == true) {
                    lastModuleToComplete_s = GetRunModule(ptrInputData->GetStringValue());
                } else if (ptrInputData->LabelContainsString("Use Sub Regions For Requirements") == true) {
                    Region::SetUsingSubRegions(ptrInputData->GetBoolValue());
                } else if (ptrInputData->LabelContainsString("Input Angle Units") == true) {
                    UnitData::SetInputAngleUnits( UnitData::GetAngleUnits(ptrInputData->GetStringValue()) );
                } else if (ptrInputData->LabelContainsString("Input Distance Units") == true) {
                    UnitData::SetInputDistanceUnits(UnitData::GetDistanceUnits(ptrInputData->GetStringValue()));
                } else if (ptrInputData->LabelContainsString("Input Time Units") == true) {
                    UnitData::SetInputTimeUnits(UnitData::GetTimeUnits(ptrInputData->GetStringValue()));
                } else if (ptrInputData->LabelContainsString("Input Reference Frame") == true) {
                    UnitData::SetInputReferenceFrame( UnitData::GetReferenceFrame(ptrInputData->GetStringValue()));
                }
            }
        }
    }

    if (UserVehicle::GetUserVehicleCount() == 0) {
        errorString += "Missing/invalid 'User Vehicles' input in ACTIVE_CONSTELLATION block";
    }

    if (GroundStation::GetGroundStationCount() == 0) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Ground Stations' input in ACTIVE_CONSTELLATION block";
    }

    if (lastModuleToComplete_s == UNKNOWN_RUN_MODULE) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Last Module To Complete' input in INPUT_PARAMETERS block";
    }

    if (UnitData::GetInputAngleUnits() == UNKNOWN_ANGLE_UNIT) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Input Angle Units' input in INPUT_PARAMETERS block";
    }

    if (UnitData::GetInputDistanceUnits() == UNKNOWN_DISTANCE_UNIT) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Input Distance Units' input in INPUT_PARAMETERS block";
    }

    if (UnitData::GetInputTimeUnits() == UNKNOWN_TIME_UNIT) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Input Time Units' input in INPUT_PARAMETERS block";
    }

    if (UnitData::GetInputReferenceFrame() == UNKNOWN_FRAME) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Input Reference Frame' input in INPUT_PARAMETERS block";
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "ExtractInputRunVariables()", errorString);
    }

    if(sortDataProviders == true) {
        UserVehicle::SortIdentificationArray();
        RelaySatellite::SortIdentificationArray();
        GroundStation::SortIdentificationArray();
    }

    return;
}

void
MURALInputProcessor::SetInputFileNames()
{
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputFileNameElement_s->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData> ptrInputData = NULL;

    try {
        for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
            ptrInputData = (*inputDataIter);

            if (ptrInputData->LabelContainsString("Position Input File") == true) {
                FileNames::SetFileName(FileNames::POSITION_INPUTFILE, ptrInputData->GetStringValue());
            } else if (ptrInputData->LabelContainsString("Position P470 Input File") == true) {
                FileNames::SetFileName(FileNames::POSITION_P470_INPUTFILE, ptrInputData->GetStringValue());
            } else if (ptrInputData->LabelContainsString("Crosslink Input File") == true) {
                if (RunLinkPathGenerator() == true) {
                    FileNames::SetFileName(FileNames::CROSSLINK_INPUTFILE, ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("Downlink Direct Input File") == true) {
                if (RunLinkPathGenerator() == true) {
                    FileNames::SetFileName(FileNames::DIRECTDOWNLINK_INPUTFILE, ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("Self Relaying Link Input File") == true) {
                if (RunLinkPathGenerator() == true) {
                    FileNames::SetFileName(FileNames::SELFRELAY_INPUTFILE, ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("Downlink Input File") == true) {
                if (RunLinkPathGenerator() == true) {
                    FileNames::SetFileName(FileNames::DOWNLINK_INPUTFILE, ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("Multihop Link Input File") == true) {
                if (RunLinkPathGenerator() == true) {
                    FileNames::SetFileName(FileNames::MULTIHOP_INPUTFILE, ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("UPA Conjunction Input File") == true) {
                if (RunLinkPathGenerator() == true) {
                    FileNames::SetFileName(FileNames::UPA_CONJ_INPUTFILE, ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("RF Conjunction Input File") == true) {
                if (RunLinkPathGenerator() == true) {
                    FileNames::SetFileName(FileNames::RF_CONJ_INPUTFILE, ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("Value Timeline Input File") == true) {
                if (RunTimelineGenerator() == true) {
                    FileNames::SetFileName(FileNames::TIMELINE_INPUTFILE, ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("OPS State Vector Input File") == true) {
                OPSStateVectorParser::InitStateVectorParser(ptrInputData->GetStringValue());
                OPSStateVectorParser::GetStateVectorParser()->ParseStateVectorFile();
            } else if (ptrInputData->LabelContainsString("JPL Supporting File Directory") == true) {
                string basePath = ptrInputData->GetValueString();
                string headerPath = basePath + "\\header.421e";
                string bin421Path = basePath + "\\bin.421";
                // Verify the JPL Header file (method throws exception if not)
                ParseUtility::VerifyInputFile(&headerPath);
                // Verify the JPL Bin file (method throws exception if not)
                ParseUtility::VerifyInputFile(&bin421Path);
                // If it exists then flag it and tell gen engineering about the path
                MURALInputProcessor::jplHeaderDefined_s = true;
                A3::addCustomPath( basePath );
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "SetInputFileNames()");
        throw;
    }

    return;
}

void
MURALInputProcessor::SetDebugFileNames()
{
    string                  fileName = "";
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrDebugFileNameElement_s->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>   ptrInputData = NULL;

    try {
        for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
            ptrInputData = (*inputDataIter);
            fileName = ptrInputData->GetStringValue();

            if (ptrInputData->LabelContainsString("Links Debug File") == true) {
                if (RunLinkPathGenerator() == true) {
                    FileNames::SetFileName(FileNames::LINKS_DEBUGFILE, ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("Conjunction Debug File") == true) {
                if (RunLinkPathGenerator() == true) {
                    FileNames::SetFileName(FileNames::CONJ_DEBUGFILE, ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("Value Timeline Debug File") == true) {
                if (RunTimelineGenerator() == true) {
                    FileNames::SetFileName(FileNames::VALT_DEBUGFILE, ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("FOR Region Inview Debug File") == true) {
                if (RunTimelineGenerator() == true) {
                    FileNames::SetFileName(FileNames::FOR_REGIONS_INVIEW_DEBUGFILE, fileName);
                }
			} else if (ptrInputData->LabelContainsString("FOR Azimuth Elevation Debug File") == true) {
				if (RunTimelineGenerator() == true) {
					FileNames::SetFileName(FileNames::FOR_AZIMUTH_ELEVATION_DEBUGFILE, fileName);
				}
			}
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "SetDebugFileNames()");
        throw;
    }

    return;
}

void
MURALInputProcessor::ExtractGlobalRunVariables()
{
    bool                            foundTimePieceInput = false;
    bool                            foundOutputParamater = false;
    bool                            foundAllocationInput = false;
    string                          errorString = "";
    shared_ptr<InputData>           ptrInputData = NULL;
    shared_ptr<InputDataElement>    ptrInputDataElement = NULL;
    InputDataElement::InputDataElementList::iterator inputDataElementIter;

    try {
        for( inputDataElementIter = ptrInputDataElementList_s->begin(); inputDataElementIter != ptrInputDataElementList_s->end(); ++inputDataElementIter) {
            ptrInputDataElement = (*inputDataElementIter);

            if (ptrInputDataElement->BlockTagContainsString("TIME_PIECE") == true) {
                // Parse time piece using the library's post processor
                MURALTimePieceExtractor timePieceExtractor;
                timePieceExtractor.ExtractTimePieceInfo(ptrInputDataElement);
                foundTimePieceInput = true;
            } else if (ptrInputDataElement->BlockTagContainsString("OUTPUT_PARAMETERS") == true) {
                OutputGenerator::ExtractOutputParameterInfo(ptrInputDataElement);
                foundOutputParamater = true;
            } else if (ptrInputDataElement->BlockTagContainsString("TARGET_PARAMETERS") == true) {
				if (RunTimelineGenerator() == true) {
					TargetDeck::ExtractTargetDeckInfo(ptrInputDataElement, false);
				}
			} else if (ptrInputDataElement->BlockTagContainsString("LINK_GENERATION") == true) {
                LinksGenerator::ExtractLinksGeneratorInfo(ptrInputDataElement);
            } else if (ptrInputDataElement->BlockTagContainsString("CRISIS_INPUT") == true) {
                if (RunTimelineGenerator() == true) {
                    CrisisData::ExtractCrisisDataInfo(ptrInputDataElement);
                }
            } 
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractGlobalRunVariables()");
        throw;
    }

    if (CreateEarth() == false) {
        errorString = "Missing REGION_INPUT block";
    }

    if (foundTimePieceInput == false) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing TIME_PIECE block";
    }

    if (foundOutputParamater == false) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing OUTPUT_PARAMETER block";
    }


    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "ExtractGlobalRunVariables()",
                                 errorString);
    }

    return;
}

bool
MURALInputProcessor::CreateEarth()
{
    bool                            foundRegionInput = false;
    InputDataElement::InputDataElementList::iterator inputDataElementIter;
    shared_ptr<InputDataElement>    ptrInputDataElement = NULL;

    for( inputDataElementIter = ptrInputDataElementList_s->begin(); (inputDataElementIter != ptrInputDataElementList_s->end() && foundRegionInput == false); ++inputDataElementIter) {
        ptrInputDataElement = (*inputDataElementIter);

        if (ptrInputDataElement->BlockTagContainsString("REGION_INPUT") == true) {
            Earth::CreateRegions(ptrInputDataElement);
            foundRegionInput = true;
        }
    }

    return(foundRegionInput);
}

MURALInputProcessor::RUN_MODULE
MURALInputProcessor::GetRunModule(const string &valueString)
{
    RUN_MODULE runModule = UNKNOWN_RUN_MODULE;

    if (valueString == "PG") {
        runModule = POSITION_GENERATOR;
    } else if (valueString == "LPG") {
        runModule = LINK_PATH_GENERATOR;
    } else if (valueString == "VTG") {
        runModule = TIMELINE_GENERATOR;
    } 

    return(runModule);
}

shared_ptr<DataProviderCollection>
MURALInputProcessor::CreateDataProviderList()
{
    const int                                        numberOfGroundStations = GroundStation::GetGroundStationCount();
    const int                                        numberOfRelays = RelaySatellite::GetRelaySatelliteCount();
    const int                                        numberOfUsers = UserVehicle::GetUserVehicleCount();
    bool                                             foundCrosslinkUser = false;
    string                                           designator = "";
    string                                           trailVehicleDesignator = "";
    string                                           errorString = "";
    Array<string>                                   *ptrTrailDesignatorArray = NULL;
    shared_ptr<GroundStation>                        ptrGroundStation = NULL;
    shared_ptr<RelaySatellite>                       ptrRelaySatellite = NULL;
    shared_ptr<UserVehicle>                          ptrUserVehicle = NULL;
    shared_ptr<UserVehicle>                          ptrTrailVehicle = NULL;
    shared_ptr<DataProviderCollection>               ptrDataProviderCollection = NULL;
    shared_ptr<MURALTypes::UserVector>               ptrTrailUserVehicleList = NULL;
    shared_ptr<InputDataElement>                     ptrInputDataElement = NULL;
    InputDataElement::InputDataElementList::iterator inputDataElementIter;
    // Create the extractor class to extract and check the data providers
    DataProviderExtractor dataProviderExtractor;
    // Create the collection object to store the data providers
    ptrDataProviderCollection = shared_ptr<DataProviderCollection>(new DataProviderCollection());

    try {
        int assetIndex = 0;
        bool foundAsset = false;
        bool isTrailVehicle = true;
        ptrTrailDesignatorArray = CheckAllActiveAssetsExist();
        assetIndex = 0;

        while (assetIndex < numberOfGroundStations) {
            foundAsset = false;
            designator = GroundStation::GetDesignatorString(assetIndex);

            for( inputDataElementIter = ptrInputDataElementList_s->begin(); (inputDataElementIter != ptrInputDataElementList_s->end() && foundAsset == false); ++inputDataElementIter) {
                ptrInputDataElement = (*inputDataElementIter);

                if (ptrInputDataElement->BlockTagContainsString("GROUND_STATION_OBJECT") == true) {
                    if (FindDesignatorString(ptrInputDataElement) == designator) {
                        foundAsset = true;
                        // Create the ground station
                        ptrGroundStation = shared_ptr<GroundStation>(new GroundStation());
                        dataProviderExtractor.ExtractGroundStationInfo(ptrGroundStation, ptrInputDataElement);
                        dataProviderExtractor.CheckGroundStationInputs(ptrGroundStation);
                        // append it to the collection
                        ptrDataProviderCollection->AppendGround(ptrGroundStation);
                        cout << "MURALInputProcessor: " << designator << " found" << NEW_LINE;
                        ptrGroundStation = NULL;
                    }
                }
            }

            ++assetIndex;
        }

        assetIndex = 0;

        while (assetIndex < numberOfRelays) {
            foundAsset = false;
            designator = RelaySatellite::GetDesignatorString(assetIndex);

            for( inputDataElementIter = ptrInputDataElementList_s->begin(); (inputDataElementIter != ptrInputDataElementList_s->end() && foundAsset == false); ++inputDataElementIter) {
                ptrInputDataElement = (*inputDataElementIter);

                if (ptrInputDataElement->BlockTagContainsString("RELAY_SATELLITE_OBJECT") == true) {
                    if (FindDesignatorString(ptrInputDataElement) == designator) {
                        foundAsset = true;
                        // create the relay satellite
                        ptrRelaySatellite = shared_ptr<RelaySatellite>(new RelaySatellite());
                        dataProviderExtractor.ExtractRelaySatelliteInfo(ptrRelaySatellite, ptrInputDataElement);
                        dataProviderExtractor.CheckRelaySatelliteInputs(ptrRelaySatellite);
                        // append it to the collection
                        ptrDataProviderCollection->AppendRelay(ptrRelaySatellite);
                        cout << "MURALInputProcessor: " << designator << " found" << NEW_LINE;
                        ptrRelaySatellite = NULL;
                    }
                }
            }

            ++assetIndex;
        }

        // create trail user vehicles first (if any)
        assetIndex = 0;

        while (assetIndex < ptrTrailDesignatorArray->Size()) {
            foundAsset = false;
            trailVehicleDesignator = ptrTrailDesignatorArray->ElementAt(assetIndex);

            for( inputDataElementIter = ptrInputDataElementList_s->begin(); (inputDataElementIter != ptrInputDataElementList_s->end() && foundAsset == false); ++inputDataElementIter) {
                ptrInputDataElement = (*inputDataElementIter);

                if (ptrInputDataElement->BlockTagContainsString("USER_VEHICLE_OBJECT") == true) {
                    if (FindDesignatorString(ptrInputDataElement) == trailVehicleDesignator) {
                        foundAsset = true;
                        // create the trail vehicle
                        ptrTrailVehicle = shared_ptr<UserVehicle>(new UserVehicle());
                        dataProviderExtractor.ExtractUserVehicleInfo(ptrTrailVehicle, ptrInputDataElement, isTrailVehicle);
                        dataProviderExtractor.CheckUserVehicleInputs(ptrTrailVehicle, isTrailVehicle);
                        cout << "MURALInputProcessor: " << trailVehicleDesignator << " (trail) found" << NEW_LINE;

                        // append it to the trail vector
                        if (ptrTrailUserVehicleList == NULL) {
                            ptrTrailUserVehicleList = shared_ptr<MURALTypes::UserVector>(new MURALTypes::UserVector());
                        }

                        ptrTrailUserVehicleList->push_back(ptrTrailVehicle);
                        ptrTrailVehicle = NULL;
                    }
                }
            }

            ++assetIndex;
        }

        // now create individual and/or lead user vehicles
        isTrailVehicle = false;
        assetIndex = 0;

        while (assetIndex < numberOfUsers) {
            foundAsset = false;
            designator = UserVehicle::GetDesignatorString(assetIndex);

            if (ptrTrailDesignatorArray->ValueExists(designator) == false) {
                for( inputDataElementIter = ptrInputDataElementList_s->begin(); (inputDataElementIter != ptrInputDataElementList_s->end() && foundAsset == false); ++inputDataElementIter) {
                    ptrInputDataElement = (*inputDataElementIter);

                    if (ptrInputDataElement->BlockTagContainsString("USER_VEHICLE_OBJECT") == true) {
                        if (FindDesignatorString(ptrInputDataElement) == designator) {
                            foundAsset = true;
                            trailVehicleDesignator = FindTrailVehicleDesignatorString(ptrInputDataElement);

                            if (trailVehicleDesignator.size() > 0) {
                                ptrTrailVehicle = FindUserVehicle(ptrTrailUserVehicleList, trailVehicleDesignator);
                            }

                            // create the user vehicle
                            ptrUserVehicle = shared_ptr<UserVehicle>(new UserVehicle(ptrTrailVehicle));
                            dataProviderExtractor.ExtractUserVehicleInfo(ptrUserVehicle, ptrInputDataElement, isTrailVehicle);
                            dataProviderExtractor.CheckUserVehicleInputs(ptrUserVehicle, isTrailVehicle);
                            // append it to the collection
                            ptrDataProviderCollection->AppendUser(ptrUserVehicle);
                            cout << "MURALInputProcessor: " << designator << " found" << NEW_LINE;

                            if (ptrUserVehicle->IsADirectDownlinkVehicle() == false) {
                                foundCrosslinkUser = true;
                            }

                            ptrUserVehicle = NULL;
                            ptrTrailVehicle = NULL;
                        }
                    }
                }
            }

            ++assetIndex;
        }

        delete ptrTrailDesignatorArray;
        ptrTrailDesignatorArray = NULL;

    } catch (Exception *ptrError) {
        if (ptrTrailDesignatorArray != NULL) {
            delete ptrTrailDesignatorArray;
            ptrTrailDesignatorArray = NULL;
        }

        ptrError->AddMethod(GetClassName(), "CreateDataProviderList()");
        throw;
    }

    if ((RelaySatellite::GetRelaySatelliteCount() == 0) && (foundCrosslinkUser == true)) {
        errorString = "Need at least one relay satellite for crosslink user(s)";
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "CreateDataProviderList()", errorString);
    }

    return(ptrDataProviderCollection);
}

string
MURALInputProcessor::FindDesignatorString(shared_ptr<InputDataElement> ptrInputDataElement) const
{
    string                  designator = "";
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData> ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); (inputDataIter != inputDataList->end()) && (designator.size() == 0); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Data Provider Designator") == true) {
            designator = ptrInputData->GetStringValue();
        }
    }

    return(designator);
}

string
MURALInputProcessor::FindTrailVehicleDesignatorString(shared_ptr<InputDataElement> ptrInputDataElement) const
{
    string                  trailVehicleDesignator = "";
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>    ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); (inputDataIter != inputDataList->end()) && (trailVehicleDesignator.size() == 0); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Trail Vehicle Designator") == true) {
            trailVehicleDesignator = ptrInputData->GetStringValue();
        }
    }

    return(trailVehicleDesignator);
}

Array<string>*
MURALInputProcessor::CheckAllActiveAssetsExist()
{
    int              index = 0;
    string            designator = "";
    string            trailVehicleDesignator = "";
    string            errorString = "";
    Array<string>    *ptrTrailDesignatorArray = new Array<string>();
    Array<string>    *ptrUserArray = new Array<string>(*(UserVehicle::GetIdentificationArray()));
    Array<string>    *ptrRelayArray = NULL;
    Array<string>    *ptrGroundArray = new Array<string>(*(GroundStation::GetIdentificationArray()));
    std::vector<shared_ptr<InputDataElement>> retainInputDataElementList;
    InputDataElement::InputDataElementList::iterator inputDataElementIter;
    shared_ptr<InputDataElement>    ptrInputDataElement = NULL;

    /*
    InputDataElement::InputDataElementList::iterator inputDataElementIter;
    shared_ptr<InputDataElement>    ptrInputDataElement = NULL;

    for( inputDataElementIter = ptrInputDataElementList_s->begin();inputDataElementIter != ptrInputDataElementList_s->end();++inputDataElementIter)
    {
      ptrInputDataElement = (*inputDataElementIter);
    */

    if (RelaySatellite::GetIdentificationArray() != NULL) {
        ptrRelayArray = new Array<string>(*(RelaySatellite::GetIdentificationArray()));
    } else {
        ptrRelayArray = new Array<string>();
    }

    for( inputDataElementIter = ptrInputDataElementList_s->begin(); inputDataElementIter != ptrInputDataElementList_s->end(); ++inputDataElementIter) {
        ptrInputDataElement = (*inputDataElementIter);

        if (ptrInputDataElement->BlockTagContainsString("GROUND_STATION_OBJECT") == true) {
            designator = FindDesignatorString(ptrInputDataElement);

            if (GroundStation::ValidGroundStation(designator) == true) {
                retainInputDataElementList.push_back(ptrInputDataElement);
                ptrGroundArray->RemoveElement(designator);
            }
        } else if (ptrInputDataElement->BlockTagContainsString("RELAY_SATELLITE_OBJECT") == true) {
            designator = FindDesignatorString(ptrInputDataElement);

            if (RelaySatellite::ValidRelaySatellite(designator) == true) {
                retainInputDataElementList.push_back(ptrInputDataElement);
                ptrRelayArray->RemoveElement(designator);
            }
        } else if (ptrInputDataElement->BlockTagContainsString("USER_VEHICLE_OBJECT") == true) {
            designator = FindDesignatorString(ptrInputDataElement);

            if (UserVehicle::ValidUserVehicle(designator) == true) {
                retainInputDataElementList.push_back(ptrInputDataElement);
                ptrUserArray->RemoveElement(designator);
                trailVehicleDesignator = FindTrailVehicleDesignatorString(ptrInputDataElement);

                if (trailVehicleDesignator.size() > 0) {
                    ptrTrailDesignatorArray->AddElement(trailVehicleDesignator);
                }
            }
        } else {
            retainInputDataElementList.push_back(ptrInputDataElement);
        }
    }

    // add the retained data providers to the input data element list
    ptrInputDataElementList_s->clear();
    std::vector<shared_ptr<InputDataElement>>::iterator retainedIter;

    for ( retainedIter = retainInputDataElementList.begin(); retainedIter != retainInputDataElementList.end(); ++retainedIter) {
        ptrInputDataElementList_s->push_back(*retainedIter);
    }

    if (ptrUserArray->Size() > 0) {
        index = 0;
        errorString = ("Didn't find User Vehicle(s)   : " + ptrUserArray->ElementAt(index));
        ++index;

        while (index < ptrUserArray->Size()) {
            errorString += ", ";
            errorString += ptrUserArray->ElementAt(index);
            ++index;
        }
    }

    if (ptrRelayArray->Size() > 0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        index = 0;
        errorString += ("Didn't find Relay Satellite(s): " + ptrRelayArray->ElementAt(index));
        ++index;

        while (index < ptrRelayArray->Size()) {
            errorString += ", ";
            errorString += ptrRelayArray->ElementAt(index);
            ++index;
        }
    }

    if (ptrGroundArray->Size() > 0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        index = 0;
        errorString += ("Didn't find Ground Station(s) : " + ptrGroundArray->ElementAt(index));
        ++index;

        while (index < ptrUserArray->Size()) {
            errorString += ", ";
            errorString += ptrGroundArray->ElementAt(index);
            ++index;
        }
    }

    index = 0;

    while (index < ptrTrailDesignatorArray->Size()) {
        trailVehicleDesignator = ptrTrailDesignatorArray->ElementAt(index);

        if (UserVehicle::ValidUserVehicle(trailVehicleDesignator) == false) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += ("Trail vehicle " +  trailVehicleDesignator + " not in active constellation");
        }

        ++index;
    }

    delete ptrUserArray;
    ptrUserArray = NULL;
    delete ptrRelayArray;
    ptrRelayArray = NULL;
    delete ptrGroundArray;
    ptrGroundArray = NULL;

    if (errorString.size() > 0) {
        delete ptrTrailDesignatorArray;
        ptrTrailDesignatorArray = NULL;
        throw new InputException(GetClassName(), "CheckAllActiveAssetsExist()", errorString);
    }

    return(ptrTrailDesignatorArray);
}

shared_ptr<UserVehicle>
MURALInputProcessor::FindUserVehicle(const shared_ptr<MURALTypes::UserVector> &ptrUserVehicleList,
                                     const string &userDesignator) const
{
    bool                      foundUser = false;
    shared_ptr<UserVehicle>              ptrUserVehicle = NULL;
    MURALTypes::UserVector::iterator userIter;

    for( userIter = ptrUserVehicleList->begin(); ((userIter != ptrUserVehicleList->end()) && (foundUser == false) ); ++userIter) {
        ptrUserVehicle = (*userIter);

        if (ptrUserVehicle->GetDesignator() == userDesignator) {
            foundUser = true;
        }
    }

    return(ptrUserVehicle);
}

bool
MURALInputProcessor::FindValidDataProvider(const shared_ptr<InputData> ptrInputData,
        string &providerDesignator)
{
    bool validProvider = false;

    if (ptrInputData->GetSpecificType(providerDesignator) == true) {
        validProvider = ValidDataProvider(providerDesignator);
    }

    return(validProvider);
}

bool
MURALInputProcessor::FindValidDataProviders(const shared_ptr<InputData> ptrInputData,
        string &providerDesignator1,
        string &providerDesignator2)
{
    bool validProviders = false;

    if (ptrInputData->GetSpecificType(providerDesignator1, providerDesignator2) == true) {
        if ( (ValidDataProvider(providerDesignator1) == true) &&
                (ValidDataProvider(providerDesignator2) == true) ) {
            validProviders = true;
        }
    }

    return(validProviders);
}

bool
MURALInputProcessor::ValidGroundStation(const string &groundDesignator)
{
    return(GroundStation::ValidGroundStation(groundDesignator));
}

bool
MURALInputProcessor::ValidRelaySatellite(const string &relayDesignator)
{
    return(RelaySatellite::ValidRelaySatellite(relayDesignator));
}

bool
MURALInputProcessor::ValidUserVehicle(const string &userDesignator)
{
    return(UserVehicle::ValidUserVehicle(userDesignator));
}

bool
MURALInputProcessor::ValidTransmitter(const string &transmitterDesignator)
{
    bool validTransmitter = false;
    validTransmitter = UserVehicle::ValidUserVehicle(transmitterDesignator);

    if (validTransmitter == false) {
        validTransmitter = RelaySatellite::ValidRelaySatellite(transmitterDesignator);
    }

    return(validTransmitter);
}

bool
MURALInputProcessor::ValidReceiver(const string &receiverDesignator)
{
    bool validReceiver = false;
    validReceiver = RelaySatellite::ValidRelaySatellite(receiverDesignator);

    if (validReceiver == false) {
        validReceiver = GroundStation::ValidGroundStation(receiverDesignator);
    }

    return(validReceiver);
}

bool
MURALInputProcessor::ValidDataProvider(const string &providerDesignator)
{
    bool validProvider = false;
    validProvider = UserVehicle::ValidUserVehicle(providerDesignator);

    if (validProvider == false) {
        validProvider = RelaySatellite::ValidRelaySatellite(providerDesignator);

        if (validProvider == false) {
            validProvider = GroundStation::ValidGroundStation(providerDesignator);
        }
    }

    return(validProvider);
}

MURALInputProcessor::~MURALInputProcessor()
{
    Earth::DestroyRegions();

    if (RunTimelineGenerator() == true) {
        TargetDeck::DestroyTargetDeck();
        CrisisData::DestroyCrisisData();
    }

    return;
}
