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
 * CrisisData.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "CrisisData.h"


#include "general/data/InputDataElement.h"
#include "general/array/ListIterator.h"
#include "general/utility/StringEditor.h"
#include "general/exception/InputException.h"

LinkList<CrisisArea>* CrisisData::ptrCrisisAreaList_s = NULL;

//------------------------------------------------------------------------------
//
//  Method Name: CrisisData::CrisisData
//
//  Purpose:
//    Constructor for the CrisisData class.
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

CrisisData::CrisisData()
{
    return;
}

CrisisData::CrisisData(const CrisisData &copyMe)
{
    return;
}

CrisisData&
CrisisData::operator = (const CrisisData &copyMe)
{
    return(*this);
}

bool
CrisisData::operator == (const CrisisData &compareMe) const
{
    return(false);
}

bool
CrisisData::operator != (const CrisisData &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
CrisisData::ExtractCrisisDataInfo(shared_ptr<InputDataElement> ptrInputDataElement)
{
    CrisisArea             *ptrCrisisArea = NULL;
    shared_ptr<InputDataElement::InputDataElementList> subElementList = ptrInputDataElement->GetInputDataElementList();
    InputDataElement::InputDataElementList::iterator subElementIter;
    shared_ptr<InputDataElement> ptrSubElement = NULL;

    try {
        //CPPCHECK_FIX Performance - prefix iterator change
        for( subElementIter = subElementList->begin(); subElementIter != subElementList->end(); ++subElementIter) {
            ptrSubElement = (*subElementIter);

            if (ptrSubElement->BlockTagContainsString("CRISIS_AREA") == true) {
                ptrCrisisArea = new CrisisArea(ptrSubElement);

                if (ptrCrisisAreaList_s == NULL) {
                    ptrCrisisAreaList_s = new LinkList<CrisisArea>();
                }

                ptrCrisisAreaList_s->Append(ptrCrisisArea);
                ptrCrisisArea = NULL;
            }
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ExtractCrisisDataInfo(InputDataElement*)");
        throw;
    }

    return;
}

void
CrisisData::PrintAttributes(ostream &outfile)
{
    ListIterator<CrisisArea> crisisAreaListIter(ptrCrisisAreaList_s);
    CrisisArea              *ptrCrisisArea = crisisAreaListIter.First();
    outfile << "<CRISIS_INPUT_START>" << NEW_LINE;

    while (ptrCrisisArea != NULL) {
        outfile << NEW_LINE;
        ptrCrisisArea->PrintAttributes(outfile);
        ptrCrisisArea = crisisAreaListIter.Next();
    }

    outfile << "<CRISIS_INPUT_END>" << NEW_LINE;
    return;
}

int
CrisisData::GetCrisisIndex(int crisisLevel)
{
    bool                     foundCrisisArea = false;
    ListIterator<CrisisArea> crisisAreaListIter(ptrCrisisAreaList_s);
    CrisisArea              *ptrCrisisArea   = crisisAreaListIter.First();
    int index                               = 0;

    while ((ptrCrisisArea != NULL) && (foundCrisisArea == false)) {
        if ( ptrCrisisArea->GetIdentificationNumber() == crisisLevel ) {
            foundCrisisArea = true;
        } else {
            ptrCrisisArea = crisisAreaListIter.Next();
            index++;
        }
    }

    return(index);
}

double
CrisisData::GetTimelineScoreMultiplier(int crisisLevel,
                                       const string &userDesignator)
{
    double                    timelineMultiplier = 0.0;
    ListIterator<CrisisArea> crisisAreaListIter(ptrCrisisAreaList_s);
    CrisisArea              *ptrCrisisArea = crisisAreaListIter.First();

    while (ptrCrisisArea != NULL) {
        if (ptrCrisisArea->GetIdentificationNumber() == crisisLevel) {
            timelineMultiplier = ptrCrisisArea->GetTimelineScoreMutliplier(userDesignator);
            ptrCrisisArea = NULL;
        } else {
            ptrCrisisArea = crisisAreaListIter.Next();
        }
    }

    return(timelineMultiplier);
}

bool
CrisisData::HasCrisisArea(const string &userDesignator)
{
    bool                     hasCrisis = false;
    ListIterator<CrisisArea> crisisAreaListIter(ptrCrisisAreaList_s);
    CrisisArea              *ptrCrisisArea = crisisAreaListIter.First();

    while (ptrCrisisArea != NULL) {
        hasCrisis = ptrCrisisArea->ContainsUserVehicle(userDesignator);

        if (hasCrisis == true) {
            ptrCrisisArea = NULL;
        } else {
            ptrCrisisArea = crisisAreaListIter.Next();
        }
    }

    return(hasCrisis);
}

CrisisArea*
CrisisData::GetCrisisArea(const string &userDesignator,
                          double regionNumber)
{
    bool                     foundCrisisArea = false;
    ListIterator<CrisisArea> crisisAreaListIter(ptrCrisisAreaList_s);
    CrisisArea              *ptrCrisisArea = crisisAreaListIter.First();

    while ((ptrCrisisArea != NULL) && (foundCrisisArea == false)) {
        if ( (ptrCrisisArea->ContainsUserVehicle(userDesignator) == true) &&
                (ptrCrisisArea->ContainsRegionNumber(regionNumber) == true) ) {
            foundCrisisArea = true;
        } else {
            ptrCrisisArea = crisisAreaListIter.Next();
        }
    }

    return(ptrCrisisArea);
}


void
CrisisData::DestroyCrisisData()
{
    if (ptrCrisisAreaList_s != NULL) {
        ptrCrisisAreaList_s->Delete();
        delete ptrCrisisAreaList_s;
        ptrCrisisAreaList_s = NULL;
    }

    return;
}

CrisisData::~CrisisData()
{
    return;
}
