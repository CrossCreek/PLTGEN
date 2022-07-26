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
 * LinkCriteria.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "LinkCriteria.h"
#include "ConjunctionAngleData.h"
#include "OverheadData.h"
#include "mural/io/FileNames.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/io/OutputGenerator.h"


#include "general/array/ListIterator.h"
#include "general/data/UnitData.h"
#include "general/utility/Constant.h"
#include "general/utility/StringEditor.h"
#include "general/utility/Utility.h"
#include "general/time/TimePiece.h"

const int LinkCriteria::MINIMUM_CONJUNCTION_SENDERS_s = 2;

LinkCriteria::LinkCriteria()
    : maximumManeuverTime_a(0), minimumICCLSolarIntrusionAngle_a(UNDEFINED_ANGLE),
      ptrRelayCoverageContourArray_a(NULL),
      linkPathMethod_a(UNKNOWN_METHOD), conjunctionMethod_a(UNKNOWN_METHOD),
      ptrNBConjunctionAngleDataList_a(NULL),
      ptrWBConjunctionAngleDataList_a(NULL), ptrOverheadDataList_a(NULL)
{
    return;
}

LinkCriteria::LinkCriteria(shared_ptr<InputDataElement> ptrInputDataElement,
                           const bool &dataTransmitter)
    : maximumManeuverTime_a(0), minimumICCLSolarIntrusionAngle_a(UNDEFINED_ANGLE),
      ptrRelayCoverageContourArray_a(NULL),
      linkPathMethod_a(UNKNOWN_METHOD), conjunctionMethod_a(UNKNOWN_METHOD),
      ptrNBConjunctionAngleDataList_a(NULL),
      ptrWBConjunctionAngleDataList_a(NULL), ptrOverheadDataList_a(NULL)
{
    try {
        ExtractLinkCriteriaInfo(ptrInputDataElement, dataTransmitter);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "LinkCriteria(InputDataElement*)");
        throw;
    }
}

LinkCriteria::LinkCriteria(const LinkCriteria &copyMe)
    : maximumManeuverTime_a(copyMe.maximumManeuverTime_a),
      minimumICCLSolarIntrusionAngle_a(copyMe.minimumICCLSolarIntrusionAngle_a),
      ptrRelayCoverageContourArray_a(NULL),
      linkPathMethod_a(copyMe.linkPathMethod_a),
      conjunctionMethod_a(copyMe.conjunctionMethod_a),
      ptrNBConjunctionAngleDataList_a(NULL), ptrWBConjunctionAngleDataList_a(NULL),
      ptrOverheadDataList_a(NULL)
{
    if (copyMe.ptrRelayCoverageContourArray_a != NULL) {
        ptrRelayCoverageContourArray_a
            = shared_ptr<vector<int>>(new vector<int>(*copyMe.ptrRelayCoverageContourArray_a));
    }

    if (copyMe.ptrNBConjunctionAngleDataList_a != NULL) {
        ptrNBConjunctionAngleDataList_a = new LinkList<ConjunctionAngleData>
        (*(copyMe.ptrNBConjunctionAngleDataList_a));
    }

    if (copyMe.ptrWBConjunctionAngleDataList_a != NULL) {
        ptrWBConjunctionAngleDataList_a = new LinkList<ConjunctionAngleData>
        (*(copyMe.ptrWBConjunctionAngleDataList_a));
    }

    if (copyMe.ptrOverheadDataList_a != NULL) {
        ptrOverheadDataList_a
            = new LinkList<OverheadData>(*(copyMe.ptrOverheadDataList_a));
    }

    return;
}

LinkCriteria&
LinkCriteria::operator = (const LinkCriteria &copyMe)
{
    maximumManeuverTime_a            = copyMe.maximumManeuverTime_a;
    minimumICCLSolarIntrusionAngle_a = copyMe.minimumICCLSolarIntrusionAngle_a;
    ptrRelayCoverageContourArray_a   = copyMe.ptrRelayCoverageContourArray_a;
    linkPathMethod_a                 = copyMe.linkPathMethod_a;
    conjunctionMethod_a              = copyMe.conjunctionMethod_a;
    ptrOverheadDataList_a            = copyMe.ptrOverheadDataList_a;
    return(*this);
}

bool
LinkCriteria::operator == (const LinkCriteria &compareMe) const
{
    return( (maximumManeuverTime_a == compareMe.maximumManeuverTime_a) &&
            (minimumICCLSolarIntrusionAngle_a == compareMe.minimumICCLSolarIntrusionAngle_a) &&
            (linkPathMethod_a == compareMe.linkPathMethod_a) &&
            (conjunctionMethod_a == compareMe.conjunctionMethod_a) &&
            (*ptrOverheadDataList_a == *(compareMe.ptrOverheadDataList_a)) );
}

bool
LinkCriteria::operator != (const LinkCriteria &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
LinkCriteria::ExtractLinkCriteriaInfo(shared_ptr<InputDataElement> ptrInputDataElement,
                                      const bool &dataTransmitter)
{
    int         defaultAquireTime = -1;
    int         defaultDroplinkTime = -1;
    int         defaultDropTimeMBB = -1;
    int         defaultStartUpTimeMBB = -1;
    int          index = 0;
    string        errorString = "";
    string        resourceDesignator = "";
    OverheadData *ptrOverheadData = NULL;
    OverheadData *ptrDefaultOverheadData = NULL;
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData> ptrInputData = NULL;

    try {
        int numberOfValues = 0;

        for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
            ptrInputData = (*inputDataIter);

            if (ptrInputData->LabelContainsString("Link Path Method") == true) {
                if (dataTransmitter == true) {
                    linkPathMethod_a = GetCreationMethod(ptrInputData->GetStringValue());
                }
            } else if (ptrInputData->LabelContainsString("Conjunction Method") == true) {
                conjunctionMethod_a = GetCreationMethod(ptrInputData->GetStringValue());
            } else if (ptrInputData->LabelContainsString("Default Acquire Time") == true) {
                defaultAquireTime
                    = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Default Droplink Time") == true) {
                defaultDroplinkTime
                    = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Default Make Before Break Droplink Time") == true) {
                defaultDropTimeMBB
                    = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Default Make Before Break Start Up Time") == true) {
                defaultStartUpTimeMBB
                    = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Maximum Maneuver Time") == true) {
                maximumManeuverTime_a
                    = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Minimum ICCL Solar Intrusion Angle") == true) {
                minimumICCLSolarIntrusionAngle_a
                    = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
            } else if (ptrInputData->LabelContainsString("Relay Coverage Contour Value(s)") == true) {
                numberOfValues = ptrInputData->GetNumberOfValues();

                if (numberOfValues > 0) {
                    if (ptrRelayCoverageContourArray_a == NULL) {
                        ptrRelayCoverageContourArray_a = shared_ptr<vector<int>>(new vector<int>());
                    }

                    index = 0;

                    while (index < numberOfValues) {
                        int contourValue = ptrInputData->GetIntValue(index);

                        if ( Utility::VectorContains(*ptrRelayCoverageContourArray_a, contourValue) == false ) {
                            ptrRelayCoverageContourArray_a->push_back(contourValue);
                        }

                        ++index;
                    }
                }
            } else if (ptrInputData->LabelContainsString("Narrowband Conjunction Angle") == true) {
                SetNarrowbandConjunctionAngleData(ptrInputData);
            } else if (ptrInputData->LabelContainsString("Wideband Conjunction Angle") == true) {
                SetWidebandConjunctionAngleData(ptrInputData);
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractLinkCriteriaInfo(InputDataElement*)");
        throw;
    }

    if (ptrNBConjunctionAngleDataList_a != NULL) {
        if ( (ptrNBConjunctionAngleDataList_a->Size() == 1) &&
                ((ptrNBConjunctionAngleDataList_a->GetFirstObject())->GetConjunctionAngle() <= 0.0) ) {
            ptrNBConjunctionAngleDataList_a->Delete();
            delete ptrNBConjunctionAngleDataList_a;
            ptrNBConjunctionAngleDataList_a = NULL;
        }
    }

    if (ptrWBConjunctionAngleDataList_a != NULL) {
        if ( (ptrWBConjunctionAngleDataList_a->Size() == 1) &&
                ((ptrWBConjunctionAngleDataList_a->GetFirstObject())->GetConjunctionAngle() <= 0.0) ) {
            ptrWBConjunctionAngleDataList_a->Delete();
            delete ptrWBConjunctionAngleDataList_a;
            ptrWBConjunctionAngleDataList_a = NULL;
        }
    }

    if (dataTransmitter == true) {
        if (linkPathMethod_a == UNKNOWN_METHOD) {
            errorString = "Missing/invalid 'Link Path Method' input";
        }

        if (defaultAquireTime < 0) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing/invalid 'Default Acquire Time' input";
        }

        if (defaultDroplinkTime < 0) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing/invalid 'Default Droplink Time' input";
        }

        if (maximumManeuverTime_a < 0) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Invalid 'Maximum Maneuver Time' input";
        }

        if (ptrRelayCoverageContourArray_a != NULL) {
            if (FileNames::HasFileNameDefined(FileNames::RELAY_COVERAGE_OUTPUTFILE) == true) {
                if (errorString.size() > 0) {
                    errorString += "\n         ";
                }

                errorString += "Must specify a 'Relay Coverage Output File' to print 'Relay Coverage Contour Value(s)'";
            } else {
                index = 0;

                while (index < (int)ptrRelayCoverageContourArray_a->size()) {
                    int contourValue = 0;
                    contourValue = ptrRelayCoverageContourArray_a->at(index);

                    if ( (contourValue < 0) || (contourValue > 100) ) {
                        if (errorString.size() > 0) {
                            errorString += "\n         ";
                        }

                        errorString += "Invalid 'Relay Coverage Contour Value' of ";
                        errorString += StringEditor::ConvertToString(contourValue);
                    }

                    ++index;
                }
            }
        }
    }

    if ((ptrNBConjunctionAngleDataList_a != NULL) || (ptrWBConjunctionAngleDataList_a != NULL)) {
        if (conjunctionMethod_a == UNKNOWN_METHOD) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Invalid 'Conjunction Method' input";
        }
    } else {
        conjunctionMethod_a = UNKNOWN_METHOD;
    }

    if ( (minimumICCLSolarIntrusionAngle_a != UNDEFINED_ANGLE) &&
            (minimumICCLSolarIntrusionAngle_a < 0.0) ) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Invalid 'Minimum ICCL Solar Intrusion Angle' input";
    }

    if ((errorString.size() == 0) && (dataTransmitter == true)) {
        int overheadTime = 0;
        ptrOverheadDataList_a = new LinkList<OverheadData>();
        ptrDefaultOverheadData = new OverheadData(DEFAULT_STRING,
                defaultAquireTime, defaultDroplinkTime,
                defaultDropTimeMBB,
                defaultStartUpTimeMBB);
        ptrOverheadDataList_a->Append(ptrDefaultOverheadData);

        for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
            ptrInputData = (*inputDataIter);

            if (MURALInputProcessor::FindValidDataProvider(ptrInputData, resourceDesignator) == true) {
                overheadTime = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
                ptrOverheadData = GetOverheadData(resourceDesignator);

                if (ptrOverheadData == NULL) {
                    ptrOverheadData = new OverheadData(*ptrDefaultOverheadData);
                    ptrOverheadData->SetWithResourceDesignator(resourceDesignator);
                    ptrOverheadDataList_a->Append(ptrOverheadData);
                }

                if (ptrInputData->LabelContainsString("Acquire Time") == true) {
                    ptrOverheadData->SetAcquistionTime(overheadTime);
                } else if (ptrInputData->LabelContainsString("Make Before Break Droplink Time") == true) {
                    ptrOverheadData->SetMakeBeforeBreakDroplinkTime(overheadTime);
                } else if (ptrInputData->LabelContainsString("Make Before Break Start Up Time") == true) {
                    ptrOverheadData->SetMakeBeforeBreakStartUpTime(overheadTime);
                } else if (ptrInputData->LabelContainsString("Droplink Time") == true) {
                    ptrOverheadData->SetDroplinkTime(overheadTime);
                }
            }
        }
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractLinkCriteriaInfo(InputDataElement*)",
                                 errorString);
    }

    return;
}

void
LinkCriteria::PrintAttributes(ostream &outfile) const
{
    int           simultaneousSenders = 0;
    string          specificTypeString = "";
    string          nearDesignator = "";
    string          farDesignator = "";
    const TIME_UNIT inputTimeUnits = UnitData::GetInputTimeUnits();
    ListIterator<OverheadData>         overheadDataListIter(ptrOverheadDataList_a);
    OverheadData                      *ptrOverheadData = overheadDataListIter.First();
    ConjunctionAngleData              *ptrConjunction = NULL;
    ListIterator<ConjunctionAngleData> conjunctionNBListIter(ptrNBConjunctionAngleDataList_a);
    ListIterator<ConjunctionAngleData> conjunctionWBListIter(ptrWBConjunctionAngleDataList_a);
    outfile << "   <LINK_GENERATION_BEGIN>" << NEW_LINE;

    if (linkPathMethod_a != UNKNOWN_METHOD) {
        outfile << "      Link Path Method (READ/GENERATE)                        : ";
        outfile << GetLinkPathMethodString() << NEW_LINE;
    }

    if (conjunctionMethod_a != UNKNOWN_METHOD) {
        outfile << "      Conjunction Method (READ/GENERATE)                      : ";
        outfile << GetConjunctionMethodString() << NEW_LINE;
    }

    if (ptrOverheadData != NULL) {
        int defaultAcquireTime = 0;
        int defaultDroplinkTime = 0;
        int defaultDropTimeMBB = 0;
        int defaultStartUpTimeMBB = 0;
        defaultAcquireTime = ptrOverheadData->GetAcquistionTime();
        outfile << "      Default Acquire Time                                    : ";
        TimePiece::PrintDuration(outfile, defaultAcquireTime, inputTimeUnits);
        outfile << NEW_LINE;
        defaultDroplinkTime = ptrOverheadData->GetDroplinkTime();
        outfile << "      Default Droplink Time                                   : ";
        TimePiece::PrintDuration(outfile, defaultDroplinkTime, inputTimeUnits);
        outfile << NEW_LINE;
        defaultDropTimeMBB = ptrOverheadData->GetMakeBeforeBreakDroplinkTime();

        if (defaultDropTimeMBB >= 0) {
            outfile << "      Default Make Before Break Droplink Time                 : ";
            TimePiece::PrintDuration(outfile, defaultDropTimeMBB, inputTimeUnits);
            outfile << NEW_LINE;
        }

        defaultStartUpTimeMBB = ptrOverheadData->GetMakeBeforeBreakStartUpTime();

        if (defaultStartUpTimeMBB >= 0) {
            outfile << "      Default Make Before Break Start Up Time                 : ";
            TimePiece::PrintDuration(outfile, defaultStartUpTimeMBB, inputTimeUnits);
            outfile << NEW_LINE;
        }

        //JOE:
        ptrOverheadData = overheadDataListIter.Next();

        while (ptrOverheadData != NULL) {
            int overheadTime = 0;
            specificTypeString = InputData::GetSpecificTypeString(ptrOverheadData->GetWithResourceDesignator());
            overheadTime = ptrOverheadData->GetAcquistionTime();

            if (overheadTime != defaultAcquireTime) {
                outfile << "        Acquire Time " << specificTypeString;
                outfile << setw(43) << " : ";
                TimePiece::PrintDuration(outfile, overheadTime, inputTimeUnits);
                outfile << NEW_LINE;
            }

            overheadTime = ptrOverheadData->GetDroplinkTime();

            if (overheadTime != defaultDroplinkTime) {
                outfile << "        Droplink Time " << specificTypeString;
                outfile << setw(42) << " : ";
                TimePiece::PrintDuration(outfile, overheadTime, inputTimeUnits);
                outfile << NEW_LINE;
            }

            overheadTime = ptrOverheadData->GetMakeBeforeBreakDroplinkTime();

            if (overheadTime != defaultDropTimeMBB) {
                outfile << "        Make Before Break Droplink Time " << specificTypeString;
                outfile << setw(24) << " : ";
                TimePiece::PrintDuration(outfile, overheadTime, inputTimeUnits);
                outfile << NEW_LINE;
            }

            overheadTime = ptrOverheadData->GetMakeBeforeBreakStartUpTime();

            if (overheadTime != defaultStartUpTimeMBB) {
                outfile << "        Make Before Break Start Up Time " << specificTypeString;
                outfile << setw(24) << " : ";
                TimePiece::PrintDuration(outfile, overheadTime, inputTimeUnits);
                outfile << NEW_LINE;
            }

            ptrOverheadData = overheadDataListIter.Next();
        }
    }

    if (maximumManeuverTime_a > 0) {
        outfile << "      Maximum Maneuver Time                                   : ";
        TimePiece::PrintDuration(outfile, maximumManeuverTime_a, inputTimeUnits);
        outfile << NEW_LINE;
    }

    if (minimumICCLSolarIntrusionAngle_a != UNDEFINED_ANGLE) {
        outfile << "      Minimum ICCL Solar Intrusion Angle                      : ";
        outfile << UnitData::GetOutputAngle(minimumICCLSolarIntrusionAngle_a) << NEW_LINE;
    }

    if (ptrRelayCoverageContourArray_a != NULL) {
        int index = 0;
        outfile << "      Relay Coverage Contour Value(s)                         : ";
        outfile << ptrRelayCoverageContourArray_a->at(index);
        ++index;

        while (index < (int)ptrRelayCoverageContourArray_a->size()) {
            outfile << ", " << ptrRelayCoverageContourArray_a->at(index);
            ++index;
        }

        outfile << NEW_LINE;
    }

    ptrConjunction = conjunctionNBListIter.First();

    while (ptrConjunction != NULL) {
        simultaneousSenders = ptrConjunction->GetNumberOfSimultaneousSenders();
        nearDesignator = ptrConjunction->GetNearDataProviderDesignator();
        farDesignator = ptrConjunction->GetFarDataProviderDesignator();

        if ((nearDesignator == DEFAULT_STRING) && (farDesignator == DEFAULT_STRING)) {
            outfile << "      Default Narrowband Conjunction Angle ";

            if (simultaneousSenders > MINIMUM_CONJUNCTION_SENDERS_s) {
                outfile << ", Senders          : ";
            } else {
                outfile << "                   : ";
            }
        } else {
            outfile << "        Narrowband Conjunction Angle ";
            outfile << setw(14) << InputData::GetSpecificTypeString(nearDesignator, farDesignator);

            if (simultaneousSenders > MINIMUM_CONJUNCTION_SENDERS_s) {
                outfile << " , Senders : ";
            } else {
                outfile << "           : ";
            }
        }

        outfile << UnitData::GetOutputAngle(ptrConjunction->GetConjunctionAngle());

        if (simultaneousSenders > MINIMUM_CONJUNCTION_SENDERS_s) {
            outfile << ", " << simultaneousSenders;
        }

        outfile << NEW_LINE;
        ptrConjunction = conjunctionNBListIter.Next();
    }

    ptrConjunction = conjunctionWBListIter.First();

    while (ptrConjunction != NULL) {
        simultaneousSenders = ptrConjunction->GetNumberOfSimultaneousSenders();
        nearDesignator = ptrConjunction->GetNearDataProviderDesignator();
        farDesignator = ptrConjunction->GetFarDataProviderDesignator();

        if ((nearDesignator == DEFAULT_STRING) && (farDesignator == DEFAULT_STRING)) {
            outfile << "      Default Wideband Conjunction Angle ";

            if (simultaneousSenders > MINIMUM_CONJUNCTION_SENDERS_s) {
                outfile << ", Senders            : ";
            } else {
                outfile << "                     : ";
            }
        } else {
            outfile << "        Wideband Conjunction Angle ";
            outfile << setw(14) << InputData::GetSpecificTypeString(nearDesignator, farDesignator);

            if (simultaneousSenders > MINIMUM_CONJUNCTION_SENDERS_s) {
                outfile << " , Senders   : ";
            } else {
                outfile << "             : ";
            }
        }

        outfile << UnitData::GetOutputAngle(ptrConjunction->GetConjunctionAngle());

        if (simultaneousSenders > MINIMUM_CONJUNCTION_SENDERS_s) {
            outfile << ", " << simultaneousSenders;
        }

        outfile << NEW_LINE;
        ptrConjunction = conjunctionWBListIter.Next();
    }

    outfile << "   <LINK_GENERATION_STOP>" << NEW_LINE;
    return;
}

void
LinkCriteria::SetNarrowbandConjunctionAngleData(const shared_ptr<InputData> ptrInputData)
{
    int        simultaneousSenders = MINIMUM_CONJUNCTION_SENDERS_s;
    const int   senderIndex = 1;
    double       previousAngle = 0.0;
    double       defaultAngle = 0.0;
    const double angle = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
    string       designator1 = "";
    string       designator2 = "";
    ConjunctionAngleData *ptrConjunctionAngleData = NULL;

    if (ptrInputData->LabelContainsString("Senders") == true) {
        if (ptrInputData->GetNumberOfValues() == senderIndex) {
            throw new InputException(GetClassName(),
                                     "SetNarrowbandConjunctionAngleData(const InputData*)",
                                     ("Must specifiy number of senders for " + ptrInputData->GetLabel()));
        }

        simultaneousSenders = ptrInputData->GetIntValue(senderIndex);

        if (simultaneousSenders <= MINIMUM_CONJUNCTION_SENDERS_s) {
            throw new InputException(GetClassName(),
                                     "SetNarrowbandConjunctionAngleData(const InputData*)",
                                     ("Number of senders must exceed two for " + ptrInputData->GetLabel()));
        }
    }

    if (ptrInputData->LabelContainsString("Default") == true) {
        if ((angle > 0.0) && (simultaneousSenders > MINIMUM_CONJUNCTION_SENDERS_s)) {
            previousAngle = GetDefaultNarrowbandConjunctionAngle(simultaneousSenders - 1);

            if ((previousAngle != UNDEFINED_ANGLE) && (angle <= previousAngle)) {
                throw new InputException(GetClassName(),
                                         "SetNarrowbandConjunctionAngleData(const InputData*)",
                                         "Each 'Default Narrowband Conjunction Angle' must exceed the previous fewer (n-1) senders angle");
            }
        }

        if (ptrNBConjunctionAngleDataList_a == NULL) {
            ptrNBConjunctionAngleDataList_a = new LinkList<ConjunctionAngleData>();
        }

        ptrConjunctionAngleData = new ConjunctionAngleData(angle, DEFAULT_STRING, DEFAULT_STRING,
                simultaneousSenders);
        ptrNBConjunctionAngleDataList_a->Append(ptrConjunctionAngleData);
        ptrConjunctionAngleData = NULL;
    } else if (MURALInputProcessor::FindValidDataProviders(ptrInputData, designator1, designator2) == true) {
        defaultAngle = GetDefaultNarrowbandConjunctionAngle(simultaneousSenders);

        if (defaultAngle == UNDEFINED_ANGLE) {
            throw new InputException(GetClassName(),
                                     "SetNarrowbandConjunctionAngleData(const InputData*)",
                                     "Must have 'Default Narrowband Conjunction Angle' before specific senders");
        }

        if ((angle > 0.0) && (simultaneousSenders > MINIMUM_CONJUNCTION_SENDERS_s)) {
            previousAngle = GetNarrowbandConjunctionAngle(designator1, designator2, (simultaneousSenders - 1));

            if ((previousAngle != UNDEFINED_ANGLE) && (angle <= previousAngle)) {
                throw new InputException(GetClassName(),
                                         "SetNarrowbandConjunctionAngleData(const InputData*)",
                                         "Each 'Narrowband Conjunction Angle' must exceed the previous fewer (n-1) senders angle");
            }
        }

        ptrConjunctionAngleData = new ConjunctionAngleData(angle, designator1, designator2,
                simultaneousSenders);
        ptrNBConjunctionAngleDataList_a->Append(ptrConjunctionAngleData);
        ptrConjunctionAngleData = NULL;
    }
}

void
LinkCriteria::SetWidebandConjunctionAngleData(const shared_ptr<InputData> ptrInputData)
{
    int        simultaneousSenders = MINIMUM_CONJUNCTION_SENDERS_s;
    const int   senderIndex = 1;
    double       previousAngle = 0.0;
    double       defaultAngle = 0.0;
    const double angle = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
    string       designator1 = "";
    string       designator2 = "";
    ConjunctionAngleData *ptrConjunctionAngleData = NULL;

    if (ptrInputData->LabelContainsString("Senders") == true) {
        if (ptrInputData->GetNumberOfValues() == senderIndex) {
            throw new InputException(GetClassName(),
                                     "SetWidebandConjunctionAngleData(const InputData*)",
                                     ("Must specifiy number of senders for " + ptrInputData->GetLabel()));
        }

        simultaneousSenders = ptrInputData->GetIntValue(senderIndex);

        if (simultaneousSenders <= MINIMUM_CONJUNCTION_SENDERS_s) {
            throw new InputException(GetClassName(),
                                     "SetWidebandConjunctionAngleData(const InputData*)",
                                     ("Number of senders must exceed two for " + ptrInputData->GetLabel()));
        }
    }

    if (ptrInputData->LabelContainsString("Default") == true) {
        if ((angle > 0.0) && (simultaneousSenders > MINIMUM_CONJUNCTION_SENDERS_s)) {
            previousAngle = GetDefaultWidebandConjunctionAngle(simultaneousSenders - 1);

            if ((previousAngle != UNDEFINED_ANGLE) && (angle <= previousAngle)) {
                throw new InputException(GetClassName(),
                                         "SetWidebandConjunctionAngleData(const InputData*)",
                                         "Each 'Default Wideband Conjunction Angle' must exceed the previous fewer (n-1) senders angle");
            }
        }

        if (ptrWBConjunctionAngleDataList_a == NULL) {
            ptrWBConjunctionAngleDataList_a = new LinkList<ConjunctionAngleData>();
        }

        ptrConjunctionAngleData = new ConjunctionAngleData(angle, DEFAULT_STRING, DEFAULT_STRING,
                simultaneousSenders);
        ptrWBConjunctionAngleDataList_a->Append(ptrConjunctionAngleData);
        ptrConjunctionAngleData = NULL;
    } else if (MURALInputProcessor::FindValidDataProviders(ptrInputData, designator1, designator2) == true) {
        defaultAngle = GetDefaultWidebandConjunctionAngle(simultaneousSenders);

        if (defaultAngle == UNDEFINED_ANGLE) {
            throw new InputException(GetClassName(),
                                     "SetWidebandConjunctionAngleData(const InputData*)",
                                     "Must have 'Default Wideband Conjunction Angle' before specific senders");
        }

        if ((angle > 0.0) && (simultaneousSenders > MINIMUM_CONJUNCTION_SENDERS_s)) {
            previousAngle = GetWidebandConjunctionAngle(designator1, designator2, (simultaneousSenders - 1));

            if ((previousAngle != UNDEFINED_ANGLE) && (angle <= previousAngle)) {
                throw new InputException(GetClassName(),
                                         "SetWidebandConjunctionAngleData(const InputData*)",
                                         "Each 'Wideband Conjunction Angle' must exceed the previous fewer (n-1) senders angle");
            }
        }

        ptrConjunctionAngleData = new ConjunctionAngleData(angle, designator1, designator2,
                simultaneousSenders);
        ptrWBConjunctionAngleDataList_a->Append(ptrConjunctionAngleData);
        ptrConjunctionAngleData = NULL;
    }
}

double
LinkCriteria::GetDefaultNarrowbandConjunctionAngle(int simultaneousSenders) const
{
    double                             defaultAngle = UNDEFINED_ANGLE;
    ListIterator<ConjunctionAngleData> conjAngleListIter(ptrNBConjunctionAngleDataList_a);
    ConjunctionAngleData              *ptrAngleData = conjAngleListIter.First();

    while (ptrAngleData != NULL) {
        if ( (ptrAngleData->GetNearDataProviderDesignator() == DEFAULT_STRING) &&
                (ptrAngleData->GetFarDataProviderDesignator() == DEFAULT_STRING) &&
                (ptrAngleData->GetNumberOfSimultaneousSenders() == simultaneousSenders) ) {
            defaultAngle = ptrAngleData->GetConjunctionAngle();
            ptrAngleData = NULL;
        } else {
            ptrAngleData = conjAngleListIter.Next();
        }
    }

    return(defaultAngle);
}

double
LinkCriteria::GetDefaultWidebandConjunctionAngle(int simultaneousSenders) const
{
    double                             defaultAngle = UNDEFINED_ANGLE;
    ListIterator<ConjunctionAngleData> conjAngleListIter(ptrWBConjunctionAngleDataList_a);
    ConjunctionAngleData              *ptrAngleData = conjAngleListIter.First();

    while (ptrAngleData != NULL) {
        if ( (ptrAngleData->GetNearDataProviderDesignator() == DEFAULT_STRING) &&
                (ptrAngleData->GetFarDataProviderDesignator() == DEFAULT_STRING) &&
                (ptrAngleData->GetNumberOfSimultaneousSenders() == simultaneousSenders) ) {
            defaultAngle = ptrAngleData->GetConjunctionAngle();
            ptrAngleData = NULL;
        } else {
            ptrAngleData = conjAngleListIter.Next();
        }
    }

    return(defaultAngle);
}

double
LinkCriteria::GetNarrowbandConjunctionAngle(const string &nearDesignator,
        const string &farDesignator,
        int simultaneousSenders) const
{
    double                             defaultAngle = UNDEFINED_ANGLE;
    double                             returnAngle = UNDEFINED_ANGLE;
    ListIterator<ConjunctionAngleData> conjAngleListIter(ptrNBConjunctionAngleDataList_a);
    ConjunctionAngleData              *ptrAngleData = conjAngleListIter.First();

    while (ptrAngleData != NULL) {
        if (ptrAngleData->GetNumberOfSimultaneousSenders() == simultaneousSenders) {
            if ( (ptrAngleData->GetNearDataProviderDesignator() == DEFAULT_STRING) &&
                    (ptrAngleData->GetFarDataProviderDesignator() == DEFAULT_STRING) ) {
                defaultAngle = ptrAngleData->GetConjunctionAngle();
            } else if ( (ptrAngleData->GetNearDataProviderDesignator() == nearDesignator) &&
                        (ptrAngleData->GetFarDataProviderDesignator() == farDesignator) ) {
                returnAngle = ptrAngleData->GetConjunctionAngle();
            }
        }

        ptrAngleData = conjAngleListIter.Next();
    }

    if ( (returnAngle == UNDEFINED_ANGLE) &&
            ((nearDesignator != farDesignator) || (nearDesignator == DEFAULT_STRING)) ) {
        returnAngle = defaultAngle;
    }

    return(returnAngle);
}

double
LinkCriteria::GetWidebandConjunctionAngle(const string &nearDesignator,
        const string &farDesignator,
        int simultaneousSenders) const
{
    double                             returnAngle = UNDEFINED_ANGLE;
    ListIterator<ConjunctionAngleData> conjAngleListIter(ptrWBConjunctionAngleDataList_a);
    ConjunctionAngleData              *ptrAngleData = conjAngleListIter.First();

    if (ptrAngleData != NULL) {
        double defaultAngle;
        defaultAngle = ptrAngleData->GetConjunctionAngle();
        //JOE:
        ptrAngleData = conjAngleListIter.Next();

        while (ptrAngleData != NULL) {
            if ( ((ptrAngleData->GetNearDataProviderDesignator() == nearDesignator) ||
                    (ptrAngleData->GetNearDataProviderDesignator() == DEFAULT_STRING)) &&
                    ((ptrAngleData->GetFarDataProviderDesignator() == farDesignator) ||
                     (ptrAngleData->GetFarDataProviderDesignator() == DEFAULT_STRING)) &&
                    (ptrAngleData->GetNumberOfSimultaneousSenders() == simultaneousSenders) ) {
                returnAngle = ptrAngleData->GetConjunctionAngle();
            }

            ptrAngleData = conjAngleListIter.Next();
        }

        if ( (returnAngle == UNDEFINED_ANGLE) &&
                ((nearDesignator != farDesignator) || (nearDesignator == DEFAULT_STRING)) ) {
            returnAngle = defaultAngle;
        }
    }

    return(returnAngle);
}

bool
LinkCriteria::HasSimultaneousConjunctionSenders(int simultaneousSenders) const
{
    bool                               hasSenders = false;
    ListIterator<ConjunctionAngleData> narrowbandConjListIter(ptrNBConjunctionAngleDataList_a);
    ConjunctionAngleData              *ptrConjunctionAngleData = narrowbandConjListIter.First();

    while ((ptrConjunctionAngleData != NULL) && (hasSenders == false)) {
        if (ptrConjunctionAngleData->GetNumberOfSimultaneousSenders() == simultaneousSenders) {
            hasSenders = true;
        } else {
            ptrConjunctionAngleData = narrowbandConjListIter.Next();
        }
    }

    if (hasSenders == false) {
        ListIterator<ConjunctionAngleData> widebandConjListIter(ptrWBConjunctionAngleDataList_a);
        ptrConjunctionAngleData = widebandConjListIter.First();

        while ((ptrConjunctionAngleData != NULL) && (hasSenders == false)) {
            if (ptrConjunctionAngleData->GetNumberOfSimultaneousSenders() == simultaneousSenders) {
                hasSenders = true;
            } else {
                ptrConjunctionAngleData = widebandConjListIter.Next();
            }
        }
    }

    return(hasSenders);
}

void
LinkCriteria::ResetMakeBeforeBreakTimes() const
{
    const int                RESET_VALUE = -1;
    ListIterator<OverheadData> overheadDataListIter(ptrOverheadDataList_a);
    OverheadData              *ptrOverheadData = overheadDataListIter.First();

    while (ptrOverheadData != NULL) {
        ptrOverheadData->SetMakeBeforeBreakDroplinkTime(RESET_VALUE);
        ptrOverheadData->SetMakeBeforeBreakStartUpTime(RESET_VALUE);
        ptrOverheadData = overheadDataListIter.Next();
    }

    return;
}

void
LinkCriteria::VerifyMakeBeforeBreakTimes() const
{
    string                     errorString = "";
    ListIterator<OverheadData> overheadDataListIter(ptrOverheadDataList_a);
    OverheadData              *ptrOverheadData = overheadDataListIter.First();

    if (ptrOverheadData->GetMakeBeforeBreakDroplinkTime() < 0) {
        errorString = "Missing/invalid 'Default Make Before Break Droplink Time' input";
    }

    if (ptrOverheadData->GetMakeBeforeBreakStartUpTime() < 0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Default Make Before Break Start Up Time' input";
    }

    //SIMPLE_WHILE: Remove assignment from condition portion of WHILE statement
    //JOE:
    ptrOverheadData = overheadDataListIter.Next();

    while (ptrOverheadData != NULL) {
        if (ptrOverheadData->GetMakeBeforeBreakDroplinkTime() < 0) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Invalid 'Make Before Break Droplink Time [";
            errorString += ptrOverheadData->GetWithResourceDesignator();
            errorString += "]' input";
        }

        if (ptrOverheadData->GetMakeBeforeBreakStartUpTime() < 0) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Invalid 'Make Before Break Start Up Time [";
            errorString += ptrOverheadData->GetWithResourceDesignator();
            errorString += "]' input";
        }

        ptrOverheadData = overheadDataListIter.Next();
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "VerifyMakeBeforeBreakTimes(const bool&)",
                                 errorString);
    }

    return;
}

int
LinkCriteria::GetMaximumSimultaneousConjunctionSenders() const
{
    int                              maximumSenders = 0;
    ConjunctionAngleData *ptrConjunctionAngleData;
    ListIterator<ConjunctionAngleData> narrowbandConjListIter(ptrNBConjunctionAngleDataList_a);
    ListIterator<ConjunctionAngleData> widebandConjListIter(ptrWBConjunctionAngleDataList_a);
    ptrConjunctionAngleData = narrowbandConjListIter.First();

    while (ptrConjunctionAngleData != NULL) {
        maximumSenders = std::max(maximumSenders,
                                  ptrConjunctionAngleData->GetNumberOfSimultaneousSenders());
        ptrConjunctionAngleData = narrowbandConjListIter.Next();
    }

    ptrConjunctionAngleData = widebandConjListIter.First();

    while (ptrConjunctionAngleData != NULL) {
        maximumSenders = std::max(maximumSenders,
                                  ptrConjunctionAngleData->GetNumberOfSimultaneousSenders());
        ptrConjunctionAngleData = widebandConjListIter.Next();
    }

    return(maximumSenders);
}

OverheadData*
LinkCriteria::GetOverheadData(const string &resourceDesignator) const
{
    bool                       foundOverheadData = false;
    ListIterator<OverheadData> overheadDataListiter(ptrOverheadDataList_a);
    OverheadData              *ptrOverheadData = overheadDataListiter.First();

    while ((ptrOverheadData != NULL) && (foundOverheadData == false)) {
        if (ptrOverheadData->GetWithResourceDesignator() == resourceDesignator) {
            foundOverheadData = true;
        } else {
            ptrOverheadData = overheadDataListiter.Next();
        }
    }

    return(ptrOverheadData);
}

const OverheadData*
LinkCriteria::GetLinkOverheadData(const string &designator) const
{
    int                index = 1;
    const int          numberOfOverheadData = ptrOverheadDataList_a->Size();
    const int          firstIndex = 0;
    const OverheadData *ptrReturnData = NULL;

    while ((index < numberOfOverheadData) && (ptrReturnData == NULL)) {
        if ((ptrOverheadDataList_a->GetObjectAt(index))->GetWithResourceDesignator() == designator) {
            ptrReturnData = ptrOverheadDataList_a->GetObjectAt(index);
        }

        ++index;
    }

    if (ptrReturnData == NULL) {
        ptrReturnData = ptrOverheadDataList_a->GetObjectAt(firstIndex);
    }

    return(ptrReturnData);
}

LinkCriteria::CREATION_METHOD
LinkCriteria::GetCreationMethod(const string &inputString) const
{
    CREATION_METHOD returnMethod = UNKNOWN_METHOD;

    if (inputString == "READ") {
        returnMethod = READ_DATA;
    } else if (inputString == "GENERATE") {
        returnMethod = GENERATE_DATA;
    }

    return(returnMethod);
}

void
LinkCriteria::RemoveConjunctionData(int maximumSenders)
{
    ConjunctionAngleData           *ptrConjunctionAngleData = NULL;
    LinkList<ConjunctionAngleData> *ptrTemporaryList = NULL;

    if (ptrNBConjunctionAngleDataList_a != NULL) {
        //JOE:
        ptrConjunctionAngleData = ptrNBConjunctionAngleDataList_a->Retrieve();

        while (ptrConjunctionAngleData != NULL) {
            if (ptrConjunctionAngleData->GetNumberOfSimultaneousSenders() > maximumSenders) {
                delete ptrConjunctionAngleData;
            } else {
                if (ptrTemporaryList == NULL) {
                    ptrTemporaryList = new LinkList<ConjunctionAngleData>();
                }

                ptrTemporaryList->Append(ptrConjunctionAngleData);
            }

            ptrConjunctionAngleData = NULL;
            ptrConjunctionAngleData = ptrNBConjunctionAngleDataList_a->Retrieve();
        }

        delete ptrNBConjunctionAngleDataList_a;
        ptrNBConjunctionAngleDataList_a = ptrTemporaryList;
        ptrTemporaryList = NULL;
    }

    if (ptrWBConjunctionAngleDataList_a != NULL) {
        //JOE:
        ptrConjunctionAngleData = ptrWBConjunctionAngleDataList_a->Retrieve();

        while (ptrConjunctionAngleData != NULL) {
            if (ptrConjunctionAngleData->GetNumberOfSimultaneousSenders() > maximumSenders) {
                delete ptrConjunctionAngleData;
            } else {
                if (ptrTemporaryList == NULL) {
                    ptrTemporaryList = new LinkList<ConjunctionAngleData>();
                }

                ptrTemporaryList->Append(ptrConjunctionAngleData);
            }

            ptrConjunctionAngleData = NULL;
            ptrConjunctionAngleData = ptrWBConjunctionAngleDataList_a->Retrieve();
        }

        delete ptrWBConjunctionAngleDataList_a;
        ptrWBConjunctionAngleDataList_a = ptrTemporaryList;
        ptrTemporaryList = NULL;
    }

    if ((ptrNBConjunctionAngleDataList_a == NULL) && (ptrWBConjunctionAngleDataList_a == NULL)) {
        conjunctionMethod_a = UNKNOWN_METHOD;
    }
}

LinkCriteria::~LinkCriteria()
{
    if (ptrOverheadDataList_a != NULL) {
        ptrOverheadDataList_a->Delete();
        delete ptrOverheadDataList_a;
        ptrOverheadDataList_a = NULL;
    }

    if (ptrNBConjunctionAngleDataList_a != NULL) {
        ptrNBConjunctionAngleDataList_a->Delete();
        delete ptrNBConjunctionAngleDataList_a;
        ptrNBConjunctionAngleDataList_a = NULL;
    }

    if (ptrWBConjunctionAngleDataList_a != NULL) {
        ptrWBConjunctionAngleDataList_a->Delete();
        delete ptrWBConjunctionAngleDataList_a;
        ptrWBConjunctionAngleDataList_a = NULL;
    }

    return;
}
