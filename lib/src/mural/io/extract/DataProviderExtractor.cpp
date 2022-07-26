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
 * DataProviderExtractor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "DataProviderExtractor.h"
#include "AntennaExtractor.h"

#include "mural/io/FileNames.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/io/extract/JourneyExtractor.h"
#include "mural/coorframe/ElementCoordinateFrame.h"
#include "mural/element/DataProvider.h"
#include "mural/element/UserVehicle.h"
#include "mural/element/RelaySatellite.h"
#include "mural/element/GroundStation.h"
#include "mural/sensor/Sensor.h"
#include "mural/sensor/VisibleSensor.h"
#include "mural/sensor/LongWaveSensor.h"
#include "mural/sensor/SARSensor.h"
#include "mural/sensor/SSARSensor.h"
#include "mural/sensor/WideFOVSensor.h"
#include "mural/sensor/SpectralSensor.h"
#include "mural/antenna/ReceiveFacilityAntenna.h"
#include "mural/antenna/SelfRelayAntenna.h"
#include "mural/antenna/UserTransmitAntenna.h"
#include "mural/antenna/DirectDownlinkAntenna.h"
#include "mural/antenna/UserPointingAntenna.h"
#include "mural/antenna/RelayPointingAntenna.h"
#include "mural/antenna/GroundPointingAntenna.h"
#include "mural/modules/lg/LinkCriteria.h"
#include "mural/modules/vtg/TimelineCriteria.h"
#include "mural/modules/ts/TargetDeck.h"

#include "general/array/Array.h"
#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"
#include "general/utility/StringEditor.h"
#include "general/math/SimpleMath.h"
//RAGUSA 12/3/2018 - Add Earth Header
#include "mural/Earth.h"

DataProviderExtractor::DataProviderExtractor()
{
    // Init the antenna extractor (to extract the antennas!)
    ptrAntennaExtractor_a = unique_ptr<AntennaExtractor>(new AntennaExtractor());
    return;
}

/* ********************************************************************************************* */

void
DataProviderExtractor::ExtractDataProviderInfo(const shared_ptr<DataProvider> &ptrDataProvider,
        const shared_ptr<InputDataElement> &ptrInputDataElement,
        const bool &dataTransmitter,
        const bool &dataCollector)
{
    string                   errorString = "";
    const bool               runLinks = MURALInputProcessor::RunLinkPathGenerator();
    Sensor::SENSOR_TYPE      sensorType= Sensor::UNKNOWN;
    Sensor                  *ptrSensor = NULL;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    shared_ptr<InputDataElement::InputDataElementList> subElementList = ptrInputDataElement->GetInputDataElementList();
    InputDataElement::InputDataList::iterator inputDataIter;
    InputDataElement::InputDataElementList::iterator subElementIter;
    shared_ptr<InputData>             ptrInputData = NULL;
    shared_ptr<InputDataElement>      ptrSubElement = NULL;
    shared_ptr<string> ptrDesignator = NULL;
    shared_ptr<string> ptrOPSDesignator = NULL;
    shared_ptr<string> ptrType = NULL; 
    shared_ptr<string> ptrSystem = NULL;
    shared_ptr<Journey> ptrJourney = NULL;
    LinkList<Sensor> *ptrSensorList = NULL;

    try {
        for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
            ptrInputData = (*inputDataIter);

            if (ptrInputData->LabelContainsString("Data Provider Designator") == true) {
                ptrDesignator = shared_ptr<string>(new string(ptrInputData->GetStringValue()));
                ptrDataProvider->SetDesignator(ptrDesignator);
            } else  if (ptrInputData->LabelContainsString("Operational Designator") == true) {
                ptrOPSDesignator = shared_ptr<string>(new string(ptrInputData->GetStringValue()));
            } else if (ptrInputData->LabelContainsString("Data Provider Type") == true) {
                ptrType = shared_ptr<string>(new string(ptrInputData->GetStringValue()));
            } else if (ptrInputData->LabelContainsString("Data Provider System") == true) {
                ptrSystem = shared_ptr<string>(new string(ptrInputData->GetStringValue()));
            }
        }

        // Default the OPS designator to the standard designator if it is not defined
        if ( ptrOPSDesignator == NULL ) {
            ptrOPSDesignator = shared_ptr<string>(new string(*ptrDesignator));
        }

        for( subElementIter = subElementList->begin(); (subElementIter != subElementList->end()) && (errorString.size() == 0); ++subElementIter) {
            ptrSubElement = (*subElementIter);

            if (ptrSubElement->BlockTagContainsString("SENSOR_OBJECT") == true) {
                sensorType = Sensor::GetSensorType(ptrSubElement);

                if (sensorType == Sensor::UNKNOWN) {
                    errorString = "Missing/invalid 'Sensor Type' input for SENSOR_OBJECT block";
                } else if (sensorType == Sensor::VISIBLE) {
                    ptrSensor = new VisibleSensor(ptrSubElement, sensorType);
                } else if (sensorType == Sensor::SPECIAL_VISIBLE) {
                    ptrSensor = new VisibleSensor(ptrSubElement, sensorType);
                } else if (sensorType == Sensor::LONG_WAVE) {
                    ptrSensor = new LongWaveSensor(ptrSubElement, sensorType);
                } else if (sensorType == Sensor::SAR) {
                    ptrSensor = new SARSensor(ptrSubElement, sensorType);
                    //RAGUSA 11/2/17 Updated to include SAR sensor
                } else if (sensorType == Sensor::SSAR) {
                    ptrSensor = new SSARSensor(ptrSubElement, sensorType);
                } else if (sensorType == Sensor::WIDE_FOV) {
                    ptrSensor = new WideFOVSensor(ptrSubElement, sensorType);
                } else if ( (sensorType == Sensor::HS_REFLECTIVE) ||
                            (sensorType == Sensor::HS_EMISSIVE) ||
                            (sensorType == Sensor::MS_REFLECTIVE) ||
                            (sensorType == Sensor::MS_EMISSIVE) ||
                            (sensorType == Sensor::US_REFLECTIVE) ||
                            (sensorType == Sensor::US_EMISSIVE) ) {
                    ptrSensor = new SpectralSensor(ptrSubElement, sensorType);
                }

                if (ptrSensor != NULL) {
                    if (ptrSensorList == NULL) {
                        ptrSensorList = new LinkList<Sensor>();
                    }

                    ptrSensorList->Append(ptrSensor);
                    ptrSensor = NULL;
                }
            } else if (ptrSubElement->BlockTagContainsString("POSITION_GENERATION") == true) {
                JourneyExtractor journeyExtractor(*ptrOPSDesignator, dataCollector);
                ptrJourney = journeyExtractor.ExtractJourney(ptrSubElement);
            } else if (ptrSubElement->BlockTagContainsString("LINK_GENERATION") == true) {
                if (runLinks == true) {
                    shared_ptr<LinkCriteria> ptrLinkCriteria = shared_ptr<LinkCriteria>(new LinkCriteria(ptrSubElement, dataTransmitter));
                    ptrDataProvider->SetLinkCriteria(ptrLinkCriteria);
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractDataProviderInfo(InputDataElement*)");
        throw;
    }

    if (errorString.size() == 0) {
        if (ptrDesignator == NULL) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString = "Must have Data Provider Designator";
        }

        if (ptrType == NULL) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString = "Must have Data Provider Type";
        }

        if (ptrSystem == NULL) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString = "Must have Data Provider System";
        }

        if (ptrJourney == NULL) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString = "Missing POSITION_GENERATION block";
        }
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractDataProviderInfo(InputDataElement*, const bool&)",
                                 errorString);
    }

    // Set the data on the data provider
    ptrDataProvider->SetType(ptrType);
    ptrDataProvider->SetSystem(ptrSystem);
    ptrDataProvider->SetOPSDesignator(ptrOPSDesignator);
    ptrDataProvider->SetJourney(ptrJourney);
    ptrDataProvider->SetSensorList(ptrSensorList);
    return;
}

/* ************************************************************************************ */

void
DataProviderExtractor::ExtractGroundStationInfo(const shared_ptr<GroundStation> &ptrGround,
        const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    Array<string>                            *ptrPooledEquipmentUsers = NULL;
    shared_ptr<ReceiveFacilityAntenna>        ptrRF = NULL;
    shared_ptr<LinkCriteria>                  ptrLinkCriteria = ptrGround->GetLinkCriteria();
    shared_ptr<InputDataElement::InputDataElementList> subElementList = ptrInputDataElement->GetInputDataElementList();
    InputDataElement::InputDataElementList::iterator subElementIter;
    shared_ptr<InputDataElement>       ptrSubElement = NULL;
    shared_ptr<MURALTypes::RFAVector>    ptrRFList = shared_ptr<MURALTypes::RFAVector>(new MURALTypes::RFAVector());;
    // extract the data provider info first
    ExtractDataProviderInfo(ptrGround, ptrInputDataElement, false, false);

    try {
        int previousCapacityRF = 0;
        int conjunctionCapacityRF = 0;

        for( subElementIter = subElementList->begin(); subElementIter != subElementList->end(); ++subElementIter) {
            ptrSubElement = (*subElementIter);

            if (ptrSubElement->BlockTagContainsString("RECEIVE_FACILITY_ANTENNA") == true) {
                ptrRF = shared_ptr<ReceiveFacilityAntenna>(new ReceiveFacilityAntenna(ptrGround, previousCapacityRF));
                // Extract the RF using the antenna extractor
                ptrAntennaExtractor_a->ExtractRF(ptrGround, ptrRF, ptrSubElement);
                int currentRFCapacity = ptrRF->GetCapacity();
                // set previous capacity for next iteration
                previousCapacityRF += currentRFCapacity;

                // add current rf capacity to the conjunction counter
                if (ptrRF->IsConjunctionAntenna() == true) {
                    conjunctionCapacityRF += currentRFCapacity;
                }

                // add the capacity to the static total count of rf capacity
                ptrRF->AddRFCountToTotal(currentRFCapacity);
                // Add the RF to the ground station
                ptrRFList->push_back(ptrRF);
            }
        }

        if (ptrLinkCriteria != NULL) {
            ptrLinkCriteria->RemoveConjunctionData(conjunctionCapacityRF);

            if (ptrLinkCriteria->ReadConjunctionData() == true) {
                FileNames::VerifyFile(FileNames::RF_CONJ_INPUTFILE);
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "ExtractGroundStationInfo(InputDataElement*)");
        throw;
    }

    // Set up the coordinate frame on the Base Element Class (replaces the hard coded method)
    ptrGround->SetCoordinateFrame( shared_ptr<ElementCoordinateFrame>(new ElementCoordinateFrame(ElementCoordinateFrame::GROUND_FRAME) ) );
    // Set the data on the ground station
    ptrGround->SetRFList(ptrRFList);

    return;
}

/* ********************************************************************************************* */

void
DataProviderExtractor::ExtractUserVehicleInfo(const shared_ptr<UserVehicle> &ptrUser,
        const shared_ptr<InputDataElement> &ptrInputDataElement,
        const bool &isTrailVehicle)
{
    int               rev = 0;
    int               numberOfRevs = 0;
    int               element = 0;
    int               numberOfElements = 0;
    double            azimuthAngle = 0.0;
    bool              runTimeline = false;
    bool              runRequests = false;
    bool              runAllocator = false;
    bool              runScheduler = false;
    bool              bodyTiltsForCollection = false;
    Sensor                            *ptrSensor = NULL;
    LinkList<Sensor>                  *ptrSensorList = ptrUser->GetSensorList();
    shared_ptr<SelfRelayAntenna>       ptrSRA = NULL;
    shared_ptr<MURALTypes::SRAVector>  ptrSRAList = shared_ptr<MURALTypes::SRAVector>(new MURALTypes::SRAVector());;
    Array<int>                        *ptrRollingRevDutyCycle = NULL;
    Array<double>                     *ptrAzimuthOffsetAngles = NULL;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    shared_ptr<InputDataElement::InputDataElementList> subElementList = ptrInputDataElement->GetInputDataElementList();
    InputDataElement::InputDataList::iterator inputDataIter;
    InputDataElement::InputDataElementList::iterator subElementIter;
    shared_ptr<InputData>              ptrInputData = NULL;
    shared_ptr<InputDataElement>       ptrSubElement = NULL;
    // extract the data provider info first
    ExtractDataProviderInfo(ptrUser, ptrInputDataElement, true, true);

    if (isTrailVehicle == false) {
        runTimeline = MURALInputProcessor::RunTimelineGenerator();
    }
    // Trail vehicles do not have sensors
    else if (ptrSensorList != NULL) {
        ptrSensorList->Delete();
        delete ptrSensorList;
        ptrSensorList = NULL;
    }

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Data Provider Group") == true) {
            ptrUser->SetGroupName( shared_ptr<string>(new string(ptrInputData->GetStringValue())) );
        } else if (ptrInputData->LabelContainsString("Body Tilts For Collection") == true) {
            bodyTiltsForCollection = ptrInputData->GetBoolValue() ;
            ptrUser->SetBodyTiltsForCollection( bodyTiltsForCollection );
        } else if (ptrInputData->LabelContainsString("Target Deck File Name") == true) {
            ptrUser->SetTargetDeckFileName( shared_ptr<string>(new string(ptrInputData->GetStringValue())) );
		//RAGUSA 11/27/2018 - Add Input for User Specific Service Tier Definition
        }else if (ptrInputData->LabelContainsString("Service Tier Definition") == true) {
			ptrUser->SetServiceTierDefinition(ptrInputData->GetIntValue());
		}
		//=======================================================================
		else if (ptrInputData->LabelContainsString("Daily Vehicle Duty Cycle") == true) {
            ptrUser->SetDailyVehicleDutyCycle( UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue()) );
        } else if (ptrInputData->LabelContainsString("Consecutive Rev Duty Cycle") == true) {
            if (ptrInputData->GetSpecificIntNumber(numberOfRevs) == true) {
                if (ptrRollingRevDutyCycle == NULL) {
                    ptrRollingRevDutyCycle = new Array<int>(numberOfRevs, LONG_ZERO);
                } else if (numberOfRevs > ptrRollingRevDutyCycle->Size()) {
                    rev = (int)ptrRollingRevDutyCycle->Size();

                    while (rev < numberOfRevs) {
                        ptrRollingRevDutyCycle->AddElement(LONG_ZERO);
                        ++rev;
                    }
                }

                (*ptrRollingRevDutyCycle)[numberOfRevs - 1]
                    = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
            }
        } else if (ptrInputData->LabelContainsString("Collection Efficiency") == true) {
            ptrUser->SetCollectionEfficiency( ptrInputData->GetDoubleValue() );
        } else if (ptrInputData->LabelContainsString("Buffer Memory Size") == true) {
            ptrUser->SetBufferMemorySize( ptrInputData->GetDoubleValue() );
        } else if (ptrInputData->LabelContainsString("Effective Buffer Empty Rate") == true) {
            ptrUser->SetEffectiveBufferEmptyRate( ptrInputData->GetDoubleValue() );
        } else if (ptrInputData->LabelContainsString("Azimuth Offset Angles") == true) {
            if (ptrAzimuthOffsetAngles == NULL) {
                ptrAzimuthOffsetAngles = new Array<double>();
            }

            element = 0;
            numberOfElements = ptrInputData->GetNumberOfValues();

            while (element < numberOfElements) {
                azimuthAngle = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue(element));
                ptrAzimuthOffsetAngles->AddUniqueElement(azimuthAngle);
                ++element;
            }
        } else if (ptrInputData->LabelContainsString("Vehicle Priority") == true) {
            ptrUser->SetVehiclePriority( ptrInputData->GetIntValue() );
        } else if (ptrInputData->LabelContainsString("Minimum Processing Revisit Time") == true) {
            ptrUser->SetMinimumProcessingRevisitTime(
                (int)UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue()) );
        } else if (ptrInputData->LabelContainsString("Sun Bathe") == true) {
            ptrUser->SetSunBatheFlag( ptrInputData->GetBoolValue() );
        }
    }

    try {
        bool isRealTimeWithBufferUser = false;
        bool isDirectDownlinkVehicle = false;
        bool isCrosslinkVehicle = false;

        for( subElementIter = subElementList->begin(); subElementIter != subElementList->end(); ++subElementIter) {
            ptrSubElement = (*subElementIter);

            if (ptrSubElement->BlockTagContainsString("USER_TRANSMIT_ANTENNA") == true) {
                shared_ptr<UserTransmitAntenna> ptrUTA = shared_ptr<UserTransmitAntenna>(new UserTransmitAntenna(ptrUser));
                // Extract the UTA using the antenna extractor
                ptrAntennaExtractor_a->ExtractUTA(ptrUser, ptrUTA, ptrSubElement);
                ptrAntennaExtractor_a->CheckUTAInputs(ptrUTA);
                // Set the antenna on the user
                ptrUser->SetUserTransmitAntenna(ptrUTA);
                isCrosslinkVehicle = true;
            } else if (ptrSubElement->BlockTagContainsString("DIRECT_DOWNLINK_ANTENNA") == true) {
                shared_ptr<DirectDownlinkAntenna> ptrDDA = shared_ptr<DirectDownlinkAntenna>(new DirectDownlinkAntenna(ptrUser));
                // Extract the DDA using the antenna extractor
                ptrAntennaExtractor_a->ExtractDDA(ptrUser, ptrDDA, ptrSubElement);
                ptrAntennaExtractor_a->CheckDDAInputs(ptrDDA);
                // Set the antenna on the user
                ptrUser->SetDirectDownlinkAntenna(ptrDDA);
                isDirectDownlinkVehicle = true;
            } else if (ptrSubElement->BlockTagContainsString("SELF_RELAY_ANTENNA") == true) {
                ptrSRA = shared_ptr<SelfRelayAntenna>(new SelfRelayAntenna(ptrUser));
                // Extract the SRA using the antenna extractor
                ptrAntennaExtractor_a->ExtractSRA(ptrUser, ptrSRA, ptrSubElement);
                ptrAntennaExtractor_a->CheckSRAInputs(ptrSRA);
                // Add the SRA to the list
                ptrSRAList->push_back(ptrSRA);
            } else if (ptrSubElement->BlockTagContainsString("VALUE_TIMELINE_GENERATION") == true) {
                if (runTimeline == true) {
                    ptrUser->SetTimelineCriteria( shared_ptr<TimelineCriteria>(new TimelineCriteria(ptrSubElement)) );
                }
            }
        }

    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractUserVehicleInfo(InputDataElement*)");
        throw;
    }

    // Set up the coordinate frame on the Base Element Class (replaces the hard coded method)
    ptrUser->SetCoordinateFrame( shared_ptr<ElementCoordinateFrame>(new ElementCoordinateFrame(ElementCoordinateFrame::USER_FRAME, bodyTiltsForCollection)) );
    // Set the any created arrays on user vehicle
    ptrUser->SetRollingRevDutyCycle(ptrRollingRevDutyCycle);
    ptrUser->SetAzimuthOffsetAngles(ptrAzimuthOffsetAngles);
    ptrUser->SetSelfRelayAntennaList(ptrSRAList);
    return;
}

/* ********************************************************************************************* */

void
DataProviderExtractor::ExtractRelaySatelliteInfo(const shared_ptr<RelaySatellite> &ptrRelay,
        const shared_ptr<InputDataElement> &ptrInputDataElement)
{

    shared_ptr<UserPointingAntenna>    ptrUPA = NULL;
    shared_ptr<GroundPointingAntenna>  ptrGPA = NULL;
    shared_ptr<RelayPointingAntenna>   ptrRPA = NULL;
    shared_ptr<MURALTypes::UPAVector>  ptrUPAList = shared_ptr<MURALTypes::UPAVector>(new MURALTypes::UPAVector());;
    shared_ptr<MURALTypes::RPAVector>  ptrRPAList = shared_ptr<MURALTypes::RPAVector>(new MURALTypes::RPAVector());;
    shared_ptr<MURALTypes::GPAVector>  ptrGPAList = shared_ptr<MURALTypes::GPAVector>(new MURALTypes::GPAVector());;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    shared_ptr<InputDataElement::InputDataElementList> subElementList = ptrInputDataElement->GetInputDataElementList();
    InputDataElement::InputDataList::iterator inputDataIter;
    InputDataElement::InputDataElementList::iterator subElementIter;
    shared_ptr<InputData>              ptrInputData = NULL;
    shared_ptr<InputDataElement>       ptrSubElement = NULL;
    // extract the data provider info first
    ExtractDataProviderInfo(ptrRelay, ptrInputDataElement, true, false);

    try {
        int previousCapacityGPA = 0;
        int previousCapacityUPA = 0;
        bool isDownlinkRelay = false;
        bool isMultiHopRelay = false;
        double bodyTiltAngle = 0.0;

        for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
            ptrInputData = (*inputDataIter);

            if (ptrInputData->LabelContainsString("Body Tilt Angle") == true) {
                bodyTiltAngle = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
                ptrRelay->SetBodyTiltAngle( bodyTiltAngle );
            }
        }

        for( subElementIter = subElementList->begin(); subElementIter != subElementList->end(); ++subElementIter) {
            ptrSubElement = (*subElementIter);

            if (ptrSubElement->BlockTagContainsString("USER_POINTING_ANTENNA") == true) {
                ptrUPA = shared_ptr<UserPointingAntenna>(new UserPointingAntenna(ptrRelay, previousCapacityUPA));
                // Extract the UPA using the antenna extractor
                ptrAntennaExtractor_a->ExtractUPA(ptrRelay, ptrUPA, ptrSubElement);
                ptrAntennaExtractor_a->CheckUPAInputs(ptrUPA);
                previousCapacityUPA += ptrUPA->GetCapacity();
                // Add the UPA to the list to later set on the relay
                ptrUPAList->push_back(ptrUPA);
            } else if (ptrSubElement->BlockTagContainsString("GROUND_POINTING_ANTENNA") == true) {
                ptrGPA = shared_ptr<GroundPointingAntenna>(new GroundPointingAntenna(ptrRelay, previousCapacityGPA));
                // Extract the GPA using the antenna extractor
                ptrAntennaExtractor_a->ExtractGPA(ptrRelay, ptrGPA, ptrSubElement);
                ptrAntennaExtractor_a->CheckGPAInputs(ptrGPA);
                previousCapacityGPA += ptrGPA->GetCapacity();
                isDownlinkRelay = true;
                // Add the GPA to the list to later set on the relay
                ptrGPAList->push_back(ptrGPA);
            } else if (ptrSubElement->BlockTagContainsString("RELAY_POINTING_ANTENNA") == true) {
                ptrRPA = shared_ptr<RelayPointingAntenna>(new RelayPointingAntenna(ptrRelay));
                // Extract the RPA using the antenna extractor
                ptrAntennaExtractor_a->ExtractRPA(ptrRelay, ptrRPA, ptrSubElement);
                ptrAntennaExtractor_a->CheckRPAInputs(ptrRPA);

                if (ptrRPA->IsTransmittingAntenna() == true) {
                    isMultiHopRelay = true;
                }

                // Add the RPA to the list to later set on the relay
                ptrRPAList->push_back(ptrRPA);
                ptrRPA = NULL;
            }
        }

        // Set up the coordinate frame on the Base Element Class (replaces the hard coded method)
        ptrRelay->SetCoordinateFrame( shared_ptr<ElementCoordinateFrame>(new ElementCoordinateFrame(ElementCoordinateFrame::RELAY_FRAME, bodyTiltAngle)) );
        // Set the antenna lists on the relay
        ptrRelay->SetUPAList(ptrUPAList);
        ptrRelay->SetRPAList(ptrRPAList);
        ptrRelay->SetGPAList(ptrGPAList);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractRelaySatelliteInfo(InputDataElement*)");
        throw;
    }

    return;
}

/* ****************************************************************************** */

void
DataProviderExtractor::CheckUserVehicleInputs(const shared_ptr<UserVehicle> &ptrUser,
        const bool &isTrailVehicle)
{
    bool                               runTimeline = false;
    bool                               runRequests = false;
    bool                               runAllocator = false;
    bool                               runScheduler = false;
    string                             errorString = "";
    string                             requiresDeckString = "";
    int                                dailyVehicleDutyCycle = ptrUser->GetDutyCycle();
	int                                serviceDefinition = ptrUser->GetServiceTierDefinition();
    int                                vehiclePriority = ptrUser->GetPriority();
    double                             collectionEfficiency = ptrUser->GetCollectionEfficiency();
    double                             bufferMemorySize = ptrUser->GetBufferMemorySize();
    double                             effectiveBufferEmptyRate = ptrUser->GetEffectiveBufferEmptyRate();
    shared_ptr<string>                 ptrGroupName = ptrUser->GetGroupNamePtr();
    shared_ptr<string>                 ptrTargetDeckFileName = ptrUser->GetTargetDeckFileNamePtr();
    LinkList<Sensor>                  *ptrSensorList = ptrUser->GetSensorList();
    shared_ptr<UserVehicle>            ptrTrailVehicle = ptrUser->GetTrailVehicle();
    shared_ptr<UserTransmitAntenna>    ptrUserTransmitAntenna = ptrUser->GetUserTransmitAntenna();
    shared_ptr<DirectDownlinkAntenna>  ptrDirectDownlinkAntenna = ptrUser->GetDirectDownlinkAntenna();
    shared_ptr<MURALTypes::SRAVector>  ptrSelfRelayAntennaList = ptrUser->GetSelfRelayAntennaList();
    shared_ptr<LinkCriteria>           ptrLinkCriteria = ptrUser->GetLinkCriteria();
    shared_ptr<TimelineCriteria>       ptrTimelineCriteria = ptrUser->GetTimelineCriteria();

    if (isTrailVehicle == false) {
        runTimeline = MURALInputProcessor::RunTimelineGenerator();
    }

    if (ptrGroupName == NULL) {
        errorString = "Missing 'Data Provider Group' input";
    }

    if (dailyVehicleDutyCycle < 0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Daily Vehicle Duty Cycle' input";
    }

	//RAGUSA 11/27/2018 - Service Tier Definitions start at 1,2,3,...
	if (serviceDefinition <= 0) {
		if (errorString.size() > 0) {
			errorString += "\n         ";
		}
		errorString += "Missing/invalid 'Service Tier Definition' input. Must be greater than 0";
	//Can't make user definition higher than the number of total definitions
	}else if (serviceDefinition > Earth::GetNumberOfServiceTierDefinitions()) {
		if (errorString.size() > 0) {
			errorString += "\n         ";
		}
		errorString += "Missing/invalid 'Service Tier Definition' input. Must be Less Than/Equal to Number of Service Tier Definitions";
	}

    if ((collectionEfficiency <= 0.0) || (collectionEfficiency > 1.0)) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid Collection Efficiency input";
    }

    if ((bufferMemorySize > 0.0) && (effectiveBufferEmptyRate <= 0.0)) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid Effective Buffer Empty Rate input";
    }

    if ((ptrSensorList == NULL) && (isTrailVehicle == false)) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Data Collector must have at least one SENSOR_OBJECT block";
    }

    if ( (ptrUserTransmitAntenna == NULL) && (ptrDirectDownlinkAntenna == NULL) &&
            (ptrSelfRelayAntennaList == NULL) ) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Must have at least one ANTENNA block (USER_TRANSMIT, DIRECT_DOWNLINK, (Transmit) SELF_RELAY)";
        /*RAGUSA 12/20/2017 - Testing revealed that if the user has more than 1 antenna this check does not consider the other
          antennas on board which could potentially not meet this constraint, so user minimumDataRate for the check (i.e ALL antennas
          must be as fast or faster than effectiveBufferEmptyRate. There are no forseen problems by doing this, if needed - will revise
          later.
          WAS: else if (effectiveBufferEmptyRate > ptrUser->GetMaximumDataRate()) {*/
    } else if (effectiveBufferEmptyRate > ptrUser->GetMinimumDataRate()) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        //Changed string too
        errorString += "Effective buffer empty rate may not exceed minimum transmit data rate";
    }

    //*******************************************************************************************************************************
    if (ptrTrailVehicle != NULL) {
        if ( (ptrUser->GetUserTransmitCapacity() > 1) || (ptrUser->GetDirectDownlinkCapacity() > 1) ||
                (ptrUser->GetSelfRelayCapacity() > 1) ) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "No ANTENNA capacity may exceed ONE when specifying a 'Trail Vehicle'";
        }
    } else if ( (ptrUser->GetUserTransmitCapacity() > (int)UserVehicle::NUMBER_OF_MBB_VEHICLES) ||
                (ptrUser->GetDirectDownlinkCapacity() > (int)UserVehicle::NUMBER_OF_MBB_VEHICLES) ) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "No ANTENNA capacity may exceed "
                       + StringEditor::ConvertToString((int)UserVehicle::NUMBER_OF_MBB_VEHICLES);
    }

    if ((ptrLinkCriteria == NULL) && (MURALInputProcessor::RunLinkPathGenerator() == true)) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing LINK_GENERATION block";
    }

    if ((runTimeline == true) && (ptrTimelineCriteria == NULL)) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing VALUE_TIMELINE_GENERATION block";
    }

    if ( (runScheduler == true) && (TargetDeck::ScheduleTargets() == true) &&
            (TargetDeck::IsTargetGroup(*ptrGroupName) == true) && (ptrTargetDeckFileName == NULL) ) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Must input 'Target Deck File Name' when scheduling targets";
    }

    if (errorString.size() > 0) {
        errorString += " for ";
        errorString += ptrUser->GetDesignator();
        throw new InputException(GetClassName(), "ExtractUserVehicleInfo(InputDataElement*)",
                                 errorString);
    }

    try {
        bool usingDeck = false;
        ListIterator<Sensor> sensorListIter(ptrSensorList);
        Sensor *ptrSensor = sensorListIter.First();

        while (ptrSensor != NULL) {
            ptrSensor->CheckInputData(MURALInputProcessor::RunTimelineGenerator());
            /*RAGUSA 11/2/17 - Removed constraint due to addition of SPECIAL_SAR, allows a user vehicle to
            //collect at a higher data rate than it can transmit. If this is used, use the RTWB or SAD
            //request methods to correctly store the extra collected data.
            //if (ptrSensor->GetDataRate() > ptrUser->GetTransmitDataRate()) {
            //if (errorString.size() > 0) {
            //    errorString += "\n         ";
            //}
            //errorString += (ptrSensor->GetSensorName() + ": ");
            //errorString += "Sensor Collection Rate CANNOT be greater than Transmit Data Rate";
            //}
            *********************************************************************************************/
            ptrSensor = sensorListIter.Next();
        }

        if ((ptrTrailVehicle != NULL) || (isTrailVehicle == true) ||
                (ptrUser->GetTotalCapacity() > 1) ) {
            UserVehicle::anyMakeBeforeBreakUsers_s = true;
        }

        if (ptrLinkCriteria != NULL) {
            if (ptrLinkCriteria->ReadLinkPaths() == true) {
                FileNames::VerifyFile(FileNames::CROSSLINK_INPUTFILE);

                if (ptrDirectDownlinkAntenna != NULL) {
                    FileNames::VerifyFile(FileNames::DIRECTDOWNLINK_INPUTFILE);
                }

                if (ptrSelfRelayAntennaList->size() > 0) {
                    FileNames::VerifyFile(FileNames::SELFRELAY_INPUTFILE);
                }
            }

            // if lead or trail vehicle, verify MBB link times are set
            if ((ptrTrailVehicle != NULL) || (isTrailVehicle == true) ||
                    (ptrUser->GetTotalCapacity() > 1) ) {
                ptrLinkCriteria->VerifyMakeBeforeBreakTimes();
            } else {
                ptrLinkCriteria->ResetMakeBeforeBreakTimes();
            }
        }

        if (ptrTimelineCriteria != NULL) {
            usingDeck = ptrTimelineCriteria->UseDeck();

            if (ptrTimelineCriteria->GetTimelineGenerationMethod()
                    == TimelineCriteria::READ_TIMELINE) {
                FileNames::VerifyFile(FileNames::TIMELINE_INPUTFILE);
            } else if (ptrTimelineCriteria->GetFlatScorePerTime() <= DOUBLE_ZERO) {
                requiresDeckString = "No 'Flat Score Per Time' was specified";
            }

            if (ptrTimelineCriteria->GenerateResourceSpecificTimeline() == true) {
                int numberOfScoreProfiles = ptrUser->GetNumberOfResources();
                ptrUser->SetNumberOfScoreProfiles(numberOfScoreProfiles);

                if (ptrTrailVehicle != NULL) {
                    ptrTrailVehicle->SetNumberOfScoreProfiles(numberOfScoreProfiles);
                }
            }
        }

        if (runScheduler == true) {
            if (requiresDeckString.size() > 0) {
                requiresDeckString += "\n         AND ";
            }

            requiresDeckString += "The Target Scheduler is to be run";
        }

        if ((requiresDeckString.size() > 0) && (usingDeck == false)) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Must have valid 'Type Of Deck' input (TARGET, REQUIREMENT, or BOTH)";
            errorString += "\n         ";
            errorString += requiresDeckString;
        } else {
            ptrSensor = sensorListIter.First();

            while (ptrSensor != NULL) {
                if (ptrSensor->GetSensorWeightFactor() <= 0.0) {
                    if (errorString.size() > 0) {
                        errorString += "\n         ";
                    }

                    errorString += "Missing/invalid 'Sensor Weight Factor' input for sensor ";
                    errorString += ptrSensor->GetSensorName();
                }

                ptrSensor = sensorListIter.Next();
            }

            if ( isTrailVehicle == false && ptrTimelineCriteria != NULL) {
                if (ptrTimelineCriteria->UseTargetDeck() == true) {
                    ptrUser->VerifyTargetDeck();
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractUserVehicleInfo(InputDataElement*)");
        throw;
    }

    if (errorString.size() > 0) {
        errorString += " for ";
        errorString += ptrUser->GetDesignator();
        throw new InputException(GetClassName(), "ExtractUserVehicleInfo(InputDataElement*)",
                                 errorString);
    }

    if (ptrDirectDownlinkAntenna != NULL) {
        UserVehicle::anyDirectDownlinkUsers_s = true;
    }
}

/* ********************************************************************************************* */

void
DataProviderExtractor::CheckGroundStationInputs(const shared_ptr<GroundStation> &ptrGround)
{
    string                         errorString = "";
    Array<string> *ptrRFDesignator = new Array<string>();
    shared_ptr<MURALTypes::RFAVector> ptrRFList = ptrGround->GetRFList();
    shared_ptr<ReceiveFacilityAntenna> ptrRF = NULL;
    MURALTypes::RFAVector::iterator rfaIter;

    for(rfaIter = ptrRFList->begin(); rfaIter != ptrRFList->end(); ++rfaIter) {
        ptrRF = (*rfaIter);

        if (ptrRFDesignator->ValueExists(ptrRF->GetAntennaDesignator()) == true) {
            errorString = "Each RF must have a unique designator!";
        } else {
            ptrRFDesignator->AddElement(ptrRF->GetAntennaDesignator());
        }
    }

    delete ptrRFDesignator;
    ptrRFDesignator = NULL;

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractGroundStationInfo(InputDataElement&)",
                                 errorString);
    }

    return;
}

/* ********************************************************************************************* */

void
DataProviderExtractor::CheckRelaySatelliteInputs(const shared_ptr<RelaySatellite> &ptrRelay)
{
    string                              errorString = "";
    Array<string>                      *ptrUPADesignator = NULL;
    Array<string>                      *ptrGPADesignator = NULL;
    Array<string>                      *ptrRPADesignator = NULL;
    double                              bodyTiltAngle = ptrRelay->GetBodyTiltAngle();
    MURALTypes::UPAVector::iterator     upaIter;
    MURALTypes::GPAVector::iterator     gpaIter;
    MURALTypes::RPAVector::iterator     rpaIter;
    shared_ptr<UserPointingAntenna>     ptrUPA = NULL;
    shared_ptr<RelayPointingAntenna>    ptrRPA = NULL;
    shared_ptr<GroundPointingAntenna>   ptrGPA = NULL;
    shared_ptr<MURALTypes::UPAVector>   ptrUPAList = ptrRelay->GetUserPointingAntennaList();
    shared_ptr<MURALTypes::RPAVector>   ptrRPAList = ptrRelay->GetRelayPointingAntennaList();
    shared_ptr<MURALTypes::GPAVector>   ptrGPAList = ptrRelay->GetGroundPointingAntennaList();
    shared_ptr<LinkCriteria>            ptrLinkCriteria = ptrRelay->GetLinkCriteria();

    try {
        int conjunctionCapacityUPA = 0;
        bool isMultiHopRelay = false;

        // Check UPA's
        for(upaIter = ptrUPAList->begin(); upaIter != ptrUPAList->end(); ++upaIter) {
            ptrUPA = (*upaIter);

            if (ptrUPA->IsConjunctionAntenna() == true) {
                conjunctionCapacityUPA += ptrUPA->GetCapacity();
            }

            if (ptrUPADesignator == NULL) {
                ptrUPADesignator = new Array<string>(1, ptrUPA->GetAntennaDesignator());
            } else if (ptrUPADesignator->ValueExists(ptrUPA->GetAntennaDesignator()) == true) {
                errorString = "Each UPA must have a unique designator!";
            } else {
                ptrUPADesignator->AddElement(ptrUPA->GetAntennaDesignator());
            }
        }

        // Check RPA's
        for(rpaIter = ptrRPAList->begin(); rpaIter != ptrRPAList->end(); ++rpaIter) {
            ptrRPA = (*rpaIter);

            if (ptrRPA->IsTransmittingAntenna() == true) {
                isMultiHopRelay = true;
            }

            if (ptrRPADesignator == NULL) {
                ptrRPADesignator = new Array<string>(1, ptrRPA->GetAntennaDesignator());
            } else if (ptrRPADesignator->ValueExists(ptrRPA->GetAntennaDesignator()) == true) {
                errorString = "Each RPA must have a unique designator!";
            } else {
                ptrRPADesignator->AddElement(ptrRPA->GetAntennaDesignator());
            }
        }

        // Check GPA's
        for(gpaIter = ptrGPAList->begin(); gpaIter != ptrGPAList->end(); ++gpaIter) {
            ptrGPA = (*gpaIter);

            if (ptrGPADesignator == NULL) {
                ptrGPADesignator = new Array<string>(1, ptrGPA->GetAntennaDesignator());
            } else if (ptrGPADesignator->ValueExists(ptrGPA->GetAntennaDesignator()) == true) {
                errorString = "Each GPA must have a unique designator!";
            } else {
                ptrGPADesignator->AddElement(ptrGPA->GetAntennaDesignator());
            }
        }

        if ((ptrLinkCriteria != NULL) && (errorString.size() == 0)) {
            if (ptrLinkCriteria->ReadLinkPaths() == true) {
                if (ptrGPAList->size() > 0) {
                    FileNames::VerifyFile(FileNames::DOWNLINK_INPUTFILE);
                }

                if (isMultiHopRelay == true) {
                    FileNames::VerifyFile(FileNames::MULTIHOP_INPUTFILE);
                }
            }

            ptrLinkCriteria->RemoveConjunctionData(conjunctionCapacityUPA);

            if (ptrLinkCriteria->ReadConjunctionData() == true) {
                FileNames::VerifyFile(FileNames::UPA_CONJ_INPUTFILE);
            }
        }

        if ((ptrRelay->IsHighlyElliptical() == false) && (bodyTiltAngle != 0.0)) {
            if (errorString.size() > 0) {
                errorString += "         \n";
            }

            errorString += "Only input 'Body Tilt Angle' for a HEO relay (NOT GEO)";
        }

        if (SimpleMath::Abs(bodyTiltAngle) >= UnitData::GetStoreQuarterCircle()) {
            if (errorString.size() > 0) {
                errorString += "         \n";
            }

            errorString += "Invalid 'Body Tilt Angle' input";
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "ExtractRelaySatelliteInfo(InputDataElement*)");
        throw;
    }

    if (ptrUPADesignator != NULL) {
        delete ptrUPADesignator;
        ptrUPADesignator = NULL;
    }

    if (ptrGPADesignator != NULL) {
        delete ptrGPADesignator;
        ptrGPADesignator = NULL;
    }

    if (ptrRPADesignator != NULL) {
        delete ptrRPADesignator;
        ptrRPADesignator = NULL;
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractRelaySatelliteInfo(InputDataElement*)",
                                 errorString);
    }

    if ((ptrUPAList == NULL) && (ptrRelay->HasReceiveRPA() == false)) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Must have at least ONE UPA or Receiving RPA!";
    }

    if ((ptrGPAList == NULL) && (ptrRelay->HasTransmitRPA() == false)) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Must have at least ONE GPA or Transmitting RPA!";
    }

    if ( (MURALInputProcessor::RunLinkPathGenerator() == true) &&
            (ptrLinkCriteria == NULL) ) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing LINK_GENERATION block";
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractRelaySatelliteInfo(InputDataElement*)",
                                 errorString);
    }

    return;
}

DataProviderExtractor::~DataProviderExtractor()
{
    return;
}

