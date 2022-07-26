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
 * DirectDownlinkAntenna.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "DirectDownlinkAntenna.h"
#include "mural/io/MURALInputProcessor.h"

//------------------------------------------------------------------------------
//
//  Operation Name: DirectDownlinkAntenna::DirectDownlinkAntenna
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
DirectDownlinkAntenna::DirectDownlinkAntenna()
    : UserAntenna()
{
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: DirectDownlinkAntenna::DirectDownlinkAntenna
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

DirectDownlinkAntenna::DirectDownlinkAntenna(shared_ptr<UserVehicle> ptrUser)
    : UserAntenna(ptrUser)
{
    return;
}

DirectDownlinkAntenna&
DirectDownlinkAntenna::operator = (const DirectDownlinkAntenna &copyMe)
{
    UserAntenna::operator = (copyMe);
    return(*this);
}

bool
DirectDownlinkAntenna::operator == (const DirectDownlinkAntenna &compareMe)
{
    return(UserAntenna::operator == (compareMe));
}

bool
DirectDownlinkAntenna::operator != (const DirectDownlinkAntenna &compareMe)
{
    return(!(operator == (compareMe)));
}

void
DirectDownlinkAntenna::PrintAttributes(ostream &outfile) const
{
    outfile << "   <DIRECT_DOWNLINK_ANTENNA_BEGIN>" << NEW_LINE;
    UserAntenna::PrintAttributes(outfile);
    outfile << "   <DIRECT_DOWNLINK_ANTENNA_STOP>" << NEW_LINE;
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: DirectDownlinkAntenna::~DirectDownlinkAntenna
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
DirectDownlinkAntenna::~DirectDownlinkAntenna()
{
    // this is definitely the transmitting antenna, so delete all links
    if (ptrLinksList_a != NULL) {
        ptrLinksList_a->Delete();
        delete ptrLinksList_a;
        ptrLinksList_a = NULL;
    }

    return;
}
