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
 * PositionState.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "PositionState.h"
#include "general/exception/Exception.h"
#include "general/math/SpaceVector.h"
#include "general/utility/Constant.h"

PositionState::PositionState()
    : ptrPosition_a(new SpaceVector()), ptrVelocity_a(new SpaceVector())
{
    return;
}

/* *************************************************************************** */

PositionState::PositionState(const SpaceVector &position,
                             const SpaceVector &velocity)
    : ptrPosition_a(new SpaceVector(position)),
      ptrVelocity_a(new SpaceVector(velocity))
{
    return;
}

/* *************************************************************************** */

PositionState::PositionState(double xPosition,
                             double yPosition,
                             double zPosition,
                             double xVelocity,
                             double yVelocity,
                             double zVelocity,
                             const REFERENCE_FRAME &frame)
    : ptrPosition_a(new SpaceVector(xPosition, yPosition, zPosition, frame)),
      ptrVelocity_a(new SpaceVector(xVelocity, yVelocity, zVelocity, frame))
{
    return;
}

/* *************************************************************************** */

PositionState::PositionState(const PositionState &copyMe)
    : ptrPosition_a(NULL), ptrVelocity_a(NULL)
{
    if (copyMe.ptrPosition_a != NULL) {
        ptrPosition_a = new SpaceVector(*(copyMe.ptrPosition_a));
    }

    if (copyMe.ptrVelocity_a != NULL) {
        ptrVelocity_a = new SpaceVector(*(copyMe.ptrVelocity_a));
    }

    return;
}

/* *************************************************************************** */

PositionState&
PositionState::operator = (const PositionState &copyMe)
{
    if (copyMe.ptrPosition_a != NULL) {
        ptrPosition_a = new SpaceVector(*(copyMe.ptrPosition_a));
    }

    if (copyMe.ptrVelocity_a != NULL) {
        ptrVelocity_a = new SpaceVector(*(copyMe.ptrVelocity_a));
    }

    return(*this);
}

/* *************************************************************************** */

bool
PositionState::operator == (const PositionState &compareMe) const
{
    return( (*ptrPosition_a == *(compareMe.ptrPosition_a)) &&
            (*ptrVelocity_a == *(compareMe.ptrVelocity_a)) );
}

/* *************************************************************************** */

bool
PositionState::operator != (const PositionState &compareMe) const
{
    return(!operator == (compareMe));
}

/* *************************************************************************** */

void
PositionState::PrintPositionState(ofstream &outfile,
                                  int timeIndex) const
{
    SpaceVector positionToPrint = ptrPosition_a->GetOutputSpaceVector(timeIndex);
    SpaceVector velocityToPrint = ptrVelocity_a->GetOutputSpaceVector(timeIndex);
    outfile << " ";
    outfile << " " << setw(15) << setprecision(3) << positionToPrint.GetX();
    outfile << " " << setw(15) << setprecision(3) << positionToPrint.GetY();
    outfile << " " << setw(15) << setprecision(3) << positionToPrint.GetZ();
    outfile << " " << setw(15) << setprecision(3) << velocityToPrint.GetX();
    outfile << " " << setw(15) << setprecision(3) << velocityToPrint.GetY();
    outfile << " " << setw(15) << setprecision(3) << velocityToPrint.GetZ();
    return;
}

/* *************************************************************************** */

SpaceVector
PositionState::GetPosition(const REFERENCE_FRAME &frame,
                           int timeIndex) const
{
    SpaceVector positionVector;

    try {
        positionVector = ptrPosition_a->ConvertTo(frame, timeIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetPosition(const REFERENCE_FRAME&, int)");
        throw;
    }

    return(positionVector);
}

/* *************************************************************************** */

SpaceVector
PositionState::GetVelocity(const REFERENCE_FRAME &frame,
                           int timeIndex) const
{
    SpaceVector velocityVector;

    try {
        velocityVector = ptrVelocity_a->ConvertTo(frame, timeIndex);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetVelocity(const REFERENCE_FRAME&, int)");
        throw;
    }

    return(velocityVector);
}

/* *************************************************************************** */

SpaceVector
PositionState::GetPositionECI(double j2000Time) const
{
    SpaceVector positionVectorECI;

    try {
        positionVectorECI = ptrPosition_a->ConvertTo(EARTH_CENTER_INERTIAL, j2000Time);
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetPositionECI(double j2000Time)");
        throw;
    }

    return(positionVectorECI);
}

/* *************************************************************************** */

SpaceVector
PositionState::GetVelocityECI(double j2000Time) const
{
    SpaceVector velocityVectorECI;

    try {
        velocityVectorECI = ptrVelocity_a->ConvertTo(EARTH_CENTER_INERTIAL, j2000Time);

        // Include the angular velocity (of the earth relative to the satellite)
        // component if the base frame is ECF
        if ( ptrVelocity_a->GetReferenceFrame() == EARTH_CENTER_FIXED ) {
            SpaceVector positionECI = ptrPosition_a->ConvertTo(EARTH_CENTER_INERTIAL, j2000Time);
            SpaceVector angularVel(0.0, 0.0, EARTH_ROTATION_RATE_RAD_PER_S, EARTH_CENTER_INERTIAL);
            SpaceVector crossAngularVel = angularVel.CrossProduct(positionECI);
            velocityVectorECI += crossAngularVel;
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetVelocityECI(double j2000Time)");
        throw;
    }

    return(velocityVectorECI);
}

/* *************************************************************************** */


PositionState::~PositionState()
{
    if (ptrPosition_a != NULL) {
        delete ptrPosition_a;
        ptrPosition_a = NULL;
    }

    if (ptrVelocity_a != NULL) {
        delete ptrVelocity_a;
        ptrVelocity_a = NULL;
    }

    return;
}
