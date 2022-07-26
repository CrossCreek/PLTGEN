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
 * Revolution.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "Revolution.h"

#include "general/time/TimePiece.h"
#include "general/data/UnitData.h"

Revolution::Revolution()
    : TimePeriod(0, TimePiece::GetEndIndexTime()), revolutionNumber_a(0),
      ascendingZeroCrossingLongitude_a(UNDEFINED_ANGLE)
{
    return;
}

Revolution::Revolution(int revNumber,
                       int startRevTime,
                       int endRevTime,
                       double zeroCrossingLongitude)
    : TimePeriod(startRevTime, endRevTime), revolutionNumber_a(revNumber),
      ascendingZeroCrossingLongitude_a(zeroCrossingLongitude)
{
    return;
}

Revolution::Revolution(const Revolution &copyMe)
    : TimePeriod(copyMe), revolutionNumber_a(copyMe.revolutionNumber_a),
      ascendingZeroCrossingLongitude_a(copyMe.ascendingZeroCrossingLongitude_a)
{
    return;
}

Revolution&
Revolution::operator = (const Revolution &copyMe)
{
    TimePeriod::operator = (copyMe);
    revolutionNumber_a               = copyMe.revolutionNumber_a;
    ascendingZeroCrossingLongitude_a = copyMe.ascendingZeroCrossingLongitude_a;
    return(*this);
}

bool
Revolution::operator == (const Revolution &compareMe)
{
    return( (TimePeriod::operator == (compareMe)) &&
            (revolutionNumber_a == compareMe.revolutionNumber_a) &&
            (ascendingZeroCrossingLongitude_a == compareMe.ascendingZeroCrossingLongitude_a) );
}

bool
Revolution::operator != (const Revolution &compareMe)
{
    return(!(operator == (compareMe)));
}

Revolution::~Revolution()
{
    return;
}

