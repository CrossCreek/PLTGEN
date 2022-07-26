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
 * AntennaCoordinateFrame.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iomanip>

#include "AntennaCoordinateFrame.h"
#include "mural/element/Element.h"

#include "general/exception/NullPointerException.h"
#include "general/math/CoordinateFrame.h"
#include "general/math/SimpleMath.h"

AntennaCoordinateFrame::AntennaCoordinateFrame(const shared_ptr<Element> &ptrElement,
        double platformOffsetAngle, double gimbalFrameOffsetAngle, double rollOffsetAngle) :
    ptrElement_a(ptrElement), platformOffsetAngle_a(platformOffsetAngle),
    gimbalFrameOffsetAngle_a(gimbalFrameOffsetAngle), rollOffsetAngle_a(rollOffsetAngle)
{
    return;
}

AntennaCoordinateFrame::AntennaCoordinateFrame(const shared_ptr<Element> &ptrElement) : ptrElement_a(ptrElement),
    platformOffsetAngle_a(0.0), gimbalFrameOffsetAngle_a(0.0), rollOffsetAngle_a(0.0)
{
    return;
}

/* ********************************************************************************* */

shared_ptr<CoordinateFrame>
AntennaCoordinateFrame::GetAntennaCoordinateFrame(int timeIndex,
        double additionalSeconds,
        double regionNumber,
        double providerAzimuthOffsetAngle) const
{
    const double     totalAzimuthOffset = platformOffsetAngle_a + providerAzimuthOffsetAngle;
    shared_ptr<CoordinateFrame> ptrFrame = ptrElement_a->GetCoordinateFrame(timeIndex, additionalSeconds, regionNumber);

    if (ptrFrame == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "GetAntennaCoordinateFrame(int, const double, double, double)",
                                       "ptrFrame");
    }

    try {
        if (totalAzimuthOffset != 0.0) {
            ptrFrame->EulerTransformAboutZ(totalAzimuthOffset);
        }

        if (gimbalFrameOffsetAngle_a != 0.0) {
            ptrFrame->EulerTransformAboutY(gimbalFrameOffsetAngle_a);
        }

        if (rollOffsetAngle_a != 0.0) {
            ptrFrame->EulerTransformAboutX(rollOffsetAngle_a);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "GetAntennaCoordinateFrame(int, const double, double, double)");
        throw;
    }

    return(ptrFrame);
}

/* **************************************************************************************** */

void
AntennaCoordinateFrame::DetermineAzElAngles(int timeIndex,
        const SpaceVector &lineOfSightVector,
        double additionalSeconds,
        double &azimuthAngle,
        double &elevationAngle,
        double regionNumber,
        double azimuthOffsetAngle) const
{
    SpaceVector      unitLOS = lineOfSightVector.GetUnitVector();
    SpaceVector      transferredLOS;
    shared_ptr<CoordinateFrame> ptrFrame = NULL;

    try {
        // This commented out logic was from the From old::GetProviderCoordinateFrame() to try and get
        // the imaged region.  It seems to have no effect because the imaged region is passed as
        // an argument.  I'm leaving as a reminder (for now)
        /*double useRegionNumber = regionNumber;
        if (useRegionNumber == 0.0)
        {
          useRegionNumber = ptrUserVehicle_a->GetImagedRegion(timeIndex);
        }*/
        ptrFrame = GetAntennaCoordinateFrame(timeIndex, additionalSeconds,
                                             regionNumber, azimuthOffsetAngle);
        transferredLOS = unitLOS.TransformCoordinates(ptrFrame->GetXAxis(),
                         ptrFrame->GetYAxis(),
                         ptrFrame->GetZAxis());

        // Compute the az/el angles based on the element type
        if ( ptrElement_a->GetElementType() == Element::USER_VEHICLE ) {
            DetermineAzElAnglesForUser(*ptrFrame, unitLOS, transferredLOS, azimuthAngle, elevationAngle);
        } else if ( ptrElement_a->GetElementType() == Element::RELAY_SATELLITE ) {
            DetermineAzElAnglesForRelay(*ptrFrame, unitLOS, transferredLOS, azimuthAngle, elevationAngle);
        } else if ( ptrElement_a->GetElementType() == Element::GROUND_STATION ) {
            DetermineAzElAnglesForGround(*ptrFrame, unitLOS, transferredLOS, azimuthAngle, elevationAngle);
        }
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "DetermineAzElAngles(int, const SpaceVector&, ...)");
        throw;
    }

    return;
}

/* **************************************************************************************** */

void
AntennaCoordinateFrame::DetermineAzElAnglesForUser(const CoordinateFrame &frame,
        const SpaceVector &unitLOS,
        const SpaceVector &transferredLOS,
        double &azimuthAngle,
        double &elevationAngle) const
{
    try {
        // azimuth is measured from the X-axis in the X-Y plane
        azimuthAngle = SimpleMath::ArcTangent(transferredLOS.GetY(), transferredLOS.GetX());
        // elevation is from the X-Y plane, above the vehicle (where comm antenna is
        // located) is positive and below is negative
        elevationAngle = unitLOS.GetSeparationAngle(frame.GetZAxis()) - UnitData::GetStoreQuarterCircle();
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "DetermineAzElAngles(int, const SpaceVector&, ...)");
        throw;
    }

    return;
}

/* **************************************************************************************** */

void
AntennaCoordinateFrame::DetermineAzElAnglesForRelay(const CoordinateFrame &frame,
        const SpaceVector &unitLOS,
        const SpaceVector &transferredLOS,
        double &azimuthAngle,
        double &elevationAngle) const
{
    try {
        // azimuth is measured from the Y-axis in the Y-Z plane
        azimuthAngle = SimpleMath::ArcTangent(transferredLOS.GetZ(), transferredLOS.GetY());
        // elevation is from the Y-Z plane
        elevationAngle = UnitData::GetStoreQuarterCircle() - unitLOS.GetSeparationAngle(frame.GetXAxis());
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "DetermineAzElAngles(int, const SpaceVector&, ...)");
        throw;
    }

    return;
}

/* **************************************************************************************** */

void
AntennaCoordinateFrame::DetermineAzElAnglesForGround(const CoordinateFrame &frame,
        const SpaceVector &unitLOS,
        const SpaceVector &transferredLOS,
        double &azimuthAngle,
        double &elevationAngle) const
{
    try {
        // azimuth is measured from the X-axis (pointing north) in the X-Y plane
        azimuthAngle = SimpleMath::ArcTangent(transferredLOS.GetY(), transferredLOS.GetX());
        // elevation is from the X-Y plane, directly overhead is 90 degrees and the
        // horizon is 0 degrees
        elevationAngle = UnitData::GetStoreQuarterCircle() - unitLOS.GetSeparationAngle(frame.GetZAxis());
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(),
                            "DetermineAzElAngles(int, const SpaceVector&, ...)");
        throw;
    }

    return;
}

/* **************************************************************************************** */

const Element& AntennaCoordinateFrame::GetElement() const
{
    return (*ptrElement_a);
}

/* **************************************************************************************** */

double
AntennaCoordinateFrame::GetPlatformOffsetAngle() const
{
    return platformOffsetAngle_a;
}

/* **************************************************************************************** */

double AntennaCoordinateFrame::GetGimbalFrameOffsetAngle() const
{
    return gimbalFrameOffsetAngle_a;
}

/* **************************************************************************************** */

double AntennaCoordinateFrame::GetRollOffsetAngle() const
{
    return rollOffsetAngle_a;
}

/* **************************************************************************************** */

void AntennaCoordinateFrame::SetPlatformOffsetAngle(double platformOffsetAngle)
{
    platformOffsetAngle_a = platformOffsetAngle;
}

/* **************************************************************************************** */

void AntennaCoordinateFrame::SetGimbalFrameOffsetAngle(double gimbalFrameOffsetAngle)
{
    gimbalFrameOffsetAngle_a = gimbalFrameOffsetAngle;
}

/* **************************************************************************************** */

void AntennaCoordinateFrame::SetRollOffsetAngle(double rollOffsetAngle)
{
    rollOffsetAngle_a = rollOffsetAngle;
}

AntennaCoordinateFrame::~AntennaCoordinateFrame()
{
    // Do nothing
}
