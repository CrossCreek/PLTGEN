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
 * AntennaBase.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "AntennaBase.h"
#include "mural/coorframe/AntennaCoordinateFrame.h"
#include "mural/element/DataProvider.h"


#include "general/math/CoordinateFrame.h"
#include "general/exception/NullPointerException.h"

AntennaBase::AntennaBase() :
    ptrElement_a(NULL), ptrAntennaDesignator_a(new string(DEFAULT_STRING)),
    antennaCapability_a(UNKNOWN_CAPABILITY)
{
    return;
}

AntennaBase::AntennaBase(shared_ptr<Element> ptrElement) :
    ptrElement_a(ptrElement), ptrAntennaDesignator_a(new string(DEFAULT_STRING)),
    antennaCapability_a(UNKNOWN_CAPABILITY)
{
    return;
}


/* ************************************************************************************* */

string
AntennaBase::GetAntennaDesignator() const
{
    return (*ptrAntennaDesignator_a);
}

/* ************************************************************************************* */

shared_ptr<Element>
AntennaBase::GetElement() const
{
    return (ptrElement_a);
}

/* ************************************************************************************* */

shared_ptr<CoordinateFrame>
AntennaBase::GetAntennaCoordinateFrame(int timeIndex,
                                       double additionalSeconds,
                                       double regionNumber,
                                       double providerAzimuthOffsetAngle) const
{
    if (ptrAntennaCoordinateFrame_a == NULL) {
        throw new NullPointerException(GetClassName(),
                                       "GetAntennaCoordinateFrame(int, const double, double, double)",
                                       "ptrFrame");
    }

    shared_ptr<CoordinateFrame> ptrFrame = ptrAntennaCoordinateFrame_a->GetAntennaCoordinateFrame(timeIndex,
                                           additionalSeconds,
                                           regionNumber,
                                           providerAzimuthOffsetAngle);
    return(ptrFrame);
}

/* ************************************************************************************* */

shared_ptr<AntennaCoordinateFrame>
AntennaBase::GetAntennaCoordinateFrame() const
{
    return ptrAntennaCoordinateFrame_a;
}

/* ************************************************************************************* */

void
AntennaBase::DetermineAzElAngles(int timeIndex,
                                 const SpaceVector &lineOfSightVector,
                                 double additionalSeconds,
                                 double &azimuthAngle,
                                 double &elevationAngle,
                                 double regionNumber,
                                 double azimuthOffsetAngle) const
{
    ptrAntennaCoordinateFrame_a->DetermineAzElAngles(timeIndex,
            lineOfSightVector, additionalSeconds, azimuthAngle,
            elevationAngle, regionNumber, azimuthOffsetAngle);
    return;
}

/* ************************************************************************************* */

string
AntennaBase::GetDataProviderDesignator() const
{
    return(ptrElement_a->GetDesignator());
}

/* ************************************************************************************* */

AntennaBase::ANTENNA_CAPABILITY
AntennaBase::GetAntennaCapability() const
{
    return antennaCapability_a;
}

/* ************************************************************************************* */

string
AntennaBase::GetAntennaCapabilityString(const ANTENNA_CAPABILITY &capability) const
{
    string returnString = "UNKNOWN_CAPABILITY";

    if (capability == TRANSMIT) {
        returnString = "TRANS";
    } else if (capability == RECEIVE) {
        returnString = "RECV";
    } else if (capability == DUPLEX) {
        returnString = "DUPLEX";
    }

    return(returnString);
}

/* ************************************************************************************* */

void
AntennaBase::SetAntennaDesignator(const shared_ptr<string> &ptrAntennaDesignator)
{
    ptrAntennaDesignator_a = ptrAntennaDesignator;
}

/* ************************************************************************************* */

void
AntennaBase::SetAntennaCoordinateFrame(const shared_ptr<AntennaCoordinateFrame> &ptrAntennaCoordinateFrame)
{
    ptrAntennaCoordinateFrame_a = ptrAntennaCoordinateFrame;
}

/* ************************************************************************************* */

void
AntennaBase::SetAntennaCapability(ANTENNA_CAPABILITY antennaCapability)
{
    antennaCapability_a = antennaCapability;
}

/* ************************************************************************************* */

AntennaBase::~AntennaBase()
{
    return;
}

