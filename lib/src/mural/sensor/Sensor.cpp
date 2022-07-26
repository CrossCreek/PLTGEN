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
 * Sensor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "Sensor.h"
#include "mural/Sun.h"
#include "mural/modules/ts/Target.h"

#include "general/math/SpaceVector.h"
#include "general/parse/ParseUtility.h"
#include "general/array/ListIterator.h"
#include "general/time/TimePiece.h"

#include "mural/Region.h"
#include "mural/io/extract/PatternExtractor.h"

//------------------------------------------------------------------------------
//
//  Method Name: Sensor::Sensor
//
//  Purpose:
//    Constructor for the Sensor class.
//
//  Calling Method:
//     Sensor();
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

Sensor::Sensor()
    : minimumPowerDownTime_a(0), sensorWeightFactor_a(0.0), dataRate_a(0.0),
      minimumSunElevationAngle_a(UNDEFINED_ANGLE),
      maximumSunElevationAngle_a(UNDEFINED_ANGLE),
      ptrRequirementDeckFileName_a(NULL), sensorType_a(UNKNOWN),
      dailyDutyCycle_a(0), ptrRollingRevDutyCycle_a(NULL),
      ptrAccessPattern_a(NULL)
{
    return;
}

Sensor::Sensor(shared_ptr<InputDataElement> ptrInputDataElement,
               const SENSOR_TYPE &type)
    : minimumPowerDownTime_a(0), sensorWeightFactor_a(0.0), dataRate_a(0.0),
      minimumSunElevationAngle_a(UNDEFINED_ANGLE),
      maximumSunElevationAngle_a(UNDEFINED_ANGLE),
      ptrRequirementDeckFileName_a(NULL), sensorType_a(type), dailyDutyCycle_a(0),
      ptrRollingRevDutyCycle_a(NULL), ptrAccessPattern_a(NULL)
{
    try {
        ExtractSensorInfo(ptrInputDataElement);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "Sensor(InputDataElement*, const SENSOR_TYPE&)");
        throw;
    }

    return;
}

Sensor::Sensor(const Sensor &copyMe)
    : minimumPowerDownTime_a(copyMe.minimumPowerDownTime_a),
      sensorWeightFactor_a(copyMe.sensorWeightFactor_a),
      dataRate_a(copyMe.dataRate_a),
      minimumSunElevationAngle_a(copyMe.minimumSunElevationAngle_a),
      maximumSunElevationAngle_a(copyMe.maximumSunElevationAngle_a),
      sensorType_a(copyMe.sensorType_a),
      ptrRequirementDeckFileName_a(NULL),
      dailyDutyCycle_a(copyMe.dailyDutyCycle_a),
      ptrRollingRevDutyCycle_a(NULL),
      ptrAccessPattern_a(NULL)
{
    if (copyMe.ptrRequirementDeckFileName_a != NULL) {
        ptrRequirementDeckFileName_a
            = shared_ptr<string>(new string(*(copyMe.ptrRequirementDeckFileName_a)));
    }

    if (copyMe.ptrRollingRevDutyCycle_a != NULL) {
        ptrRollingRevDutyCycle_a
            = new Array<int>(*(copyMe.ptrRollingRevDutyCycle_a));
    }

    if (copyMe.ptrAccessPattern_a != NULL) {
        ptrAccessPattern_a = shared_ptr<AccessPattern>(copyMe.ptrAccessPattern_a);
    }

    return;
}

Sensor&
Sensor::operator = (const Sensor &copyMe)
{
    minimumSunElevationAngle_a = copyMe.minimumSunElevationAngle_a;
    maximumSunElevationAngle_a = copyMe.maximumSunElevationAngle_a;
    return(*this);
}

bool
Sensor::operator == (const Sensor &compareMe) const
{
    return ( (minimumPowerDownTime_a == compareMe.minimumPowerDownTime_a) &&
             (sensorWeightFactor_a == compareMe.sensorWeightFactor_a) &&
             (dataRate_a == compareMe.dataRate_a) &&
             (minimumSunElevationAngle_a == compareMe.minimumSunElevationAngle_a) &&
             (maximumSunElevationAngle_a == compareMe.maximumSunElevationAngle_a) &&
             (sensorType_a == compareMe.sensorType_a) &&
             (dailyDutyCycle_a == compareMe.dailyDutyCycle_a) &&
             (*ptrAccessPattern_a == *(compareMe.ptrAccessPattern_a)) );
}

bool
Sensor::operator != (const Sensor &compareMe) const
{
    return(!(operator == (compareMe)));
}

bool
Sensor::operator < (const Sensor &compareMe) const
{
    return(sensorType_a < compareMe.sensorType_a);
}

void
Sensor::ExtractSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    int                   rev = 0;
    int                   numberOfRevs = 0;
    const double            QUARTER_CIRCLE = UnitData::GetStoreQuarterCircle();
    string                  errorString = "";
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    shared_ptr<InputDataElement::InputDataElementList> subElementList = ptrInputDataElement->GetInputDataElementList();
    InputDataElement::InputDataList::iterator inputDataIter;
    InputDataElement::InputDataElementList::iterator subElementIter;
    shared_ptr<InputData> ptrInputData = NULL;
    shared_ptr<InputDataElement>      ptrSubElement = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Collection Data Rate") == true) {
            dataRate_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Minimum Sun Elevation Angle") == true) {
            minimumSunElevationAngle_a
                = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Maximum Sun Elevation Angle") == true) {
            maximumSunElevationAngle_a
                = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Daily Sensor Duty Cycle") == true) {
            dailyDutyCycle_a
                = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Consecutive Rev Duty Cycle") == true) {
            if (ptrInputData->GetSpecificIntNumber(numberOfRevs) == true) {
                if (ptrRollingRevDutyCycle_a == NULL) {
                    ptrRollingRevDutyCycle_a = new Array<int>(numberOfRevs, LONG_ZERO);
                } else if (numberOfRevs > ptrRollingRevDutyCycle_a->Size()) {
                    rev = ptrRollingRevDutyCycle_a->Size();

                    while (rev < numberOfRevs) {
                        ptrRollingRevDutyCycle_a->AddElement(LONG_ZERO);
                        ++rev;
                    }
                }

                (*ptrRollingRevDutyCycle_a)[numberOfRevs - 1]
                    = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
            }
        } else if (ptrInputData->LabelContainsString("Minimum Power Down Time") == true) {
            minimumPowerDownTime_a
                = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Sensor Weight Factor") == true) {
            sensorWeightFactor_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Requirement Deck File Name") == true) {
            ptrRequirementDeckFileName_a = shared_ptr<string>(new string(ptrInputData->GetStringValue()));
        }
    }

    try {
        //CPPCHECK_FIX Performance - prefix iterator change
        for( subElementIter = subElementList->begin(); subElementIter != subElementList->end(); ++subElementIter) {
            ptrSubElement = (*subElementIter);

            if (ptrSubElement->BlockTagContainsString("ACCESS_PATTERN_OBJECT") == true) {
                PatternExtractor patternExtractor;
                ptrAccessPattern_a = patternExtractor.ExtractPattern(ptrSubElement);
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractSensorInfo(InputDataElement*)");
        throw;
    }

    if (dataRate_a <= 0.0) {
        errorString = "Missing/invalid 'Collection Data Rate' input";
    }

    if ( (minimumSunElevationAngle_a != UNDEFINED_ANGLE) &&
            ((minimumSunElevationAngle_a < -QUARTER_CIRCLE) ||
             (minimumSunElevationAngle_a > QUARTER_CIRCLE)) ) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Invalid 'Minimum Sun Elevation Angle' input";
    }

    if ( (maximumSunElevationAngle_a != UNDEFINED_ANGLE) &&
            ((maximumSunElevationAngle_a < -QUARTER_CIRCLE) ||
             (maximumSunElevationAngle_a > QUARTER_CIRCLE)) ) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Invalid 'Maximum Sun Elevation Angle' input";
    }

    if ( (minimumSunElevationAngle_a != UNDEFINED_ANGLE) &&
            (maximumSunElevationAngle_a != UNDEFINED_ANGLE) &&
            (maximumSunElevationAngle_a < minimumSunElevationAngle_a) ) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "'Maximum Sun Elevation Angle' input must ";
        errorString += "exceed 'Minimum Sun Elevation Angle' input";
    }

    if (ptrAccessPattern_a == NULL) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing ACCESS_PATTERN_OBJECT in sensor block";
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "ExtractSensorInfo(InputDataElement*)",
                                 errorString);
    }

    return;
}

void
Sensor::CheckInputData(bool runTimelineGenerator) const
{
    return;
}

void
Sensor::PrintAttributes(ostream &outfile) const
{
    const TIME_UNIT inputTimeUnits = UnitData::GetInputTimeUnits();
    //RAGUSA 11/2/17 Updated to include SSAR sensor
    outfile << "      Sensor Type (VIS/LW/SAR/SSAR/MSR/MSE/USR/USE/HSR/HSE/WFOV) : ";
    outfile << GetSensorName() << NEW_LINE;
    outfile << "      Collection Data Rate (Gbits/sec)                      : ";
    outfile << dataRate_a << NEW_LINE;
    outfile.unsetf(ios::right);
    outfile.setf(ios::left);

    if (minimumSunElevationAngle_a != UNDEFINED_ANGLE) {
        outfile << "      Minimum Sun Elevation                                 : ";
        outfile << UnitData::GetInputAngle(minimumSunElevationAngle_a) << NEW_LINE;
    }

    if (maximumSunElevationAngle_a != UNDEFINED_ANGLE) {
        outfile << "      Maximum Sun Elevation                                 : ";
        outfile << UnitData::GetInputAngle(maximumSunElevationAngle_a) << NEW_LINE;
    }

    if (dailyDutyCycle_a > 0) {
        outfile << "      Daily Sensor Duty Cycle                               : ";
        TimePiece::PrintDuration(outfile, dailyDutyCycle_a, inputTimeUnits);
        outfile << NEW_LINE;
    }

    if (ptrRollingRevDutyCycle_a != NULL) {
        int arrayIndex = 0;
        arrayIndex = ptrRollingRevDutyCycle_a->Size() - 1;

        while (arrayIndex >= 0) {
            int dutyCycle = 0;
            dutyCycle = ptrRollingRevDutyCycle_a->ElementAt(arrayIndex);

            if (dutyCycle > 0) {
                outfile << "      Consecutive Rev Duty Cycle [" << (arrayIndex + 1) << "]";

                if (arrayIndex < 9) {
                    outfile << " ";
                }

                outfile << "                       : ";
                TimePiece::PrintDuration(outfile, dutyCycle, inputTimeUnits);
                outfile << NEW_LINE;
            }

            --arrayIndex;
        }
    }

    if (minimumPowerDownTime_a > 0) {
        outfile << "      Minimum Power Down Time                               : ";
        TimePiece::PrintDuration(outfile, minimumPowerDownTime_a, inputTimeUnits);
        outfile << NEW_LINE;
    }

    outfile.unsetf(ios::left);
    outfile.setf(ios::right);

    if (ptrRequirementDeckFileName_a != NULL) {
        outfile << "      Requirement Deck File Name                            : ";
        outfile << *ptrRequirementDeckFileName_a << NEW_LINE;
    }

    if (ptrAccessPattern_a != NULL) {
        outfile << NEW_LINE;
        AccessPattern::PrintAttributes(ptrAccessPattern_a,outfile);
        outfile << NEW_LINE;
    }

    if (sensorWeightFactor_a > 0.0) {
        outfile << "      Sensor Weight Factor                                  : ";
        outfile << sensorWeightFactor_a << NEW_LINE;
    }

    return;
}

Sensor::SENSOR_TYPE
Sensor::GetSensorType(shared_ptr<InputDataElement> ptrSensorElement)
{
    string                  sensorTypeString = "";
    SENSOR_TYPE             sensorType = UNKNOWN;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrSensorElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    for( inputDataIter = inputDataList->begin(); (inputDataIter != inputDataList->end()) && (sensorType == UNKNOWN); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Sensor Type") == true) {
            sensorTypeString = ptrInputData->GetStringValue();

            if (sensorTypeString == "VIS") {
                sensorType = VISIBLE;
            } else if (sensorTypeString == "SPC") {
                sensorType = SPECIAL_VISIBLE;
            } else if (sensorTypeString == "LW") {
                sensorType = LONG_WAVE;
            } else if (sensorTypeString == "SAR") {
                sensorType = SAR;
                // RAGUSA 11/2/17 Updated to include SSAR sensor
            } else if (sensorTypeString == "SSAR") {
                sensorType = SSAR;
            } else if (sensorTypeString == "HSR") {
                sensorType = HS_REFLECTIVE;
            } else if (sensorTypeString == "HSE") {
                sensorType = HS_EMISSIVE;
            } else if (sensorTypeString == "MSR") {
                sensorType = MS_REFLECTIVE;
            } else if (sensorTypeString == "MSE") {
                sensorType = MS_EMISSIVE;
            } else if (sensorTypeString == "USR") {
                sensorType = US_REFLECTIVE;
            } else if (sensorTypeString == "USE") {
                sensorType = US_EMISSIVE;
            } else if (sensorTypeString == "WFOV") {
                sensorType = WIDE_FOV;
            }
        }
    }

    return(sensorType);
}

int
Sensor::CalculateQualityLevel(double elevationAngle,
                              const string &missionType,
                              const SpaceVector &targetPos,
                              const PositionState &ecfVehilceState,
                              bool enhancedAreaMode) const
{
    int    qualityLevel = 0;
    double quality = 0.0;

    // If there is a image quality equation, get the calculated quality
    if ( ptrIQE_a != NULL ) {
        quality = ptrIQE_a->CalculateQuality(elevationAngle, missionType, targetPos, ecfVehilceState, enhancedAreaMode);
    }

    if ( IsSpectralSensor(sensorType_a) == true ) {
        // convert GSD to quality level (i.e. 0.13 Meters is a level 13)
        qualityLevel = (int)(quality * 100.0);
    } else {
        // convert NIIRS/NRIS to quality level (i.e. NIIRS 6.5 is a level 65)
        qualityLevel = (int)(quality * 10.0);
    }

    return(qualityLevel);
}

void
Sensor::UpdateRollingRevs(int numberOfRevs)
{
    Array<int> *ptrTempRollingRevs = NULL;

    if (ptrRollingRevDutyCycle_a != NULL) {
        int revIndex = 0;

        if (ptrRollingRevDutyCycle_a->Size() > numberOfRevs) {
            ptrTempRollingRevs = new Array<int>(*ptrRollingRevDutyCycle_a);
            delete ptrRollingRevDutyCycle_a;
            ptrRollingRevDutyCycle_a = new Array<int>(numberOfRevs);

            while (revIndex < numberOfRevs) {
                (*ptrRollingRevDutyCycle_a)[revIndex] = ptrTempRollingRevs->ElementAt(revIndex);
                ++revIndex;
            }

            delete ptrTempRollingRevs;
            ptrTempRollingRevs = NULL;
        }

        revIndex = (ptrRollingRevDutyCycle_a->Size() - 1);

        while (revIndex > 0) {
            if (ptrRollingRevDutyCycle_a->ElementAt(revIndex) <= ptrRollingRevDutyCycle_a->ElementAt(revIndex - 1)) {
                (*ptrRollingRevDutyCycle_a)[revIndex] = LONG_ZERO;
            }

            --revIndex;
        }
    }

    return;
}

int
Sensor::GetDutyCycle(int revIndex) const
{
    int   dutyCycle = 0;

    if (revIndex < 0) {
        double dailyFactor = (TimePiece::GetReportableTimeSteps() * TimePiece::GetSecondsPerTimeStep()) / SECONDS_PER_DAY;
        dutyCycle = (int)((double)dailyDutyCycle_a * dailyFactor);
    } else if (ptrRollingRevDutyCycle_a != NULL) {
        if (revIndex < ptrRollingRevDutyCycle_a->Size()) {
            dutyCycle = ptrRollingRevDutyCycle_a->ElementAt(revIndex);
        }
    }

    return(dutyCycle);
}

double
Sensor::GetCollectionRate(int missionIndex,
                          int qualityIndex) const
{
    return(0.0);
}

double
Sensor::GetResourceWeight(int missionIndex,
                          int qualityIndex) const
{
    return(0.0);
}

double
Sensor::GetOuterConeElevationAngle(const AccessPattern::PATTERN &pattern,
                                   int missionIndex) const
{
    return(ptrAccessPattern_a->GetOuterConeElevationAngle(pattern, Target::GetMissionString(missionIndex)));
}

double
Sensor::GetOuterConeElevationAngle(const string &missionString) const
{
    return(ptrAccessPattern_a->GetOuterConeElevationAngle(missionString));
}

double
Sensor::GetInnerConeElevationAngle(const AccessPattern::PATTERN &pattern,
                                   int missionIndex) const
{
    return(ptrAccessPattern_a->GetInnerConeElevationAngle(pattern, Target::GetMissionString(missionIndex)));
}

double
Sensor::GetInnerConeElevationAngle(const string &missionString) const
{
    return(ptrAccessPattern_a->GetInnerConeElevationAngle(missionString));
}

bool
Sensor::InsideAccessPattern(int missionIndex,
                            const SpaceVector &targetPosition,
                            const SpaceVector &vehiclePosition,
                            const SpaceVector &vehicleVelocity) const
{
    return(ptrAccessPattern_a->InsideAccessPattern(Target::GetMissionString(missionIndex), targetPosition,
            vehiclePosition, vehicleVelocity));
}

bool
Sensor::MissionSpecificEqualsLargest(int missionIndex) const
{
    return(ptrAccessPattern_a->MissionSpecificEqualsLargest(Target::GetMissionString(missionIndex)));
}

/**
 * Method to return the index which maps to the target deck and the SENSOR enum.  This default implementation simply
 * returns the sensorType
 *
 * @return int sensor Index
 */
int
Sensor::GetSensorIndex() const
{
    return(sensorType_a);
}

Sensor::~Sensor()
{
    if (ptrRollingRevDutyCycle_a != NULL) {
        delete ptrRollingRevDutyCycle_a;
        ptrRollingRevDutyCycle_a = NULL;
    }

    return;
}
