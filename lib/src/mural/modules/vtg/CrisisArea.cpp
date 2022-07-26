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
 * CrisisArea.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "CrisisArea.h"
#include "mural/Region.h"
#include "mural/element/UserVehicle.h"


#include "general/array/ListIterator.h"
#include "general/data/InputDataElement.h"
#include "general/exception/InputException.h"
#include "general/utility/Constant.h"


//------------------------------------------------------------------------------
//
//  Method Name: CrisisArea::CrisisArea
//
//  Purpose:
//    Constructor for the CrisisArea class.
//
//  Calling Method:
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

CrisisArea::CrisisArea()
    : identificationNumber_a(0), ptrRegionNumberArray_a(NULL),
      ptrMultiplierArray_a(NULL), ptrIdentificationLabel_a(NULL),
      ptrUserVehicleArray_a(NULL)
{
    return;
}

CrisisArea::CrisisArea(shared_ptr<InputDataElement> ptrInputDataElement)
    : identificationNumber_a(0), ptrRegionNumberArray_a(NULL),
      ptrMultiplierArray_a(NULL), ptrIdentificationLabel_a(NULL),
      ptrUserVehicleArray_a(NULL)
{
    try {
        ExtractCrisisAreaInfo(ptrInputDataElement);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "CrisisArea(InputDataElement*)");
        throw;
    }

    return;
}

CrisisArea::CrisisArea(const CrisisArea &copyMe)
    : identificationNumber_a(copyMe.identificationNumber_a),
      ptrRegionNumberArray_a(new Array<double>(*(copyMe.ptrRegionNumberArray_a))),
      ptrMultiplierArray_a(new Array<double>(*(copyMe.ptrMultiplierArray_a))),
      ptrIdentificationLabel_a(new string(*(copyMe.ptrIdentificationLabel_a))),
      ptrUserVehicleArray_a(new Array<string>(*(copyMe.ptrUserVehicleArray_a)))
{
    return;
}

CrisisArea&
CrisisArea::operator = (const CrisisArea &copyMe)
{
    identificationNumber_a   = copyMe.identificationNumber_a;
    ptrRegionNumberArray_a   = copyMe.ptrRegionNumberArray_a;
    ptrMultiplierArray_a     = copyMe.ptrMultiplierArray_a;
    ptrIdentificationLabel_a = copyMe.ptrIdentificationLabel_a;
    ptrUserVehicleArray_a    = copyMe.ptrUserVehicleArray_a;
    return(*this);
}

bool
CrisisArea::operator == (const CrisisArea &compareMe) const
{
    return( (identificationNumber_a == compareMe.identificationNumber_a) &&
            (*ptrIdentificationLabel_a == *(compareMe.ptrIdentificationLabel_a)) );
}

bool
CrisisArea::operator != (const CrisisArea &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
CrisisArea::ExtractCrisisAreaInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    int       index = 0;
    int       numberOfValues = 0;
    double      multiplierValue = -1.0;
    string     userDesignator = "";
    string     errorString = "";
    shared_ptr<InputDataElement::InputDataList> inputDataList = ptrInputDataElement->GetInputDataList();
    shared_ptr<InputData> ptrInputData = NULL;
    InputDataElement::InputDataList::iterator inputDataIter;

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if (ptrInputData->LabelContainsString("Crisis Area Identification") == true) {
            identificationNumber_a = ptrInputData->GetIntValue();
        } else if (ptrInputData->LabelContainsString("Crisis Area Label") == true) {
            if (ptrIdentificationLabel_a != NULL) {
                delete ptrIdentificationLabel_a;
            }

            ptrIdentificationLabel_a = new string(ptrInputData->GetStringValue());
        } else if (ptrInputData->LabelContainsString("Crisis Region Number") == true) {
            ptrRegionNumberArray_a = new Array<double>();
            index = 0;
            numberOfValues = ptrInputData->GetNumberOfValues();

            while (index < numberOfValues) {
                ptrRegionNumberArray_a->AddUniqueElement(ptrInputData->GetDoubleValue(index));
                ++index;
            }

            if (ptrRegionNumberArray_a->Size() == 0) {
                delete ptrRegionNumberArray_a;
                ptrRegionNumberArray_a = NULL;
            }
        } else if (ptrInputData->LabelContainsString("User Vehicle") == true) {
            ptrUserVehicleArray_a = new Array<string>();
            index = 0;

            if (ptrInputData->GetStringValue(index) == DEFAULT_STRING) {
                numberOfValues = UserVehicle::GetUserVehicleCount();

                while (index < numberOfValues) {
                    ptrUserVehicleArray_a->AddElement(UserVehicle::GetDesignatorString(index));
                    ++index;
                }
            } else {
                numberOfValues = ptrInputData->GetNumberOfValues();

                while (index < numberOfValues) {
                    userDesignator = ptrInputData->GetStringValue(index);

                    if (UserVehicle::ValidUserVehicle(userDesignator) == true) {
                        ptrUserVehicleArray_a->AddUniqueElement(userDesignator);
                    }

                    ++index;
                }
            }

            if (ptrUserVehicleArray_a->Size() == 0) {
                delete ptrUserVehicleArray_a;
                ptrUserVehicleArray_a = NULL;
            }
        } else if (ptrInputData->LabelContainsString("Default Timeline Score Multiplier") == true) {
            multiplierValue = ptrInputData->GetDoubleValue();
        }
    }

    if (identificationNumber_a <= 0) {
        errorString = "Missing/invalid 'Crisis Area Identification' input";
    }

    if (ptrIdentificationLabel_a == NULL) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing 'Crisis Area Label' input";
    }

    if (ptrRegionNumberArray_a == NULL) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Crisis Region Number(s)' input";
    }

    if (ptrUserVehicleArray_a == NULL) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        /*RAGUSA 8/16/2018 - Edited Error Message to be more specific - the error is thrown because the
          analyst has a crisis region block inputted but the users aren't in the run. It was initially intended to
          remove this exception but it is necessary because MURAL assumes later the Crisis Area must exist if it
          made it past this check*/
        errorString += "Missing/invalid 'User Vehicle(s)' input: Crisis Region User is not defined in this run!";
    }

    if (multiplierValue < 0.0) {
        if (errorString.size() > 0) {
            errorString += "\n         ";
        }

        errorString += "Missing/invalid 'Default Timeline Score Multiplier' input";
    }

    if (errorString.size() > 0) {
        throw new InputException(GetClassName(),
                                 "ExtractCrisisAreaInfo(InputDataElement*)", errorString);
    }

    ptrMultiplierArray_a = new Array<double>(ptrUserVehicleArray_a->Size(), multiplierValue);

    for( inputDataIter = inputDataList->begin(); inputDataIter != inputDataList->end(); ++inputDataIter) {
        ptrInputData = (*inputDataIter);

        if ( (ptrInputData->LabelContainsString("Timeline Score Multiplier") == true) &&
                (ptrInputData->GetSpecificType(userDesignator) == true) ) {
            index = ptrUserVehicleArray_a->FindIndex(userDesignator);
            multiplierValue = ptrInputData->GetDoubleValue();

            if ( (index >= 0) && (multiplierValue >= 0.0) ) {
                (*ptrMultiplierArray_a)[index] = multiplierValue;
            }
        }
    }

    return;
}

void
CrisisArea::PrintAttributes(ostream &outfile) const
{
    int  index = 0;
    double defaultMultiplier = 0.0;
    outfile << "   <CRISIS_AREA_BEGIN>" << NEW_LINE;
    outfile << "      Crisis Area Identification  : ";
    outfile << identificationNumber_a << NEW_LINE;
    outfile << "      Crisis Area Label           : ";
    outfile << *ptrIdentificationLabel_a << NEW_LINE;
    outfile << "      Crisis Region Number(s)     : ";
    Region::PrintRegionNumber(outfile, ptrRegionNumberArray_a->ElementAt(index));
    ++index;

    while (index < ptrRegionNumberArray_a->Size()) {
        outfile << ", ";
        Region::PrintRegionNumber(outfile, ptrRegionNumberArray_a->ElementAt(index));
        ++index;
    }

    outfile << NEW_LINE;
    index = 0;
    outfile << "      User Vehicle(s)             : ";
    outfile << ptrUserVehicleArray_a->ElementAt(index);
    ++index;

    while (index < ptrUserVehicleArray_a->Size()) {
        outfile << ", " << ptrUserVehicleArray_a->ElementAt(index);
        ++index;
    }

    outfile << NEW_LINE;
    index = 0;
    defaultMultiplier = ptrMultiplierArray_a->ElementAt(index);
    outfile << "      Default Timeline Multiplier : ";
    outfile << defaultMultiplier << NEW_LINE;
    ++index;

    while (index < ptrMultiplierArray_a->Size()) {
        if (ptrMultiplierArray_a->ElementAt(index) != defaultMultiplier) {
            outfile << "        Timeline Multiplier [";
            outfile << ptrUserVehicleArray_a->ElementAt(index) << "]   : ";
            outfile << ptrMultiplierArray_a->ElementAt(index) << NEW_LINE;
        }

        ++index;
    }

    outfile << "   <CRISIS_AREA_STOP>" << NEW_LINE;
    return;
}

CrisisArea::~CrisisArea()
{
    if (ptrRegionNumberArray_a != NULL) {
        delete ptrRegionNumberArray_a;
        ptrRegionNumberArray_a = NULL;
    }

    if (ptrMultiplierArray_a != NULL) {
        delete ptrMultiplierArray_a;
        ptrMultiplierArray_a = NULL;
    }

    if (ptrIdentificationLabel_a != NULL) {
        delete ptrIdentificationLabel_a;
        ptrIdentificationLabel_a = NULL;
    }

    if (ptrUserVehicleArray_a != NULL) {
        delete ptrUserVehicleArray_a;
        ptrUserVehicleArray_a = NULL;
    }

    return;
}
