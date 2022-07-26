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
 * GroundPointingAntenna.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <algorithm>

#include "GroundPointingAntenna.h"
#include "mural/element/RelaySatellite.h"


#include "general/array/ListIterator.h"

//------------------------------------------------------------------------------
//
//  Operation Name: GroundPointingAntenna::GroundPointingAntenna
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
GroundPointingAntenna::GroundPointingAntenna()
    : RelayAntenna()
{
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: GroundPointingAntenna::GroundPointingAntenna
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

GroundPointingAntenna::GroundPointingAntenna(shared_ptr<RelaySatellite> ptrRelay,
        int previousCapacity)
    : RelayAntenna(ptrRelay, previousCapacity)
{
    return;
}

GroundPointingAntenna&
GroundPointingAntenna::operator = (const GroundPointingAntenna &copyMe)
{
    RelayAntenna::operator = (copyMe);
    return(*this);
}

bool
GroundPointingAntenna::operator == (const GroundPointingAntenna &compareMe)
{
    return(RelayAntenna::operator == (compareMe));
}

bool
GroundPointingAntenna::operator != (const GroundPointingAntenna &compareMe)
{
    return(!(operator == (compareMe)));
}

void
GroundPointingAntenna::PrintAttributes(ostream &outfile) const
{
    outfile << "   <GROUND_POINTING_ANTENNA_BEGIN>" << NEW_LINE;
    RelayAntenna::PrintAttributes(outfile);
    outfile << "   <GROUND_POINTING_ANTENNA_STOP>" << NEW_LINE;
    return;
}


//------------------------------------------------------------------------------
//
//  Operation Name: GroundPointingAntenna::~GroundPointingAntenna
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
GroundPointingAntenna::~GroundPointingAntenna()
{
    // this is definitely the transmitting antenna, so delete all links
    if (ptrLinksList_a != NULL) {
        ptrLinksList_a->Delete();
        delete ptrLinksList_a;
        ptrLinksList_a = NULL;
    }

    return;
}
