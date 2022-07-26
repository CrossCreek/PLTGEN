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
 * TrekNode.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "TrekNode.h"


#include "general/exception/InputException.h"
#include "general/data/PositionState.h"
#include "general/math/SpaceVector.h"

TrekNode::TrekNode()
    : arrivalTimeStep_a(0), latitude_a(0.00), longitude_a(0.00), altitude_a(0.00),
      numberOfLayoverTimeSteps_a(0)
{
    return;
}

TrekNode::TrekNode(const int   &arrivalTimeStep,
                   double latitude,
                   double longitude,
                   double altitude,
                   const int  &layoverTimeSteps)
    : arrivalTimeStep_a(arrivalTimeStep), latitude_a(latitude),
      longitude_a(longitude), altitude_a(altitude),
      numberOfLayoverTimeSteps_a(layoverTimeSteps)
{
    return;
}

TrekNode::TrekNode(const TrekNode &copyMe)
    : arrivalTimeStep_a(copyMe.arrivalTimeStep_a), latitude_a(copyMe.latitude_a),
      longitude_a(copyMe.longitude_a), altitude_a(copyMe.altitude_a),
      numberOfLayoverTimeSteps_a(copyMe.numberOfLayoverTimeSteps_a)
{
    return;
}

TrekNode&
TrekNode::operator = (const TrekNode &copyMe)
{
    arrivalTimeStep_a          = copyMe.arrivalTimeStep_a;
    latitude_a                 = copyMe.latitude_a;
    longitude_a                = copyMe.longitude_a;
    altitude_a                 = copyMe.altitude_a;
    numberOfLayoverTimeSteps_a = copyMe.numberOfLayoverTimeSteps_a;
    return(*this);
}

bool
TrekNode::operator == (const TrekNode &compareMe)
{
    return( (arrivalTimeStep_a == compareMe.arrivalTimeStep_a) &&
            (latitude_a == compareMe.latitude_a) &&
            (longitude_a == compareMe.longitude_a) &&
            (altitude_a == compareMe.altitude_a) &&
            (numberOfLayoverTimeSteps_a == compareMe.numberOfLayoverTimeSteps_a) );
}

SpaceVector
TrekNode::GetPositionVector(int timeIndex,
                            REFERENCE_FRAME frame) const
{
    SpaceVector positionVector;

    if ((timeIndex < GetArrivalTimeStep()) || (timeIndex > GetDepartureTimeStep())) {
        throw new InputException(GetClassName(),
                                 "GetPositionVector(int, const REFERENCE_FRAME&)",
                                 "Invalid time index");
    }

    try {
        positionVector = SpaceVector(latitude_a, longitude_a, altitude_a,
                                     LAT_LON_ALTITUDE);

        if (frame != LAT_LON_ALTITUDE) {
            positionVector = positionVector.ConvertTo(frame, timeIndex);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetPositionVector(int, REFERENCE_FRAME)");
        throw;
    }

    return(positionVector);
}

SpaceVector
TrekNode::GetVelocityVector(int timeIndex,
                            REFERENCE_FRAME frame) const
{
    if ((timeIndex < GetArrivalTimeStep()) || (timeIndex > GetDepartureTimeStep())) {
        throw new InputException(GetClassName(),
                                 "GetVelocityVector(int, const REFERENCE_FRAME&)",
                                 "Invalid time index");
    }

    return(SpaceVector(frame));
}

PositionState
TrekNode::GetPositionState(int timeIndex,
                           REFERENCE_FRAME frame) const
{
    SpaceVector positionVector;
    SpaceVector velocityVector;

    try {
        positionVector = GetPositionVector(timeIndex, frame);
        velocityVector = GetVelocityVector(timeIndex, frame);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetPositionState(int, REFERENCE_FRAME)");
        throw;
    }

    return(PositionState(positionVector, velocityVector));
}

TrekNode::~TrekNode()
{
    return;
}
