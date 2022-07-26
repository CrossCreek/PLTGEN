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
 * PatternExtractor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "PatternExtractor.h"
#include "mural/access/ConeAngleData.h"
#include "mural/access/ConePattern.h"
#include "mural/access/DonutPattern.h"
#include "mural/access/AxebladePattern.h"
#include "mural/access/ButterflyPattern.h"

// From rageneral
#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"
#include "general/data/UnitData.h"
#include "general/array/ListIterator.h"
#include "constants.h"
PatternExtractor::PatternExtractor()
{
    return;
}

/* ***************************************************************************** */

shared_ptr<AccessPattern>
PatternExtractor::ExtractPattern(const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    string         errorString = "";
    AccessPattern::PATTERN_TYPE   patternType;
    shared_ptr<AccessPattern> ptrParsedPattern;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Access Pattern Type") == true) {
            patternType = AccessPattern::GetPatternTypeFromString(ptrInputData->GetStringValue());
        }
    }

    if (patternType == AccessPattern::UNKNOWN_TYPE) {
        errorString = "Missing/invalid 'Access Pattern Type' input";
    } else {
        if ( patternType == AccessPattern::VISIBILITY_CONE ) {
            ptrParsedPattern = shared_ptr<AccessPattern>(ParseConePattern(ptrInputDataElement));
        } else if ( patternType == AccessPattern::DOUGHNUT ) {
            ptrParsedPattern = shared_ptr<AccessPattern>(ParseDonutPattern(ptrInputDataElement));
        } else if ( patternType == AccessPattern::AXEBLADE ) {
            ptrParsedPattern = shared_ptr<AccessPattern>(ParseAxebladePattern(ptrInputDataElement));
        } else if ( patternType == AccessPattern::BUTTERFLY ) {
            ptrParsedPattern = shared_ptr<AccessPattern>(ParseButterflyPattern(ptrInputDataElement));
        }
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractPatternExtractorInfo(InputDataElement*)",
                                 errorString);
    }

    return ptrParsedPattern;
}

/* ***************************************************************************** */

shared_ptr<ConeAngleData>
PatternExtractor::ParseConeAngleData(const string &keyString, const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    double         defaultAngle = UNDEFINED_ANGLE;
    string         missionString = "";
    string         errorString = "";
    string         defaultString = "Default " + keyString + " Cone Elevation Angle";
    string         specificString = keyString + " Cone Elevation Angle";
    shared_ptr<ConeAngleData> ptrConeData = NULL;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData> ptrInputData = NULL;

    // Parse the other cone parameters
    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString(defaultString) == true) {
            defaultAngle= UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
        }
    }

    if (defaultAngle == UNDEFINED_ANGLE) {
        /*RAGUSA 1/17/2018 - Fixed errorString outputting messages, they were the same for
          both cases of error. It also was checking the angles in deg (range 0-90), this is
          incorrect because when the angle is read in it is immediately converted to radians... woops.*/
        errorString = "Missing ' " + defaultString + " ' input";
    } else if ((defaultAngle < 0.00) || (defaultAngle >  HALF_PI)) {
        errorString = "Invalid '" + defaultString + "' input";
    }

    //==================================================================================================
    if (errorString.size() == 0) {
        double coneAngle = 0.0;
        ptrConeData = shared_ptr<ConeAngleData>(new ConeAngleData(defaultAngle));

        for( inputDataIter = inputDataList->begin(); (inputDataIter != inputDataList->end()) && (errorString.size() == 0); ++inputDataIter) {
            ptrInputData = (*inputDataIter);

            if (ptrInputData->GetSpecificType(missionString) == true) {
                coneAngle = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());

                /*RAGUSA 1/17/2018 - Added exception handling for the mission specific angles too
                  same constraints as the default angle.*/
                if (coneAngle == UNDEFINED_ANGLE) {
                    errorString = "Missing Mission Specfic Angle input";
                } else if ((coneAngle < 0.00) || (coneAngle >  HALF_PI)) {
                    errorString = "Invalid Mission Specfic Angle input";
                }

                if (ptrInputData->LabelContainsString(specificString) == true) {
                    /*RAGUSA 1/16/2018 - If CRISIS angle is the same as the default then
                      let it also be added. Before it would not do this but because we require
                      this input for crisis region now it's needed.*/
                    if ((coneAngle != defaultAngle) || (missionString == "CRISIS")) {
                        ptrConeData->AddSpecificMission(missionString, coneAngle);
                    }

                    //===============================================================================
                }
            }
        }
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractPatternExtractorInfo(InputDataElement*)",
                                 errorString);
    }

    return ptrConeData;
}

/* ***************************************************************************** */

shared_ptr<ConePattern>
PatternExtractor::ParseConePattern(const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    double         defaultInnerAngle = UNDEFINED_ANGLE;
    shared_ptr<ConeAngleData> ptrInnerConeData = NULL;
    shared_ptr<ConePattern> conePattern = shared_ptr<ConePattern>(new ConePattern());
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;
    // Set the pattern to visibility cone
    conePattern->SetPatternType( AccessPattern::VISIBILITY_CONE );

    // Parse the other cone parameters
    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Always Include Sub Satellite Region") == true) {
            conePattern->SetAlwaysIncludeSubSatelliteRegion( ptrInputData->GetBoolValue() );
        }
    }

    // Set the Outer cone
    conePattern->SetOuterConeAngleData(ParseConeAngleData("Outer", ptrInputDataElement));
    // Blank out the inner cone because we don't need it
    ptrInnerConeData = shared_ptr<ConeAngleData>(new ConeAngleData(defaultInnerAngle));
    conePattern->SetInnerConeAngleData(ptrInnerConeData);
    return conePattern;
}

/* ***************************************************************************** */

shared_ptr<DonutPattern>
PatternExtractor::ParseDonutPattern(const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    shared_ptr<DonutPattern> donutPattern = shared_ptr<DonutPattern>(new DonutPattern());
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;
    // Set the pattern to visibility cone
    donutPattern->SetPatternType( AccessPattern::DOUGHNUT );

    // Parse the other cone parameters
    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Always Include Sub Satellite Region") == true) {
            donutPattern->SetAlwaysIncludeSubSatelliteRegion( ptrInputData->GetBoolValue() );
        }
    }

    donutPattern->SetOuterConeAngleData(ParseConeAngleData("Outer", ptrInputDataElement));
    donutPattern->SetInnerConeAngleData(ParseConeAngleData("Inner", ptrInputDataElement));
    return donutPattern;
}

/* ***************************************************************************** */

shared_ptr<AxebladePattern>
PatternExtractor::ParseAxebladePattern(const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    double         broadsideAngle = UNDEFINED_ANGLE;
    bool           alwaysIncludeSubSat = false;
    string         errorString = "";
    ConeAngleData *ptrOuterConeData = NULL;
    ConeAngleData *ptrInnerConeData = NULL;
    ConeAngleData *ptrRevisitConeData = NULL;
    ConeAngleData *ptrConeData = NULL;
    shared_ptr<AxebladePattern> axebladePattern = shared_ptr<AxebladePattern>(new AxebladePattern());
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;
    // Set the pattern to visibility cone
    axebladePattern->SetPatternType( AccessPattern::AXEBLADE );

    // Parse the other cone parameters
    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Always Include Sub Satellite Region") == true) {
            alwaysIncludeSubSat = ptrInputData->GetBoolValue();
        } else if (ptrInputData->LabelContainsString("Broadside Angle") == true) {
            broadsideAngle = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
        }
    }

    // Error Checks
    if (broadsideAngle == UNDEFINED_ANGLE) {
        if (errorString.size() > 0) {
            errorString += "/n         ";
        }

        errorString += "Missing 'Broadside Angle' input";
    } else if ( (broadsideAngle <= 0.00) || (broadsideAngle >= UnitData::GetStoreQuarterCircle()) ) {
        errorString = "Invalid 'Broadside Angle' input";
    }

    // Continue if no errors
    if (errorString.size() == 0) {
        // set the fully created angle data objects on the pattern
        axebladePattern->SetOuterConeAngleData(ParseConeAngleData("Outer", ptrInputDataElement));
        axebladePattern->SetInnerConeAngleData(ParseConeAngleData("Inner", ptrInputDataElement));
        axebladePattern->SetAlwaysIncludeSubSatelliteRegion( alwaysIncludeSubSat );
        axebladePattern->SetBroadSideAngle( broadsideAngle );
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractPatternExtractorInfo(InputDataElement*)",
                                 errorString);
    }

    return axebladePattern;
}

/* ***************************************************************************** */

shared_ptr<ButterflyPattern>
PatternExtractor::ParseButterflyPattern(const shared_ptr<InputDataElement> &ptrInputDataElement)
{
    double         broadsideAngle = UNDEFINED_ANGLE;
    bool           alwaysIncludeSubSat = false;
    string         errorString = "";
    ConeAngleData *ptrOuterConeData = NULL;
    ConeAngleData *ptrInnerConeData = NULL;
    ConeAngleData *ptrRevisitConeData = NULL;
    ConeAngleData *ptrConeData = NULL;
    shared_ptr<ButterflyPattern> butterflyPattern = shared_ptr<ButterflyPattern>(new ButterflyPattern());
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;
    // Set the pattern to visibility cone
    butterflyPattern->SetPatternType( AccessPattern::BUTTERFLY );

    // Parse the other cone parameters
    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Always Include Sub Satellite Region") == true) {
            alwaysIncludeSubSat = ptrInputData->GetBoolValue();
        } else if (ptrInputData->LabelContainsString("Broadside Angle") == true) {
            broadsideAngle = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
        }
    }

    // Error Checks
    if (broadsideAngle == UNDEFINED_ANGLE) {
        if (errorString.size() > 0) {
            errorString += "/n         ";
        }

        errorString += "Missing 'Broadside Angle' input";
    } else if ( (broadsideAngle <= 0.00) || (broadsideAngle >= UnitData::GetStoreQuarterCircle()) ) {
        errorString = "Invalid 'Broadside Angle' input";
    }

    // Continue if no errors
    if (errorString.size() == 0) {
        // set the fully created angle data objects on the pattern
        butterflyPattern->SetOuterConeAngleData(ParseConeAngleData("Outer", ptrInputDataElement));
        butterflyPattern->SetInnerConeAngleData(ParseConeAngleData("Inner", ptrInputDataElement));
        butterflyPattern->SetAlwaysIncludeSubSatelliteRegion( alwaysIncludeSubSat );
        butterflyPattern->SetBroadSideAngle( broadsideAngle );
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractPatternExtractorInfo(InputDataElement*)",
                                 errorString);
    }

    return butterflyPattern;
}

/* ***************************************************************************** */

PatternExtractor::~PatternExtractor()
{
    return;
}
