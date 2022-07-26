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
 * UserAntenna.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "UserAntenna.h"
#include "mural/element/UserVehicle.h"
#include "mural/io/MURALInputProcessor.h"

#include "general/array/ListIterator.h"
#include "general/math/SimpleMath.h"
#include "general/math/CoordinateFrame.h"
#include "general/utility/StringEditor.h"

#include "mural/sensor/Sensor.h"
#include "mural/coorframe/AntennaCoordinateFrame.h"

//#include "optimizer/Constraint.h"

//------------------------------------------------------------------------------
//
//  Operation Name: UserAntenna::UserAntenna
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
UserAntenna::UserAntenna()
    : Antenna()
{
    return;
}

//------------------------------------------------------------------------------
//
//  Operation Name: UserAntenna::UserAntenna
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

UserAntenna::UserAntenna(shared_ptr<UserVehicle> ptrUser,
                         int previousCapacity)
    : Antenna(ptrUser, previousCapacity)
{
    return;
}

UserAntenna&
UserAntenna::operator = (const UserAntenna &copyMe)
{
    Antenna::operator = (copyMe);
    return(*this);
}

bool
UserAntenna::operator == (const UserAntenna &compareMe)
{
    return( (Antenna::operator == (compareMe)) );
}

bool
UserAntenna::operator != (const UserAntenna &compareMe)
{
    return(!(operator == (compareMe)));
}

void
UserAntenna::PrintAttributes(ostream &outfile) const
{
    Antenna::PrintAttributes(outfile);
    return;
}

/* ****************************************************************************************** */

shared_ptr<UserVehicle>
UserAntenna::GetUser() const
{
    shared_ptr<UserVehicle> ptrUser(dynamic_pointer_cast<UserVehicle>(ptrElement_a));
    return(ptrUser);
}

/* ****************************************************************************************** */

int
UserAntenna::GetNumberOfAssetsModeled() const
{
    return(GetUser()->GetNumberOfVehiclesModeled());
}

int
UserAntenna::GetDataProviderIndex() const
{
    return(GetUser()->GetIndex());
}

int
UserAntenna::GetDataProviderNumber() const
{
    return(GetUser()->GetNumber());
}

double
UserAntenna::GetCollectionDataRate(int timeIndex) const
{
    return(GetUser()->GetMaximumCollectionDataRate(timeIndex));
}

double
UserAntenna::GetMaximumCollectionDataRate() const
{
    return(GetUser()->GetMaximumSensorDataRate());
}

double
UserAntenna::GetTransmitDataRate(int timeIndex, int resourceIndex) const
{
    Sensor* ptrCollectingSensor;
    double sensorDataRate       = 0.0;
    ptrCollectingSensor = GetUser()->GetSensorCollecting(timeIndex, resourceIndex);

    if ( ptrCollectingSensor != NULL ) {
        sensorDataRate      = ptrCollectingSensor->GetDataRate();
    }

    return(sensorDataRate);
}

//------------------------------------------------------------------------------
//
//  Operation Name: UserAntenna::~UserAntenna
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
UserAntenna::~UserAntenna()
{
    return;
}
