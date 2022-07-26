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
 * UserTransmitAntenna.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "UserTransmitAntenna.h"
#include "mural/io/MURALInputProcessor.h"


#include "general/array/ListIterator.h"
#include "general/data/InputData.h"

//------------------------------------------------------------------------------
//
//  Operation Name: UserTransmitAntenna::UserTransmitAntenna
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
UserTransmitAntenna::UserTransmitAntenna()
    : UserAntenna(), fixedGimbal_a(false), ptrCableWrapOffsetAngles_a(NULL)
{
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: UserTransmitAntenna::UserTransmitAntenna
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

UserTransmitAntenna::UserTransmitAntenna(shared_ptr<UserVehicle> ptrUser)
    : UserAntenna(ptrUser), fixedGimbal_a(false),
      ptrCableWrapOffsetAngles_a(NULL)
{
    return;
}

UserTransmitAntenna&
UserTransmitAntenna::operator = (const UserTransmitAntenna &copyMe)
{
    UserAntenna::operator = (copyMe);
    fixedGimbal_a              = copyMe.fixedGimbal_a;
    ptrCableWrapOffsetAngles_a = NULL;

    if (copyMe.ptrCableWrapOffsetAngles_a != NULL) {
        ptrCableWrapOffsetAngles_a
            = new Array<double>(*(copyMe.ptrCableWrapOffsetAngles_a));
    }

    return(*this);
}

bool
UserTransmitAntenna::operator == (const UserTransmitAntenna &compareMe)
{
    return( (UserAntenna::operator == (compareMe)) &&
            (fixedGimbal_a == compareMe.fixedGimbal_a) );
}

bool
UserTransmitAntenna::operator != (const UserTransmitAntenna &compareMe)
{
    return(!(operator == (compareMe)));
}

void
UserTransmitAntenna::PrintAttributes(ostream &outfile) const
{
    outfile << "   <USER_TRANSMIT_ANTENNA_BEGIN>" << NEW_LINE;
    UserAntenna::PrintAttributes(outfile);

    if (fixedGimbal_a == true) {
        outfile << "      Fixed Gimbal                                            : true" << NEW_LINE;
    }

    if (ptrCableWrapOffsetAngles_a != NULL) {
        int index = 0;
        outfile << "      Cable Wrap Offset Angles                                : ";
        outfile << UnitData::GetInputAngle(ptrCableWrapOffsetAngles_a->ElementAt(index));
        ++index;

        while (index < ptrCableWrapOffsetAngles_a->Size()) {
            outfile << ", "<< UnitData::GetInputAngle(ptrCableWrapOffsetAngles_a->ElementAt(index));
            ++index;
        }

        outfile << NEW_LINE;
    }

    outfile << "   <USER_TRANSMIT_ANTENNA_STOP>" << NEW_LINE;
    return;
}

bool
UserTransmitAntenna::IsThereAnyCrosslink(int timeIndex) const
{
    bool               isACrosslink = false;
    ListIterator<Link> linkIter(ptrLinksList_a);
    Link              *ptrLink = linkIter.First();

    while ((ptrLink != NULL) && (isACrosslink == false)) {
        isACrosslink = ptrLink->IsCrosslinkInview(timeIndex);
        ptrLink = linkIter.Next();
    }

    return(isACrosslink);
}

/* *********************************************************************************** */

void
UserTransmitAntenna::SetFixedGimbal(bool fixedGimbal)
{
    fixedGimbal_a = fixedGimbal;
}

/* *********************************************************************************** */

void
UserTransmitAntenna::SetCableWrapOffsetAngles(Array<double>* ptrCableWrapOffsetAngles)
{
    ptrCableWrapOffsetAngles_a = ptrCableWrapOffsetAngles;
}

/* *********************************************************************************** */

//------------------------------------------------------------------------------
//
//  Operation Name: UserTransmitAntenna::~UserTransmitAntenna
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
UserTransmitAntenna::~UserTransmitAntenna()
{
    // this is definitely the transmitting antenna, so delete all links
    if (ptrLinksList_a != NULL) {
        ptrLinksList_a->Delete();
        delete ptrLinksList_a;
        ptrLinksList_a = NULL;
    }

    if (ptrCableWrapOffsetAngles_a != NULL) {
        delete ptrCableWrapOffsetAngles_a;
        ptrCableWrapOffsetAngles_a = NULL;
    }

    return;
}
