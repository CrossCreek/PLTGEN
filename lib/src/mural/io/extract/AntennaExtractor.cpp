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
 * AntennaExtractor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "AntennaExtractor.h"
#include "general/exception/InputException.h"
#include "general/data/UnitData.h"
#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"
#include "general/array/ListIterator.h"
#include "general/utility/StringEditor.h"
#include "general/utility/Utility.h"

#include "mural/io/MURALInputProcessor.h"
#include "mural/coorframe/AntennaCoordinateFrame.h"
#include "mural/antenna/UserTransmitAntenna.h"
#include "mural/antenna/UserPointingAntenna.h"
#include "mural/antenna/GroundPointingAntenna.h"
#include "mural/antenna/RelayPointingAntenna.h"
#include "mural/antenna/SelfRelayAntenna.h"
#include "mural/antenna/DirectDownlinkAntenna.h"
#include "mural/antenna/ReceiveFacilityAntenna.h"
#include "mural/modules/lg/AzimuthAngleData.h"
#include "mural/modules/lg/LinkConstraintData.h"

AntennaExtractor::AntennaExtractor()
{
    return;
}

/* ********************************************************************************************* */

bool
AntennaExtractor::sortAzEl(const shared_ptr<AzimuthAngleData> &ptrI, const shared_ptr<AzimuthAngleData> ptrJ)
{
    return (ptrI->GetAzimuthAngle() < ptrJ->GetAzimuthAngle());
}

/* ********************************************************************************************* */

void
AntennaExtractor::ExtractAntennaInfo(const shared_ptr<Element> &ptrElement,
                                     const shared_ptr<Antenna> &ptrAntenna,
                                     const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    //int numberOfAntenna;
    const int           startIndexTime = TimePiece::GetStartIndexTime();
    const int           endIndexTime = TimePiece::GetEndIndexTime();
    double              value = 0.0;
    double              minimumMinElev = 0.0;
    double              maximumMaxElev = 0.0;
    double              defaultMaxElev = UNDEFINED_ANGLE;
    double              defaultMinElev = UNDEFINED_ANGLE;
    double              defaultMinGrazing = UNDEFINED_DISTANCE;
    double              defaultMaxRange = UNDEFINED_DISTANCE;
    double              defaultMinRange = UNDEFINED_DISTANCE;
    double              defaultMaxSun = UNDEFINED_ANGLE;
    double              defaultMinSun = UNDEFINED_ANGLE;
    double              platformOffsetAngle = 0.0;
    double              gimbalFrameOffsetAngle = 0.0;
    double              rollOffsetAngle = 0.0;
    bool                setRequired = false;
    string              designator = "";
    string              capacityString = "";
    string              errorString = "";
    shared_ptr<LinkConstraintData>  ptrDefaultConstraintData = NULL;
    shared_ptr<LinkConstraintData>  ptrConstraintData = NULL;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator   inputDataIter;
    shared_ptr<InputData>                       ptrInputData = NULL;
    shared_ptr<string>                              ptrAntennaDesignator = NULL;
    Antenna::FREQUENCY_BAND                         frequencyBand = Antenna::UNKNOWN_BAND;
    shared_ptr<MURALTypes::AzimuthAngleDataVector>  ptrAzimuthAngleDataList = NULL;
    shared_ptr<vector<string>>                      ptrDedicatedTransmitterArray = NULL;
    shared_ptr<vector<string>>                      ptrPrecludedReceiverArray = NULL;
    shared_ptr<AntennaCoordinateFrame>              ptrAntennaCoordinateFrame = NULL;

    try {
        int element = 0;
        int numberOfElements = 0;
        int preferencedOutageCapacity = 0;
        int startTime = 0;
        int endTime = 0;
        int capacity = 0;
        int preferencedCapacity = 0;
        int preparationTime = 0;
        double dataRate = 0.0;
        double minimumGimbalStopAngle = 0.0;
        double maximumGimbalStopAngle = 0.0;

        for (inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); inputDataIter++) {
            ptrInputData = (*inputDataIter);

            if (ptrInputData->LabelContainsString("Antenna Designator") == true) {
                ptrAntennaDesignator = shared_ptr<string>(new string(ptrInputData->GetStringValue()));
                ptrAntenna->SetAntennaDesignator(ptrAntennaDesignator);
            } else if (ptrInputData->LabelContainsString("Communication Frequency Band") == true) {
                frequencyBand = GetFrequencyBandValue(ptrInputData->GetStringValue());
                ptrAntenna->SetFrequencyBand(frequencyBand);
            } else if (ptrInputData->LabelContainsString("Capacity") == true) {
                capacity = ptrInputData->GetIntValue();
                ptrAntenna->SetCapacity(capacity);
            } else if (ptrInputData->LabelContainsString("Preferenced Antenna") == true) {
                preferencedCapacity = ptrInputData->GetIntValue();
                ptrAntenna->SetPreferencedCapacity(preferencedCapacity);
            } else if (ptrInputData->LabelContainsString("Preparation Time") == true) {
                preparationTime = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
                ptrAntenna->SetPreparationTime(preparationTime);
            } else if (ptrInputData->LabelContainsString("Data Rate") == true) {
                dataRate = ptrInputData->GetDoubleValue();
                ptrAntenna->SetDataRate(dataRate);
            } else if (ptrInputData->LabelContainsString("Platform Offset Angle") == true) {
                platformOffsetAngle
                    = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Gimbal Frame Offset Angle") == true) {
                gimbalFrameOffsetAngle
                    = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Roll Offset Angle") == true) {
                rollOffsetAngle
                    = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Minimum Gimbal Stop Angle") == true) {
                minimumGimbalStopAngle
                    = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
                ptrAntenna->SetMinimumGimbalStopAngle(minimumGimbalStopAngle);
            } else if (ptrInputData->LabelContainsString("Maximum Gimbal Stop Angle") == true) {
                maximumGimbalStopAngle
                    = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
                ptrAntenna->SetMaximumGimbalStopAngle(maximumGimbalStopAngle);
            } else if (ptrInputData->LabelContainsString("Azimuth Based Elevation Angles") == true) {
                int                           index = -1;
                const double                  azimuthAngle = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue(++index));
                const double                  minElevation = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue(++index));
                const double                  maxElevation = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue(++index));
                shared_ptr<AzimuthAngleData>  ptrAzimuthAngle = NULL;

                if (ptrAzimuthAngleDataList == NULL) {
                    ptrAzimuthAngleDataList = shared_ptr<MURALTypes::AzimuthAngleDataVector>(new MURALTypes::AzimuthAngleDataVector());
                }

                ptrAzimuthAngle = shared_ptr<AzimuthAngleData>(new AzimuthAngleData(minElevation, maxElevation, azimuthAngle));

                if (Utility::VectorContains(*ptrAzimuthAngleDataList, ptrAzimuthAngle) == false) {
                    ptrAzimuthAngleDataList->push_back(ptrAzimuthAngle);
                }
            } else if (ptrInputData->LabelContainsString("Default Maximum Elevation Angle") == true) {
                defaultMaxElev = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Default Minimum Elevation Angle") == true) {
                defaultMinElev = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Default Minimum Grazing Altitude") == true) {
                defaultMinGrazing = UnitData::GetInternalDistance(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Default Maximum Range") == true) {
                defaultMaxRange = UnitData::GetInternalDistance(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Default Minimum Range") == true) {
                defaultMinRange = UnitData::GetInternalDistance(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Default Maximum Sun Separation Angle") == true) {
                defaultMaxSun = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Default Minimum Sun Separation Angle") == true) {
                defaultMinSun = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Outage Time Period") == true) {
                capacityString = "";

                if ((ptrInputData->GetSpecificType(designator, capacityString) == true) ||
                        (ptrInputData->GetSpecificType(designator) == true)) {
                    if ((designator == DEFAULT_STRING) || (designator == CAPACITY_STRING) ||
                            (MURALInputProcessor::ValidDataProvider(designator) == true)) {
                        startTime = UnitData::GetInternalTimeIndex(ptrInputData->GetDoubleValue(0));
                        startTime = std::max(startTime, startIndexTime);
                        endTime = UnitData::GetInternalTimeIndex(ptrInputData->GetDoubleValue(1));
                        endTime = std::min(endTime, endIndexTime);

                        if (capacityString.size() > 0) {
                            preferencedOutageCapacity = StringEditor::ConvertToInt(capacityString);
						}
						else {
							preferencedOutageCapacity = 0;
						}
                    }
                }
            } else if (ptrInputData->LabelContainsString("Dedicated Transmitter") == true) {
                element = 0;
                numberOfElements = ptrInputData->GetNumberOfValues();

                while (element < numberOfElements) {
                    designator = ptrInputData->GetStringValue(element);

                    if (MURALInputProcessor::ValidDataProvider(designator) == true) {
                        if (ptrDedicatedTransmitterArray == NULL) {
                            ptrDedicatedTransmitterArray = shared_ptr<vector<string>>(new vector<string>());
                        }

                        // Append unique
                        if (Utility::VectorContains(*ptrDedicatedTransmitterArray, designator) == false) {
                            ptrDedicatedTransmitterArray->push_back(designator);
                        }
                    }

                    ++element;
                }
            } else if (ptrInputData->LabelContainsString("Precluded Receiver") == true) {
                element = 0;
                numberOfElements = ptrInputData->GetNumberOfValues();

                while (element < numberOfElements) {
                    designator = ptrInputData->GetStringValue(element);

                    if (MURALInputProcessor::ValidDataProvider(designator) == true) {
                        if (ptrPrecludedReceiverArray == NULL) {
                            ptrPrecludedReceiverArray = shared_ptr<vector<string>>(new vector<string>());
                        }

                        if (Utility::VectorContains(*ptrPrecludedReceiverArray, designator) == false) {
                            ptrPrecludedReceiverArray->push_back(designator);
                        }
                    }

                    ++element;
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractAntennaInfo()");
        throw;
    }

    // Create/Populate the antenna coordinate frame,
    ptrAntennaCoordinateFrame = shared_ptr<AntennaCoordinateFrame>(new AntennaCoordinateFrame(ptrElement));

    if (ptrAntennaCoordinateFrame != NULL) {
        ptrAntennaCoordinateFrame->SetPlatformOffsetAngle(platformOffsetAngle);
        ptrAntennaCoordinateFrame->SetGimbalFrameOffsetAngle(gimbalFrameOffsetAngle);
        ptrAntennaCoordinateFrame->SetRollOffsetAngle(rollOffsetAngle);
        ptrAntenna->SetAntennaCoordinateFrame(ptrAntennaCoordinateFrame);
    }

    // construct the min/max data
    maximumMaxElev = defaultMaxElev;
    minimumMinElev = defaultMinElev;
    ptrDefaultConstraintData = shared_ptr<LinkConstraintData>(
                                   new LinkConstraintData(DEFAULT_STRING, defaultMaxElev,
                                           defaultMinElev, defaultMinGrazing, defaultMaxRange,
                                           defaultMinRange, defaultMaxSun, defaultMinSun));
    ptrAntenna->SetDefaultLinkConstraintData(ptrDefaultConstraintData);

    for (inputDataIter = inputDataList->begin(); (inputDataIter != inputDataList->end()) && (errorString.size() == 0); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if ((ptrInputData->GetSpecificType(designator) == true) &&
                ((ptrInputData->LabelContainsString("Maximum Elevation Angle") == true) ||
                 (ptrInputData->LabelContainsString("Minimum Elevation Angle") == true) ||
                 (ptrInputData->LabelContainsString("Minimum Grazing Altitude") == true) ||
                 (ptrInputData->LabelContainsString("Maximum Range") == true) ||
                 (ptrInputData->LabelContainsString("Minimum Range") == true) ||
                 (ptrInputData->LabelContainsString("Maximum Sun Separation Angle") == true) ||
                 (ptrInputData->LabelContainsString("Minimum Sun Separation Angle") == true))) {
            setRequired = false;

            // If we find a new specific value for a specific designator then we need to make a copy of the default data
            if ((ptrAntenna->HasLinkConstraintData(designator)) == false) {
                ptrConstraintData = shared_ptr<LinkConstraintData>(new LinkConstraintData(*ptrDefaultConstraintData));
                ptrConstraintData->SetDataProviderDesignator(designator);
                // If we create a new data we need to set it on the antenna, otherwise the pointer will set the values
                // appropriately and there is not need to reset on the antenna
                setRequired = true;
            }
            // Get the pointer to the specfic constraint data
            else {
                ptrConstraintData = ptrAntenna->GetLinkConstraintData(designator);
            }

            value = ptrInputData->GetDoubleValue();

            if (ptrInputData->LabelContainsString("Maximum Elevation Angle") == true) {
                if (defaultMaxElev == UNDEFINED_ANGLE) {
                    setRequired = false;
                    errorString = "Missing 'Default Maximum Elevation Angle' input";
                } else {
                    value = UnitData::GetInternalAngle(value);

                    if (value != defaultMaxElev) {
                        ptrConstraintData->SetMaximumElevationAngle(value);
                        maximumMaxElev = std::max(maximumMaxElev, value);
                    }
                }
            } else if (ptrInputData->LabelContainsString("Minimum Elevation Angle") == true) {
                if (defaultMinElev == UNDEFINED_ANGLE) {
                    setRequired = false;
                    errorString = "Missing 'Default Minimum Elevation Angle' input";
                } else {
                    value = UnitData::GetInternalAngle(value);

                    if (value != defaultMinElev) {
                        ptrConstraintData->SetMinimumElevationAngle(value);
                        minimumMinElev = std::min(minimumMinElev, value);
                    }
                }
            } else if (ptrInputData->LabelContainsString("Minimum Grazing Altitude") == true) {
                if (defaultMinGrazing == UNDEFINED_DISTANCE) {
                    setRequired = false;
                    errorString = "Missing 'Default Minimum Grazing Altitude' input";
                } else {
                    value = UnitData::GetInternalDistance(value);

                    if (value != defaultMinGrazing) {
                        ptrConstraintData->SetMinimumGrazingAltitude(value);
                    }
                }
            } else if (ptrInputData->LabelContainsString("Maximum Range") == true) {
                if (defaultMaxRange == UNDEFINED_DISTANCE) {
                    setRequired = false;
                    errorString = "Missing 'Default Maximum Range' input";
                } else {
                    value = UnitData::GetInternalDistance(value);

                    if (value != defaultMaxRange) {
                        ptrConstraintData->SetMaximumRange(value);
                    }
                }
            } else if (ptrInputData->LabelContainsString("Minimum Range") == true) {
                if (defaultMinRange == UNDEFINED_DISTANCE) {
                    setRequired = false;
                    errorString = "Missing 'Default Minimum Range' input";
                } else {
                    value = UnitData::GetInternalDistance(value);

                    if (value != defaultMinRange) {
                        ptrConstraintData->SetMinimumRange(value);
                    }
                }
            } else if (ptrInputData->LabelContainsString("Maximum Sun Separation Angle") == true) {
                if (defaultMaxSun == UNDEFINED_ANGLE) {
                    setRequired = false;
                    errorString = "Missing 'Default Maximum Sun Separation Angle' input";
                } else {
                    value = UnitData::GetInternalAngle(value);

                    if (value != defaultMaxSun) {
                        ptrConstraintData->SetMaximumSunSeparationAngle(value);
                    }
                }
            } else if (ptrInputData->LabelContainsString("Minimum Sun Separation Angle") == true) {
                if (defaultMinSun == UNDEFINED_ANGLE) {
                    setRequired = false;
                    errorString = "Missing 'Default Minimum Sun Separation Angle' input";
                } else {
                    value = UnitData::GetInternalAngle(value);

                    if (value != defaultMinSun) {
                        ptrConstraintData->SetMinimumSunSeparationAngle(value);
                    }
                }
            }

            if (setRequired == true) {
                ptrAntenna->SetSpecificLinkConstraintData(designator, ptrConstraintData);
            }
        }
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "ExtractAntennaInfo()", errorString);
    }

    // Sort the azimuth list
    if (ptrAzimuthAngleDataList != NULL) {
        std::sort(ptrAzimuthAngleDataList->begin(), ptrAzimuthAngleDataList->end(), sortAzEl);
    }

    // Azimuth Data Lists, Dedicated Transmitter array,
    // precluded receiver array, and link constraint array
    ptrAntenna->SetAzimuthAngleDataList(ptrAzimuthAngleDataList);
    ptrAntenna->SetDedicatedTransmitterArray(ptrDedicatedTransmitterArray);
    ptrAntenna->SetPrecludedReceiverArray(ptrPrecludedReceiverArray);

    //=====================================================================================================================
    return;
}

/* ********************************************************************************************* */

void
AntennaExtractor::ExtractUTA(const shared_ptr<Element> &ptrElement,
                             const shared_ptr<UserTransmitAntenna> &ptrUTA,
                             const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    // Extract  Generic Antenna info
    ExtractAntennaInfo(ptrElement, ptrUTA, ptrInputDataElement);
    // Set the Antenna capability for the fixed antenna type
    ptrUTA->SetAntennaCapability(AntennaBase::TRANSMIT);
    // Extract the User Transmit Antenna info
    int       index = 0;
    int       numberOfValues = 0;
    double     cableWrapAngle = 0.0;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;
    bool                fixedGimbal = false;
    Array<double>*      ptrCableWrapOffsetAngles = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Fixed Gimbal") == true) {
            fixedGimbal = ptrInputData->GetBoolValue();
            ptrUTA->SetFixedGimbal(fixedGimbal);
        } else if (ptrInputData->LabelContainsString("Cable Wrap Offset Angles") == true) {
            if (ptrCableWrapOffsetAngles == NULL) {
                ptrCableWrapOffsetAngles = new Array<double>();
            }

            index = 0;
            numberOfValues = ptrInputData->GetNumberOfValues();

            while (index < numberOfValues) {
                cableWrapAngle = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue(index));
                ptrCableWrapOffsetAngles->AddUniqueElement(cableWrapAngle);
                ++index;
            }
        }
    }

    // Set the arrays on the antennas
    ptrUTA->SetCableWrapOffsetAngles(ptrCableWrapOffsetAngles);
}

/* ********************************************************************************************* */

void
AntennaExtractor::ExtractDDA(const shared_ptr<Element> &ptrElement,
                             const shared_ptr<DirectDownlinkAntenna> &ptrDDA,
                             const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    // Extract  Generic Antenna info
    ExtractAntennaInfo(ptrElement, ptrDDA, ptrInputDataElement);
    // Set the Antenna capability for the fixed antenna type
    ptrDDA->SetAntennaCapability(AntennaBase::TRANSMIT);
}

/* ********************************************************************************************* */

void
AntennaExtractor::ExtractSRA(const shared_ptr<Element> &ptrElement,
                             const shared_ptr<SelfRelayAntenna> &ptrSRA,
                             const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    // Extract  Generic Antenna info
    ExtractAntennaInfo(ptrElement, ptrSRA, ptrInputDataElement);
    // Extract the SRA (basically the capability)
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Self Relay Antenna Capability") == true) {
            ptrSRA->SetAntennaCapability( GetAntennaCapabilityValue(ptrInputData->GetStringValue()) );
        }
    }
}

/* ********************************************************************************************* */

void
AntennaExtractor::ExtractUPA(const shared_ptr<Element> &ptrElement,
                             const shared_ptr<UserPointingAntenna> &ptrUPA,
                             const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    // Extract  Generic Antenna info
    ExtractAntennaInfo(ptrElement, ptrUPA, ptrInputDataElement);
    // Set the Antenna capability for the fixed antenna type
    ptrUPA->SetAntennaCapability(AntennaBase::RECEIVE);
    // Extract the User Pointing Antenna info
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator   inputDataIter;
    shared_ptr<InputData>                       ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Number Of Transmit Channels") == true) {
            ptrUPA->SetNumberOfTransmitChannels( ptrInputData->GetIntValue() );
        }
    }

    return;
}

/* ********************************************************************************************* */

void
AntennaExtractor::ExtractGPA(const shared_ptr<Element> &ptrElement,
                             const shared_ptr<GroundPointingAntenna> &ptrGPA,
                             const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    // Extract  Generic Antenna info
    ExtractAntennaInfo(ptrElement, ptrGPA, ptrInputDataElement);
    // Set the Antenna capability for the fixed antenna type
    ptrGPA->SetAntennaCapability(AntennaBase::TRANSMIT);
    // Extract the Ground Pointing Antenna info
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator   inputDataIter;
    shared_ptr<InputData>                       ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Number Of Transmit Channels") == true) {
            ptrGPA->SetNumberOfTransmitChannels( ptrInputData->GetIntValue() );
        }
    }

    return;
}

/* ********************************************************************************************* */

void
AntennaExtractor::ExtractRPA(const shared_ptr<Element> &ptrElement,
                             const shared_ptr<RelayPointingAntenna> &ptrRPA,
                             const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    // Extract  Generic Antenna info
    ExtractAntennaInfo(ptrElement, ptrRPA, ptrInputDataElement);
    // Extract the User Pointing Antenna info
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator   inputDataIter;
    shared_ptr<InputData>                       ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Number Of Transmit Channels") == true) {
            ptrRPA->SetNumberOfTransmitChannels( ptrInputData->GetIntValue() );
        } else if (ptrInputData->LabelContainsString("Relay Pointing Antenna Capability") == true) {
            ptrRPA->SetAntennaCapability( GetAntennaCapabilityValue(ptrInputData->GetStringValue()) );
        }
    }

    return;
}

/* ********************************************************************************************* */

void
AntennaExtractor::ExtractRF(const shared_ptr<Element> &ptrElement,
                            const shared_ptr<ReceiveFacilityAntenna> &ptrRF,
                            const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    // Extract Generic Antenna info
    ExtractAntennaInfo(ptrElement, ptrRF, ptrInputDataElement);
    // Set the Antenna capability for the fixed antenna type
    ptrRF->SetAntennaCapability(AntennaBase::RECEIVE);
    // Extract RF Specific information
    const int                                    startIndexTime = TimePiece::GetStartIndexTime();
    const int                                    endIndexTime = TimePiece::GetEndIndexTime();
    string                                       userDesignator = "";
    string                                       errorString = "";
    PooledEquipmentPeriod                       *ptrPooledEquipment = NULL;
    Array<string>                               *ptrPooledEquipmentUsers = NULL;
    Array<double>                               *ptrPESpecificTokens = NULL;
    shared_ptr<InputDataElement::InputDataList>  inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator    inputDataIter;
    shared_ptr<InputData>                        ptrInputData = NULL;
    LinkList<PooledEquipmentPeriod>*             ptrPooledEquipmentList = NULL;
    Array<string>*                               ptrPrecludedUserArray = NULL;

    try {
        int numberOfPooledEquipment = 0;
        int specificTokenCount = 0;
        int element = 0;
        int numberOfElements = 0;
        int startTime = 0;
        int endTime = 0;

        for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
            ptrInputData = (*inputDataIter);

            if (ptrInputData->LabelContainsString("Limited User Vehicle Pooled Equipment") == true) {
                // The specific input for Pooled Equipment can be in one of two forms:
                // Limited User Vehicle Pooled Equipment [<PE>]                         : <user list>
                // Limited User Vehicle Pooled Equipment [<PE>,<start time>,<end time>] : <user list>
                // Get the specific tokens
                ptrPESpecificTokens = new Array<double>();
                specificTokenCount = ptrInputData->GetSpecificNumbers(ptrPESpecificTokens);

                if ( (specificTokenCount != 1) && (specificTokenCount != 3) ) {
                    errorString = "Limited User Vehicle Pooled Equipment input ill formed.  Invalid tokens inside brackets";
                    throw new InputException(GetClassName(),
                                             "ExtractReceiveFacilityAntennaInfo(InputDataElement*)",
                                             errorString);
                }

                // Grab the PE value and specified time period if there (else default to whole day)
                numberOfPooledEquipment = (int)ptrPESpecificTokens->ElementAt(0);
                startTime = startIndexTime;
                endTime = endIndexTime;

                if ( specificTokenCount == 3 ) {
                    startTime = UnitData::GetInternalTimeIndex(ptrPESpecificTokens->ElementAt(1));
                    startTime = std::max(startTime, startIndexTime);
                    endTime = UnitData::GetInternalTimeIndex(ptrPESpecificTokens->ElementAt(2));
                    endTime = std::min(endTime, endIndexTime);
                }

                // Delete the token array
                delete ptrPESpecificTokens;
                ptrPESpecificTokens = NULL;
                // Read in the PE User List
                numberOfElements = ptrInputData->GetNumberOfValues();

                if (numberOfElements > 0) {
                    ptrPooledEquipmentUsers = new Array<string>();
                    element = 0;

                    while (element < numberOfElements) {
                        userDesignator = ptrInputData->GetStringValue(element);

                        if (MURALInputProcessor::ValidUserVehicle(userDesignator) == true) {
                            ptrPooledEquipmentUsers->AddUniqueElement(userDesignator);
                        }

                        ++element;
                    }

                    // Allow for a pooled equipment constraint to equal zero allowing users
                    // being prohibited from using a particular RF
                    if ( (numberOfPooledEquipment >= 0) &&
                            (ptrPooledEquipmentUsers->Size() > numberOfPooledEquipment) ) {


                        if (ptrPooledEquipmentList == NULL) {
                            ptrPooledEquipmentList = new LinkList<PooledEquipmentPeriod>();
                        }

                        ptrPooledEquipmentList->Append(ptrPooledEquipment);
                        ptrPooledEquipment = NULL;
                    }

                    delete ptrPooledEquipmentUsers;
                    ptrPooledEquipmentUsers = NULL;
                }
            } else if (ptrInputData->LabelContainsString("Precluded User Vehicle") == true) {
                element = 0;
                numberOfElements = ptrInputData->GetNumberOfValues();

                while (element < numberOfElements) {
                    userDesignator = ptrInputData->GetStringValue(element);

                    if (MURALInputProcessor::ValidUserVehicle(userDesignator) == true) {
                        if (ptrPrecludedUserArray == NULL) {
                            ptrPrecludedUserArray = new Array<string>();
                        }

                        ptrPrecludedUserArray->AddUniqueElement(userDesignator);
                    }

                    ++element;
                }
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "ExtractReceiveFacilityAntennaInfo(InputDataElement*)");
        throw;
    }

    // Set the arrays on the antenna
    ptrRF->SetPooledEquipmentList(ptrPooledEquipmentList);
    ptrRF->SetPrecludedUserArray(ptrPrecludedUserArray);
}

/* ******************************************************************************* */

//CPPCHECK_RELATED
//Removed CheckAntennaInputs() since it wasn't called in code base
/*
void
AntennaExtractor::CheckAntennaInputs(const shared_ptr<Antenna> &ptrAntenna)
{
    string                  errorString = "";
    Antenna::FREQUENCY_BAND frequencyBand = ptrAntenna->GetFrequencyBand();
    int                     capacity = ptrAntenna->GetCapacity();
    int                     preferencedCapacity = ptrAntenna->GetPreferencedCapacity();
    int                     preparationTime = ptrAntenna->GetPreparationTime();
    double                  dataRate = ptrAntenna->GetDataRate();

    if (frequencyBand == Antenna::UNKNOWN_BAND) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Communication Frequency Band' input";
        errorString += "\n         Valid: X_BAND, MA_X_BAND, S_BAND, MA_S_BAND, V_BAND, K_BAND, MA_K_BAND,";
        errorString += "\n                KU_BAND, MA_KU_BAND, KA_BAND, MA_KA_BAND, UHF, VHF, EHF, LASER, MA_LASER";
    }

    if (capacity <= 0) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Capacity' input";
    } else if (preferencedCapacity > capacity) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Invalid 'Preferenced Antenna' input";
    }

    if (preparationTime < 0) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Invalid 'Preparation Time' input";
    }

    if (dataRate <= 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n        ";
        }

        errorString += "Missing/invalid 'Data Rate' input";
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "ExtractAntennaInfo()", errorString);
    }

    // Now Check the Azimuth Angle data
    CheckAzimuthAngleData(ptrAntenna);
}
*/

/* ********************************************************************************************* */

void
AntennaExtractor::CheckUTAInputs(const shared_ptr<UserTransmitAntenna> &ptrUTA)
{
    const shared_ptr<vector<string>>   ptrPrecludedReceiverArray = ptrUTA->GetPrecludedReceiverArray();
    const shared_ptr<vector<string>>   ptrDedicatedTransmitterArray = ptrUTA->GetDedicatedTransmitterArray();
    string                             relayDesignator = "";

    // Need to implement a mechinisim to delete/clear the ptrDedicatedTransmitterArray_a if
    // there are ones present (UTA's cant transmit)
    if ( ptrDedicatedTransmitterArray != NULL ) {
        ptrDedicatedTransmitterArray->clear();
    }

    // Check to make sure that the relays are valid
    if (ptrPrecludedReceiverArray != NULL) {
        int index = 0;

        while (index < (int)ptrPrecludedReceiverArray->size()) {
            relayDesignator = ptrPrecludedReceiverArray->at(index);

            if (MURALInputProcessor::ValidRelaySatellite(relayDesignator) == false) {
                throw new InputException(GetClassName(),
                                         "ExtractUserTransmitAntennaInfo(InputDataElement*)",
                                         ("Invalid precluded receiver " + relayDesignator));
            }

            ++index;
        }
    }
}

/* ********************************************************************************************* */

void
AntennaExtractor::CheckDDAInputs(const shared_ptr<DirectDownlinkAntenna> &ptrDDA)
{
    string                            groundDesignator = "";
    const shared_ptr<vector<string>>  ptrPrecludedReceiverArray = ptrDDA->GetPrecludedReceiverArray();

    // CHeck the Precluded Receiver array for valid ground stations
    if (ptrPrecludedReceiverArray != NULL) {
        int element = 0;

        while (element < (int)ptrPrecludedReceiverArray->size()) {
            groundDesignator = ptrPrecludedReceiverArray->at(element);

            if (MURALInputProcessor::ValidGroundStation(groundDesignator) == false) {
                throw new InputException(GetClassName(),
                                         "DirectDownlinkAntenna(InputDataElement*, UserVehicle*)",
                                         ("Invalid precluded receiver " + groundDesignator));
            }

            ++element;
        }
    }
}

/* ********************************************************************************************* */

void
AntennaExtractor::CheckSRAInputs(const shared_ptr<SelfRelayAntenna> &ptrSRA)
{
    string                             userDesignator = "";
    const shared_ptr<vector<string>>   ptrPrecludedReceiverArray = ptrSRA->GetPrecludedReceiverArray();
    AntennaBase::ANTENNA_CAPABILITY    antennaCapability = ptrSRA->GetAntennaCapability();

    if (antennaCapability == AntennaBase::UNKNOWN_CAPABILITY) {
        throw new InputException(GetClassName(), "ExtractSelfRelayAntennaInfo(InputDataElement*)",
                                 "Missing/invalid 'Self Relay Antenna Capability' input : TRANS, RECV, or DUPLEX");
    }

    // Check the Precluded Receiver array for valid users
    if (ptrPrecludedReceiverArray != NULL) {
        int element = 0;

        while (element < (int)ptrPrecludedReceiverArray->size()) {
            userDesignator = ptrPrecludedReceiverArray->at(element);

            if (MURALInputProcessor::ValidUserVehicle(userDesignator) == false) {
                throw new InputException(GetClassName(),
                                         "SelfRelayAntenna(InputDataElement*, UserVehicle*)",
                                         ("Invalid precluded receiver " + userDesignator));
            }

            ++element;
        }
    }
}

/* ********************************************************************************************* */

void
AntennaExtractor::CheckUPAInputs(const shared_ptr<UserPointingAntenna> &ptrUPA)
{
    string                            userDesignator = "";
    const string                      operationName = "UserPointingAntenna(InputDataElement*, RelaySatellite*, int)";
    Antenna::FREQUENCY_BAND           frequencyBand = ptrUPA->GetFrequencyBand();
    int                               numberOfTransmitChannels = ptrUPA->GetNumberOfTransmitChannels();
    const shared_ptr<vector<string>>  ptrDedicatedTransmitterArray = ptrUPA->GetDedicatedTransmitterArray();
    const shared_ptr<vector<string>>  ptrPrecludedRecieverArray = ptrUPA->GetPrecludedReceiverArray();

    if ( (frequencyBand >= Antenna::LASER) &&
            (numberOfTransmitChannels > 1) ) {
        throw new InputException(GetClassName(),
                                 "AntennaExtractor::CheckUPAInputs()",
                                 "LASER_COMM relay antenna (RPA or GPA) can have at most ONE transmit channel");
    }

    if (numberOfTransmitChannels > 0) {
        throw new InputException(GetClassName(), operationName,
                                 "Can NOT define 'Number Of Transmit Channels' input on a USER_POINTING_ANTENNA");
    }

    if (ptrUPA->GetMinimumGrazingAltitude(DEFAULT_STRING) == UNDEFINED_ANGLE) {
        throw new InputException(GetClassName(), operationName,
                                 "Missing 'Default Minimum Grazing Altitude' input");
    }

    // a UPA can never have a precluded receiver
    // Need to implement a mechinisim to delete/clear the ptrPrecludedReceiverArray_a if
    // there are ones present (UPA's cant receive)
    if ( ptrPrecludedRecieverArray != NULL ) {
        ptrPrecludedRecieverArray->clear();
    }

    // Check the Dedicated Transmitter Array for valid users
    if (ptrDedicatedTransmitterArray != NULL) {
        int element = 0;

        while (element < (int)ptrDedicatedTransmitterArray->size()) {
            userDesignator = ptrDedicatedTransmitterArray->at(element);

            if (MURALInputProcessor::ValidUserVehicle(userDesignator) == false) {
                throw new InputException(GetClassName(), operationName,
                                         ("Invalid dedicated transmitter (user) " + userDesignator));
            }

            ++element;
        }

        if ((int)ptrDedicatedTransmitterArray->size() > ptrUPA->GetMinimumCapacity()) {
            throw new InputException(GetClassName(), operationName,
                                     "Input too many dedicated transmitters");
        }
    }
}

/* ********************************************************************************************* */

void
AntennaExtractor::CheckGPAInputs(const shared_ptr<GroundPointingAntenna> &ptrGPA)
{
    Antenna::FREQUENCY_BAND frequencyBand = ptrGPA->GetFrequencyBand();
    int numberOfTransmitChannels          = ptrGPA->GetNumberOfTransmitChannels();

    if ( (frequencyBand >= Antenna::LASER) &&
            (numberOfTransmitChannels > 1) ) {
        throw new InputException(GetClassName(),
                                 "AntennaExtractor::CheckGPAInputs()",
                                 "LASER_COMM relay antenna (RPA or GPA) can have at most ONE transmit channel");
    }
}

/* ********************************************************************************************* */
void
AntennaExtractor::CheckRPAInputs(const shared_ptr<RelayPointingAntenna> &ptrRPA)
{
    Antenna::FREQUENCY_BAND         frequencyBand = ptrRPA->GetFrequencyBand();
    int                             numberOfTransmitChannels = ptrRPA->GetNumberOfTransmitChannels();
    AntennaBase::ANTENNA_CAPABILITY antennaCapability = ptrRPA->GetAntennaCapability();

    if ( (frequencyBand >= Antenna::LASER) &&
            (numberOfTransmitChannels > 1) ) {
        throw new InputException(GetClassName(),
                                 "AntennaExtractor::CheckRPAInputs()",
                                 "LASER_COMM relay antenna (RPA or GPA) can have at most ONE transmit channel");
    }

    if (antennaCapability == AntennaBase::UNKNOWN_CAPABILITY) {
        throw new InputException(GetClassName(), "ExtractRelayPointingAntennaInfo(InputDataElement*)",
                                 "Missing/invalid 'Relay Pointing Antenna Capability' input : TRANS, RECV, or DUPLEX");
    }
}

/* ********************************************************************************************* */

void
AntennaExtractor::CheckRFInputs(const shared_ptr<ReceiveFacilityAntenna> &ptrRF)
{
    string                            transmitterDesignator = "";
    const shared_ptr<vector<string>>  ptrDedicatedTransmitterArray = ptrRF->GetDedicatedTransmitterArray();
    const shared_ptr<vector<string>>  ptrPrecludedReceiverArray = ptrRF->GetPrecludedReceiverArray();

    // a RF can never have a precluded receiver
    // Need to implement a mechinisim to delete/clear the ptrPrecludedReceiverArray_a if
    // there are ones present (RF's cant receive)
    if ( ptrPrecludedReceiverArray != NULL ) {
        ptrPrecludedReceiverArray->clear();
    }

    // Check the dedicated transmitter for valid transmitters (Relay GPA's, User DDA's)
    if (ptrDedicatedTransmitterArray != NULL) {
        int element = 0;

        while (element < (int)ptrDedicatedTransmitterArray->size()) {
            transmitterDesignator = ptrDedicatedTransmitterArray->at(element);

            if (MURALInputProcessor::ValidTransmitter(transmitterDesignator) == false) {
                throw new InputException(GetClassName(),
                                         "ReceiveFacilityAntenna(InputDataElement*, GroundStation*, int)",
                                         ("Invalid dedicated transmitter " + transmitterDesignator));
            }

            ++element;
        }

        if ((int)ptrDedicatedTransmitterArray->size() > ptrRF->GetMinimumCapacity()) {
            throw new InputException(GetClassName(),
                                     "ReceiveFacilityAntenna(InputDataElement*, GroundStation*, int)",
                                     "Input too many dedicated transmitters");
        }
    }
}

/* ********************************************************************************************* */

//CPPCHECK_RELATED
//Removed CheckAzimuthAngleData() since it wasn't called in code base
/*
void
AntennaExtractor::CheckAzimuthAngleData(const shared_ptr<Antenna> &ptrAntenna) const
{
    string                                          errorString          = "";



    const double                                    halfCircle = UnitData::GetStoreHalfCircle();
    shared_ptr<AzimuthAngleData>                    ptrAzimuthAngleData = NULL;
    shared_ptr<MURALTypes::AzimuthAngleDataVector>  ptrAzimuthAngleDataList = ptrAntenna->GetAzimuthAngleDataList();
    MURALTypes::AzimuthAngleDataVector::iterator    azimuthDataIter;
    ptrAzimuthAngleData = *(ptrAzimuthAngleDataList->begin());

    if (ptrAzimuthAngleData->GetAzimuthAngle() != -halfCircle) {
        throw new InputException(GetClassName(), "CheckAzimuthAngleData(const double&, const double&)",
                                 "Must define starting azimuth angle (-180 deg)");
    }

    ptrAzimuthAngleData = *(ptrAzimuthAngleDataList->end());

    if (ptrAzimuthAngleData->GetAzimuthAngle() != halfCircle) {
        throw new InputException(GetClassName(), "CheckAzimuthAngleData(const double&, const double&)",
                                 "Must define ending azimuth angle (180 deg)");
    }

    if ( ptrAzimuthAngleDataList != NULL ) {




		double previousAzimuthAngle;

        previousAzimuthAngle = (*ptrAzimuthAngleDataList->end())->GetAzimuthAngle();

        for( azimuthDataIter = ptrAzimuthAngleDataList->begin(); azimuthDataIter != ptrAzimuthAngleDataList->end(); ++azimuthDataIter ) {
            ptrAzimuthAngleData = (*azimuthDataIter);
            double minimumMinElev = ptrAzimuthAngleData->GetMinimumElevationAngle();
            double maximumMaxElev = ptrAzimuthAngleData->GetMaximumElevationAngle();
            double azimuthAngle = ptrAzimuthAngleData->GetAzimuthAngle();

            if (minimumMinElev > maximumMaxElev) {
                throw new InputException(GetClassName(), "SetAzimuthAngleData(const InputData*)",
                                         "Minimum is greater than maximum elevation angle for 'Azimuth Based Elevation Angles' input");
            }

            if (previousAzimuthAngle >= azimuthAngle) {
                errorString = "Invalid 'Azimuth Based Elevation Angles' input, Az angle : ";
                errorString += StringEditor::ConvertToString(UnitData::GetInputAngle(azimuthAngle));
                errorString += " must be greater then the previous Az angle of : ";
                errorString += StringEditor::ConvertToString(UnitData::GetInputAngle(previousAzimuthAngle));
                throw new InputException(GetClassName(), "SetAzimuthAngleData(const InputData*)", errorString);
            }

            if (ptrAzimuthAngleData->GetMaximumElevationAngle() > maximumMaxElev) {
                throw new InputException(GetClassName(), "CheckAzimuthAngleData(const double&, const double&)",
                                         "Maximum azimuth based elevation angle exceeds maximum elevation angle");
            }

            if (ptrAzimuthAngleData->GetMinimumElevationAngle() < minimumMinElev) {
                throw new InputException(GetClassName(), "CheckAzimuthAngleData(const double&, const double&)",
                                         "Maximum azimuth based elevation angle exceeds maximum elevation angle");
            }

            previousAzimuthAngle = azimuthAngle;
        }
    }

    return;
}
*/

/* ********************************************************************************************* */

Antenna::FREQUENCY_BAND
AntennaExtractor::GetFrequencyBandValue(const string &type) const
{
    Antenna::FREQUENCY_BAND frequencyBand = Antenna::UNKNOWN_BAND;

    if (type == "X_BAND") {
        frequencyBand = Antenna::X_BAND;
    } else if (type == "MA_X_BAND") {
        frequencyBand = Antenna::MA_X_BAND;
    } else if (type == "S_BAND") {
        frequencyBand = Antenna::S_BAND;
    } else if (type == "MA_S_BAND") {
        frequencyBand = Antenna::MA_S_BAND;
    } else if (type == "V_BAND") {
        frequencyBand = Antenna::V_BAND;
    } else if (type == "K_BAND") {
        frequencyBand = Antenna::K_BAND;
    } else if (type == "MA_K_BAND") {
        frequencyBand = Antenna::MA_K_BAND;
    } else if (type == "KU_BAND") {
        frequencyBand = Antenna::KU_BAND;
    } else if (type == "MA_KU_BAND") {
        frequencyBand = Antenna::MA_KU_BAND;
    } else if (type == "KA_BAND") {
        frequencyBand = Antenna::KA_BAND;
    } else if (type == "MA_KA_BAND") {
        frequencyBand = Antenna::MA_KA_BAND;
    } else if (type == "UHF") {
        frequencyBand = Antenna::UHF;
    } else if (type == "VHF") {
        frequencyBand = Antenna::VHF;
    } else if (type == "EHF") {
        frequencyBand = Antenna::EHF;
    } else if (type == "LASER") {
        frequencyBand = Antenna::LASER;
    } else if (type == "MA_LASER") {
        frequencyBand = Antenna::MA_LASER;
    }

    return(frequencyBand);
}

/* ************************************************************************************ */

inline
AntennaBase::ANTENNA_CAPABILITY
AntennaExtractor::GetAntennaCapabilityValue(const string &type) const
{
    AntennaBase::ANTENNA_CAPABILITY returnCapability = AntennaBase::UNKNOWN_CAPABILITY;

    if (type == "TRANS") {
        returnCapability = AntennaBase::TRANSMIT;
    } else if (type == "RECV") {
        returnCapability = AntennaBase::RECEIVE;
    } else if (type == "DUPLEX") {
        returnCapability = AntennaBase::DUPLEX;
    }

    return(returnCapability);
}

/* *********************************************************************************** */
AntennaExtractor::~AntennaExtractor()
{
    return;
}

