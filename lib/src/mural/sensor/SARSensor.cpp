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
 * SARSensor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "SARSensor.h"
#include "mural/modules/ts/Target.h"
#include "mural/sensor/iqe/SarIQE.h"

#include "general/exception/InputException.h"
#include "general/array/ListIterator.h"
#include "general/data/UnitData.h"
#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"

//------------------------------------------------------------------------------
//
//  Method Name: SARSensor::SARSensor
//
//  Purpose:
//    Constructor for the SARSensor class.
//
//  Calling Method:
//     SARSensor();
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

SARSensor::SARSensor()
    : Sensor(),
      ptrCollectionRate_a(NULL), ptrResourceWeight_a(NULL)
{
    return;
}

SARSensor::SARSensor(shared_ptr<InputDataElement> ptrInputDataElement,
                     const SENSOR_TYPE &type)
    : Sensor(ptrInputDataElement, type),
      ptrCollectionRate_a(NULL), ptrResourceWeight_a(NULL)
{
    try {
        ExtractSARSensorInfo(ptrInputDataElement);
        // Init the IQE here
        ptrIQE_a = shared_ptr<SarIQE>(new SarIQE(ptrInputDataElement));
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "SARSensor(InputDataElement*)");
        throw;
    }

    return;
}

SARSensor::SARSensor(const SARSensor &copyMe)
    : Sensor(copyMe), ptrCollectionRate_a(NULL), ptrResourceWeight_a(NULL)
{
    return;
}

SARSensor&
SARSensor::operator = (const SARSensor &copyMe)
{
    Sensor::operator = (copyMe);
    ptrCollectionRate_a      = copyMe.ptrCollectionRate_a;
    ptrResourceWeight_a      = copyMe.ptrResourceWeight_a;
    return(*this);
}

bool
SARSensor::operator == (const SARSensor &compareMe) const
{
    return( (Sensor::operator == (compareMe)) );
}

bool
SARSensor::operator != (const SARSensor &compareMe) const
{
    return(!(operator == (compareMe)));
}

bool
SARSensor::operator < (const SARSensor &compareMe) const
{
    return(Sensor::operator < (compareMe));
}

void
SARSensor::ExtractSARSensorInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    int        typeIndex = 0;
    int        index = 0;
    int        numberOfInputs = 0;
    const int  numberOfQualities = Target::GetNumberOfQualities();
    double       collectionRate = 0.0;
    string      typeString = "";
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    InputDataElement::InputDataList::iterator inputDataIter;
    shared_ptr<InputData> ptrInputData = NULL;

    //CPPCHECK_FIX Performance - prefix iterator change
    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Default Collection Rate") == true) {
            collectionRate = ptrInputData->GetDoubleValue();

            if (ptrCollectionRate_a != NULL) {
                delete ptrCollectionRate_a;
            }

            ptrCollectionRate_a = new Array2D<double>(NUMBER_OF_COLLECTION_TYPES,
                    numberOfQualities, collectionRate);
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
        } else if (ptrInputData->LabelContainsString("Resource Weight") == true) {
            if (ptrResourceWeight_a != NULL) {
                delete ptrResourceWeight_a;
            }

            ptrResourceWeight_a = new Array<double>(numberOfQualities, 0.0);
            index = 0;
            numberOfInputs = std::min(ptrInputData->GetNumberOfValues(),
                                      numberOfQualities);

            while (index < numberOfInputs) {
                (*ptrResourceWeight_a)[index] = ptrInputData->GetDoubleValue(index);
                ++index;
            }
        }
    }

    return;
}

void
SARSensor::CheckInputData(bool runTimelineGenerator) const
{
    string errorString = "";
    // Get the attributes from the IQE
    double nrisD1 = UNDEFINED_DISTANCE;
    double nrisD2 = UNDEFINED_COEFFICIENT;
    double nrisD3 = UNDEFINED_COEFFICIENT;
    double nrisD4 = UNDEFINED_COEFFICIENT;
    double nrisD5 = UNDEFINED_COEFFICIENT;
    double nrisD6 = UNDEFINED_COEFFICIENT;
    double imageProductResolution = UNDEFINED_COEFFICIENT;

    if ( ptrIQE_a != NULL ) {
        nrisD1 = ptrIQE_a->GetNRISD1();
        nrisD2 = ptrIQE_a->GetNRISD2();
        nrisD3 = ptrIQE_a->GetNRISD3();
        nrisD4 = ptrIQE_a->GetNRISD4();
        nrisD5 = ptrIQE_a->GetNRISD5();
        nrisD6 = ptrIQE_a->GetNRISD6();
        imageProductResolution = ptrIQE_a->GetIPR();
    }

    if (nrisD1 < 0.0) {
        errorString = "Missing/invalid 'NRIS D1' input";
    }

    if (nrisD2 < 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'NRIS D2' input";
    }

    if (nrisD3 < 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'NRIS D3' input";
    }

    if (nrisD4 < 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'NRIS D4' input";
    }

    if (nrisD5 < 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'NRIS D5' input";
    }

    if (nrisD6 < 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'NRIS D6' input";
    }

    if (imageProductResolution <= 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Image Product Resolution' input";
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

            errorString += "Missing 'Resource Weight' input";
        }
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(), "CheckInputData()", errorString);
    }

    return;
}

void
SARSensor::PrintAttributes(ostream &outfile) const
{
    int        typeIndex = 0;
    const int  numberOfQualities = Target::GetNumberOfQualities();
    string      specificTypeString = "";
    // Get the attributes from the IQE
    double nrisD1 = UNDEFINED_DISTANCE;
    double nrisD2 = UNDEFINED_COEFFICIENT;
    double nrisD3 = UNDEFINED_COEFFICIENT;
    double nrisD4 = UNDEFINED_COEFFICIENT;
    double nrisD5 = UNDEFINED_COEFFICIENT;
    double nrisD6 = UNDEFINED_COEFFICIENT;
    double imageProductResolution = UNDEFINED_COEFFICIENT;

    if ( ptrIQE_a != NULL ) {
        nrisD1 = ptrIQE_a->GetNRISD1();
        nrisD2 = ptrIQE_a->GetNRISD2();
        nrisD3 = ptrIQE_a->GetNRISD3();
        nrisD4 = ptrIQE_a->GetNRISD4();
        nrisD5 = ptrIQE_a->GetNRISD5();
        nrisD6 = ptrIQE_a->GetNRISD6();
        imageProductResolution = ptrIQE_a->GetIPR();
    }

    outfile << "   <SENSOR_OBJECT_BEGIN>" << NEW_LINE;
    Sensor::PrintAttributes(outfile);

    if (ptrCollectionRate_a != NULL) {
        typeIndex = 0;
        //OBE: qualityIndex = 0;
        int qualityIndex = 0;
        outfile << "      Default Collection Rate (nmi^2/s) : ";
        outfile << setw(8) << setprecision(5)
                << ptrCollectionRate_a->ElementAt(typeIndex, qualityIndex) << NEW_LINE;

        while (typeIndex < NUMBER_OF_COLLECTION_TYPES) {
            qualityIndex = 0;
            specificTypeString = InputData::GetSpecificTypeString(GetCollectionRateString(typeIndex));
            outfile << "        Collection Rate " << specificTypeString;
            outfile << setw(18 - (int)(specificTypeString.size())) << " : ";
            outfile << setw(8) << setprecision(5)
                    << ptrCollectionRate_a->ElementAt(typeIndex, qualityIndex);
            ++qualityIndex;

            while (qualityIndex < numberOfQualities) {
                outfile << "," << setw(8) << setprecision(5)
                        << ptrCollectionRate_a->ElementAt(typeIndex, qualityIndex);
                ++qualityIndex;
            }

            outfile << NEW_LINE;
            ++typeIndex;
        }
    }

    if (ptrResourceWeight_a != NULL) {
        typeIndex = 0;
        outfile << "      Resource Weight                   : ";
        outfile << setw(6) << setprecision(2) << ptrResourceWeight_a->ElementAt(typeIndex);
        ++typeIndex;

        while (typeIndex < ptrResourceWeight_a->Size()) {
            outfile << "," << setw(6) << setprecision(2)
                    << ptrResourceWeight_a->ElementAt(typeIndex);
            ++typeIndex;
        }

        outfile << NEW_LINE;
    }

    if (nrisD1 != UNDEFINED_COEFFICIENT) {
        outfile << "      NRIS D1                           : ";
        outfile << nrisD1 << NEW_LINE;
    }

    if (nrisD2 != UNDEFINED_COEFFICIENT) {
        outfile << "      NRIS D2                           : ";
        outfile << nrisD2 << NEW_LINE;
    }

    if (nrisD3 != UNDEFINED_COEFFICIENT) {
        outfile << "      NRIS D3                           : ";
        outfile << nrisD3 << NEW_LINE;
    }

    if (nrisD4 != UNDEFINED_COEFFICIENT) {
        outfile << "      NRIS D4                           : ";
        outfile << nrisD4 << NEW_LINE;
    }

    if (nrisD5 != UNDEFINED_COEFFICIENT) {
        outfile << "      NRIS D5                           : ";
        outfile << nrisD5 << NEW_LINE;
    }

    if (nrisD6 != UNDEFINED_COEFFICIENT) {
        outfile << "      NRIS D6                           : ";
        outfile << nrisD6 << NEW_LINE;
    }

    if (imageProductResolution != UNDEFINED_COEFFICIENT) {
        outfile << "      Image Product Resolution          : ";
        outfile << imageProductResolution << NEW_LINE;
    }

    outfile << "   <SENSOR_OBJECT_STOP>" << NEW_LINE;
    return;
}

double
SARSensor::GetCollectionRate(int missionIndex,
                             int qualityIndex) const
{
    const int collectionIndex = ConvertCollectionRateIndex(missionIndex);
    return(ptrCollectionRate_a->ElementAt(collectionIndex, qualityIndex));
}

double
SARSensor::GetResourceWeight(int missionIndex,
                             int qualityIndex) const
{
    return(ptrResourceWeight_a->ElementAt(qualityIndex) * sensorWeightFactor_a);
}

int
SARSensor::GetCollectionRateTypeIndex(const string &typeString)
{
    int typeIndex = -1;

    if (typeString == "POINT") {
        typeIndex = POINT;
    } else if (typeString == "AREA") {
        typeIndex = AREA;
    }

    return(typeIndex);
}

int
SARSensor::ConvertCollectionRateIndex(int missionIndex)
{
    int collectionIndex = -1;

    if (missionIndex <= Target::GROUP_POINT) {
        collectionIndex = POINT;
    } else {
        collectionIndex = AREA;
    }

    return(collectionIndex);
}

string
SARSensor::GetCollectionRateString(int typeIndex)
{
    string collectionString = "UNKNWN";

    if (typeIndex == POINT) {
        collectionString = "POINT";
    } else if (typeIndex == AREA) {
        collectionString = "AREA";
    }

    return(collectionString);
}

/**
 * Method to return the index which maps to the target deck and the SENSOR enum.  In the case of the SAR Sensor
 * the sensor type of SAR is always returned.
      SAR = 2,
 *
 * @return int sensor Index
 */
int
SARSensor::GetSensorIndex() const
{
    return(SAR);
}

SARSensor::~SARSensor()
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
