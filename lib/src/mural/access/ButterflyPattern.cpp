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
 * ButterflyPattern.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include <algorithm>

#include "ButterflyPattern.h"
#include "mural/Earth.h"
#include "mural/Region.h"
#include "ConeAngleData.h"

// From rageneral
#include "general/utility/Constant.h"
#include "general/array/ListIterator.h"
#include "general/math/SimpleMath.h"
#include "general/math/SpaceVector.h"
#include "general/utility/StringEditor.h"
#include "general/data/UnitData.h"
#include "general/data/InputData.h"
#include "general/data/InputDataElement.h"

ButterflyPattern::ButterflyPattern()
{
    return;
}

/* ***************************************************************************** */

void
ButterflyPattern::GetAllContainedRegions(LinkList<Region> *ptrRegionList,
        const SpaceVector &vehiclePosition,
        const SpaceVector &vehicleVelocity,
        const PATTERN &pattern,
        const string &missionString) const
{
    const int        numberOfRegionsBefore = ptrRegionList->Size();
    const double      quarterCircle = UnitData::GetStoreQuarterCircle();
    const double      semiCircle = 2.0 * quarterCircle;
    const double      earthRadius = Earth::GetStoreRadius();
    const double      userMagnitude = vehiclePosition.GetMagnitude();
    const ANGLE_UNIT  storeAngleUnits = UnitData::GetInternalAngleUnits();
    SpaceVector       vehicleToTargetLOS;
    Region      *ptrRegion = NULL;
    shared_ptr<LinkList<Region>> ptrContainedRegionsList = NULL;

    try {
        // Do the basic outer cone first
        double alpha;
        double beta;
        double maximumEarthAngle;
        double minimumEarthAngle;
        double minimumBroadside;
        double maximumBroadside;
        double maximumGroundRange;
        double radiusInDegrees;
        alpha = GetOuterConeElevationAngle(pattern, missionString) + quarterCircle;
        beta = SimpleMath::ArcSine((earthRadius * sin(alpha)) / userMagnitude);
        maximumEarthAngle = semiCircle - alpha - beta;
        maximumGroundRange = earthRadius * maximumEarthAngle;
        radiusInDegrees = SimpleMath::ConvertDistanceToDegrees(maximumGroundRange,
                          UnitData::GetInternalDistanceUnits());
        // this grabs all the Regions within the outer cone
        ptrContainedRegionsList = shared_ptr<LinkList<Region>>(
                                      GetRegionsInTheCone(vehiclePosition, radiusInDegrees, maximumEarthAngle));
        // Data for the broadside
        alpha = GetInnerConeElevationAngle(pattern, missionString) + quarterCircle;
        beta = SimpleMath::ArcSine((earthRadius * sin(alpha)) / userMagnitude);
        minimumEarthAngle = semiCircle - alpha - beta;
        minimumBroadside = broadsideAngle_a;
        maximumBroadside = semiCircle - broadsideAngle_a;
        // weed out the regions not in the butterfly
        //JOE:
        ptrRegion = ptrContainedRegionsList->Retrieve();

        while (ptrRegion != NULL) {
            double earthAngle;
            double squintAngle;
            earthAngle = ptrRegion->GetCenterEarthAngle(vehiclePosition);
            vehicleToTargetLOS = ptrRegion->GetRegionCenter(EARTH_CENTER_FIXED) - vehiclePosition;
            squintAngle = CalculateSquintAngle(vehicleToTargetLOS, vehiclePosition, vehicleVelocity);

            // check that the Region center is outside the minimum earth angle
            if (earthAngle >= minimumEarthAngle) {
                // check that the center of the Region is within the broadside region
                if ((squintAngle >= minimumBroadside) && (squintAngle <= maximumBroadside)) {
                    ptrRegionList->Append(ptrRegion);
                }
            }

            ptrRegion = ptrContainedRegionsList->Retrieve();
        }

        // if no Regions were found and the subsatellite Region is always to be added, add it
        if ( (alwaysIncludeSubSatelliteRegion_a == true) &&
                (ptrRegionList->Size() == numberOfRegionsBefore) ) {
            ptrRegion = Earth::GetRegion(vehiclePosition);
            ptrRegionList->Append(ptrRegion);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetAllContainedRegions(LinkList<Region>*, const SpaceVector&, const SpaceVector&, ...)");
        throw;
    }

    return;
}

/* ***************************************************************************** */

bool
ButterflyPattern::InsideAccessPattern(const string &missionString,
                                      const SpaceVector &regionPosition,
                                      const SpaceVector &vehiclePosition,
                                      const SpaceVector &vehicleVelocity) const
{
    const double quarterCircle = UnitData::GetStoreQuarterCircle();
    bool         insideAccessPattern = MissionSpecificEqualsLargest(missionString);
    SpaceVector  regionCenterToVehicle = vehiclePosition - regionPosition;

    if (insideAccessPattern == false) {
        double elevationAngle;
        elevationAngle = (regionPosition.Negate()).GetSeparationAngle(regionCenterToVehicle) - quarterCircle;

        if (elevationAngle >= GetOuterConeElevationAngle(MISSION_SPECIFIC, missionString)) {
            double innerElevationAngle;
            innerElevationAngle = GetInnerConeElevationAngle(MISSION_SPECIFIC, missionString);

            if ( innerElevationAngle < GetInnerConeElevationAngle(LARGEST_PATTERN) ) {
                if (elevationAngle <= innerElevationAngle) {
                    insideAccessPattern = true;
                }
            } else {
                insideAccessPattern = true;
            }
        }

        // if not inside the access pattern but the sub-satellite region is always to
        // be included, check if this is the sub-satellite region

        if ((insideAccessPattern == false) && (alwaysIncludeSubSatelliteRegion_a == true)) {
            if (Earth::GetRegion(regionPosition) == Earth::GetRegion(vehiclePosition)) {
                insideAccessPattern = true;
            }
        }
    }

    return(insideAccessPattern);
}

/* ***************************************************************************** */

void
ButterflyPattern::SetBroadSideAngle(double broadsideAngle)
{
    broadsideAngle_a = broadsideAngle;
}

/* ***************************************************************************** */

double
ButterflyPattern::GetBroadSideAngle() const
{
    return broadsideAngle_a;
}

/* ***************************************************************************** */

ButterflyPattern::~ButterflyPattern()
{
    return;
}
