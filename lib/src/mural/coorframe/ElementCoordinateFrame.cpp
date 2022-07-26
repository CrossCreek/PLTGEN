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
 * ElementCoordinateFrame.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "ElementCoordinateFrame.h"
#include "mural/Earth.h"
#include "mural/Region.h"
#include "mural/element/Element.h"
#include "mural/modules/pg/Journey.h"

#include "general/exception/NullPointerException.h"
#include "general/time/TimePiece.h"
#include "general/math/SpaceVector.h"
#include "general/math/SimpleMath.h"
#include "general/utility/Utility.h"

ElementCoordinateFrame::ElementCoordinateFrame(const COORDINATE_FRAME_TYPE &type) :
    type_a(type), bodyTiltsForCollection_a(false), bodyTiltAngle_a(0.0)
{
    return;
}

/* **************************************************************************************** */

ElementCoordinateFrame::ElementCoordinateFrame(const COORDINATE_FRAME_TYPE &type, bool bodyTiltsForCollection) :
    type_a(type), bodyTiltsForCollection_a(bodyTiltsForCollection), bodyTiltAngle_a(0.0)
{
    return;
}

/* **************************************************************************************** */

ElementCoordinateFrame::ElementCoordinateFrame(const COORDINATE_FRAME_TYPE &type, double bodyTiltAngle) :
    type_a(type), bodyTiltsForCollection_a(false), bodyTiltAngle_a(bodyTiltAngle)
{
    return;
}

/* **************************************************************************************** */

shared_ptr<CoordinateFrame>
ElementCoordinateFrame::GetCoordinateFrame(const Element &element,
        int timeIndex,
        double additionalSeconds,
        double regionNumber)
{
    shared_ptr<CoordinateFrame> returnFrame = NULL;

    if ( type_a == USER_FRAME ) {
        returnFrame = GetUserCoordinateFrame(element, timeIndex, additionalSeconds, regionNumber);
    } else if ( type_a == RELAY_FRAME ) {
        returnFrame = GetRelayCoordinateFrame(element, timeIndex, additionalSeconds);
    } else if ( type_a == GROUND_FRAME ) {
        returnFrame = GetGroundCoordinateFrame(element, timeIndex, additionalSeconds);
    }

    return returnFrame;
}

/* **************************************************************************************** */

shared_ptr<CoordinateFrame>
ElementCoordinateFrame::GetUserCoordinateFrame(const Element &element,
        int timeIndex,
        double additionalSeconds,
        double regionNumber)
{
    const shared_ptr<Journey> ptrJourney      = element.GetJourney();
    const SpaceVector userPosition = ptrJourney->GetInterimPosition(timeIndex, additionalSeconds);
    const SpaceVector userVelocity = ptrJourney->GetInterimVelocity(timeIndex, additionalSeconds);
    SpaceVector       userAxisX;
    SpaceVector       userAxisY;
    SpaceVector       userAxisZ;
    SpaceVector       northVector;
    shared_ptr<CoordinateFrame> ptrCoordinateFrame = NULL;

    try {
        // a user's z-axis is the the body center line pointing down
        if ((bodyTiltsForCollection_a == true) && (regionNumber > 0.0)) {
            // if the user tilts to collect and is collecting a region, the Z-axis is
            // pointing at the center of that region
            userAxisZ = (Earth::GetRegion(regionNumber))->GetRegionCenter(timeIndex) - userPosition;
            // Ignoring/removing the sensor offset capability (it is currently not being used and I do not think
            // it should effect the body coordinate frame)
            /*if ((ptrSensor = GetSensorCollecting(timeIndex)) != NULL)
            {
              sensorOffsetAngle = ptrSensor->GetOffsetAngle();
            }*/
        } else {
            // else it is simply the nadir pointing vector
            userAxisZ = userPosition.Negate();
        }

        // Y-axis is perpendicular to the body center line
        if (userVelocity.GetMagnitude() > 0.0) {
            // if the user has a velocity at this time, the Y-axis is
            // also perpendicular to the velocity
            userAxisY = userAxisZ.CrossProduct(userVelocity);
        } else {
            // else Y-axis is pointing east
            northVector = SpaceVector(0.0, 0.0, 1.0);
            userAxisY = userAxisZ.CrossProduct(northVector);
        }

        // X-axis is in the direction of the velocity if this is one or
        // pointing north if no velocity
        userAxisX = userAxisY.CrossProduct(userAxisZ);
        ptrCoordinateFrame = shared_ptr<CoordinateFrame>(new CoordinateFrame(userAxisX, userAxisY, userAxisZ));
        // Ignoring the sensor offset capability (it is currently not being used (see note above))
        /*if (sensorOffsetAngle != 0.0)
        {
          ptrCoordinateFrame->EulerTransformAboutY(sensorOffsetAngle);
        }*/
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetUserCoordinateFrame(...)");
        throw;
    }

    return(ptrCoordinateFrame);
}

/* **************************************************************************************** */

shared_ptr<CoordinateFrame>
ElementCoordinateFrame::GetRelayCoordinateFrame(const Element &element,
        int timeIndex,
        double additionalSeconds)
{
    const shared_ptr<Journey> ptrJourney      = element.GetJourney();
    SpaceVector relayPosition = ptrJourney->GetInterimPosition(timeIndex, additionalSeconds);
    SpaceVector relayVelocity = ptrJourney->GetInterimVelocity(timeIndex, additionalSeconds);
    double       eccentricity = ptrJourney->GetEccentricity();
    const double inclination = ptrJourney->GetInclination();
    const double cosInclination = cos(inclination);
    const double sinInclination = sin(inclination);
    const double argumentOfPerigee = ptrJourney->GetArgumentOfPerigee();
    const double sinArgumentOfPerigee = sin(argumentOfPerigee);
    const double cosArgumentOfPerigee = cos(argumentOfPerigee);
    const double QUARTER_CIRCLE = UnitData::GetStoreQuarterCircle();
    const double HALF_CIRCLE = 2.0 * QUARTER_CIRCLE;
    const double FULL_CIRCLE = 4.0 * QUARTER_CIRCLE;
    SpaceVector  relayAxisX;
    SpaceVector  relayAxisY;
    SpaceVector  relayAxisZ;

    try {
        if (Utility::IsHighlyElliptical(eccentricity) == true) {
            // relay satellite is highly elliptical
            double amc;
            double declination;
            double rightAscension = 0.0;
            amc = SimpleMath::ArcTangent(sinArgumentOfPerigee, (cosInclination * cosArgumentOfPerigee))
                  + QUARTER_CIRCLE;
            declination = SimpleMath::ArcTangent((sinArgumentOfPerigee * cos(amc))
                                                 - (cosInclination*cosArgumentOfPerigee * sin(amc)),
                                                 (cosArgumentOfPerigee * sinInclination)) + bodyTiltAngle_a;

            while (declination > 0.00) {
                amc += HALF_CIRCLE;
                declination = SimpleMath::ArcTangent((sinArgumentOfPerigee * cos(amc))
                                                     - (cosInclination*cosArgumentOfPerigee * sin(amc)),
                                                     (cosArgumentOfPerigee * sinInclination)) + bodyTiltAngle_a;
            }

            rightAscension = relayPosition.GetLongitude() - ptrJourney->GetTrueAnomaly(timeIndex, additionalSeconds);

            while (rightAscension < 0.0) {
                rightAscension += FULL_CIRCLE;
            }

            // X-axis is parallel to the earth's polar axis (unless there is
            // tilt) pointing down the spin axis (body center line)
            // de-spun (antenna) platform is on the bottom for HEOs
            relayAxisX = SpaceVector((cos(declination) * cos(rightAscension)),
                                     (cos(declination) * sin(rightAscension)),
                                     sin(declination));
            // Z-axis is normal to the earth's polar axis
            relayAxisZ = relayAxisX.CrossProduct(relayPosition.Negate());
            // Y-axis is pointing at the earth's polar axis
            relayAxisY = relayAxisZ.CrossProduct(relayAxisX);
        } else {
            relayVelocity = ptrJourney->GetInterimVelocity(timeIndex, additionalSeconds);
            // Y-axis is pointing at the earth's center
            relayAxisY = relayPosition.Negate();
            // X-axis is normal to it's position and orbital plane and
            // is pointing up the spin axis (body center line)
            // de-spun (antenna) platform is on the top for GEOs
            relayAxisX = relayVelocity.CrossProduct(relayAxisY);
            // Z-axis is in the orbital plane
            relayAxisZ = relayAxisX.CrossProduct(relayAxisY);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "GetCoordinateFrame(int)");
        throw;
    }

    return(shared_ptr<CoordinateFrame>(new CoordinateFrame(relayAxisX, relayAxisY, relayAxisZ)));
}

/* ******************************************************************************************* */

shared_ptr<CoordinateFrame>
ElementCoordinateFrame::GetGroundCoordinateFrame(const Element &element,
        int timeIndex,
        double additionalSeconds)
{
    const shared_ptr<Journey> ptrJourney      = element.GetJourney();
    const SpaceVector northVector = SpaceVector(0.0, 0.0, Earth::GetStoreRadius());
    // Z-axis is pointing over head
    const SpaceVector groundAxisZ = ptrJourney->GetInterimPosition(timeIndex, additionalSeconds);
    // Y-axis is pointing west
    const SpaceVector groundAxisY = groundAxisZ.CrossProduct(northVector);
    // X-axis is pointing north
    const SpaceVector groundAxisX = groundAxisY.CrossProduct(groundAxisZ);
    return(shared_ptr<CoordinateFrame>(new CoordinateFrame(groundAxisX, groundAxisY, groundAxisZ)));
}

/* ****************************************************************************************** */
/**
 * Destructor
 */
ElementCoordinateFrame::~ElementCoordinateFrame()
{
    // Do nothing
}
