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
 * SelfRelayAntenna.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "SelfRelayAntenna.h"
#include "mural/element/UserVehicle.h"
#include "mural/io/MURALInputProcessor.h"

#include "general/utility/StringEditor.h"

//------------------------------------------------------------------------------
//
//  Operation Name: SelfRelayAntenna::SelfRelayAntenna
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
SelfRelayAntenna::SelfRelayAntenna()
    : UserAntenna(),
      ptrTransmitLinkArray_a(NULL)
{
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: SelfRelayAntenna::SelfRelayAntenna
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

SelfRelayAntenna::SelfRelayAntenna(shared_ptr<UserVehicle> ptrUser)
    : UserAntenna(ptrUser), ptrTransmitLinkArray_a(NULL)
{
    return;
}

SelfRelayAntenna&
SelfRelayAntenna::operator = (const SelfRelayAntenna &copyMe)
{
    UserAntenna::operator = (copyMe);
    antennaCapability_a = copyMe.antennaCapability_a;
    return(*this);
}

bool
SelfRelayAntenna::operator == (const SelfRelayAntenna &compareMe)
{
    return( (UserAntenna::operator == (compareMe)) &&
            (antennaCapability_a == compareMe.antennaCapability_a) );
}

bool
SelfRelayAntenna::operator != (const SelfRelayAntenna &compareMe)
{
    return(!(operator == (compareMe)));
}

void
SelfRelayAntenna::PrintAttributes(ostream &outfile) const
{
    outfile << "   <SELF_RELAY_ANTENNA_BEGIN>" << NEW_LINE;
    outfile << "      Self Relay Antenna Capability (TRANS/RECV/DUPLEX)       : "
            << GetAntennaCapabilityString(antennaCapability_a) << NEW_LINE;
    UserAntenna::PrintAttributes(outfile);
    outfile << "   <SELF_RELAY_ANTENNA_STOP>" << NEW_LINE;
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: SelfRelayAntenna::~SelfRelayAntenna
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
SelfRelayAntenna::~SelfRelayAntenna()
{
    Link *ptrSelfRelayLink = NULL;

    if (ptrLinksList_a != NULL) {
        if (antennaCapability_a == TRANSMIT) {
            // this is definitely the transmitting antenna, so delete all links
            ptrLinksList_a->Delete();
        } else if (antennaCapability_a == RECEIVE) {
            // this is definitely the receiving antenna, so only remove (don't delete) all links
            ptrLinksList_a->RemoveAll();
        } else {
            // this is a DUPLEX antenna, so only delete the links
            // that are being transmitted from this antenna
            int linkIndex = 0;
            //JOE:
            ptrSelfRelayLink = ptrLinksList_a->Retrieve();

            while (ptrSelfRelayLink != NULL) {
                if (ptrTransmitLinkArray_a->ElementAt(linkIndex) == true) {
                    delete ptrSelfRelayLink;
                }

                ptrSelfRelayLink = NULL;
                ++linkIndex;
                ptrSelfRelayLink = ptrLinksList_a->Retrieve();
            }

            delete ptrTransmitLinkArray_a;
            ptrTransmitLinkArray_a = NULL;
        }

        delete ptrLinksList_a;
        ptrLinksList_a = NULL;
    }

    return;
}
