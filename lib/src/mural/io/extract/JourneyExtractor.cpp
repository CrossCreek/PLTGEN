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
 * JourneyExtractor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "JourneyExtractor.h"
#include "mural/modules/pg/Orbit.h"
#include "mural/modules/pg/Trek.h"
#include "mural/io/parse/OPSStateVectorParser.h"

#include "general/time/Calendar.h"
#include "general/time/TimePiece.h"
#include "general/math/SpaceVector.h"
#include "general/parse/ParseUtility.h"
#include "general/data/UnitData.h"
#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"
//RAGUSA 7/24/2017 - Needed to resolve min/max function removal from std library.
#include <algorithm>
JourneyExtractor::JourneyExtractor(const string &opsDesignator, bool dataCollector) : dataCollector_a(dataCollector),
    opsDesignator_a(opsDesignator)
{
    return;
}

/* ************************************************************************************* */

shared_ptr<Journey>
JourneyExtractor::ExtractJourney(const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    shared_ptr<Journey> ptrJourney;
    bool isOrbitingDataProvider = false;
    bool foundOrbitingInput = false;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData> ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Orbiting Data Provider") == true) {
            isOrbitingDataProvider = ptrInputData->GetBoolValue();
            foundOrbitingInput = true;
        }
    }

    if (foundOrbitingInput == false) {
        throw new InputException(GetClassName(),
                                 "IsOrbitingDataProvider(InputDataElement*)",
                                 "Missing/invalid 'Orbiting Data Provider' input");
    }

    // Parse according to the data's type
    if ( isOrbitingDataProvider == true ) {
        ptrJourney = ExtractOrbitInfo(ptrInputDataElement);
    } else {
        ptrJourney = ExtractTrekInfo(ptrInputDataElement);
    }

    return ptrJourney;
}

/* ************************************************************************************* */

shared_ptr<Orbit>
JourneyExtractor::ExtractOrbitInfo(const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    int                  index = 0;
    const int            firstTimeIndex = 0;
    double                stateX = 0.0;
    double                stateY = 0.0;
    double                stateZ = 0.0;
    bool                  useInitialStateVectors = false;
    bool                  foundStatePositionVector = false;
    bool                  foundStateVelocityVector = false;
    string                epochDateString = "";
    string                epochTimeString = "";
    string                errorString = "";
    string                referenceString = "";
    string                stateFormatString = "";
    REFERENCE_FRAME       inputFrame = UNKNOWN_FRAME;
    STATE_VECTOR_FORMAT   inputPositionSVFormat = OSCULATING;
    STATE_VECTOR_FORMAT   inputVelocitySVFormat = OSCULATING;
    STATE_VECTOR_FORMAT   stateVectorFormat = OSCULATING;
    const REFERENCE_FRAME storeFrame = UnitData::GetInternalReferenceFrame();
    const ANGLE_UNIT      inputAngle = UnitData::GetInputAngleUnits();
    const DISTANCE_UNIT   inputDistance = UnitData::GetInputDistanceUnits();
    SpaceVector           statePositionVector;
    SpaceVector           stateVelocityVector;
    shared_ptr<OPSStateVectorParser>  ptrOPSStateParser = OPSStateVectorParser::GetStateVectorParser();
    shared_ptr<Orbit>     ptrOrbit = shared_ptr<Orbit>(new Orbit());
    // Values to set on Orbit
    double                epochTime = 0.0;
    double                semiMajorAxis = 0.0;
    double                eccentricity = 0.0;
    double                inclination = 0.0;
    double                rightAscension = 0.0;
    double                argumentOfPerigee = 0.0;
    double                trueAnomaly = 0.0;
    double                meanAnomaly = 0.0;
    int                   revolutionNumberOffset = -1;
    STATE_VECTOR_FORMAT   readStateVectorFormat = MEAN;
    Journey::POSITION_METHOD     positionGenerationMethod = Journey::UNKNOWN_POSITION_METHOD;
    Orbit::REVOLUTION_BOUNDARY   revolutionBoundary = Orbit::UNKNOWN_BOUNDARY;
    // EXTRACTING
    // ---------------------
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData> ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Position Generation Method") == true) {
            positionGenerationMethod = Journey::ConvertPositionMethod(ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Epoch Date") == true) {
            epochDateString = ptrInputData->GetStringValue();
        } else if (ptrInputData->LabelContainsString("Epoch Time") == true) {
            epochTimeString = ptrInputData->GetStringValue();
        } else if (ptrInputData->LabelContainsString("Mean/Osculating Definition") == true) {
            stateFormatString = ptrInputData->GetStringValue();
            readStateVectorFormat = Orbit::ConvertStateVectorFormat(stateFormatString);
        } else if (ptrInputData->LabelContainsString("Semi-Major Axis") == true) {
            semiMajorAxis = UnitData::GetInternalDistance(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Eccentricity") == true) {
            eccentricity = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Inclination") == true) {
            inclination = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Argument of Perigee") == true) {
            argumentOfPerigee = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Right Ascension") == true) {
            rightAscension = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
            //Adding capability to input Mean Anomaly- RAGUSA 7/20/2017********************************************
        } else if (ptrInputData->LabelContainsString("True Anomaly") == true) {
            trueAnomaly = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
            ptrOrbit->SetTrueAnomaly(trueAnomaly);
            ptrOrbit->SetMeanAnomaly(Orbit::AnomalyConverter(trueAnomaly, Orbit::TRUE_TO_MEAN, eccentricity));
        } else if (ptrInputData->LabelContainsString("Mean Anomaly") == true) {
            meanAnomaly = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
            ptrOrbit->SetMeanAnomaly(meanAnomaly);
            ptrOrbit->SetTrueAnomaly(Orbit::AnomalyConverter(meanAnomaly, Orbit::MEAN_TO_TRUE, eccentricity));
            //*****************************************************************************************************
        } else if (ptrInputData->LabelContainsString("Use Initial State Vectors") == true) {
            useInitialStateVectors = ptrInputData->GetBoolValue();
        } else if (ptrInputData->LabelContainsString("Initial State Position Vector") == true) {
            index = -1;
            stateX = ptrInputData->GetDoubleValue(++index);
            stateY = ptrInputData->GetDoubleValue(++index);
            stateZ = ptrInputData->GetDoubleValue(++index);
            referenceString = ptrInputData->GetStringValue(++index);

            // Read in the State Vector Format (if present)
            if ( ptrInputData->GetNumberOfValues() > 4 ) {
                stateFormatString = ptrInputData->GetStringValue(++index);
                inputPositionSVFormat = Orbit::ConvertStateVectorFormat(stateFormatString);
            }

            inputFrame = UnitData::GetReferenceFrame(referenceString);

            if (inputFrame == UNKNOWN_FRAME) {
                throw new InputException(GetClassName(),
                                         "ExtractOrbitInfo(InputDataElement*)",
                                         "Unknown reference frame for 'Initial State Position Vector' input");
            }

            statePositionVector = SpaceVector(stateX, stateY, stateZ,
                                              inputFrame, inputAngle, inputDistance);
            foundStatePositionVector = true;
        } else if (ptrInputData->LabelContainsString("Initial State Velocity Vector") == true) {
            index = -1;
            stateX = ptrInputData->GetDoubleValue(++index);
            stateY = ptrInputData->GetDoubleValue(++index);
            stateZ = ptrInputData->GetDoubleValue(++index);
            referenceString = ptrInputData->GetStringValue(++index);

            // Read in the State Vector Format (if present)
            if ( ptrInputData->GetNumberOfValues() > 4 ) {
                stateFormatString = ptrInputData->GetStringValue(++index);
                inputVelocitySVFormat = Orbit::ConvertStateVectorFormat(stateFormatString);
            }

            inputFrame = UnitData::GetReferenceFrame(referenceString);

            if (inputFrame == UNKNOWN_FRAME) {
                throw new InputException(GetClassName(),
                                         "ExtractOrbitInfo(InputDataElement*)",
                                         "Unknown reference frame for 'Initial State Velocity Vector' input");
            }

            stateVelocityVector = SpaceVector(stateX, stateY, stateZ,
                                              inputFrame, inputAngle, inputDistance);
            foundStateVelocityVector = true;
        } else if (ptrInputData->LabelContainsString("Revolution Boundary") == true) {
            revolutionBoundary = Orbit::ConvertRevolutionBoundary(ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Revolution Number Offset") == true) {
            revolutionNumberOffset = ptrInputData->GetIntValue();
        }
    }

    // ERROR CHECKING
    // ----------------------------------------------
    if (positionGenerationMethod == Journey::UNKNOWN_POSITION_METHOD) {
        throw new InputException(GetClassName(), "ExtractJourneyInfo(InputDataElement*)",
                                 "Missing/invalid 'Position Generation Method' input");
    }

    if (epochDateString.size() == 0) {
        errorString = "Missing 'Epoch Date' input";
    }

    if (epochTimeString.size() == 0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing 'Epoch Time' input";
    }

    if (useInitialStateVectors == true) {
        if (foundStatePositionVector == false) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing 'Initial State Position Vector' input";
        }

        if (foundStateVelocityVector == false) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing 'Initial State Velocity Vector' input";
        }

        if ( inputPositionSVFormat == UNKNOWN_FORMAT ) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Unknown 'State Vector Format' for Position";
        }

        if ( inputVelocitySVFormat == UNKNOWN_FORMAT ) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Unknown 'State Vector Format' for Velocity";
        }

        if ( inputPositionSVFormat != inputVelocitySVFormat ) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "State Vector Formats are different for Position and Velocity";
        }
    }

    if (dataCollector_a == true) {
        if (revolutionBoundary == Orbit::UNKNOWN_BOUNDARY) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing 'Revolution Boundary' input";
        }

        if (revolutionNumberOffset < 0) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing 'Revolution Number Offset' input";
        }
    }

    if ( (positionGenerationMethod == Journey::READ_OPS) && (ptrOPSStateParser == NULL) ) {
        errorString += "Missing 'OPS State Vector Input File' input file.";
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "ExtractOrbitInfo(InputDataElement*)",
                                 errorString);
    }

    // CONVERSION FROM STATE_VECTORS TO CLASSICAL
    // --------------------------------------------
    try {
        // Set the state vector format if using initial state vectors
        if (useInitialStateVectors == true) {
            // The error check above ensures that the position and velocity formats are the same
            // so I can assume the postion state vector format
            stateVectorFormat = inputPositionSVFormat;
        }

        // if reading in positions, the epoch time is the first 'time step' of the
        // simulation, else base it off the input epoch date/time
        if ( (positionGenerationMethod == Journey::READ_MURAL) ||
                (positionGenerationMethod == Journey::READ_P470) ) {
            epochTime = TimePiece::GetJ2000Time(firstTimeIndex);
            useInitialStateVectors = false;
        } else if (positionGenerationMethod == Journey::READ_OPS) {
            useInitialStateVectors = true;
            // OPS state vector formats are osculating
            stateVectorFormat = OSCULATING;
            epochTime = ptrOPSStateParser->GetJ2000Epoch(opsDesignator_a);

            // Check to see if the parser found a valid epoch, if it did then get the position and veclocity vectors
            if ( epochTime > 0 ) {
                statePositionVector = ptrOPSStateParser->GetPosition(opsDesignator_a);
                stateVelocityVector = ptrOPSStateParser->GetVelocity(opsDesignator_a);
            } else {
                throw new InputException(GetClassName(), "ExtractOrbitInfo(InputDataElement*)",
                                         "Did not find designator " + opsDesignator_a + " in the 'OPS State Vector Input File'");
            }
        } else {
            epochTime = Calendar(epochDateString, epochTimeString).GetJ2000Time();
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractOrbitInfo(InputDataElement*) - Converting Mean/Osculating");
        throw;
    }

    // SETTING
    // ------------------------------------------
    try {
        // Set the values on the Orbit
        ptrOrbit->SetPositionGenerationMethod(positionGenerationMethod);
        ptrOrbit->SetEpochTime(epochTime);
        ptrOrbit->SetSemiMajorAxis(semiMajorAxis);
        ptrOrbit->SetEccentricity(eccentricity);
        ptrOrbit->SetInclination(inclination);
        ptrOrbit->SetRightAscension(rightAscension);
        ptrOrbit->SetArgumentOfPerigee(argumentOfPerigee);
        //Already set when extracted - RAGUSA 7/20/2017
        //ptrOrbit->SetTrueAnomaly(trueAnomaly);
        ptrOrbit->SetRevolutionBoundary(revolutionBoundary);
        ptrOrbit->SetRevolutionNumberOffset(revolutionNumberOffset);
        ptrOrbit->SetStateVectorFormat(readStateVectorFormat);

        // Compute classical if state vectors are used
        if (useInitialStateVectors == true) {
            ptrOrbit->CalculateClassicalElements(statePositionVector, stateVelocityVector, stateVectorFormat);
        }

        // Compute the mean anomaly  - Removed and Added above to allow Mean Anomaly Input. RAGUSA 7/20/2017
        //ptrOrbit->SetMeanAnomaly(Orbit::AnomalyConverter(trueAnomaly, Orbit::TRUE_TO_MEAN, eccentricity));
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractOrbitInfo(InputDataElement*) - Setting values and converting to classical");
        throw;
    }

    return ptrOrbit;
}

/* *************************************************************************** */

shared_ptr<Trek>
JourneyExtractor::ExtractTrekInfo(const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    int                          initialLayover = -1;
    double                       latitude = UNDEFINED_ANGLE;
    double                       longitude = UNDEFINED_ANGLE;
    double                       altitude = UNDEFINED_ANGLE;
    const double                 quarterCircle = UnitData::GetStoreQuarterCircle();
    Journey::POSITION_METHOD     positionGenerationMethod = Journey::UNKNOWN_POSITION_METHOD;
    string                       errorString = "";
    shared_ptr<string>           ptrTrekFilename;
    shared_ptr<Trek>             ptrTrek;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData> ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Position Generation Method") == true) {
            positionGenerationMethod = Journey::ConvertPositionMethod(ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Latitude") == true) {
            latitude = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Longitude") == true) {
            longitude = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Altitude") == true) {
            altitude = UnitData::GetInternalDistance(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Initial Layover Time Steps") == true) {
            initialLayover = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Trek File Name") == true) {
            ptrTrekFilename = shared_ptr<string>(new string(ptrInputData->GetStringValue()));
        }
    }

    if ((latitude < -quarterCircle) || (latitude > quarterCircle)) {
        errorString = "Missing/invalid 'Latitude' input";
    }

    if ((longitude < 0) || (longitude > (4.0 * quarterCircle))) {
        if (errorString.size() > 0) {
            errorString += "\n       ";
        }

        errorString += "Missing/invalid 'Longitude' input";
    }

    if (altitude == UNDEFINED_ANGLE) {
        if (errorString.size() > 0) {
            errorString += "\n       ";
        }

        errorString += "Missing 'Altitude' input";
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "ExtractTrekInfo(InputDataElement&)",
                                 errorString);
    }

    // Set the values on Trek
    ptrTrek = shared_ptr<Trek>(new Trek());
    ptrTrek->SetPositionGenerationMethod(positionGenerationMethod);
    ptrTrek->SetInitialLayover(initialLayover);
    ptrTrek->SetLatitude(latitude);
    ptrTrek->SetLongitude(longitude);
    ptrTrek->SetAltitude(altitude);
    ptrTrek->SetTrekFileName(ptrTrekFilename);
    return ptrTrek;
}

/* ********************************************************************************************* */

JourneyExtractor::~JourneyExtractor()
{
    return;
}

