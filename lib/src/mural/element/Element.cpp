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
 * Element.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "Element.h"
#include "mural/coorframe/ElementCoordinateFrame.h"

#include "general/math/CoordinateFrame.h"
#include "general/math/SpaceVector.h"

Element::Element(const ELEMENT_TYPE &elementType) : elementType_a(elementType), ptrDesignator_a(NULL),   ptrJourney_a(NULL), ptrCoordinateFrame_a(NULL)
{
    return;
}

Element::Element(const Element &copyMe)
    : elementType_a(copyMe.elementType_a),
      ptrDesignator_a(copyMe.ptrDesignator_a),
      ptrJourney_a(copyMe.ptrJourney_a),
      ptrCoordinateFrame_a(copyMe.ptrCoordinateFrame_a)
{
}

/* ************************************************************************************* */

Element::ELEMENT_TYPE
Element::GetElementType() const
{
    return elementType_a;
}

/* ************************************************************************************* */

string
Element::GetDesignator() const
{
    return (*ptrDesignator_a);
}

/* ************************************************************************************* */

shared_ptr<Journey>
Element::GetJourney() const
{
    return (ptrJourney_a);
}

/* ************************************************************************************* */

double
Element::GetLatitude(int timeIndex) const
{
    return(ptrJourney_a->GetLatitude(timeIndex));
}

/* ************************************************************************************* */

double
Element::GetLongitude(int timeIndex) const
{
    return(ptrJourney_a->GetLongitude(timeIndex));
}

/* ************************************************************************************* */

double
Element::GetAltitude(int timeIndex) const
{
    return(ptrJourney_a->GetAltitude(timeIndex));
}

/* ************************************************************************************* */

double
Element::GetSemiMajorAxis() const
{
    return(ptrJourney_a->GetSemiMajorAxis());
}

/* ************************************************************************************* */

double
Element::GetEccentricity() const
{
    return(ptrJourney_a->GetEccentricity());
}

/* ************************************************************************************* */

double
Element::GetInclination() const
{
    return(ptrJourney_a->GetInclination());
}

/* ************************************************************************************* */

double
Element::GetRightAscension() const
{
    return(ptrJourney_a->GetRightAscension());
}

/* ************************************************************************************* */

double
Element::GetArgumentOfPerigee() const
{
    return(ptrJourney_a->GetArgumentOfPerigee());
}

/* ************************************************************************************* */

double
Element::GetTrueAnomaly() const
{
    return(ptrJourney_a->GetTrueAnomaly());
}

/* ************************************************************************************* */

double
Element::GetTrueAnomaly(int timeIndex,
                        double additionalSeconds) const
{
    return(ptrJourney_a->GetTrueAnomaly(timeIndex, additionalSeconds));
}

/* ************************************************************************************* */

double
Element::GetNodalPrecessionRate() const
{
    return(ptrJourney_a->GetNodalPrecessionRate());
}

/* ************************************************************************************* */

double
Element::GetApsidalRotationRate() const
{
    return(ptrJourney_a->GetApsidalRotationRate());
}

/* ************************************************************************************* */

double
Element::GetAnomalisticMeanMotion() const
{
    return(ptrJourney_a->GetAnomalisticMeanMotion());
}

/* ************************************************************************************* */

double
Element::GetSemiLatusRectum() const
{
    return(ptrJourney_a->GetSemiLatusRectum());
}

/* ************************************************************************************* */

int
Element::GetRevNumber(int timeIndex) const
{
    return(ptrJourney_a->GetRevolutionNumber(timeIndex));
}

/* ************************************************************************************* */

int
Element::GetRevIndex(int timeIndex) const
{
    return(ptrJourney_a->GetRevolutionIndex(timeIndex));
}

/* ************************************************************************************* */

int
Element::GetNumberOfRevs() const
{
    return(ptrJourney_a->GetNumberOfRevs());
}

/* ************************************************************************************* */

int
Element::GetOrbitPeriod() const
{
    return(ptrJourney_a->GetOrbitPeriod());
}

/* ************************************************************************************* */

int
Element::GetRevStartTime(int revIndex) const
{
    return(ptrJourney_a->GetRevStartTime(revIndex));
}

/* ************************************************************************************* */

int
Element::GetRevEndTime(int revIndex) const
{
    return(ptrJourney_a->GetRevEndTime(revIndex));
}

/* ************************************************************************************* */

bool
Element::UsesSouthPointBoundary() const
{
    return(ptrJourney_a->UsesSouthPointBoundary());
}

/* ************************************************************************************* */

SpaceVector
Element::GetPosition(int timeIndex) const
{
    return(ptrJourney_a->GetPosition(timeIndex));
}

/* ************************************************************************************* */

SpaceVector
Element::GetPosition(int timeIndex,
                     const REFERENCE_FRAME &returnFrame) const
{
    return(ptrJourney_a->GetPosition(timeIndex, returnFrame));
}

/* ************************************************************************************* */

SpaceVector
Element::GetInterimPosition(int timeIndex,
                            double additionalSeconds) const
{
    return(ptrJourney_a->GetInterimPosition(timeIndex, additionalSeconds));
}

/* ************************************************************************************* */

SpaceVector
Element::GetVelocity(int timeIndex) const
{
    return(ptrJourney_a->GetVelocity(timeIndex));
}

/* ************************************************************************************* */

SpaceVector
Element::GetVelocity(int timeIndex,
                     const REFERENCE_FRAME &returnFrame) const
{
    return(ptrJourney_a->GetVelocity(timeIndex, returnFrame));
}

/* ************************************************************************************* */

SpaceVector
Element::GetInterimVelocity(int timeIndex,
                            double additionalSeconds) const
{
    return(ptrJourney_a->GetInterimVelocity(timeIndex, additionalSeconds));
}

/* ************************************************************************************* */

shared_ptr<CoordinateFrame>
Element::GetCoordinateFrame(int timeIndex,
                            double additionalSeconds,
                            double regionNumber) const
{
    shared_ptr<CoordinateFrame> ptrCoordinateFrame = ptrCoordinateFrame_a->GetCoordinateFrame(
                *this, timeIndex, additionalSeconds, regionNumber);
    return (ptrCoordinateFrame);
}

/* ************************************************************************************* */

void
Element::SetCoordinateFrame(shared_ptr<ElementCoordinateFrame> ptrCoordinateFrame)
{
    ptrCoordinateFrame_a = ptrCoordinateFrame;
}

/* ************************************************************************************* */

void
Element::SetJourney(shared_ptr<Journey> ptrJourney)
{
    ptrJourney_a = ptrJourney;
}

/* ************************************************************************************* */

void
Element::SetDesignator(shared_ptr<string> ptrDesignator)
{
    ptrDesignator_a = ptrDesignator;
}

/* ************************************************************************************* */

Element::~Element()
{
    return;
}

