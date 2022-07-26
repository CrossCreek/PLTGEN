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
 * OpticalSensor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "OpticalSensor.h"
#include "mural/modules/ts/Target.h"

#include "general/array/ListIterator.h"
#include "general/data/UnitData.h"
#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"
#include "general/data/DefaultedData.h"

//------------------------------------------------------------------------------
//
//  Method Name: OpticalSensor::OpticalSensor
//
//  Purpose:
//    Constructor for the OpticalSensor class.
//
//  Calling Method:
//     OpticalSensor();
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

OpticalSensor::OpticalSensor()
    : Sensor(), opticalFieldOfView_a(UNDEFINED_ANGLE),
      ptrCollectionRate_a(NULL), ptrResourceWeight_a(NULL),numberOfDetectorElements_a(0)
{
    return;
}

OpticalSensor::OpticalSensor(shared_ptr<InputDataElement> ptrInputDataElement,
                             const SENSOR_TYPE &type)
    : Sensor(ptrInputDataElement, type), opticalFieldOfView_a(UNDEFINED_ANGLE),
      ptrCollectionRate_a(NULL), ptrResourceWeight_a(NULL),numberOfDetectorElements_a(0)
{
    try {
        ExtractOpticalSensorInfo(ptrInputDataElement);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "OpticalSensor(InputDataElement*, const SENSOR_TYPE&)");
        throw;
    }

    return;
}

OpticalSensor::OpticalSensor(const OpticalSensor &copyMe)
    : Sensor(copyMe), opticalFieldOfView_a(copyMe.opticalFieldOfView_a),
      ptrCollectionRate_a(NULL), ptrResourceWeight_a(NULL),
      numberOfDetectorElements_a(copyMe.numberOfDetectorElements_a)
{
    return;
}

OpticalSensor&
OpticalSensor::operator = (const OpticalSensor &copyMe)
{
    Sensor::operator = (copyMe);
    opticalFieldOfView_a  = copyMe.opticalFieldOfView_a;
    ptrCollectionRate_a   = copyMe.ptrCollectionRate_a;
    ptrResourceWeight_a   = copyMe.ptrResourceWeight_a;
    numberOfDetectorElements_a = copyMe.numberOfDetectorElements_a;
    return(*this);
}

bool
OpticalSensor::operator == (const OpticalSensor &compareMe) const
{
    return( (Sensor::operator == (compareMe)) &&
            (opticalFieldOfView_a  == compareMe.opticalFieldOfView_a) &&
            (numberOfDetectorElements_a  == compareMe.numberOfDetectorElements_a) );
}

bool
OpticalSensor::operator != (const OpticalSensor &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
OpticalSensor::ExtractOpticalSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    int        typeIndex = 0;
    int        index = 0;
    int        numberOfInputs = 0;
    const int  numberOfQualities = Target::GetNumberOfQualities();
    string      typeString = "";
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData>              ptrInputData = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Optical Field Of View") == true) {
            opticalFieldOfView_a = ptrInputData->GetDoubleValue();
        } else if (ptrInputData->LabelContainsString("Number Of Detector Elements") == true) {
            numberOfDetectorElements_a = ptrInputData->GetIntValue();
        } else if (ptrInputData->LabelContainsString("Default Collection Rate") == true) {
            if (ptrCollectionRate_a != NULL) {
                delete ptrCollectionRate_a;
            }

            ptrCollectionRate_a = new Array2D<double>(NUMBER_OF_COLLECTION_TYPES,
                    numberOfQualities, ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Collection Rate") == true) {
            if (ptrCollectionRate_a == NULL) {
                throw new InputException(GetClassName(),
                                         "ExtractOpticalSensorInfo(InputDataElement*)",
                                         "Must input Default Collection Rate before specific");
            }

            if (ptrInputData->GetSpecificType(typeString) == true) {
                typeIndex = GetCollectionRateTypeIndex(typeString);

                if (typeIndex >= 0) {
                    index = 0;
                    numberOfInputs = std::min(ptrInputData->GetNumberOfValues(),
                                              numberOfQualities);

                    while (index < numberOfInputs) {
                        (*ptrCollectionRate_a)[typeIndex][index] = ptrInputData->GetDoubleValue(index);
                        ++index;
                    }
                }
            }
        } else if (ptrInputData->LabelContainsString("Default Resource Weight") == true) {
            if (ptrResourceWeight_a != NULL) {
                delete ptrResourceWeight_a;
            }

            ptrResourceWeight_a = new Array2D<double>(NUMBER_OF_RESOURCE_WEIGHT_TYPES,
                    numberOfQualities, ptrInputData->GetDoubleValue());
        } else if (ptrInputData->LabelContainsString("Resource Weight") == true) {
            if (ptrResourceWeight_a == NULL) {
                throw new InputException(GetClassName(),
                                         "ExtractOpticalSensorInfo(InputDataElement*)",
                                         "Must input Default Resource Weight before specific");
            }

            if (ptrInputData->GetSpecificType(typeString) == true) {
                typeIndex = GetResourceWeightTypeIndex(typeString);

                if (typeIndex >= 0) {
                    index = 0;
                    numberOfInputs = std::min(ptrInputData->GetNumberOfValues(),
                                              numberOfQualities);

                    while (index < numberOfInputs) {
                        (*ptrResourceWeight_a)[typeIndex][index] = ptrInputData->GetDoubleValue(index);
                        ++index;
                    }
                }
            }
        }
    }

    return;
}

void
OpticalSensor::CheckInputData(bool runTimelineGenerator) const
{
    string errorString = "";
    shared_ptr<DefaultedData<double>> ptrFocalLength = ptrIQE_a->GetFocalLength();

    if (opticalFieldOfView_a == UNDEFINED_ANGLE) {
        errorString = "Missing 'Optical Field Of View' input";
    }

    if (ptrIQE_a->GetPitch() <= 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString = "Missing/invalid 'Pitch' input";
    }

    if (ptrFocalLength == NULL) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing 'Default Focal Length' input";
    } else {
        shared_ptr<vector<string>> tagVector = ptrFocalLength->GetSpecificTags();

        if ( tagVector->size() > 0 ) {
            vector<string>::iterator missionIter;

            //CPPCHECK_FIX Performance - prefix iterator change
            for (missionIter = tagVector->begin(); missionIter != tagVector->end(); ++missionIter) {
                if (ptrFocalLength->GetValue((*missionIter)) <= 0.0) {
                    if (errorString.size() > 0) {
                        errorString += "\n         ";
                    }

                    errorString += "Invalid 'Focal Length' input for ";
                    errorString += (*missionIter);
                }
            }
        }
    }

    if (runTimelineGenerator == true) {
        if (ptrCollectionRate_a == NULL) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing 'Default Collection Rate' input";
        }

        if (ptrResourceWeight_a == NULL) {
            if (errorString.size() > 0) {
                errorString += "\n         ";
            }

            errorString += "Missing 'Default Resource Weight' input";
        }
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "CheckInputData()", errorString);
    }

    return;
}

void
OpticalSensor::PrintAttributes(ostream &outfile) const
{
    int        typeIndex = 0;
    int        qualityIndex = 0;
    const int  numberOfQualities = Target::GetNumberOfQualities();
    string      specificTypeString = "";
    // Get the attributes from the IQE
    double  pitch = UNDEFINED_DISTANCE;
    double niirsA = UNDEFINED_COEFFICIENT;
    double niirsB = UNDEFINED_COEFFICIENT;
    double niirsC = UNDEFINED_COEFFICIENT;
    double niirsD = UNDEFINED_COEFFICIENT;
    shared_ptr<DefaultedData<double>> ptrFocalLength = NULL;

    if ( ptrIQE_a != NULL ) {
        pitch = ptrIQE_a->GetPitch();
        ptrFocalLength = ptrIQE_a->GetFocalLength();
        niirsA = ptrIQE_a->GetNIIRSA();
        niirsB = ptrIQE_a->GetNIIRSB();
        niirsC = ptrIQE_a->GetNIIRSC();
        niirsD = ptrIQE_a->GetNIIRSD();
    }

    Sensor::PrintAttributes(outfile);

    if (ptrCollectionRate_a != NULL) {
        typeIndex = 0;
        qualityIndex = 0;
        outfile << "      Default Collection Rate (nmi^2/s) : ";
        outfile << setw(6) << setprecision(2)
                << ptrCollectionRate_a->ElementAt(typeIndex, qualityIndex) << NEW_LINE;

        while (typeIndex < NUMBER_OF_COLLECTION_TYPES) {
            qualityIndex = 0;
            specificTypeString = InputData::GetSpecificTypeString(GetCollectionRateString(typeIndex));
            outfile << "        Collection Rate " << specificTypeString;
            outfile << setw(18 - (int)(specificTypeString.size())) << " : ";
            outfile << setw(6) << setprecision(2)
                    << ptrCollectionRate_a->ElementAt(typeIndex, qualityIndex);
            ++qualityIndex;

            while (qualityIndex < numberOfQualities) {
                outfile << "," << setw(6) << setprecision(2)
                        << ptrCollectionRate_a->ElementAt(typeIndex, qualityIndex);
                ++qualityIndex;
            }

            outfile << NEW_LINE;
            ++typeIndex;
        }
    }

    if (ptrResourceWeight_a != NULL) {
        typeIndex = 0;
        qualityIndex = 0;
        outfile << "      Default Resource Weight           : ";
        outfile << setw(6) << setprecision(2)
                << ptrResourceWeight_a->ElementAt(typeIndex, qualityIndex) << NEW_LINE;

        while (typeIndex < NUMBER_OF_RESOURCE_WEIGHT_TYPES) {
            specificTypeString = InputData::GetSpecificTypeString(GetResourceWeightString(typeIndex));
            outfile << "        Resource Weight " << specificTypeString;
            outfile << setw(18 - (int)(specificTypeString.size())) << " : ";
            qualityIndex = 0;
            outfile << setw(6) << setprecision(2)
                    << ptrResourceWeight_a->ElementAt(typeIndex, qualityIndex);
            ++qualityIndex;

            while (qualityIndex < numberOfQualities) {
                outfile << "," << setw(6) << setprecision(2)
                        << ptrResourceWeight_a->ElementAt(typeIndex, qualityIndex);
                ++qualityIndex;
            }

            outfile << NEW_LINE;
            ++typeIndex;
        }
    }

    if (opticalFieldOfView_a != UNDEFINED_ANGLE) {
        outfile << "      Optical Field Of View (urad)      : ";
        outfile << opticalFieldOfView_a << NEW_LINE;
    }

    if (pitch != UNDEFINED_DISTANCE) {
        outfile << "      Pitch (inches)                    : ";
        outfile << pitch << NEW_LINE;
    }

    if (ptrFocalLength != NULL) {
        double defaultFocalLength = ptrFocalLength->GetDefaultValue();
        outfile << "      Default Focal Length (inches)     : ";
        outfile << defaultFocalLength << NEW_LINE;
        shared_ptr<vector<string>> tagVector = ptrFocalLength->GetSpecificTags();

        if ( tagVector->size() > 0 ) {
            double focalLength = 0.0;
            vector<string>::iterator missionIter;

            //CPPCHECK_FIX Performance - prefix iterator change
            for (missionIter = tagVector->begin(); missionIter != tagVector->end(); ++missionIter) {
                specificTypeString = (*missionIter);
                outfile << "         Focal Length " << specificTypeString;
                outfile << setw(20 - specificTypeString.size()) << " : " << focalLength << NEW_LINE;
            }
        }
    }

    if (niirsA != UNDEFINED_COEFFICIENT) {
        outfile << "      NIIRS A                           : ";
        outfile << niirsA << NEW_LINE;
    }

    if (niirsB != UNDEFINED_COEFFICIENT) {
        outfile << "      NIIRS B                           : ";
        outfile << niirsB << NEW_LINE;
    }

    if (niirsC != UNDEFINED_COEFFICIENT) {
        outfile << "      NIIRS C                           : ";
        outfile << niirsC << NEW_LINE;
    }

    if (niirsD != UNDEFINED_COEFFICIENT) {
        outfile << "      NIIRS D                           : ";
        outfile << niirsD << NEW_LINE;
    }

    if (numberOfDetectorElements_a != 0) {
        outfile << "      Number Of Detector Elements       : ";
        outfile << numberOfDetectorElements_a << NEW_LINE;
    }

    return;
}

double
OpticalSensor::GetCollectionRate(int missionIndex,
                                 int qualityIndex) const
{
    const int collectionIndex = ConvertCollectionRateIndex(missionIndex);
    return(ptrCollectionRate_a->ElementAt(collectionIndex, qualityIndex));
}

double
OpticalSensor::GetResourceWeight(int missionIndex,
                                 int qualityIndex) const
{
    const int  weightIndex = ConvertResourceWeightIndex(missionIndex);
    const double resourceWeight = ptrResourceWeight_a->ElementAt(weightIndex, qualityIndex);
    return(resourceWeight * sensorWeightFactor_a);
}

int
OpticalSensor::GetCollectionRateTypeIndex(const string &typeString)
{
    int collectionIndex = -1;

    if (typeString == "POINT") {
        collectionIndex = POINT_CR;
    } else if (typeString == "LARGE") {
        collectionIndex = LARGE_AREA_CR;
    } else if (typeString == "MCG") {
        collectionIndex = MCG_AREA_CR;
    } else if (typeString == "SMALL") {
        collectionIndex = SMALL_AREA_CR;
    }

    return(collectionIndex);
}

int
OpticalSensor::GetResourceWeightTypeIndex(const string &typeString)
{
    int weightIndex = -1;

    if (typeString == "POINT") {
        weightIndex = POINT_RW;
    } else if (typeString == "AREA") {
        weightIndex = AREA_RW;
    } else if (typeString == "LOC") {
        weightIndex = LOC_RW;
    }

    return(weightIndex);
}

int
OpticalSensor::ConvertCollectionRateIndex(int missionIndex)
{
    int collectionIndex = -1;

    if (missionIndex <= Target::GROUP_POINT) {
        collectionIndex = POINT_CR;
    } else if (missionIndex >= Target::DIRECTED_SEARCH_AREA) {
        collectionIndex = SMALL_AREA_CR;
    } else if ( (missionIndex == Target::MCG_MONO_BOX) ||
                (missionIndex == Target::MCG_STEREO_AREA) ) {
        collectionIndex = MCG_AREA_CR;
    } else {
        collectionIndex = LARGE_AREA_CR;
    }

    return(collectionIndex);
}

int
OpticalSensor::ConvertResourceWeightIndex(int missionIndex)
{
    int weightIndex = -1;

    if (missionIndex <= Target::GROUP_POINT) {
        weightIndex = POINT_RW;
    } else if (missionIndex == Target::LINE_OF_COMMUNICATION) {
        weightIndex = LOC_RW;
    } else {
        weightIndex = AREA_RW;
    }

    return(weightIndex);
}

string
OpticalSensor::GetCollectionRateString(int typeIndex)
{
    string collectionString = "UNKNWN";

    if (typeIndex == POINT_CR) {
        collectionString = "POINT";
    } else if (typeIndex == LARGE_AREA_CR) {
        collectionString = "LARGE";
    } else if (typeIndex == MCG_AREA_CR) {
        collectionString = "MCG";
    } else if (typeIndex == SMALL_AREA_CR) {
        collectionString = "SMALL";
    }

    return(collectionString);
}

string
OpticalSensor::GetResourceWeightString(int typeIndex)
{
    string resourceString = "UNKNWN";

    if (typeIndex == POINT_RW) {
        resourceString = "POINT";
    } else if (typeIndex == AREA_RW) {
        resourceString = "AREA";
    } else if (typeIndex == LOC_RW) {
        resourceString = "LOC";
    }

    return(resourceString);
}

OpticalSensor::~OpticalSensor()
{
    if (ptrCollectionRate_a != NULL) {
        delete ptrCollectionRate_a;
        ptrCollectionRate_a = NULL;
    }

    if (ptrResourceWeight_a != NULL) {
        delete ptrResourceWeight_a;
        ptrResourceWeight_a = NULL;
    }

    return;
}
