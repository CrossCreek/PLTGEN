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
 * GroundStation.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <memory>

#include "GroundStation.h"

//#include "optimizer/Constraint.h"
#include "RelaySatellite.h"
#include "mural/Earth.h"
#include "mural/coorframe/ElementCoordinateFrame.h"
#include "mural/antenna/ReceiveFacilityAntenna.h"
#include "mural/modules/lg/LinkCriteria.h"
#include "mural/modules/lg/LinksGenerator.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/io/OutputGenerator.h"
#include "mural/io/FileNames.h"

#include "general/array/Array.h"
#include "general/array/ListIterator.h"
#include "general/math/CoordinateFrame.h"
#include "general/utility/StringEditor.h"

Array<string>* GroundStation::ptrIdentificationArray_s = NULL;

//----------------------------------------------------------------------
//
//  Method Name: GroundStation::GroundStation
//
//  Purpose:
//    Constructor for the GroundStation class.
//
//  Calling Method:
//     GroundStation();
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
//----------------------------------------------------------------------

GroundStation::GroundStation()
    : DataProvider(), ptrRFList_a(NULL)
{
    elementType_a = GROUND_STATION;
    return;
}

GroundStation&
GroundStation::operator = (const GroundStation &copyMe)
{
    DataProvider::operator = (copyMe);
    ptrRFList_a             = copyMe.ptrRFList_a;
    return(*this);
}

bool
GroundStation::operator == (const GroundStation &compareMe) const
{
    return( (DataProvider::operator == (compareMe)) &&
            (*ptrRFList_a == *(compareMe.ptrRFList_a)) );
}

bool
GroundStation::operator != (const GroundStation &compareMe) const
{
    return(!(operator == (compareMe)));
}

void
GroundStation::ResetIdentification()
{
    if (ptrIdentificationArray_s != NULL) {
        delete ptrIdentificationArray_s;
        ptrIdentificationArray_s = NULL;
    }
}

void
GroundStation::SetIdentification(const Array<string> *ptrIdentification)
{
    int       groundStationIndex = 0;
    const int numberOfGroundStations = ptrIdentification->Size();
    string     designator = "";

    if (ptrIdentificationArray_s == NULL) {
        ptrIdentificationArray_s = new Array<string>();
    }

    while (groundStationIndex < numberOfGroundStations) {
        designator = ptrIdentification->ElementAt(groundStationIndex);

        if (ptrIdentificationArray_s->ValueExists(designator) == true) {
            throw new InputException(GetClassName(),
                                     "SetIdentification(const Array<string>*)",
                                     "Can not have multiple Ground Stations with the same Designator!");
        }

        ptrIdentificationArray_s->AddElement(designator);
        ++groundStationIndex;
    }

    return;
}

void
GroundStation::SortIdentificationArray()
{
    Array<string> *ptrSortedIdentificationArray = NULL;

    if (ptrIdentificationArray_s != NULL) {
        int arrayIndex = 0;
        int arraySize = 0;
        arraySize = ptrIdentificationArray_s->Size();
        ptrSortedIdentificationArray = new Array<string>();

        while (arrayIndex < arraySize) {
            ptrSortedIdentificationArray->InsertElement(ptrIdentificationArray_s->ElementAt(arrayIndex));
            ++arrayIndex;
        }

        delete ptrIdentificationArray_s;
        ptrIdentificationArray_s = ptrSortedIdentificationArray;
        ptrSortedIdentificationArray = NULL;
    }
}

void
GroundStation::PrintAttributes(ostream &outfile) const
{
    MURALTypes::RFAVector::iterator rfaIter;
    shared_ptr<ReceiveFacilityAntenna> ptrRF = NULL;
    outfile << "<GROUND_STATION_OBJECT_START>" << NEW_LINE;
    PrintAllNames(outfile);

    for(rfaIter = ptrRFList_a->begin(); rfaIter != ptrRFList_a->end(); ++rfaIter) {
        ptrRF = (*rfaIter);
        outfile << NEW_LINE;
        ptrRF->PrintAttributes(outfile);
    }

    DataProvider::PrintAttributes(outfile);

    outfile << "<GROUND_STATION_OBJECT_END>" << NEW_LINE;
    return;
}

void
GroundStation::SetRequiredNumberOfRFs()
{
    MURALTypes::RFAVector::iterator rfaIter;
    shared_ptr<ReceiveFacilityAntenna> ptrRF = NULL;

    for(rfaIter = ptrRFList_a->begin(); rfaIter != ptrRFList_a->end(); ++rfaIter) {
        ptrRF = (*rfaIter);
        ptrRF->SetRequiredNumberOfRFs();
    }
}

int
GroundStation::GetTotalRFCapacity() const
{
    int                                numberOfRFs = 0;
    MURALTypes::RFAVector::iterator rfaIter;
    shared_ptr<ReceiveFacilityAntenna> ptrRF = NULL;

    for(rfaIter = ptrRFList_a->begin(); rfaIter != ptrRFList_a->end(); ++rfaIter) {
        ptrRF = (*rfaIter);
        numberOfRFs += ptrRF->GetCapacity();
    }

    return(numberOfRFs);
}

int
GroundStation::GetNumberOfDedicatedTransmitters() const
{
    int  numberOfDedicatedTransmitters = 0;
    MURALTypes::RFAVector::iterator rfaIter;
    shared_ptr<ReceiveFacilityAntenna> ptrRF = NULL;

    for(rfaIter = ptrRFList_a->begin(); rfaIter != ptrRFList_a->end(); ++rfaIter) {
        ptrRF = (*rfaIter);
        numberOfDedicatedTransmitters += ptrRF->GetNumberOfDedicatedTransmitters();
    }

    return(numberOfDedicatedTransmitters);
}

double
GroundStation::GetMaximumReceiveDataRate() const
{
    double                               maximumReceiveDataRate = 0.0;
    MURALTypes::RFAVector::iterator rfaIter;
    shared_ptr<ReceiveFacilityAntenna> ptrRF = NULL;

    if ( ptrRFList_a != NULL ) {
        for(rfaIter = ptrRFList_a->begin(); rfaIter != ptrRFList_a->end(); ++rfaIter) {
            ptrRF = (*rfaIter);
            maximumReceiveDataRate += (ptrRF->GetDataRate() * (double)ptrRF->GetCapacity());
        }
    }

    return(maximumReceiveDataRate);
}

bool
GroundStation::ConvertDesignatorToIndex(const string &groundDesignator,
                                        int &index)
{
    index = -1;

    if (ptrIdentificationArray_s != NULL) {
        index = ptrIdentificationArray_s->FindIndex(groundDesignator);
    }

    return(index >= 0);
}

bool
GroundStation::ConvertIndexToDesignator(int index,
                                        string &groundDesignator)
{
    bool validDesignator = false;

    if (ptrIdentificationArray_s != NULL) {
        if ((index >= 0) && (index < ptrIdentificationArray_s->Size())) {
            groundDesignator = ptrIdentificationArray_s->ElementAt(index);
            validDesignator = true;
        }
    }

    return(validDesignator);
}

bool
GroundStation::HasDownlinkInview(const string &relayDesignator,
	int timeIndex) const
{
	bool                                 hasDownlinkInview = false;
	Link                                *ptrDownlink = NULL;
	MURALTypes::RFAVector::iterator rfaIter;
	shared_ptr<ReceiveFacilityAntenna> ptrRF = NULL;

	for (rfaIter = ptrRFList_a->begin(); (rfaIter != ptrRFList_a->end()) && (hasDownlinkInview == false); ++rfaIter) {
		ptrRF = (*rfaIter);
		ListIterator<Link> downlinksListIter(ptrRF->GetLinksList());
		ptrDownlink = downlinksListIter.First();

		while ((ptrDownlink != NULL) && (hasDownlinkInview == false)) {
			if ((ptrDownlink->GetTransmitDesignator() == relayDesignator) &&
				(ptrDownlink->IsInview(timeIndex) == true)) {
				hasDownlinkInview = true;
			}

			ptrDownlink = downlinksListIter.Next();
		}
	}

	return(hasDownlinkInview);
}

string
GroundStation::GetDesignatorString(int index)
{
    string designator = "";

    try {
        designator = ptrIdentificationArray_s->ElementAt(index);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetIdentification(int)");
        throw;
    }

    return(designator);
}

bool
GroundStation::HasPooledEquipment() const
{
    bool                               hasPooledEquipment = false;
    MURALTypes::RFAVector::iterator    rfaIter;
    shared_ptr<ReceiveFacilityAntenna> ptrRF = NULL;

    for(rfaIter = ptrRFList_a->begin(); (rfaIter != ptrRFList_a->end()) && (hasPooledEquipment == false); ++rfaIter) {
        ptrRF = (*rfaIter);
        hasPooledEquipment = ptrRF->HasPooledEquipment();
    }

    return(hasPooledEquipment);
}

bool
GroundStation::HasPrecludedUser() const
{
    bool                               hasPrecludedUser = false;
    MURALTypes::RFAVector::iterator    rfaIter;
    shared_ptr<ReceiveFacilityAntenna> ptrRF = NULL;

    for(rfaIter = ptrRFList_a->begin(); (rfaIter != ptrRFList_a->end()) && (hasPrecludedUser == false); ++rfaIter) {
        ptrRF = (*rfaIter);
        hasPrecludedUser = ptrRF->HasPrecludedUser();
    }

    return(hasPrecludedUser);
}

shared_ptr<ReceiveFacilityAntenna>
GroundStation::GetRF(const string &antennaDesignator,
                     int &rfIndex) const
{
    bool                               foundRF = false;
    MURALTypes::RFAVector::iterator    rfaIter;
    shared_ptr<ReceiveFacilityAntenna> ptrRF = NULL;
    rfIndex = 0;

    for(rfaIter = ptrRFList_a->begin(); (rfaIter != ptrRFList_a->end()) && (foundRF == false); ++rfaIter) {
        ptrRF = (*rfaIter);

        if (ptrRF->GetAntennaDesignator() == antennaDesignator) {
            foundRF = true;
        } else {
            ++rfIndex;
        }
    }

    return(ptrRF);
}


/* ******************************************************************************************* */

shared_ptr<MURALTypes::AntennaVector>
GroundStation::GetConjunctionAntennaList()
{
    shared_ptr<MURALTypes::AntennaVector> ptrAntennaVector = NULL;
    MURALTypes::RFAVector::iterator rfaIter;
    shared_ptr<Antenna> ptrAntenna = NULL;
    ptrAntennaVector = shared_ptr<MURALTypes::AntennaVector>( new MURALTypes::AntennaVector() );

    for( rfaIter = ptrRFList_a->begin(); rfaIter != ptrRFList_a->end(); ++rfaIter) {
        ptrAntenna = dynamic_pointer_cast<Antenna>((*rfaIter));

        if ( ptrAntenna != NULL ) {
            ptrAntennaVector->push_back(ptrAntenna);
        }
    }

    return(ptrAntennaVector);
}

/* ******************************************************************************************* */

void
GroundStation::SetRFList(const shared_ptr<MURALTypes::RFAVector> &ptrRFList)
{
    ptrRFList_a = ptrRFList;
}

/* ******************************************************************************************* */

GroundStation::~GroundStation()
{
    if (ptrIdentificationArray_s != NULL) {
        ptrIdentificationArray_s->RemoveElement(GetDesignator());

        if (ptrIdentificationArray_s->Size() == 0) {
            delete ptrIdentificationArray_s;
            ptrIdentificationArray_s = NULL;
        }
    }

    return;
}

