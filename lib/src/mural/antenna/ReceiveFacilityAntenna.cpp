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
 * ReceiveFacilityAntenna.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "ReceiveFacilityAntenna.h"

#include "mural/coorframe/AntennaCoordinateFrame.h"
#include "mural/element/GroundStation.h"
#include "mural/element/UserVehicle.h"
#include "mural/io/MURALInputProcessor.h"
#include "mural/io/OutputGenerator.h"

#include "general/array/ListIterator.h"
#include "general/math/CoordinateFrame.h"
#include "general/math/SimpleMath.h"
#include "general/data/InputDataElement.h"
#include "general/utility/StringEditor.h"

int          ReceiveFacilityAntenna::totalNumberOfRFs_s = 0;
Array2D<char>* ReceiveFacilityAntenna::ptrIdentificationArray_s = NULL;

//------------------------------------------------------------------------------
//
//  Operation Name: ReceiveFacilityAntenna::ReceiveFacilityAntenna
//
//  Purpose:
//     This is the default constructor
//
//  Calling Operation:
//
//  Calling Parameters
//  Parameters        i/o/r  description
//  ----------        -----  -----------
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------
ReceiveFacilityAntenna::ReceiveFacilityAntenna()
    : Antenna(), ptrPrecludedUserArray_a(NULL),
      ptrPooledEquipmentList_a(NULL), ptrUserToGroundPathList_a(NULL)
{
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: ReceiveFacilityAntenna::ReceiveFacilityAntenna
//
//  Purpose:
//     This is the alternate constructor
//
//  Calling Operation:
//
//  Calling Parameters
//  Parameters           i/o/r  description
//  ----------           -----  -----------
//  ptrInputDataElement*   i    input data to create the Antenna (parent) class
//  ptrUser*               i    pointer the user vehicle creating this
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

ReceiveFacilityAntenna::ReceiveFacilityAntenna(shared_ptr<GroundStation> ptrGround,
        int previousCapacity)
    : Antenna(ptrGround, previousCapacity), ptrPrecludedUserArray_a(NULL),
      ptrPooledEquipmentList_a(NULL), ptrUserToGroundPathList_a(NULL), userConstrained_a(false)
{
    return;
}

ReceiveFacilityAntenna&
ReceiveFacilityAntenna::operator = (const ReceiveFacilityAntenna &copyMe)
{
    Antenna::operator = (copyMe);
    ptrPrecludedUserArray_a   = copyMe.ptrPrecludedUserArray_a;
    ptrPooledEquipmentList_a  = copyMe.ptrPooledEquipmentList_a;
    ptrUserToGroundPathList_a = copyMe.ptrUserToGroundPathList_a;
    userConstrained_a         = copyMe.userConstrained_a;
    return(*this);
}

bool
ReceiveFacilityAntenna::operator == (const ReceiveFacilityAntenna &compareMe)
{
    return( (Antenna::operator == (compareMe)) );
}

bool
ReceiveFacilityAntenna::operator != (const ReceiveFacilityAntenna &compareMe)
{
    return(!(operator == (compareMe)));
}

/* ****************************************************************************************** */

shared_ptr<GroundStation>
ReceiveFacilityAntenna::GetGround() const
{
    shared_ptr<GroundStation> ptrGround(dynamic_pointer_cast<GroundStation>(ptrElement_a));
    return(ptrGround);
}

/* ****************************************************************************************** */

/**
 * This method prints the attributes of the RF to the mrl file
 *
 * @param outfile the reference to the mrl file
 */
void
ReceiveFacilityAntenna::PrintAttributes(ostream &outfile) const
{
    int                          userIndex = 0;
    const TIME_UNIT     inputTimeUnits = UnitData::GetInputTimeUnits();
    ListIterator<PooledEquipmentPeriod> pooledEquipmentListIter(ptrPooledEquipmentList_a);
    PooledEquipmentPeriod              *ptrPooledEquipment = pooledEquipmentListIter.First();
    outfile << "   <RECEIVE_FACILITY_ANTENNA_BEGIN>" << NEW_LINE;
    Antenna::PrintAttributes(outfile);

    if (ptrPrecludedUserArray_a != NULL) {
        userIndex = 0;
        outfile << "      Precluded User Vehicle(s)                               : ";
        outfile << ptrPrecludedUserArray_a->ElementAt(userIndex);
        ++userIndex;

        while (userIndex < ptrPrecludedUserArray_a->Size()) {
            outfile << ", " << ptrPrecludedUserArray_a->ElementAt(userIndex);
            ++userIndex;
        }

        outfile << NEW_LINE;
    }

    outfile << "   <RECEIVE_FACILITY_ANTENNA_STOP>" << NEW_LINE;
    return;
}

/**
 * This method sets the boolean value to signal that this RF is part of
 * a user capacity constraint (9.3)
 *
 * @param userConstrained the value to set
 */
void
ReceiveFacilityAntenna::SetUserConstrained(const bool userConstrained)
{
    userConstrained_a = userConstrained;
}

/**
 * This method creates the identification array for the individual capacities
 * of the RF.  The first capacity is represented by 'A' and so forth.  It
 * sets the array on the static 2D identification array
 *
 * @param ptrCapacityArray
 */
void
ReceiveFacilityAntenna::CreateIdentificationArray(const Array<int> *ptrCapacityArray)
{
    int         capacityIndex = 0;
    int         overallCapacityIndex = 0;
    int         groundIndex = 0;
    const int   numberOfGroundStations = ptrCapacityArray->Size();
    Array<char> *ptrArray = NULL;

    if (ptrIdentificationArray_s != NULL) {
        delete ptrIdentificationArray_s;
        ptrIdentificationArray_s = NULL;
    }

    ptrIdentificationArray_s = new Array2D<char>(numberOfGroundStations);

    while (groundIndex < numberOfGroundStations) {
        int         numberOfRFs = 0;
        numberOfRFs = ptrCapacityArray->ElementAt(groundIndex);

        if (numberOfRFs > 0) {
            ptrArray = new Array<char>(numberOfRFs, BLANK_CHARACTER);
            capacityIndex = 0;

            while (capacityIndex < numberOfRFs) {
                (*ptrArray)[capacityIndex] = 'A' + overallCapacityIndex;
                ++overallCapacityIndex;
                ++capacityIndex;
            }
        }

        ptrIdentificationArray_s->AddArray(groundIndex, ptrArray);
        ptrArray = NULL;
        ++groundIndex;
    }
}

int
ReceiveFacilityAntenna::GetDataProviderIndex() const
{
    return(GetGround()->GetIndex());
}

int
ReceiveFacilityAntenna::GetDataProviderNumber() const
{
    return(GetGround()->GetNumber());
}

char
ReceiveFacilityAntenna::GetIdentification(int groundIndex,
        int rfIndex)
{
    char identification = ERROR_CHARACTER;

    if (ptrIdentificationArray_s != NULL) {
        try {
            identification = ptrIdentificationArray_s->ElementAt(groundIndex, rfIndex);
        } catch (Exception *ptrError) {
            ptrError->AddMethod(GetClassName(),
                                "GetIdentification(int, int)");
            throw;
        }
    }

    return(identification);
}

/* ***************************************************************************** */
int
ReceiveFacilityAntenna::GetTotalNumberOfRFs()
{
    return(totalNumberOfRFs_s);
}

/* ***************************************************************************** */
void
ReceiveFacilityAntenna::SetRequiredNumberOfRFs()
{
    capacity_a = std::max(GetNumberOfLinks(), capacity_a);
}

/* ***************************************************************************** */
bool
ReceiveFacilityAntenna::HasPooledEquipment() const
{
    return(ptrPooledEquipmentList_a != NULL);
}

/* ***************************************************************************** */
bool
ReceiveFacilityAntenna::HasPrecludedUser() const
{
    return(ptrPrecludedUserArray_a != NULL);
}

/* ***************************************************************************** */
bool
ReceiveFacilityAntenna::HasUserToGroundPaths() const
{
    return(ptrUserToGroundPathList_a != NULL);
}

/* ***************************************************************************** */
bool
ReceiveFacilityAntenna::IsPrecludedUser(const string &userDesignator) const
{
    bool isPrecludedUser = false;

    if (ptrPrecludedUserArray_a != NULL) {
        isPrecludedUser = ptrPrecludedUserArray_a->ValueExists(userDesignator);
    }

    return(isPrecludedUser);
}

/* ***************************************************************************** */
bool
ReceiveFacilityAntenna::IsUserConstrained() const
{
    return(userConstrained_a);
}

/* ***************************************************************************** */
bool
ReceiveFacilityAntenna::IsTransmittingAntenna() const
{
    return(false);
}

/* ***************************************************************************** */
bool
ReceiveFacilityAntenna::IsReceiveAntenna() const
{
    return(true);
}

/* ***************************************************************************** */
LinkList<UserToGroundPath>*
ReceiveFacilityAntenna::GetUserToGroundPathList() const
{
    return(ptrUserToGroundPathList_a);
}

/* ***************************************************************************** */
void
ReceiveFacilityAntenna::SetPooledEquipmentList(LinkList<PooledEquipmentPeriod>  *ptrPooledEquipmentList)
{
    ptrPooledEquipmentList_a = ptrPooledEquipmentList;
}

/* ***************************************************************************** */
void
ReceiveFacilityAntenna::SetPrecludedUserArray(Array<string>* ptrPrecludedUserArray)
{
    ptrPrecludedUserArray_a = ptrPrecludedUserArray;
}

/* ***************************************************************************** */

void
ReceiveFacilityAntenna::AddRFCountToTotal(int capacity)
{
    totalNumberOfRFs_s += capacity_a;
}
//------------------------------------------------------------------------------
//
//  Operation Name: ReceiveFacilityAntenna::~ReceiveFacilityAntenna
//
//  Purpose:
//     This is the destructor
//
//  Calling Operation:
//
//  Calling Parameters
//  Parameters        i/o/r  description
//  ----------        -----  -----------
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------
ReceiveFacilityAntenna::~ReceiveFacilityAntenna()
{
    if (ptrIdentificationArray_s != NULL) {
        delete ptrIdentificationArray_s;
        ptrIdentificationArray_s = NULL;
    }

    // this is definitely the receiving antenna, so only remove (don't delete) all links
    if (ptrLinksList_a != NULL) {
        ptrLinksList_a->RemoveAll();
        delete ptrLinksList_a;
        ptrLinksList_a = NULL;
    }

    if (ptrPrecludedUserArray_a != NULL) {
        delete ptrPrecludedUserArray_a;
        ptrPrecludedUserArray_a = NULL;
    }

    if (ptrPooledEquipmentList_a != NULL) {
        ptrPooledEquipmentList_a->Delete();
        delete ptrPooledEquipmentList_a;
        ptrPooledEquipmentList_a = NULL;
    }

    if (ptrUserToGroundPathList_a != NULL) {
        ptrUserToGroundPathList_a->Delete();
        delete ptrUserToGroundPathList_a;
        ptrUserToGroundPathList_a = NULL;
    }

    return;
}
