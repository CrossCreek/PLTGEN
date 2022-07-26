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
 * UserPointingAntenna.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "UserPointingAntenna.h"
#include "mural/element/RelaySatellite.h"
#include "mural/io/MURALInputProcessor.h"
#include "general/array/ListIterator.h"
#include "general/data/InputDataElement.h"
#include "general/utility/StringEditor.h"


//#include "optimizer/Constraint.h"

//------------------------------------------------------------------------------
//
//  Operation Name: UserPointingAntenna::UserPointingAntenna
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
UserPointingAntenna::UserPointingAntenna()
    : RelayAntenna()
{
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: UserPointingAntenna::UserPointingAntenna
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
//  ptrRelay*              i    pointer the relay satellite creating this
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

UserPointingAntenna::UserPointingAntenna(shared_ptr<RelaySatellite> ptrRelay,
        int previousCapacity)
    : RelayAntenna(ptrRelay, previousCapacity)
{
    return;
}

UserPointingAntenna&
UserPointingAntenna::operator = (const UserPointingAntenna &copyMe)
{
    RelayAntenna::operator = (copyMe);
    return(*this);
}

bool
UserPointingAntenna::operator == (const UserPointingAntenna &compareMe)
{
    return(RelayAntenna::operator == (compareMe));
}

bool
UserPointingAntenna::operator != (const UserPointingAntenna &compareMe)
{
    return(!(operator == (compareMe)));
}

void
UserPointingAntenna::PrintAttributes(ostream &outfile) const
{
    outfile << "   <USER_POINTING_ANTENNA_BEGIN>" << NEW_LINE;
    RelayAntenna::PrintAttributes(outfile);
    outfile << "   <USER_POINTING_ANTENNA_STOP>" << NEW_LINE;
    return;
}

Link*
UserPointingAntenna::GetCrosslink(const string &userDesignator) const
{
    return(GetLinkFrom(userDesignator));
}

//------------------------------------------------------------------------------
//
//  Operation Name: UserPointingAntenna::~UserPointingAntenna
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
UserPointingAntenna::~UserPointingAntenna()
{
    // this is definitely the receiving antenna, so only remove (don't delete) all links
    if (ptrLinksList_a != NULL) {
        ptrLinksList_a->RemoveAll();
        delete ptrLinksList_a;
        ptrLinksList_a = NULL;
    }

    return;
}
