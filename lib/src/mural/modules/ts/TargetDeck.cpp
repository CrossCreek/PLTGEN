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
 * TargetDeck.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <memory>

#include "TargetDeck.h"
#include "mural/Region.h"
#include "CountryFactor.h"
#include "mural/modules/vtg/TimelineGenerator.h"
#include "mural/modules/vtg/RequirementMap.h"

#include "general/array/ListIterator.h"
#include "general/array/Array3D.h"
#include "general/time/Date.h"
#include "general/parse/ParseUtility.h"
#include "general/math/SimpleMath.h"
#include "general/utility/StringEditor.h"
#include "general/time/TimePiece.h"

#include "mural/modules/ts/CollectionProblemSet.h"
#include "mural/modules/ts/Target.h"
#include "mural/modules/ts/TargetRegion.h"
#include "mural/sensor/SpectralSensor.h"

Array<int>*            TargetDeck::ptrSubPriorityThreshold_s = NULL;
double                    TargetDeck::opticalPriorityFactorCoeff_s = 0.0;
double                    TargetDeck::opticalPriorityFactorExpnt_s = 0.0;
double                    TargetDeck::radarPriorityFactorCoeff_s = 0.0;
double                    TargetDeck::radarPriorityFactorExpnt_s = 0.0;
double                    TargetDeck::areaFactor_s = 0.0;
const char               TargetDeck::ACTIVE_TARGET_ID_s = 'A';
bool                     TargetDeck::printDeckAsRequirements_s = false;
bool                     TargetDeck::scheduleTargets_s = false;
bool                     TargetDeck::useFullOpticalScoreEquation_s = true;
string*                  TargetDeck::ptrDeckFileBaseDirectory_s = NULL;
Array<string>*           TargetDeck::ptrTargetDeckFileNameArray_s = NULL;
Array<string>*           TargetDeck::ptrTargetGroupArray_s = NULL;
TargetDeck::EFFECTIVITY  TargetDeck::effectivity_s = UNKNOWN_EFFECTIVITY;
LinkList<CountryFactor>* TargetDeck::ptrCountryFactorList_s = NULL;

TargetDeck::TargetDeck()
    : deckFileName_a(""), ptrTargetList_a(NULL)
{
    return;
}

TargetDeck::TargetDeck(int targetDeckIndex,
                       const Array2D<bool> *ptrActiveRequirements,
                       int numberOfResources)
    : deckFileName_a(*ptrDeckFileBaseDirectory_s + ptrTargetDeckFileNameArray_s->ElementAt(targetDeckIndex)),
      ptrTargetList_a(NULL)
{
    if (scheduleTargets_s == true) {
        ptrTargetList_a = new LinkList<Target>();
    }

    try {
        RetrieveTargetsAndBuildRequirements(targetDeckIndex, ptrActiveRequirements,
                                            numberOfResources);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "TargetDeck(int, const Array2D<bool>*, ...)");
        throw;
    }

    return;
}

void
TargetDeck::ExtractTargetDeckInfo(shared_ptr<InputDataElement> ptrElement,
                                  const bool &runTargetScheduler)
{
    int                    index = 0;
    int                    numberOfInputs = 0;
    const int              numberOfQualities = Target::GetNumberOfQualities();
    double                   factor = 0.0;
    Array<double>           *ptrSpectralQuality = NULL;
    string                  errorString = "";
    CountryFactor          *ptrCountryFactor = NULL;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Optical Priority Factor Coefficient") == true) {
            opticalPriorityFactorCoeff_s = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Optical Priority Factor Exponential") == true) {
            opticalPriorityFactorExpnt_s = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Use Full Optical Score Equation") == true) {
            useFullOpticalScoreEquation_s = ptrInputData->GetBoolValue();
        } else if (ptrInputData->LabelContainsString("Radar Priority Factor Coefficient") == true) {
            radarPriorityFactorCoeff_s = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Radar Priority Factor Exponential") == true) {
            radarPriorityFactorExpnt_s = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Area Factor") == true) {
            areaFactor_s = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Spectral Quality Bins") == true) {
            numberOfInputs = ptrInputData->GetNumberOfValues();

            if (numberOfInputs <= numberOfQualities) {
                if (ptrSpectralQuality == NULL) {
                    ptrSpectralQuality = new Array<double>(numberOfQualities, FLT_MAX);
                } else {
                    ptrSpectralQuality->ResetValues(FLT_MAX);
                }

                index = 0;

                while (index < numberOfInputs) {
                    (*ptrSpectralQuality)[index] = ptrInputData->GetDoubleValue(index);
                    ++index;
                }
            }
        } else if (ptrInputData->LabelContainsString("Deck File Base Directory") == true) {
            if (ptrDeckFileBaseDirectory_s != NULL) {
                delete ptrDeckFileBaseDirectory_s;
            }

            ptrDeckFileBaseDirectory_s = new string(ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Print Deck As Requirements") == true) {
            printDeckAsRequirements_s = ptrInputData->GetBoolValue();
        } else if (ptrInputData->LabelContainsString("Sub Priority Level Threshold") == true) {
            if (ptrSubPriorityThreshold_s == NULL) {
                ptrSubPriorityThreshold_s = new Array<int>();
            }

            index = 0;
            numberOfInputs = ptrInputData->GetNumberOfValues();

            while (index < numberOfInputs) {
                ptrSubPriorityThreshold_s->InsertUniqueElement(ptrInputData->GetIntValue(index));
                ++index;
            }
        } else if (ptrInputData->LabelContainsString("Effectivity") == true) {
            effectivity_s = GetEffectivity(ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Country Code Factor") == true) {
            if (ptrInputData->GetSpecificDoubleNumber(factor) == true) {
                ptrCountryFactor = new CountryFactor(factor, ptrInputData->GetValueArray());

                if (ptrCountryFactorList_s == NULL) {
                    ptrCountryFactorList_s = new LinkList<CountryFactor>();
                }

                ptrCountryFactorList_s->Append(ptrCountryFactor);
                ptrCountryFactor = NULL;
            }
        }
    }

    if (effectivity_s == UNKNOWN_EFFECTIVITY) {
        errorString = "Missing/invalid 'Effectivity' input";
    }

    if (opticalPriorityFactorCoeff_s <= 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString = "Missing/invalid 'Optical Priority Factor Coefficient' input";
    }

    if (opticalPriorityFactorExpnt_s <= 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Optical Priority Factor Exponential' input";
    }

    if (radarPriorityFactorCoeff_s <= 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Radar Priority Factor Coefficient' input";
    }

    if (radarPriorityFactorExpnt_s <= 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Radar Priority Factor Exponential' input";
    }

    if (areaFactor_s <= 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Area Factor' input";
    }

    if (ptrSpectralQuality == NULL) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Spectral Quality Bins' input";
    }

    if (ptrDeckFileBaseDirectory_s == NULL) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing 'Deck File Base Directory' input";
    } else if (ptrDeckFileBaseDirectory_s->at(ptrDeckFileBaseDirectory_s->length() - 1) != '\\') {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Invalid 'Deck File Base Directory' input.  MUST end with a back slash!";
    }

    if ((runTargetScheduler == true) && (ptrTargetGroupArray_s == NULL)) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Target Groups' input";
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "ExtractTargetInfo(InputDataElement*)",
                                 errorString);
    }

    SpectralSensor::SetQualityBinArray(ptrSpectralQuality);
    delete ptrSpectralQuality;
    ptrSpectralQuality = NULL;
    return;
}

void
TargetDeck::PrintAttributes(ostream &outfile)
{
    string factorString = "";
    ListIterator<CountryFactor> countryFactorListIter(ptrCountryFactorList_s);
    CountryFactor              *ptrCountryFactor = countryFactorListIter.First();
    outfile << "<TARGET_PARAMETERS_START>" << NEW_LINE;
    outfile << "   Optical Priority Factor Coefficient : ";
    outfile << opticalPriorityFactorCoeff_s << NEW_LINE;
    outfile << "   Optical Priority Factor Exponential : ";
    outfile << opticalPriorityFactorExpnt_s << NEW_LINE;
    outfile << "   Use Full Optical Score Equation     : ";
    outfile << StringEditor::ConvertToString(useFullOpticalScoreEquation_s) << NEW_LINE;
    outfile << "   Radar Priority Factor Coefficient   : ";
    outfile << radarPriorityFactorCoeff_s << NEW_LINE;
    outfile << "   Radar Priority Factor Exponential   : ";
    outfile << radarPriorityFactorExpnt_s << NEW_LINE;
    outfile << "   Area Factor                         : ";
    outfile << areaFactor_s << NEW_LINE;
    outfile << "   Spectral Quality Bins               : ";
    SpectralSensor::PrintQualityBinArray(outfile);
    outfile << NEW_LINE;

    if (ptrDeckFileBaseDirectory_s != NULL) {
        outfile << "   Deck File Base Directory            : ";
        outfile << *ptrDeckFileBaseDirectory_s << NEW_LINE;
    }

    outfile << "   Print Deck As Requirements          : ";
    outfile << StringEditor::ConvertToString(printDeckAsRequirements_s) << NEW_LINE;

    if (ptrTargetGroupArray_s != NULL) {
        outfile << "   Schedule Targets                    : ";
        outfile << StringEditor::ConvertToString(scheduleTargets_s) << NEW_LINE;
        outfile << "   Target Groups                       : ";
        ptrTargetGroupArray_s->Print(outfile);
        outfile << NEW_LINE;
    }

    if (ptrSubPriorityThreshold_s != NULL) {
        outfile << "   Sub Priority Level Threshold(s)     : ";
        ptrSubPriorityThreshold_s->Print(outfile);
        outfile << NEW_LINE;
    }

    outfile << "   Effectivity (MO/MAD/ALL)            : ";
    outfile << GetEffectivityString() << NEW_LINE;

    while (ptrCountryFactor != NULL) {
        factorString = InputData::GetSpecificTypeString(StringEditor::ConvertToString(ptrCountryFactor->GetFactor()));
        outfile << "   Country Code Factor " << factorString << setw(17) << " : ";
        ptrCountryFactor->PrintCountryCodes(outfile);
        outfile << NEW_LINE;
        ptrCountryFactor = countryFactorListIter.Next();
    }

    outfile << "<TARGET_PARAMETERS_END>" << NEW_LINE;
    return;
}

void
TargetDeck::RetrieveTargetsAndBuildRequirements(int targetDeckIndex,
        const Array2D<bool> *ptrActiveRequirements,
        int numberOfResources)
{
    int          simulationStart = 0;
    int          simulationEnd = 0;
    const int    startYear = GetEffectiveSimulationTimes(simulationStart, simulationEnd);
    //int deckIndex;
    //WAS:int sensorIndex = 0;
    //int sensorIndex;
    const int     ONE = 1;
    const int     numberOfSensors = ptrActiveRequirements->Size();
    Array<double>  *ptrQuality = new Array<double>(numberOfSensors, 0);
    Array<int>  *ptrPriority = new Array<int>(numberOfSensors, 0);
    Array<double>  *ptrRegionNumber = NULL;
    Array<double>  *ptrRegionArea = NULL;
    Array<double>  *ptrSensorScore = new Array<double>(numberOfSensors, 0.0);
    Array<char>   *ptrPreference = new Array<char>(numberOfSensors, BLANK_CHARACTER);
    Array<char>   *ptrMode = new Array<char>(numberOfSensors, BLANK_CHARACTER);
    const bool     anySpectralSensors = SpectralSensor::AnySpectralSensors();
    string         inputString = "";
    string         targetID = "";
    string         previousTargetID = "UNKNOWN";
    string         countryCode = "";
    string         fcpsID = "";
    ifstream       targetDeckFile;
    Target        *ptrTarget = NULL;

    try {
        int duration = 0;
        int missionNumber = 0;
        int previousMissionNumber = -1;
        int sampleSize = 0;
        int region = 0;
        int subRegion = 0;
        int missionIndex = 0;
        int arrayIndex = 0;
        double periodicity = 0.0;
        double fullRegionNumber = 0.0;
        double area = 0.0;
        double totalTargetArea = 0.0;
        ParseUtility::OpenInputFile(targetDeckFile, deckFileName_a);
        // get the header line first
        getline(targetDeckFile, inputString, NEW_LINE);

        while (getline(targetDeckFile, inputString, NEW_LINE)) {
            istringstream inputStream(inputString);

            if (ValidTarget(inputStream, simulationStart, simulationEnd, startYear,
                            ptrActiveRequirements, targetID, duration, missionNumber,
                            ptrQuality, anySpectralSensors) == true) {
                RetrieveCollectionData(inputStream, ptrQuality, ptrPriority, ptrPreference,
                                       ptrMode, sampleSize, periodicity, countryCode, fcpsID);
                missionIndex = Target::ConvertMissionToIndex(missionNumber);

                if (missionIndex == Target::MCG_MONO_BOX) {
                    if (ptrMode->ValueExists('S') == true) {
                        cout << "WARNING: MCG Mono Target " << targetID
                             << " has a stereo collection requirement!" << NEW_LINE;
                    }
                } else if (missionIndex == Target::MCG_STEREO_AREA) {
                    if (ptrMode->ValueExists('M') == true) {
                        cout << "WARNING: MCG Stereo Target " << targetID
                             << " has a mono collection requirement!" << NEW_LINE;
                    }
                }

                if ((targetID == previousTargetID) && (missionNumber == previousMissionNumber)) {
                    CalculateSensorScore(ptrSensorScore, ptrMode, ptrPriority,
                                         countryCode, duration, sampleSize,
                                         periodicity, totalTargetArea);
                    ptrTarget->AddCollectionProblemSet(fcpsID, ptrQuality, ptrPriority,
                                                       ptrPreference, ptrMode, ptrSensorScore);
                } else {
                    if (ptrTarget != NULL) {
                        BuildRequirements(ptrTarget, ptrActiveRequirements,
                                          numberOfResources, totalTargetArea);

                        if (ptrTargetList_a != NULL) {
                            ptrTargetList_a->Append(ptrTarget);
                        } else {
                            delete ptrTarget;
                        }

                        ptrTarget = NULL;
                    }

                    totalTargetArea = 0.0;

                    while (inputStream >> region >> subRegion >> area) {
                        // this is to standardize the collection time of a point target
                        if (Target::IsPointTarget(missionIndex) == true) {
                            area = 1.0;
                        }

                        totalTargetArea += area;
                        fullRegionNumber = Region::GetFullRegionNumber(region, subRegion);

                        if (ptrRegionNumber == NULL) {
                            ptrRegionNumber = new Array<double>(ONE, fullRegionNumber);
                            ptrRegionArea = new Array<double>(ONE, area);
                        } else {
                            arrayIndex = ptrRegionNumber->FindIndex(fullRegionNumber);

                            if (arrayIndex >= 0) {
                                (*ptrRegionArea)[arrayIndex] += area;
                            } else {
                                ptrRegionNumber->AddElement(fullRegionNumber);
                                ptrRegionArea->AddElement(area);
                            }
                        }
                    }

                    if (ptrRegionNumber != NULL) {
                        CalculateSensorScore(ptrSensorScore, ptrMode, ptrPriority,
                                             countryCode, duration, sampleSize,
                                             periodicity, totalTargetArea);
                        ptrTarget = new Target(targetID, missionNumber, targetDeckIndex,
                                               fcpsID, ptrQuality, ptrPriority, ptrPreference,
                                               ptrMode, ptrSensorScore, ptrRegionNumber,
                                               ptrRegionArea, scheduleTargets_s);

                        // if scheduling targets, add this target region's to the
                        // corresponding (geographical) region in the requirement map
                        if (scheduleTargets_s == true) {
                            ListIterator<TargetRegion> targetRegionIter = ListIterator<TargetRegion>(ptrTarget->GetTargetRegionList());
                            TargetRegion *ptrTargetRegion = targetRegionIter.First();

                            while( ptrTargetRegion != NULL ) {
                                TimelineGenerator::requirementMap_s->AddTargetRegion(ptrTargetRegion->GetRegionNumber(), ptrTargetRegion);
                                ptrTargetRegion = targetRegionIter.Next();
                            }
                        }

                        delete ptrRegionNumber;
                        ptrRegionNumber = NULL;
                        delete ptrRegionArea;
                        ptrRegionArea = NULL;
                        previousTargetID = targetID;
                        previousMissionNumber = missionNumber;
                    }
                }
            }
        }

        if (ptrTarget != NULL) {
            BuildRequirements(ptrTarget, ptrActiveRequirements,
                              numberOfResources, totalTargetArea);

            if (ptrTargetList_a != NULL) {
                ptrTargetList_a->Append(ptrTarget);
            } else {
                delete ptrTarget;
            }

            ptrTarget = NULL;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "RetrieveTargetsAndBuildRequirements(int, const Array<bool>*, ...)");
        throw;
    }

    targetDeckFile.close();
    delete ptrQuality;
    ptrQuality = NULL;
    delete ptrPriority;
    ptrPriority = NULL;
    delete ptrPreference;
    ptrPreference = NULL;
    delete ptrMode;
    ptrMode = NULL;
    delete ptrSensorScore;
    ptrSensorScore = NULL;
    return;
}

bool
TargetDeck::ValidTarget(istringstream &inputStream,
                        int simulationStart,
                        int simulationEnd,
                        int startYear,
                        const Array2D<bool> *ptrActiveRequirements,
                        string &targetID,
                        int &duration,
                        int &missionNumber,
                        Array<double> *&ptrQuality,
                        const bool &anySpectralSensors) const
{
    bool       validTarget = false;
    char       statusFlag = ' ';
    string     scenarioFlag = "";
    string     dateString = "";
    inputStream >> targetID >> statusFlag >> scenarioFlag >> dateString >> duration;

    if (EffectiveTarget(statusFlag, dateString, duration,
                        simulationStart, simulationEnd, startYear) == true) {
        int sensorIndex = 0;
        double qualityLevel = 0.0;
        ptrQuality->ResetValues(qualityLevel);
        inputStream >> missionNumber;

        while (sensorIndex < ptrQuality->Size()) {
            inputStream >> qualityLevel;

            if ( (ptrActiveRequirements->ValueExists(sensorIndex, true) == true) &&
                    (qualityLevel > 0.0) ) {
                (*ptrQuality)[sensorIndex] = qualityLevel;
                validTarget = true;
            }

            ++sensorIndex;
        }
    }

    return(validTarget);
}

void
TargetDeck::RetrieveCollectionData(istringstream &inputStream,
                                   const Array<double> *ptrQuality,
                                   Array<int> *&ptrPriority,
                                   Array<char> *&ptrPreference,
                                   Array<char> *&ptrMode,
                                   int &sampleSize,
                                   double &periodicity,
                                   string &countryCode,
                                   string &fcpsID) const
{
    int        priorityLevel = 0;
    int        scanInterval = 0;
    int         acceptableViewDuration = 0;
    int         sensorIndex = 0;
    const int   NON_SPECTRAL_SENSORS = Sensor::GetNumberOfNonSpectralSensors();
    double        linkRPD = 0.0;
    char         preference = BLANK_CHARACTER;
    char         coverage = BLANK_CHARACTER;
    char         mode = BLANK_CHARACTER;
    Array<char> *ptrCoverageType = new Array<char>(ptrPreference->Size(), BLANK_CHARACTER);
    string       errorString = "";

    while (sensorIndex < NON_SPECTRAL_SENSORS) {
        inputStream >> (*ptrPriority)[sensorIndex];
        ++sensorIndex;
    }

    inputStream >> priorityLevel;

    while (sensorIndex < ptrPriority->Size()) {
        (*ptrPriority)[sensorIndex] = priorityLevel;
        ++sensorIndex;
    }

    sensorIndex = 0;

    while (sensorIndex < NON_SPECTRAL_SENSORS) {
        inputStream >> (*ptrPreference)[sensorIndex];
        ++sensorIndex;
    }

    // preference read in once per spectral sensor (MS, US, HS) and
    // set twice (for reflective and emissive)
    while (sensorIndex < ptrPreference->Size()) {
        inputStream >> preference;
        (*ptrPreference)[sensorIndex] = preference;
        ++sensorIndex;
        (*ptrPreference)[sensorIndex] = preference;
        ++sensorIndex;
    }

    sensorIndex = 0;

    while (sensorIndex < NON_SPECTRAL_SENSORS) {
        inputStream >> (*ptrCoverageType)[sensorIndex];
        ++sensorIndex;
    }

    // coverage read in once per spectral sensor (MS, US, HS) and
    // set twice (for reflective and emissive)
    while (sensorIndex < ptrCoverageType->Size()) {
        inputStream >> coverage;
        (*ptrCoverageType)[sensorIndex] = coverage;
        ++sensorIndex;
        (*ptrCoverageType)[sensorIndex] = coverage;
        ++sensorIndex;
    }

    inputStream >> acceptableViewDuration >> scanInterval;
    sensorIndex = 0;

    while (sensorIndex < NON_SPECTRAL_SENSORS) {
        inputStream >> (*ptrMode)[sensorIndex];
        ++sensorIndex;
    }

    // mode read in once per spectral sensor (MS, US, HS) and
    // set twice (for reflective and emissive)
    while (sensorIndex < ptrMode->Size()) {
        inputStream >> mode;
        (*ptrMode)[sensorIndex] = mode;
        ++sensorIndex;
        (*ptrMode)[sensorIndex] = mode;
        ++sensorIndex;
    }

    sensorIndex = 0;

    while ((sensorIndex < ptrQuality->Size()) && (errorString.size() == 0)) {
        if (ptrQuality->ElementAt(sensorIndex) > 0.0) {
            if (ptrPriority->ElementAt(sensorIndex) <= 0) {
                errorString = "Invalid priority: "
                              + StringEditor::ConvertToString(ptrPriority->ElementAt(sensorIndex));
            } else if (ptrPreference->ElementAt(sensorIndex) == '-') {
                errorString = "No sensor preference";
            } else if (Target::ConvertModeToIndex(ptrMode->ElementAt(sensorIndex)) < 0) {
                errorString = "Missing/invalid collection mode";
            } else {
                ++sensorIndex;
            }
        } else {
            (*ptrPriority)[sensorIndex] = 0;
            ++sensorIndex;
        }
    }

    delete ptrCoverageType;
    ptrCoverageType = NULL;

    if (errorString.size() > 0) {
        errorString += " for sensor ";
        errorString += Sensor::GetSensorTypeString(sensorIndex);
        throw new InputException(GetClassName(),
                                 "RetrieveCollectionData(istringstream&, const Array<double>*, ...)", errorString);
    }

    inputStream >> sampleSize >> periodicity >> countryCode >> fcpsID >> linkRPD;
    return;
}

void
TargetDeck::CalculateSensorScore(Array<double> *&ptrSensorScore,
                                 const Array<char> *ptrMode,
                                 const Array<int> *ptrPriority,
                                 const string &countryCode,
                                 int duration,
                                 int sampleSize,
                                 double periodicity,
                                 double totalTargetArea) const
{
    int        sensorIndex = 0;
    const int  numberOfSensors = ptrSensorScore->Size();
    double       sensorScore = 0.0;
    double       priorityFactorCoeff = 0.0;
    double       priorityFactorExpnt = 0.0;
    double       exponent = 0.0;
    const double countryFactor = GetCountryFactor(countryCode);
    ptrSensorScore->ResetValues(0.0);

    while (sensorIndex < numberOfSensors) {
        if (ptrPriority->ElementAt(sensorIndex) > 0) {
            // score for all sensors include a country code factor (if any)
            sensorScore = countryFactor;

            if (sensorIndex != Sensor::SAR) {
                if (useFullOpticalScoreEquation_s == true) {
                    // score for visible and int wave sensors include an area adjust factor
                    if (totalTargetArea > areaFactor_s) {
                        sensorScore *= (totalTargetArea / areaFactor_s);
                    }

                    // score for visible and int wave sensors also include a timeliness factor
                    sensorScore *= ((double)(Target::ConvertModeToIndex(ptrMode->ElementAt(sensorIndex)) + 1)
                                    * (((double)sampleSize / 100.0) / periodicity)
                                    * ((double)duration / 365.0) * 100.0);
                }

                priorityFactorCoeff = opticalPriorityFactorCoeff_s;
                priorityFactorExpnt = opticalPriorityFactorExpnt_s;
            } else {
                priorityFactorCoeff = radarPriorityFactorCoeff_s;
                priorityFactorExpnt = radarPriorityFactorExpnt_s;
            }

            exponent = 9.0 - ((double)ptrPriority->ElementAt(sensorIndex) / 100.0);
            // apply the appropriate priority weight factors (optical versus radar)
            sensorScore *= (priorityFactorCoeff * SimpleMath::Power(priorityFactorExpnt, exponent));
            (*ptrSensorScore)[sensorIndex] = sensorScore;
        }

        ++sensorIndex;
    }

    return;
}

void
TargetDeck::BuildRequirements(Target *ptrTarget,
                              const Array2D<bool> *ptrActiveRequirements,
                              int numberOfResources,
                              double totalTargetArea) const
{
    int                       sensorIndex = 0;
    const int                 numberOfSensors = ptrActiveRequirements->Size();
    const int                 numberOfDecks = ptrActiveRequirements->GetSize(sensorIndex);
    ListIterator<TargetRegion> targetRegionListIter(ptrTarget->GetTargetRegionList());
    TargetRegion              *ptrTargetRegion = targetRegionListIter.First();
    Region                    *ptrRegion = NULL;
    CollectionProblemSet      *ptrCPS = NULL;
    ListIterator<CollectionProblemSet> cpsListIter(ptrTarget->GetCollectionProblemSetList());

    try {
        int numberOfPoints = 0;
        int deckIndex = 0;
        double effectiveArea = 0.0;
        double cpsScore = 0.0;

        if (ptrTarget->IsPointTarget() == true) {
            numberOfPoints = 1;
        }

        while (ptrTargetRegion != NULL) {
            double targetRegionArea;
            double areaRatio;
            targetRegionArea = ptrTargetRegion->GetTargetArea();
            areaRatio = targetRegionArea / totalTargetArea;
            ptrRegion = ptrTargetRegion->GetRegion();
            ptrCPS = cpsListIter.First();

            while (ptrCPS != NULL) {
                sensorIndex = 0;

                while (sensorIndex < numberOfSensors) {
                    cpsScore = ptrCPS->GetScore(sensorIndex);

                    if (cpsScore > 0.0) {
                        effectiveArea = targetRegionArea;

                        if (ptrCPS->IsStereoMode(sensorIndex) == true) {
                            effectiveArea *= 2.0;
                        }

                        deckIndex = 0;

                        while (deckIndex < numberOfDecks) {
                            if (ptrActiveRequirements->ElementAt(sensorIndex, deckIndex) == true) {
                                TimelineGenerator::requirementMap_s->UpdateRequirementData(
                                    ptrRegion->GetFullRegionNumber(),
                                    deckIndex, sensorIndex,
                                    ptrTarget->GetMissionIndex(ptrCPS->GetCollectionMode(sensorIndex)),
                                    ptrCPS->GetQualityIndex(sensorIndex),
                                    Target::ConvertPriorityToIndex(ptrCPS->GetPriority(sensorIndex)),
                                    GetSubPriorityLevel(ptrCPS->GetPriority(sensorIndex)),
                                    (cpsScore * areaRatio), effectiveArea, numberOfPoints,
                                    numberOfDecks, numberOfResources);
                            }

                            ++deckIndex;
                        }
                    }

                    ++sensorIndex;
                }

                ptrCPS = cpsListIter.Next();
            }

            ptrTargetRegion = targetRegionListIter.Next();
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "BuildRequirements(Target*, const Array2D<bool>*, int, ...)");
        throw;
    }

    return;
}

void
TargetDeck::DetermineBestCollectionProblemSet() const
{
    Region              *ptrRegion = NULL;
    TargetRegion        *ptrTargetRegion = NULL;
    ListIterator<Target> targetListIter(ptrTargetList_a);
    Target              *ptrTarget = targetListIter.First();

    while (ptrTarget != NULL) {
        ptrTarget->DetermineBestCollectionProblemSet();
        ptrTarget = targetListIter.Next();
    }

    return;
}

int
TargetDeck::GetEffectiveSimulationTimes(int &simulationStart,
                                        int &simulationEnd) const
{
    const int startYear = TimePiece::GetStartYear();
    const int startMonth = TimePiece::GetStartMonth();
    simulationStart = 0;
    simulationEnd = 0;

    if (effectivity_s == MONTH_ONLY) {
        simulationStart = Date::GetCummulativeDays(startYear, (startMonth - 1)) + 1;
        simulationEnd = Date::GetCummulativeDays(startYear, startMonth);
    } else if (effectivity_s == MONTH_AND_DAY) {
        simulationStart = Date::GetCummulativeDays(startYear, startMonth,
                          TimePiece::GetStartDay());
        simulationEnd = simulationStart;
    }

    return(startYear);
}

bool
TargetDeck::EffectiveTarget(const char &statusFlag,
                            const string &dateString,
                            int duration,
                            int simulationStart,
                            int simulationEnd,
                            int startYear) const
{
    int  effectiveStart = 0;
    int  effectiveEnd = 0;
    bool   effectiveTarget = false;
    Date  *ptrDate = NULL;

    if (effectivity_s == ALL_TARGETS) {
        effectiveTarget = true;
    } else if (statusFlag == ACTIVE_TARGET_ID_s) {
        if (effectivity_s == ALL_ACTIVE_TARGETS) {
            effectiveTarget = true;
        } else if (duration >= (int)DAYS_PER_YEAR) {
            // a duration of 365 indicates a 'standing target'
            effectiveTarget = true;
        } else {
            ptrDate = new Date(dateString);
            effectiveStart = Date::GetCummulativeDays(startYear, ptrDate->GetMonth(),
                             ptrDate->GetDay());
            // The minus one is important because the duration includes the target effective day
            effectiveEnd = effectiveStart + duration - 1;

            // if this effectivity starts after the simulation end, move it back
            // a year (since some effectivities span more than one year)
            if (effectiveStart > simulationEnd) {
                effectiveStart -= (int)DAYS_PER_YEAR;
                effectiveEnd -= (int)DAYS_PER_YEAR;
            }

            if ( (effectiveStart <= simulationEnd) &&
                    (effectiveEnd >= simulationStart) ) {
                effectiveTarget = true;
            }

            delete ptrDate;
            ptrDate = NULL;
        }
    }

    return(effectiveTarget);
}

double
TargetDeck::GetCountryFactor(const string &countryCode)
{
    double           coutryCodeFactor = 1.0;
    ListIterator<CountryFactor> countryFactorListIter(ptrCountryFactorList_s);
    CountryFactor  *ptrCountryFactor = countryFactorListIter.First();

    while (ptrCountryFactor != NULL) {
        if (ptrCountryFactor->ContainsCountryCode(countryCode) == true) {
            coutryCodeFactor = ptrCountryFactor->GetFactor();
            ptrCountryFactor = NULL;
        } else {
            ptrCountryFactor = countryFactorListIter.Next();
        }
    }

    return(coutryCodeFactor);
}

string
TargetDeck::GetTargetDeckFileName(int deckIndex)
{
    string targetFileName = "";

    if (ptrTargetDeckFileNameArray_s == NULL) {
        throw new NullPointerException(GetClassName(), "GetTargetDeckFileName(int)",
                                       "ptrTargetDeckFileNameArray_s");
    }

    try {
        targetFileName = ptrTargetDeckFileNameArray_s->ElementAt(deckIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetTargetDeckFileName(int)");
        throw;
    }

    return(targetFileName);
}

void
TargetDeck::PrintSummary(ofstream &outfile,
                         const Array<bool> *ptrActiveSensor) const
{
    int                quality = 0;
    Array<int>        *ptrPriorityLevel = new Array<int>();
    Array<int>        *ptrQualityLevel = new Array<int>();
    int                 missionIndex = 0;
    int                 sensorIndex = 0;
    int                 numberOfQualities = 0;
    int                 numberOfPriorities = 0;
    const int           numberOfMissions = Target::GetNumberOfMissions();
    const int           numberOfSensors = ptrActiveSensor->Size();
    Array<int>         *ptrTotalTargets = new Array<int>(numberOfMissions, 0);
    Array<int>         *ptrCommonTargets = new Array<int>(numberOfMissions, 0);
    Array<int>         *ptrUniqueTargets = new Array<int>(numberOfMissions, 0);
    Array<int>         *ptrTargetsCollected = new Array<int>(numberOfMissions, 0);
    double                targetArea = 0.0;
    double                areaCollected = 0.0;
    Array<double>       *ptrAmountOfArea = new Array<double>(numberOfMissions, 0.0);
    Array<double>       *ptrAreaCollected = new Array<double>(numberOfMissions, 0.0);
    Array2D<double>     *ptrPriorityBreakout = NULL;
    Array3D<double>     *ptrQualityBreakout = NULL;
    string               missionString = "";
    ListIterator<Target> targetListIter(ptrTargetList_a);
    Target              *ptrTarget = targetListIter.First();

    while (ptrTarget != NULL) {
        missionIndex = ptrTarget->GetMissionIndex();
        (*ptrTotalTargets)[missionIndex]++;

        if (ptrTarget->HasAchievableCollectionProblemSet() == true) {
            targetArea = ptrTarget->GetTotalTargetArea();

            if (ptrTarget->IsCommonCountdown() == true) {
                (*ptrCommonTargets)[missionIndex]++;
            } else {
                (*ptrUniqueTargets)[missionIndex]++;
            }

            (*ptrAmountOfArea)[missionIndex] += (double)targetArea;
            areaCollected = ptrTarget->GetTargetAreaCollected();

            if (areaCollected > 0.0) {
                (*ptrTargetsCollected)[missionIndex]++;
                (*ptrAreaCollected)[missionIndex] += (double)areaCollected;
                ptrPriorityLevel->InsertUniqueElement(ptrTarget->GetCollectedPriority());
                sensorIndex = 0;

                while (sensorIndex < numberOfSensors) {
                    quality = ptrTarget->GetRequiredQuality(sensorIndex);

                    if (quality > 0) {
                        ptrQualityLevel->InsertUniqueElement(quality);
                    }

                    ++sensorIndex;
                }
            }
        }

        ptrTarget = targetListIter.Next();
    }

    outfile << NEW_LINE << NEW_LINE;
    outfile << "           Total                               Area      Targets      Area      Target    Area" << NEW_LINE;
    outfile << "Mission   Targets   Common/Unique/Unachv    Achievable  Collected   Collected    Sat.     Sat." << NEW_LINE;
    outfile << "-------   -------   --------------------    ----------  ---------   ---------   ------   ------" << NEW_LINE;
    missionIndex = 0;

    while (missionIndex < numberOfMissions) {
        missionString = Target::GetMissionString(missionIndex);
        outfile << setw(8) << missionString;
        outfile << " " << setw(9) << ptrTotalTargets->ElementAt(missionIndex);
        outfile << " " << setw(8) << ptrCommonTargets->ElementAt(missionIndex);
        outfile << "/" << setw(6) << ptrUniqueTargets->ElementAt(missionIndex);
        outfile << "/" << setw(6) << (ptrTotalTargets->ElementAt(missionIndex)
                                      - ptrCommonTargets->ElementAt(missionIndex)
                                      - ptrUniqueTargets->ElementAt(missionIndex));
        outfile << " " << setw(13) << setprecision(1) << ptrAmountOfArea->ElementAt(missionIndex);
        outfile << " " << setw(10) << ptrTargetsCollected->ElementAt(missionIndex);
        outfile << " " << setw(11) << setprecision(1) << ptrAreaCollected->ElementAt(missionIndex);
        outfile << " " << setw(8) << setprecision(2)
                << SimpleMath::Percent(ptrTargetsCollected->ElementAt(missionIndex),
                                       ptrTotalTargets->ElementAt(missionIndex));
        outfile << " " << setw(8) << setprecision(2)
                << SimpleMath::Percent(ptrAreaCollected->ElementAt(missionIndex),
                                       ptrAmountOfArea->ElementAt(missionIndex)) << NEW_LINE;
        ++missionIndex;
    }

    outfile << NEW_LINE << "Totals :";
    outfile << setw(9) << ptrTotalTargets->GetSum();
    outfile << " " << setw(8) << ptrCommonTargets->GetSum();
    outfile << "/" << setw(6) << ptrUniqueTargets->GetSum();
    outfile << "/" << setw(6) << (ptrTotalTargets->GetSum() - ptrCommonTargets->GetSum()
                                  - ptrUniqueTargets->GetSum());
    outfile << " " << setw(13) << setprecision(1) << ptrAmountOfArea->GetSum();
    outfile << " " << setw(10) << ptrTargetsCollected->GetSum();
    outfile << " " << setw(11) << setprecision(1) << ptrAreaCollected->GetSum();
    outfile << " " << setw(8) << setprecision(2)
            << SimpleMath::Percent(ptrTargetsCollected->GetSum(), ptrTotalTargets->GetSum());
    outfile << " " << setw(8) << setprecision(2)
            << SimpleMath::Percent(ptrAreaCollected->GetSum(), ptrAmountOfArea->GetSum()) << NEW_LINE;
    delete ptrTotalTargets;
    ptrTotalTargets = NULL;
    delete ptrCommonTargets;
    ptrCommonTargets = NULL;
    delete ptrUniqueTargets;
    ptrUniqueTargets = NULL;
    delete ptrTargetsCollected;
    ptrTargetsCollected = NULL;
    delete ptrAmountOfArea;
    ptrAmountOfArea = NULL;
    delete ptrAreaCollected;
    ptrAreaCollected = NULL;
    numberOfQualities = ptrQualityLevel->Size();
    numberOfPriorities = ptrPriorityLevel->Size();

    if ( (numberOfQualities > 0) && (numberOfPriorities > 0) ) {
        int qualityIndex = 0;
        int priorityIndex = 0;
        double breakout = 0.0;
        double totalBreakout = 0.0;
        ptrPriorityBreakout = new Array2D<double>(numberOfMissions, numberOfPriorities, 0.0);
        ptrQualityBreakout = new Array3D<double>(numberOfSensors, numberOfMissions, numberOfQualities, 0.0);
        ptrTarget = targetListIter.First();

        while (ptrTarget != NULL) {
            areaCollected = ptrTarget->GetTargetAreaCollected();

            if (areaCollected > 0.0) {
                missionIndex = ptrTarget->GetMissionIndex();
                priorityIndex = ptrPriorityLevel->FindIndex(ptrTarget->GetCollectedPriority());

                if (priorityIndex >= 0) {
                    if (missionIndex <= Target::GROUP_POINT) {
                        (*ptrPriorityBreakout)[missionIndex][priorityIndex] += 1.0;
                    } else {
                        (*ptrPriorityBreakout)[missionIndex][priorityIndex] += (double)areaCollected;
                    }
                }

                sensorIndex = 0;

                while (sensorIndex < numberOfSensors) {
                    if (ptrActiveSensor->ElementAt(sensorIndex) == true) {
                        qualityIndex = ptrQualityLevel->FindIndex(ptrTarget->GetRequiredQuality(sensorIndex));
                        areaCollected = ptrTarget->GetTargetAreaCollected(sensorIndex);

                        if ( (qualityIndex >= 0) && (areaCollected > 0.0) ) {
                            if (missionIndex <= Target::GROUP_POINT) {
                                (*ptrQualityBreakout)[sensorIndex][missionIndex][qualityIndex] += 1.0;
                            } else {
                                (*ptrQualityBreakout)[sensorIndex][missionIndex][qualityIndex] += (double)areaCollected;
                            }
                        }
                    }

                    ++sensorIndex;
                }
            }

            ptrTarget = targetListIter.Next();
        }

        outfile << NEW_LINE << NEW_LINE << "              Target Collection By Priority" << NEW_LINE << NEW_LINE;
        outfile << "Priority:";
        priorityIndex = 0;

        while (priorityIndex < numberOfPriorities) {
            outfile << setw(9) << ptrPriorityLevel->ElementAt(priorityIndex);
            ++priorityIndex;
        }

        outfile << "    Total" << NEW_LINE << NEW_LINE;
        missionIndex = 0;

        while (missionIndex < numberOfMissions) {
            missionString = Target::GetMissionString(missionIndex);
            outfile << setw(9) << missionString;
            totalBreakout = 0.0;
            priorityIndex = 0;

            while (priorityIndex < numberOfPriorities) {
                breakout = ptrPriorityBreakout->ElementAt(missionIndex, priorityIndex);

                if (missionIndex <= Target::GROUP_POINT) {
                    outfile << " " << setw(8) << (int)breakout;
                } else {
                    outfile << " " << setw(8) << setprecision(1) << breakout;
                }

                totalBreakout += breakout;
                ++priorityIndex;
            }

            if (missionIndex <= Target::GROUP_POINT) {
                outfile << " " << setw(8) << totalBreakout;
            } else {
                outfile << " " << setw(8) << setprecision(1) << totalBreakout;
            }

            outfile << NEW_LINE;
            ++missionIndex;
        }

        outfile << NEW_LINE << "Points   ";
        totalBreakout = 0.0;
        priorityIndex = 0;

        while (priorityIndex < numberOfPriorities) {
            breakout = 0.0;
            missionIndex = 0;

            while (missionIndex <= Target::GROUP_POINT) {
                breakout += ptrPriorityBreakout->ElementAt(missionIndex, priorityIndex);
                ++missionIndex;
            }

            outfile << " " << setw(8) << (int)breakout;
            totalBreakout += breakout;
            ++priorityIndex;
        }

        outfile << " " << setw(8) << (int)totalBreakout << NEW_LINE << NEW_LINE;
        outfile << "Area     ";
        totalBreakout = 0.0;
        priorityIndex = 0;

        while (priorityIndex < numberOfPriorities) {
            breakout = 0.0;
            missionIndex = Target::BROAD_AREA_SEARCH;

            while (missionIndex < numberOfMissions) {
                breakout += ptrPriorityBreakout->ElementAt(missionIndex, priorityIndex);
                ++missionIndex;
            }

            outfile << " " << setw(8) << setprecision(1) << breakout;
            totalBreakout += breakout;
            ++priorityIndex;
        }

        outfile << " " << setw(8) << setprecision(1) << totalBreakout << NEW_LINE;
        delete ptrPriorityBreakout;
        ptrPriorityBreakout = NULL;
        sensorIndex = 0;

        while (sensorIndex < numberOfSensors) {
            if (ptrActiveSensor->ElementAt(sensorIndex) == true) {
                outfile << NEW_LINE << NEW_LINE << "              Target Collection By Quality For ";
                outfile << Sensor::GetSensorTypeString(sensorIndex) << NEW_LINE << NEW_LINE;
                outfile << "Quality :";
                qualityIndex = 0;

                while (qualityIndex < numberOfQualities) {
                    outfile << setw(9) << ptrQualityLevel->ElementAt(qualityIndex);
                    ++qualityIndex;
                }

                outfile << "    Total" << NEW_LINE << NEW_LINE;
                missionIndex = 0;

                while (missionIndex < numberOfMissions) {
                    missionString = Target::GetMissionString(missionIndex);
                    outfile << setw(9) << missionString;
                    totalBreakout = 0.0;
                    qualityIndex = 0;

                    while (qualityIndex < numberOfQualities) {
                        breakout = ptrQualityBreakout->ElementAt(sensorIndex, missionIndex, qualityIndex);

                        if (missionIndex <= Target::GROUP_POINT) {
                            outfile << " " << setw(8) << (int)breakout;
                        } else {
                            outfile << " " << setw(8) << setprecision(1) << breakout;
                        }

                        totalBreakout += breakout;
                        ++qualityIndex;
                    }

                    if (missionIndex <= Target::GROUP_POINT) {
                        outfile << " " << setw(8) << (int)totalBreakout;
                    } else {
                        outfile << " " << setw(8) << setprecision(1) << totalBreakout;
                    }

                    outfile << NEW_LINE;
                    ++missionIndex;
                }

                outfile << NEW_LINE << "Points   ";
                totalBreakout = 0.0;
                qualityIndex = 0;

                while (qualityIndex < numberOfQualities) {
                    breakout = 0.0;
                    missionIndex = 0;

                    while (missionIndex <= Target::GROUP_POINT) {
                        breakout += ptrQualityBreakout->ElementAt(sensorIndex, missionIndex, qualityIndex);
                        ++missionIndex;
                    }

                    outfile << " " << setw(8) << (int)breakout;
                    totalBreakout += breakout;
                    ++qualityIndex;
                }

                outfile << " " << setw(8) << (int)totalBreakout << NEW_LINE << NEW_LINE;
                outfile << "Area     ";
                totalBreakout = 0.0;
                qualityIndex = 0;

                while (qualityIndex < numberOfQualities) {
                    breakout = 0.0;
                    missionIndex = Target::BROAD_AREA_SEARCH;

                    while (missionIndex < numberOfMissions) {
                        breakout += ptrQualityBreakout->ElementAt(sensorIndex, missionIndex, qualityIndex);
                        ++missionIndex;
                    }

                    outfile << " " << setw(8) << setprecision(1) << breakout;
                    totalBreakout += breakout;
                    ++qualityIndex;
                }

                outfile << " " << setw(8) << setprecision(1) << totalBreakout << NEW_LINE;
            }

            ++sensorIndex;
        }

        delete ptrQualityBreakout;
        ptrQualityBreakout = NULL;
    }

    //CPPCHECK_FIX Error - memory leak
    delete ptrPriorityLevel;
    ptrPriorityLevel = NULL;
    //CPPCHECK_FIX Error - memory leak
    delete ptrQualityLevel;
    ptrQualityLevel = NULL;
    return;
}

void
TargetDeck::DestroyTargetDeck()
{
    if (ptrDeckFileBaseDirectory_s != NULL) {
        delete ptrDeckFileBaseDirectory_s;
        ptrDeckFileBaseDirectory_s = NULL;
    }

    if (ptrTargetDeckFileNameArray_s != NULL) {
        delete ptrTargetDeckFileNameArray_s;
        ptrTargetDeckFileNameArray_s = NULL;
    }

    if (ptrSubPriorityThreshold_s != NULL) {
        delete ptrSubPriorityThreshold_s;
        ptrSubPriorityThreshold_s = NULL;
    }

    if (ptrCountryFactorList_s != NULL) {
        ptrCountryFactorList_s->Delete();
        delete ptrCountryFactorList_s;
        ptrCountryFactorList_s = NULL;
    }

    return;
}

TargetDeck::~TargetDeck()
{
    if (ptrTargetList_a != NULL) {
        ptrTargetList_a->Delete();
        delete ptrTargetList_a;
        ptrTargetList_a = NULL;
    }

    return;
}
