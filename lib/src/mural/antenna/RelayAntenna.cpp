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
 * RelayAntenna.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "RelayAntenna.h"
#include "mural/element/RelaySatellite.h"
#include "mural/io/MURALInputProcessor.h"

#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"
#include "general/math/CoordinateFrame.h"
#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"


#include "mural/coorframe/AntennaCoordinateFrame.h"

//------------------------------------------------------------------------------
//
//  Operation Name: RelayAntenna::RelayAntenna
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
RelayAntenna::RelayAntenna()
    : Antenna(), numberOfTransmitChannels_a(0)
{
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: RelayAntenna::RelayAntenna
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

RelayAntenna::RelayAntenna(shared_ptr<RelaySatellite> ptrRelay,
                           int previousCapacity)
    : Antenna(ptrRelay, previousCapacity), numberOfTransmitChannels_a(0)
{
    return;
}

RelayAntenna&
RelayAntenna::operator = (const RelayAntenna &copyMe)
{
    Antenna::operator = (copyMe);
    numberOfTransmitChannels_a = copyMe.numberOfTransmitChannels_a;
    return(*this);
}

bool
RelayAntenna::operator == (const RelayAntenna &compareMe)
{
    return( (Antenna::operator == (compareMe)) &&
            (numberOfTransmitChannels_a == compareMe.numberOfTransmitChannels_a) );
}

bool
RelayAntenna::operator != (const RelayAntenna &compareMe)
{
    return(!(operator == (compareMe)));
}

/* ****************************************************************************************** */

shared_ptr<RelaySatellite>
RelayAntenna::GetRelay() const
{
    shared_ptr<RelaySatellite> ptrRelay(dynamic_pointer_cast<RelaySatellite>(ptrElement_a));
    return(ptrRelay);
}

/* ****************************************************************************************** */

void
RelayAntenna::PrintAttributes(ostream &outfile) const
{
    Antenna::PrintAttributes(outfile);

    if (numberOfTransmitChannels_a > 0) {
        outfile << "      Number Of Transmit Channels                             : ";
        outfile << numberOfTransmitChannels_a << NEW_LINE;
    }

    return;
}

/* ****************************************************************************************** */

int
RelayAntenna::GetDataProviderIndex() const
{
    return(GetRelay()->GetIndex());
}

/* ****************************************************************************************** */

int
RelayAntenna::GetDataProviderNumber() const
{
    return(GetRelay()->GetNumber());
}

/* ****************************************************************************************** */

LinkList<Link>*
RelayAntenna::GetCrosslinks(const string &userDesignator) const
{
    return(GetRelay()->GetCrosslinks(userDesignator));
}

/* ****************************************************************************************** */

void
RelayAntenna::SetNumberOfTransmitChannels(int numberOfTransmitChannels)
{
    numberOfTransmitChannels_a = numberOfTransmitChannels;
}

/* ****************************************************************************************** */

RelayAntenna::~RelayAntenna()
{
    return;
}
