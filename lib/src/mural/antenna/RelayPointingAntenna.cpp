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
 * RelayPointingAntenna.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "RelayPointingAntenna.h"
#include "mural/element/RelaySatellite.h"

#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"
#include "general/utility/StringEditor.h"

//------------------------------------------------------------------------------
//
//  Operation Name: RelayPointingAntenna::RelayPointingAntenna
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
RelayPointingAntenna::RelayPointingAntenna()
    : RelayAntenna(), antennaCapability_a(UNKNOWN_CAPABILITY),
      ptrTransmitLinkArray_a(NULL)
{
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: RelayPointingAntenna::RelayPointingAntenna
//
//  Purpose:
//     This is the alternate constructor
//
//  Calling Operation:
//
//  Calling Parameters
//  Parameters            i/o/r  description
//  ----------            -----  -----------
//  ptrInputDataElement*    i    input data to create the Antenna (parent) class
//  ptrRelay*               i    pointer the relay satellite creating this
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

RelayPointingAntenna::RelayPointingAntenna(shared_ptr<RelaySatellite> ptrRelay)
    : RelayAntenna(ptrRelay), antennaCapability_a(UNKNOWN_CAPABILITY),
      ptrTransmitLinkArray_a(NULL)
{
    return;
}

RelayPointingAntenna&
RelayPointingAntenna::operator = (const RelayPointingAntenna &copyMe)
{
    RelayAntenna::operator = (copyMe);
    antennaCapability_a = copyMe.antennaCapability_a;

    if (copyMe.ptrTransmitLinkArray_a != NULL) {
        ptrTransmitLinkArray_a = new Array<bool>(*(copyMe.ptrTransmitLinkArray_a));
    }

    return(*this);
}

bool
RelayPointingAntenna::operator == (const RelayPointingAntenna &compareMe)
{
    return( (RelayAntenna::operator == (compareMe)) &&
            (antennaCapability_a == compareMe.antennaCapability_a) );
}

bool
RelayPointingAntenna::operator != (const RelayPointingAntenna &compareMe)
{
    return(!(operator == (compareMe)));
}

void
RelayPointingAntenna::PrintAttributes(ostream &outfile) const
{
    outfile << "   <RELAY_POINTING_ANTENNA_BEGIN>" << NEW_LINE;
    outfile << "      Relay Pointing Antenna Capability (TRANS/RECV/DUPLEX)   : "
            << GetAntennaCapabilityString(antennaCapability_a) << NEW_LINE;
    RelayAntenna::PrintAttributes(outfile);
    outfile << "   <RELAY_POINTING_ANTENNA_STOP>" << NEW_LINE;
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: RelayPointingAntenna::~RelayPointingAntenna
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
RelayPointingAntenna::~RelayPointingAntenna()
{
    if (ptrLinksList_a != NULL) {
        int  linkIndex = 0;
        Link *ptrMultiHopLink = NULL;

        if (antennaCapability_a == TRANSMIT) {
            // this is definitely the transmitting antenna, so delete all links
            ptrLinksList_a->Delete();
        } else if (antennaCapability_a == RECEIVE) {
            // this is definitely the receiving antenna, so only remove (don't delete) all links
            ptrLinksList_a->RemoveAll();
        } else {
            // this is a DUPLEX antenna, so only delete the links
            // that are being transmitted from this antenna
            //JOE:
            ptrMultiHopLink = ptrLinksList_a->Retrieve();

            while (ptrMultiHopLink != NULL) {
                if (ptrTransmitLinkArray_a->ElementAt(linkIndex) == true) {
                    delete ptrMultiHopLink;
                }

                ptrMultiHopLink = NULL;
                ++linkIndex;
                ptrMultiHopLink = ptrLinksList_a->Retrieve();
            }

            delete ptrTransmitLinkArray_a;
            ptrTransmitLinkArray_a = NULL;
        }

        delete ptrLinksList_a;
        ptrLinksList_a = NULL;
    }

    return;
}
