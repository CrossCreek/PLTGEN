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
 * AzimuthAngleData.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "AzimuthAngleData.h"


#include "general/utility/Constant.h"

bool AzimuthAngleData::useAzimuthConstraint_s = false;

AzimuthAngleData::AzimuthAngleData()
    : minimumElevationAngle_a(UNDEFINED_ANGLE),
      maximumElevationAngle_a(UNDEFINED_ANGLE),
      minimumAzimuthAngle_a(UNDEFINED_ANGLE),
      maximumAzimuthAngle_a(UNDEFINED_ANGLE)
{
    return;
}

AzimuthAngleData::AzimuthAngleData(double minimumElevation,
                                   double maximumElevation,
                                   double minimumAzimuthAngle,
                                   double maximumAzimuthAngle)
    : minimumElevationAngle_a(minimumElevation),
      maximumElevationAngle_a(maximumElevation),
      minimumAzimuthAngle_a(minimumAzimuthAngle),
      maximumAzimuthAngle_a(maximumAzimuthAngle)
{
    useAzimuthConstraint_s = true;

    if(maximumAzimuthAngle_a == UNDEFINED_ANGLE) {
        maximumAzimuthAngle_a = minimumAzimuthAngle_a;
    }
}

AzimuthAngleData::AzimuthAngleData(const AzimuthAngleData &copyMe)
    : minimumElevationAngle_a(copyMe.minimumElevationAngle_a),
      maximumElevationAngle_a(copyMe.maximumElevationAngle_a),
      minimumAzimuthAngle_a(copyMe.minimumAzimuthAngle_a),
      maximumAzimuthAngle_a(copyMe.maximumAzimuthAngle_a)
{
}

AzimuthAngleData&
AzimuthAngleData::operator = (const AzimuthAngleData &copyMe)
{
    minimumElevationAngle_a = copyMe.minimumElevationAngle_a;
    maximumElevationAngle_a = copyMe.maximumElevationAngle_a;
    minimumAzimuthAngle_a   = copyMe.minimumAzimuthAngle_a;
    maximumAzimuthAngle_a   = copyMe.maximumAzimuthAngle_a;
    return(*this);
}

bool
AzimuthAngleData::operator == (const AzimuthAngleData &compareMe) const
{
    return( (minimumElevationAngle_a == compareMe.minimumElevationAngle_a) &&
            (maximumElevationAngle_a == compareMe.maximumElevationAngle_a) &&
            (minimumAzimuthAngle_a == compareMe.minimumAzimuthAngle_a) &&
            (maximumAzimuthAngle_a == compareMe.maximumAzimuthAngle_a));
}

AzimuthAngleData::~AzimuthAngleData()
{
}
