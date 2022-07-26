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
 * TimelineCriteria.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include "TimelineCriteria.h"
#include "OffRegionData.h"


#include "general/array/ListIterator.h"
#include "general/math/SimpleMath.h"
#include "general/utility/StringEditor.h"
#include "general/time/TimePiece.h"
#include "general/parse/ParseUtility.h"

TimelineCriteria::TimelineCriteria()
    : timeToCollectReset_a(0), ptrElevationFactorArray_a(NULL),
      flatScorePerTime_a(-1.0), generateResourceSpecificTimeline_a(false),
      ptrElevationFactorFileName_a(NULL),
      timelineGenerationMethod_a(UNKNOWN_TIMELINE_METHOD), typeOfDeck_a(NO_DECK),
      ptrMinimumAscendingList_a(NULL), ptrMinimumDescendingList_a(NULL),
      ptrMaximumAscendingList_a(NULL), ptrMaximumDescendingList_a(NULL)
{
    return;
}

TimelineCriteria::TimelineCriteria(shared_ptr<InputDataElement> ptrInputDataElement)
    : timeToCollectReset_a(0), ptrElevationFactorArray_a(NULL),
      flatScorePerTime_a(-1.0), generateResourceSpecificTimeline_a(false),
      ptrElevationFactorFileName_a(NULL),
      timelineGenerationMethod_a(UNKNOWN_TIMELINE_METHOD), typeOfDeck_a(NO_DECK),
      ptrMinimumAscendingList_a(NULL), ptrMinimumDescendingList_a(NULL),
      ptrMaximumAscendingList_a(NULL), ptrMaximumDescendingList_a(NULL)

{
    try {
        ExtractTimelineCriteriaInfo(ptrInputDataElement);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "TimelineCriteria(InputDataElement*)");
        throw;
    }
}

TimelineCriteria::TimelineCriteria(const TimelineCriteria &copyMe)
    : timeToCollectReset_a(copyMe.timeToCollectReset_a),
      ptrElevationFactorArray_a(NULL),
      flatScorePerTime_a(copyMe.flatScorePerTime_a),
      generateResourceSpecificTimeline_a(copyMe.generateResourceSpecificTimeline_a),
      ptrElevationFactorFileName_a(NULL),
      timelineGenerationMethod_a(copyMe.timelineGenerationMethod_a),
      typeOfDeck_a(copyMe.typeOfDeck_a),
      ptrMinimumAscendingList_a(NULL), ptrMinimumDescendingList_a(NULL),
      ptrMaximumAscendingList_a(NULL), ptrMaximumDescendingList_a(NULL)

{
    if (copyMe.ptrElevationFactorArray_a != NULL) {
        ptrElevationFactorArray_a = new Array2D<double>(*(copyMe.ptrElevationFactorArray_a));
    }

    if (copyMe.ptrElevationFactorFileName_a != NULL) {
        ptrElevationFactorFileName_a = new string(*(copyMe.ptrElevationFactorFileName_a));
    }

    if (copyMe.ptrMinimumAscendingList_a != NULL) {
        ptrMinimumAscendingList_a = new
        LinkList<OffRegionData>(*(copyMe.ptrMinimumAscendingList_a));
    }

    if (copyMe.ptrMinimumDescendingList_a != NULL) {
        ptrMinimumDescendingList_a = new
        LinkList<OffRegionData>(*(copyMe.ptrMinimumDescendingList_a));
    }

    if (copyMe.ptrMaximumAscendingList_a != NULL) {
        ptrMaximumAscendingList_a = new
        LinkList<OffRegionData>(*(copyMe.ptrMaximumAscendingList_a));
    }

    if (copyMe.ptrMaximumDescendingList_a != NULL) {
        ptrMaximumDescendingList_a = new
        LinkList<OffRegionData>(*(copyMe.ptrMaximumDescendingList_a));
    }

    return;
}

TimelineCriteria&
TimelineCriteria::operator = (const TimelineCriteria &copyMe)
{
    timeToCollectReset_a                 = copyMe.timeToCollectReset_a;
    ptrElevationFactorArray_a            = NULL;
    flatScorePerTime_a                   = copyMe.flatScorePerTime_a;
    generateResourceSpecificTimeline_a   = copyMe.generateResourceSpecificTimeline_a;
    ptrElevationFactorFileName_a         = NULL;
    timelineGenerationMethod_a           = copyMe.timelineGenerationMethod_a;
    typeOfDeck_a                         = copyMe.typeOfDeck_a;
    ptrMinimumAscendingList_a            = NULL;
    ptrMinimumDescendingList_a           = NULL;
    ptrMaximumAscendingList_a            = NULL;
    ptrMaximumDescendingList_a           = NULL;

    if (copyMe.ptrElevationFactorArray_a != NULL) {
        ptrElevationFactorArray_a = new Array2D<double>(*(copyMe.ptrElevationFactorArray_a));
    }

    if (copyMe.ptrElevationFactorFileName_a != NULL) {
        ptrElevationFactorFileName_a = new string(*(copyMe.ptrElevationFactorFileName_a));
    }

    if (copyMe.ptrMinimumAscendingList_a != NULL) {
        ptrMinimumAscendingList_a = new
        LinkList<OffRegionData>(*(copyMe.ptrMinimumAscendingList_a));
    }

    if (copyMe.ptrMinimumDescendingList_a != NULL) {
        ptrMinimumDescendingList_a = new
        LinkList<OffRegionData>(*(copyMe.ptrMinimumDescendingList_a));
    }

    if (copyMe.ptrMaximumAscendingList_a != NULL) {
        ptrMaximumAscendingList_a = new
        LinkList<OffRegionData>(*(copyMe.ptrMaximumAscendingList_a));
    }

    if (copyMe.ptrMaximumDescendingList_a != NULL) {
        ptrMaximumDescendingList_a = new
        LinkList<OffRegionData>(*(copyMe.ptrMaximumDescendingList_a));
    }

    return(*this);
}

bool
TimelineCriteria::operator == (const TimelineCriteria &compareMe) const
{
    return( (timeToCollectReset_a == compareMe.timeToCollectReset_a) &&
            (flatScorePerTime_a == compareMe.flatScorePerTime_a) &&
            (generateResourceSpecificTimeline_a == compareMe.generateResourceSpecificTimeline_a) &&
            (timelineGenerationMethod_a == compareMe.timelineGenerationMethod_a) &&
            (typeOfDeck_a == compareMe.typeOfDeck_a) );
}

bool
TimelineCriteria::operator != (const TimelineCriteria &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
TimelineCriteria::ExtractTimelineCriteriaInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    int                    numberOfFactors = 0;
    string                  errorString = "";
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData> ptrInputData = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Value Timeline Generation Method") == true) {
            timelineGenerationMethod_a
                = GetTimelineGenerationMethod(ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Type Of Deck") == true) {
            typeOfDeck_a = ConvertTypeOfDeck(ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Time To Collect Reset") == true) {
            timeToCollectReset_a
                = UnitData::GetInternalTimeDuration(ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Flat Score Per Time Step") == true) {
            flatScorePerTime_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Generate Resource Specific Timeline") == true) {
            generateResourceSpecificTimeline_a = ptrInputData->GetBoolValue();
        } else if (ptrInputData->LabelContainsString("Number Of Elevation Angle Factors") == true) {
            numberOfFactors = ptrInputData->GetIntValue();
        } else if (ptrInputData->LabelContainsString("Elevation Angle Factor File Name") == true) {
            if (ptrElevationFactorFileName_a != NULL) {
                delete ptrElevationFactorFileName_a;
            }

            ptrElevationFactorFileName_a = new string(ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Default Minimum Ascending Off Region") == true) {
            SetDefaultOffRegion(ptrMinimumAscendingList_a, ptrInputData);
        } else if (ptrInputData->LabelContainsString("Minimum Ascending Off Region") == true) {
            if (ptrMinimumAscendingList_a == NULL) {
                errorString = "Must have Default Minimum Ascending Off Region before specific";
            } else {
                SetOffRegion(ptrMinimumAscendingList_a, ptrInputData);
            }
        } else if (ptrInputData->LabelContainsString("Default Minimum Descending Off Region") == true) {
            SetDefaultOffRegion(ptrMinimumDescendingList_a, ptrInputData);
        } else if (ptrInputData->LabelContainsString("Minimum Descending Off Region") == true) {
            if (ptrMinimumDescendingList_a == NULL) {
                errorString = "Must have Default Minimum Descending Off Region before specific";
            } else {
                SetOffRegion(ptrMinimumDescendingList_a, ptrInputData);
            }
        } else if (ptrInputData->LabelContainsString("Default Maximum Ascending Off Region") == true) {
            SetDefaultOffRegion(ptrMaximumAscendingList_a, ptrInputData);
        } else if (ptrInputData->LabelContainsString("Maximum Ascending Off Region") == true) {
            if (ptrMaximumAscendingList_a == NULL) {
                errorString = "Must have Default Maximum Ascending Off Region before specific";
            } else {
                SetOffRegion(ptrMaximumAscendingList_a, ptrInputData);
            }
        } else if (ptrInputData->LabelContainsString("Default Maximum Descending Off Region") == true) {
            SetDefaultOffRegion(ptrMaximumDescendingList_a, ptrInputData);
        } else if (ptrInputData->LabelContainsString("Maximum Descending Off Region") == true) {
            if (ptrMaximumDescendingList_a == NULL) {
                errorString = "Must have Default Maximum Descending Off Region before specific";
            } else {
                SetOffRegion(ptrMaximumDescendingList_a, ptrInputData);
            }
        }
    }

    if (timelineGenerationMethod_a == UNKNOWN_TIMELINE_METHOD) {
        errorString = "Missing/invalid 'Value Timeline Generation Method' input";
    } else if (timelineGenerationMethod_a == READ_TIMELINE) {
        DeleteOffRegionData();
    } else if (timelineGenerationMethod_a == NOMINAL_REGION) {
        if (ptrMinimumAscendingList_a == NULL) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing 'Minimum Ascending Off Region' input for REGION user";
        }

        if (ptrMinimumDescendingList_a == NULL) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing 'Minimum Descending Off Region' input for REGION user";
        }

        if (ptrMinimumAscendingList_a == NULL) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing 'Maximum Ascending Off Region' input for REGION user";
        }

        if (ptrMaximumDescendingList_a == NULL) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing 'Maximum Descending Off Region' input for REGION user";
        }

        if ((flatScorePerTime_a <= 0.0) && (typeOfDeck_a == NO_DECK)) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "NOMINAL_REGION timeline user must either use a deck";
            errorString += " OR have a valid 'Flat Score Per Time Step' input";
        }
    } else {
        if (typeOfDeck_a == NO_DECK) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing/invalid 'Type Of Deck' input for MVR or TCA method";
        }

        if (timeToCollectReset_a <= 0) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing/invalid 'Time To Collect Reset' input";
        }

        flatScorePerTime_a = -1.0;
        DeleteOffRegionData();
    }

    if (numberOfFactors > 0) {
        if (ptrElevationFactorFileName_a == NULL) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing 'Elevation Angle Factor File Name' input";
        } else {
            try {
                RetrieveElevationAngleFactors(numberOfFactors);
            } catch (Exception *ptrError) {
                ptrError->AddMethod(GetClassName(),
                                    "ExtractTimelineCriteriaInfo(InputDataElement*)");
                throw;
            }
        }
    } else if (ptrElevationFactorFileName_a != NULL) {
        delete ptrElevationFactorFileName_a;
        ptrElevationFactorFileName_a = NULL;
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractTimelineCriteriaInfo(InputDataElement*)",
                                 errorString);
    }

    if (typeOfDeck_a != NO_DECK) {
        flatScorePerTime_a = DOUBLE_ZERO;
    }

    return;
}

void
TimelineCriteria::PrintAttributes(ostream &outfile) const
{
    const int      originalPrecision = (int)outfile.precision();
    double         startLongitude = 0.0;
    double         endLongitude = 0.0;
    string         longitudeString = "";
    OffRegionData *ptrOffRegion = NULL;
    outfile << "   <VALUE_TIMELINE_GENERATION_BEGIN>" << NEW_LINE;
    outfile << "      Value Timeline Generation Method (READ/MVR/TCA/REGION)  : ";
    outfile << GetTimelineGenerationMethodString() << NEW_LINE;
    outfile << "      Type Of Deck (NONE/TARGET/REQUIREMENT/BOTH)             : ";
    outfile << GetTypeOfDeckString() << NEW_LINE;
    outfile << "      Time To Collect Reset                                   : ";
    outfile.unsetf(ios::right);
    outfile.setf(ios::left);
    TimePiece::PrintDuration(outfile, timeToCollectReset_a,
                             UnitData::GetInputTimeUnits());
    outfile << NEW_LINE;
    outfile.unsetf(ios::left);
    outfile.setf(ios::right);

    if (flatScorePerTime_a >= 0.0) {
        outfile << "      Flat Score Per Time Step                                : ";
        outfile << flatScorePerTime_a << NEW_LINE;
    }

    outfile << "      Generate Resource Specific Timeline                     : ";
    outfile << StringEditor::ConvertToString(generateResourceSpecificTimeline_a) << NEW_LINE;

    if (ptrElevationFactorFileName_a != NULL) {
        outfile << "      Number Of Elevation Angle Factors                       : ";
        outfile << GetNumberOfElevationFactors() << NEW_LINE;
        outfile << "      Elevation Angle Factor File Name                        : ";
        outfile << *ptrElevationFactorFileName_a << NEW_LINE;
    }

    if (ptrMinimumAscendingList_a != NULL) {
        ListIterator<OffRegionData> offRegionListIter(ptrMinimumAscendingList_a);
        ptrOffRegion = offRegionListIter.First();

        if (ptrOffRegion != NULL) {
            outfile << "      Default Minimum Ascending Off Region (latitude)         : ";
            outfile << UnitData::GetInputAngle(ptrOffRegion->GetLatitude()) << NEW_LINE;
            //JOE:
            ptrOffRegion = offRegionListIter.Next();

            while (ptrOffRegion != NULL) {
                startLongitude = UnitData::GetInputAngle(ptrOffRegion->GetStartLongitude());
                endLongitude = UnitData::GetInputAngle(ptrOffRegion->GetEndLongitude());
                longitudeString = InputData::GetSpecificNumberString(startLongitude, endLongitude);
                outfile << "        Minimum Ascending Off Region ";
                outfile << setw(24) << longitudeString;
                outfile << " : ";
                outfile.precision(originalPrecision);
                outfile << UnitData::GetInputAngle(ptrOffRegion->GetLatitude()) << NEW_LINE;
                ptrOffRegion = offRegionListIter.Next();
            }
        }
    }

    if (ptrMinimumDescendingList_a != NULL) {
        ListIterator<OffRegionData> offRegionListIter(ptrMinimumDescendingList_a);
        ptrOffRegion = offRegionListIter.First();

        if (ptrOffRegion != NULL) {
            outfile << "      Default Minimum Descending Off Region (latitude)        : ";
            outfile << UnitData::GetInputAngle(ptrOffRegion->GetLatitude()) << NEW_LINE;
            //JOE:
            ptrOffRegion = offRegionListIter.Next();

            while (ptrOffRegion != NULL) {
                startLongitude = UnitData::GetInputAngle(ptrOffRegion->GetStartLongitude());
                endLongitude = UnitData::GetInputAngle(ptrOffRegion->GetEndLongitude());
                longitudeString = InputData::GetSpecificNumberString(startLongitude, endLongitude);
                outfile << "        Minimum Descending Off Region ";
                outfile << setw(24) << longitudeString;
                outfile << ": ";
                outfile.precision(originalPrecision);
                outfile << UnitData::GetInputAngle(ptrOffRegion->GetLatitude()) << NEW_LINE;
                ptrOffRegion = offRegionListIter.Next();
            }
        }
    }

    if (ptrMaximumAscendingList_a != NULL) {
        ListIterator<OffRegionData> offRegionListIter(ptrMaximumAscendingList_a);
        ptrOffRegion = offRegionListIter.First();

        if (ptrOffRegion != NULL) {
            outfile << "      Default Maximum Ascending Off Region (latitude)         : ";
            outfile << UnitData::GetInputAngle(ptrOffRegion->GetLatitude()) << NEW_LINE;
            //JOE:
            ptrOffRegion = offRegionListIter.Next();

            while (ptrOffRegion != NULL) {
                startLongitude = UnitData::GetInputAngle(ptrOffRegion->GetStartLongitude());
                endLongitude = UnitData::GetInputAngle(ptrOffRegion->GetEndLongitude());
                longitudeString = InputData::GetSpecificNumberString(startLongitude, endLongitude);
                outfile << "        Maximum Ascending Off Region ";
                outfile << setw(24) << longitudeString;
                outfile << " : ";
                outfile.precision(originalPrecision);
                outfile << UnitData::GetInputAngle(ptrOffRegion->GetLatitude()) << NEW_LINE;
                ptrOffRegion = offRegionListIter.Next();
            }
        }
    }

    if (ptrMaximumDescendingList_a != NULL) {
        ListIterator<OffRegionData> offRegionListIter(ptrMaximumDescendingList_a);
        ptrOffRegion = offRegionListIter.First();

        if (ptrOffRegion != NULL) {
            outfile << "      Default Maximum Descending Off Region (latitude)        : ";
            outfile << UnitData::GetInputAngle(ptrOffRegion->GetLatitude()) << NEW_LINE;
            //JOE:
            ptrOffRegion = offRegionListIter.Next();

            while (ptrOffRegion != NULL) {
                startLongitude = UnitData::GetInputAngle(ptrOffRegion->GetStartLongitude());
                endLongitude = UnitData::GetInputAngle(ptrOffRegion->GetEndLongitude());
                longitudeString = InputData::GetSpecificNumberString(startLongitude, endLongitude);
                outfile << "        Maximum Descending Off Region ";
                outfile << setw(24) << longitudeString;
                outfile << ": ";
                outfile.precision(originalPrecision);
                outfile << UnitData::GetInputAngle(ptrOffRegion->GetLatitude()) << NEW_LINE;
                ptrOffRegion = offRegionListIter.Next();
            }
        }
    }

    outfile << "   <VALUE_TIMELINE_GENERATION_STOP>" << NEW_LINE;
    outfile.precision(originalPrecision);
    return;
}

void
TimelineCriteria::RetrieveElevationAngleFactors(int numberOfFactors)
{
    const int   numberOfAngles = SEMI_CIRCLE + 1;
    string       inputString = "";
    ifstream     elevFactorFile;

    try {
        double elevationAngle = 0.0;
        ParseUtility::OpenInputFile(elevFactorFile, ptrElevationFactorFileName_a);
        ptrElevationFactorArray_a = new Array2D<double>(numberOfFactors, numberOfAngles, 0.0);
        // get the header line
        getline(elevFactorFile, inputString, NEW_LINE);

        while (getline(elevFactorFile, inputString, NEW_LINE)) {
            int integerAngle = 0;
            int factorIndex = 0;
            istringstream inputStream(inputString);
            inputStream >> elevationAngle;
            integerAngle = (int)(elevationAngle + 0.5);
            factorIndex = 0;

            while (factorIndex < numberOfFactors) {
                inputStream >> (*ptrElevationFactorArray_a)[factorIndex][integerAngle];
                ++factorIndex;
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "RetrieveElevationAngleFactors(int)");
        throw;
    }

    return;
}

void
TimelineCriteria::SetDefaultOffRegion(LinkList<OffRegionData> *&ptrOffRegionList,
                                      const shared_ptr<InputData> ptrInputData)
{
    const ANGLE_UNIT storeAngleUnits = UnitData::GetInternalAngleUnits();
    const double     latitude = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
    const double     HALF_CIRCLE = UnitData::GetStoreHalfCircle();
    OffRegionData   *ptrOffRegion = new OffRegionData(latitude, -HALF_CIRCLE, HALF_CIRCLE);

    if (ptrOffRegionList != NULL) {
        ptrOffRegionList->Delete();
        delete ptrOffRegionList;
    }

    ptrOffRegionList = new LinkList<OffRegionData>();
    ptrOffRegionList->Append(ptrOffRegion);
    ptrOffRegion = NULL;
    return;
}

void
TimelineCriteria::SetOffRegion(LinkList<OffRegionData> *&ptrOffRegionList,
                               const shared_ptr<InputData> ptrInputData)
{
    double         startLongitude = UNDEFINED_ANGLE;
    double         endLongitude = UNDEFINED_ANGLE;
    const double   latitude = UnitData::GetInternalAngle(ptrInputData->GetDoubleValue());
    OffRegionData *ptrOffRegion = NULL;

    if (ptrInputData->GetSpecificNumbers(startLongitude, endLongitude) == true) {
        ptrOffRegion = new OffRegionData(latitude,
                                         UnitData::GetInternalAngle(startLongitude),
                                         UnitData::GetInternalAngle(endLongitude));
        ptrOffRegionList->Append(ptrOffRegion);
        ptrOffRegion = NULL;
    }

    return;
}

double
TimelineCriteria::GetElevationFactor(double elevationAngle,
                                     int position) const
{
    double  elevationFactor = 1.0;

    if (ptrElevationFactorArray_a != NULL) {
        try {
            int lowerAngle = 0;
            double angleInDegrees;
            angleInDegrees = SimpleMath::ConvertAngle(elevationAngle,
                             UnitData::GetInternalAngleUnits(),
                             DEGREES);
            lowerAngle = (int)angleInDegrees;

            // if not the exact integer angle, then interpolate
            if ((double)lowerAngle != angleInDegrees) {
                int upperAngle = 0;
                upperAngle = lowerAngle + 1;
                elevationFactor = (SimpleMath::Interpolate((double)lowerAngle,
                                   (double)upperAngle, angleInDegrees,
                                   ptrElevationFactorArray_a->ElementAt(position, lowerAngle),
                                   ptrElevationFactorArray_a->ElementAt(position, upperAngle)));
            } else {
                elevationFactor = ptrElevationFactorArray_a->ElementAt(position, lowerAngle);
            }
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(),
                                "GetElevationFactor(const double&, int)");
            throw;
        }
    }

    return(elevationFactor);
}

double
TimelineCriteria::GetMinAscendingOffRegion(double longitude) const
{
    double                      minimumLatitude = UNDEFINED_ANGLE;
    ListIterator<OffRegionData> offRegionListIter(ptrMinimumAscendingList_a);
    OffRegionData              *ptrOffRegion = offRegionListIter.First();

    if (ptrOffRegion != NULL) {
        // first element is default value
        minimumLatitude = ptrOffRegion->GetLatitude();
        // now check if longitude is in a non-default input range
        //JOE:
        ptrOffRegion = offRegionListIter.Next();

        while (ptrOffRegion != NULL) {
            if (ptrOffRegion->UseOffRegion(longitude) == true) {
                minimumLatitude = ptrOffRegion->GetLatitude();
            }

            ptrOffRegion = offRegionListIter.Next();
        }
    }

    return(minimumLatitude);
}

double
TimelineCriteria::GetMinDescendingOffRegion(double longitude) const
{
    double                      minimumLatitude = UNDEFINED_ANGLE;
    ListIterator<OffRegionData> offRegionListIter(ptrMinimumDescendingList_a);
    OffRegionData              *ptrOffRegion = offRegionListIter.First();

    if (ptrOffRegion != NULL) {
        // first element is default value
        minimumLatitude = ptrOffRegion->GetLatitude();
        // now check if longitude is in a non-default input range
        //JOE:
        ptrOffRegion = offRegionListIter.Next();

        while (ptrOffRegion != NULL) {
            if (ptrOffRegion->UseOffRegion(longitude) == true) {
                minimumLatitude = ptrOffRegion->GetLatitude();
            }

            ptrOffRegion = offRegionListIter.Next();
        }
    }

    return(minimumLatitude);
}

double
TimelineCriteria::GetMaxAscendingOffRegion(double longitude) const
{
    double                      maximumLatitude = UNDEFINED_ANGLE;
    ListIterator<OffRegionData> offRegionListIter(ptrMaximumAscendingList_a);
    OffRegionData              *ptrOffRegion = offRegionListIter.First();

    if (ptrOffRegion != NULL) {
        // first element is default value
        maximumLatitude = ptrOffRegion->GetLatitude();
        // now check if longitude is in a non-default input range
        //JOE:
        ptrOffRegion = offRegionListIter.Next();

        while (ptrOffRegion != NULL) {
            if (ptrOffRegion->UseOffRegion(longitude) == true) {
                maximumLatitude = ptrOffRegion->GetLatitude();
            }

            ptrOffRegion = offRegionListIter.Next();
        }
    }

    return(maximumLatitude);
}

double
TimelineCriteria::GetMaxDescendingOffRegion(double longitude) const
{
    double                      maximumLatitude = UNDEFINED_ANGLE;
    ListIterator<OffRegionData> offRegionListIter(ptrMaximumDescendingList_a);
    OffRegionData              *ptrOffRegion = offRegionListIter.First();

    if (ptrOffRegion != NULL) {
        // first element is default value
        maximumLatitude = ptrOffRegion->GetLatitude();
        // now check if longitude is in a non-default input range
        //JOE:
        ptrOffRegion = offRegionListIter.Next();

        while (ptrOffRegion != NULL) {
            if (ptrOffRegion->UseOffRegion(longitude) == true) {
                maximumLatitude = ptrOffRegion->GetLatitude();
            }

            ptrOffRegion = offRegionListIter.Next();
        }
    }

    return(maximumLatitude);
}

TimelineCriteria::TIMELINE_METHOD
TimelineCriteria::GetTimelineGenerationMethod(const string &inputString) const
{
    TIMELINE_METHOD returnMethod = UNKNOWN_TIMELINE_METHOD;

    if (inputString == "READ") {
        returnMethod = READ_TIMELINE;
    } else if (inputString == "MVR") {
        returnMethod = MOST_VALUABLE_REGION;
    } else if (inputString == "TCA") {
        returnMethod = TIME_CLOSEST_APPROACH;
    } else if (inputString == "REGION") {
        returnMethod = NOMINAL_REGION;
    }

    return(returnMethod);
}

TimelineCriteria::TYPE_OF_DECK
TimelineCriteria::ConvertTypeOfDeck(const string &inputString) const
{
    TYPE_OF_DECK returnType = NO_DECK;

    if (inputString == "TARGET") {
        returnType = TARGET_DECK;
    } else if (inputString == "REQUIREMENT") {
        returnType = REQUIREMENT_DECK;
    } else if (inputString == "BOTH") {
        returnType = BOTH_DECKS;
    }

    return(returnType);
}

/* *************************************************************************** */

TimelineCriteria::TYPE_OF_DECK
TimelineCriteria::GetTypeOfDeck() const
{
    return typeOfDeck_a;
}

/* *************************************************************************** */

void
TimelineCriteria::DeleteOffRegionData()
{
    if (ptrMinimumAscendingList_a != NULL) {
        ptrMinimumAscendingList_a->Delete();
        delete ptrMinimumAscendingList_a;
        ptrMinimumAscendingList_a = NULL;
    }

    if (ptrMinimumDescendingList_a != NULL) {
        ptrMinimumDescendingList_a->Delete();
        delete ptrMinimumDescendingList_a;
        ptrMinimumDescendingList_a = NULL;
    }

    if (ptrMaximumAscendingList_a != NULL) {
        ptrMaximumAscendingList_a->Delete();
        delete ptrMaximumAscendingList_a;
        ptrMaximumAscendingList_a = NULL;
    }

    if (ptrMaximumDescendingList_a != NULL) {
        ptrMaximumDescendingList_a->Delete();
        delete ptrMaximumDescendingList_a;
        ptrMaximumDescendingList_a = NULL;
    }

    return;
}

TimelineCriteria::~TimelineCriteria()
{
    if (ptrElevationFactorArray_a != NULL) {
        delete ptrElevationFactorArray_a;
        ptrElevationFactorArray_a = NULL;
    }

    if (ptrElevationFactorFileName_a != NULL) {
        delete ptrElevationFactorFileName_a;
        ptrElevationFactorFileName_a = NULL;
    }

    DeleteOffRegionData();
    return;
}
