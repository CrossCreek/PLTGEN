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
 * CollectionElement.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "mural/CollectionElement.h"
#include "mural/modules/vtg/RegionData.h"

//------------------------------------------------------------------------------
//
//  Method Name: CollectionElement::CollectionElement
//
//  Purpose:
//    Constructor for the CollectionElement class.
//
//  Calling Method:
//     CollectionElement();
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

CollectionElement::CollectionElement()
    : ptrCollectedPointTargets_a(NULL), ptrCollectedArea_a(NULL),
      collectionScore_a(0.0), ptrResourceFactor_a(NULL),
      ptrMostValuableRegionData_a(NULL), ptrRegionDataList_a(NULL)
{
    return;
}

CollectionElement::CollectionElement(double score,
                                     LinkList<RegionData> *ptrValidRegionDataList,
                                     RegionData *ptrMVRegionData,
                                     //RAGUSA 1/30/2018
                                     int crisisLevel,
                                     Array<double> *ptrResourceFactor,
                                     Array<int> *ptrCollectedPoints,
                                     Array<double> *ptrCollectedArea)
    : ptrCollectedPointTargets_a(ptrCollectedPoints), ptrCollectedArea_a(ptrCollectedArea),
      collectionScore_a(score), ptrResourceFactor_a(NULL), crisisLevel_a(crisisLevel),
      ptrMostValuableRegionData_a(ptrMVRegionData), ptrRegionDataList_a(NULL)
{
    ListIterator<RegionData> regionDataListIter(ptrValidRegionDataList);
    RegionData              *ptrRegionData = regionDataListIter.First();

    while (ptrRegionData != NULL) {
        AddRegionData(ptrRegionData);
        ptrRegionData = regionDataListIter.Next();
    }

    if (ptrResourceFactor != NULL) {
        ptrResourceFactor_a = new Array<double>(*ptrResourceFactor);
    }

    if (ptrCollectedPoints != NULL) {
        ptrCollectedPointTargets_a = new Array<int>(*ptrCollectedPoints);
    }

    if (ptrCollectedArea != NULL) {
        ptrCollectedArea_a = new Array<double>(*ptrCollectedArea);
    }

    if ((ptrMostValuableRegionData_a == NULL) && (ptrRegionDataList_a != NULL)) {
        if (ptrRegionDataList_a->Size() == 1) {
            ptrMostValuableRegionData_a = ptrRegionDataList_a->GetFirstObject();
        }
    }

    return;
}

CollectionElement::CollectionElement(const CollectionElement &copyMe)
    : ptrCollectedPointTargets_a(NULL), ptrCollectedArea_a(NULL),
      collectionScore_a(copyMe.collectionScore_a), ptrResourceFactor_a(NULL),
      ptrMostValuableRegionData_a(copyMe.ptrMostValuableRegionData_a),
      ptrRegionDataList_a(NULL)
{
    ListIterator<RegionData> regionDataListIter(copyMe.ptrRegionDataList_a);
    RegionData *ptrRegionData = regionDataListIter.First();

    if (copyMe.ptrCollectedPointTargets_a != NULL) {
        ptrCollectedPointTargets_a = new Array<int>(*(copyMe.ptrCollectedPointTargets_a));
    }

    if (copyMe.ptrCollectedArea_a != NULL) {
        ptrCollectedArea_a = new Array<double>(*(copyMe.ptrCollectedArea_a));
    }

    if (copyMe.ptrResourceFactor_a != NULL) {
        ptrResourceFactor_a = new Array<double>(*(copyMe.ptrResourceFactor_a));
    }

    while (ptrRegionData != NULL) {
        if (ptrRegionDataList_a == NULL) {
            ptrRegionDataList_a = new LinkList<RegionData>();
        }

        ptrRegionDataList_a->Append(ptrRegionData);
        ptrRegionData = regionDataListIter.Next();
    }

    return;
}

CollectionElement&
CollectionElement::operator = (const CollectionElement &copyMe)
{
    ptrCollectedPointTargets_a  = copyMe.ptrCollectedPointTargets_a;
    ptrCollectedArea_a          = copyMe.ptrCollectedArea_a;
    collectionScore_a           = copyMe.collectionScore_a;
    ptrMostValuableRegionData_a = copyMe.ptrMostValuableRegionData_a;
    ptrRegionDataList_a         = copyMe.ptrRegionDataList_a;
    crisisLevel_a               = copyMe.crisisLevel_a;
    return(*this);
}

bool
CollectionElement::operator == (const CollectionElement &compareMe) const
{
    return(collectionScore_a == compareMe.collectionScore_a);
}

bool
CollectionElement::operator != (const CollectionElement &compareMe) const
{
    return (!(operator == (compareMe)));
}

void
CollectionElement::AddRegionData(RegionData *ptrRegionData)
{
    if (ptrRegionDataList_a == NULL) {
        ptrRegionDataList_a = new LinkList<RegionData>();
    }

    ptrRegionDataList_a->Append(ptrRegionData);
}

/*RAGUSA 1/30/2018 - Deprecated. Moved GetCrisisLevel() to header.
  Added crisisLevel to CollectionElement because not all of the timeline generation
  methods populate ptrMostValuableRegion_a... Meaning that crisis region never actually
  worked correctly for READ,TCA, & REGION!!! Also, crisis region method is no longer
  "collection based" we now just want to know if the collection element was requested
  &/or allocated - it doesn't matter the region. Besides... crisis regions
  shouldn't have been default MVRs in the first place
int
CollectionElement::GetCrisisLevel() const
{

    int crisisLevel = 0;

    if (ptrMostValuableRegionData_a != NULL) {
        crisisLevel = ptrMostValuableRegionData_a->GetCrisisLevel();
	}

    return(crisisLevel);
}
===========================================================================================*/
int
CollectionElement::GetMissionNumber(int resourceIndex) const
{
    int missionNumber = 0;

    if (ptrMostValuableRegionData_a != NULL) {
        missionNumber = ptrMostValuableRegionData_a->GetMissionNumber();
    }

    return(missionNumber);
}

int
CollectionElement::GetSensorNumber(int resourceIndex) const
{
    int sensorNumber = 0;

    if (ptrMostValuableRegionData_a != NULL) {
        sensorNumber = ptrMostValuableRegionData_a->GetSensorNumber();
    }

    return(sensorNumber);
}

Array<double>*
CollectionElement::GetRegionsInView() const
{
    Array<double> *ptrRegionInViewArray = NULL;
    ListIterator<RegionData> regionDataListIter(ptrRegionDataList_a);
    RegionData              *ptrRegionData = regionDataListIter.First();

    while (ptrRegionData != NULL) {
        if (ptrRegionInViewArray == NULL) {
            ptrRegionInViewArray = new Array<double>();
        }

        ptrRegionInViewArray->InsertUniqueElement(ptrRegionData->GetRegionNumber());
        ptrRegionData = regionDataListIter.Next();
    }

    return(ptrRegionInViewArray);
}

string
CollectionElement::GetSensorName() const
{
    string sensorName = "NONE";

    if (ptrMostValuableRegionData_a != NULL) {
        sensorName = ptrMostValuableRegionData_a->GetSensorName();
    } else if (ptrRegionDataList_a != NULL) {
        sensorName = (ptrRegionDataList_a->GetFirstObject())->GetSensorName();
    }

    return(sensorName);
}

int
CollectionElement::GetSubPriorityLevel(int resourceIndex) const
{
    int subPriorityLevel = 0;

    if (ptrMostValuableRegionData_a != NULL) {
        subPriorityLevel = ptrMostValuableRegionData_a->GetSubPriorityLevel();
    }

    return(subPriorityLevel);
}

double
CollectionElement::GetImagedRegion(int resourceIndex) const
{
    double imagedRegion = 0.0;

    if (ptrMostValuableRegionData_a != NULL) {
        imagedRegion = ptrMostValuableRegionData_a->GetRegionNumber();
    }

    return(imagedRegion);
}

bool
CollectionElement::IsRegionIncluded(int regionNumber) const
{
    bool   regionIncluded = false;
    ListIterator<RegionData> regionDataListIter(ptrRegionDataList_a);
    RegionData              *ptrRegionData = regionDataListIter.First();

    while (ptrRegionData != NULL) {
        if ((int)ptrRegionData->GetRegionNumber() == regionNumber) {
            regionIncluded = true;
            ptrRegionData = NULL;
        } else {
            ptrRegionData = regionDataListIter.Next();
        }
    }

    return(regionIncluded);
}

void
CollectionElement::RemoveNonCrisisRegionDataRequirements()
{
    ListIterator<RegionData> regionDataListIter(ptrRegionDataList_a);
    RegionData              *ptrRegionData = regionDataListIter.First();

    while (ptrRegionData != NULL) {
        if (ptrRegionData->IsCrisisRegion() == false) {
            ptrRegionData->RemoveAllRequirements();
        }

        ptrRegionData = regionDataListIter.Next();
    }
}

void
CollectionElement::RemoveAllRegionData()
{
    if (ptrRegionDataList_a != NULL) {
        ptrRegionDataList_a->Delete();
        delete ptrRegionDataList_a;
        ptrRegionDataList_a = NULL;
    }
}

CollectionElement::~CollectionElement()
{
    if (ptrCollectedPointTargets_a != NULL) {
        delete ptrCollectedPointTargets_a;
        ptrCollectedPointTargets_a = NULL;
    }

    if (ptrCollectedArea_a != NULL) {
        delete ptrCollectedArea_a;
        ptrCollectedArea_a = NULL;
    }

    if (ptrResourceFactor_a != NULL) {
        delete ptrResourceFactor_a;
        ptrResourceFactor_a = NULL;
    }

    ptrMostValuableRegionData_a = NULL;

    // If this is just a CollectionElement instance, it is pointing to
    // RegionData classes controlled by the ValueTimelineElement class.
    // If this is a parent to a VTE class they have already been deleted
    // by the VTE destructor
    if (ptrRegionDataList_a != NULL) {
        ptrRegionDataList_a->RemoveAll();
        delete ptrRegionDataList_a;
        ptrRegionDataList_a = NULL;
    }

    return;
}
