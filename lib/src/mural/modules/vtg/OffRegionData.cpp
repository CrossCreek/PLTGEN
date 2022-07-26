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
 * OffRegionData.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "OffRegionData.h"
#include "mural/io/MURALInputProcessor.h"

//------------------------------------------------------------------------------
//
//  Method Name: OffRegionData::OffRegionData
//
//  Purpose:
//    Default constructor for the OffRegionData class.
//
//  Calling Method:
//     OffRegionData();
//
//  Calling Parameters
//  Parameters      i/o  description
//  ----------      ---  -----------
//
//  Limitations:
//     None
//
//------------------------------------------------------------------------------

OffRegionData::OffRegionData()
    : latitude_a(0.0), startLongitude_a(0.0), endLongitude_a(0.0)
{
    return;
}

OffRegionData::OffRegionData(double latitude,
                             double startLongitude,
                             double endLongitude)
    : latitude_a(latitude), startLongitude_a(startLongitude),
      endLongitude_a(endLongitude)
{
    if (startLongitude_a >= endLongitude_a) {
        throw new InputException(GetClassName(),
                                 "OffRegionData(const double&, const double&, const double&)",
                                 "Start longitude exceeds end longitude");
    }

    return;
}

OffRegionData::OffRegionData(const OffRegionData &copyMe)
    : latitude_a(copyMe.latitude_a), startLongitude_a(copyMe.startLongitude_a),
      endLongitude_a(copyMe.endLongitude_a)
{
    return;
}

OffRegionData&
OffRegionData::operator = (const OffRegionData &copyMe)
{
    latitude_a       = copyMe.latitude_a;
    startLongitude_a = copyMe.startLongitude_a;
    endLongitude_a   = copyMe.endLongitude_a;
    return(*this);
}

bool
OffRegionData::operator == (const OffRegionData &compareMe) const
{
    return( (latitude_a  == compareMe.latitude_a) &&
            (startLongitude_a  == compareMe.startLongitude_a) &&
            (endLongitude_a  == compareMe.endLongitude_a) );
}

bool
OffRegionData::operator != (const OffRegionData &compareMe) const
{
    return(!(operator == (compareMe)));
}

OffRegionData::~OffRegionData()
{
    return;
}
