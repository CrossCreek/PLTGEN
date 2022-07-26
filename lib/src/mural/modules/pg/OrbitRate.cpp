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
 * OrbitRate.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


static char sccsid [] = "@(#)OrbitRate.cpp  1.10 00/11/03";

#include "OrbitRate.h"

const string OrbitRate::CLASS_NAME_s = "OrbitRate";

OrbitRate::OrbitRate()
    : nodalPrecessionRate_a(0.00), apsidalRotationRate_a(0.00),
      anomalisticMeanMotion_a(0.00)
{
    return;
}

OrbitRate::OrbitRate(const double &nodalPrecessionRate,
                     const double &apsidalRotationRate,
                     const double &anomalisticMeanMotion)
    : nodalPrecessionRate_a(nodalPrecessionRate),
      apsidalRotationRate_a(apsidalRotationRate),
      anomalisticMeanMotion_a(anomalisticMeanMotion)
{
    return;
}

OrbitRate::OrbitRate(const OrbitRate &copyMe)
    : nodalPrecessionRate_a(copyMe.nodalPrecessionRate_a),
      apsidalRotationRate_a(copyMe.apsidalRotationRate_a),
      anomalisticMeanMotion_a(copyMe.anomalisticMeanMotion_a)
{
    return;
}

bool
OrbitRate::operator == (const OrbitRate &compareMe) const
{
    return( (nodalPrecessionRate_a == compareMe.nodalPrecessionRate_a) &&
            (apsidalRotationRate_a == compareMe.apsidalRotationRate_a) &&
            (anomalisticMeanMotion_a == compareMe.anomalisticMeanMotion_a) );
}

bool
OrbitRate::operator != (const OrbitRate &compareMe) const
{
    return(!(operator == (compareMe)));
}

OrbitRate::~OrbitRate()
{
    return;
}
