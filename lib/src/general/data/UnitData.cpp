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
 * UnitData.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include "UnitData.h"
#include "general/math/SimpleMath.h"
#include "general/time/TimePiece.h"

// Defaults (for now)
ANGLE_UNIT      UnitData::internalAngleUnits_s = RADIANS;
DISTANCE_UNIT   UnitData::internalDistanceUnits_s = METERS;
REFERENCE_FRAME UnitData::internalReferenceFrame_s = EARTH_CENTER_FIXED;
TIME_UNIT       UnitData::internalTimeUnits_s = TIME_STEPS;

ANGLE_UNIT      UnitData::inputAngleUnits_s = RADIANS;
DISTANCE_UNIT   UnitData::inputDistanceUnits_s = METERS;
REFERENCE_FRAME UnitData::inputReferenceFrame_s = EARTH_CENTER_FIXED;
TIME_UNIT       UnitData::inputTimeUnits_s = TIME_STEPS;

ANGLE_UNIT      UnitData::outputAngleUnits_s = RADIANS;
DISTANCE_UNIT   UnitData::outputDistanceUnits_s = METERS;
REFERENCE_FRAME UnitData::outputReferenceFrame_s = EARTH_CENTER_FIXED;
TIME_UNIT       UnitData::outputTimeUnits_s = TIME_STEPS;

UnitData::UnitData()
{
    return;
}

/* ***************************************************************************** */

void
UnitData::SetInternalReferenceFrame(const REFERENCE_FRAME &frame)
{
    internalReferenceFrame_s = frame;
}

/* ***************************************************************************** */

void
UnitData::SetInternalAngleUnits(const ANGLE_UNIT &units)
{
    internalAngleUnits_s = units;
}

/* ***************************************************************************** */

void
UnitData::SetInternalDistanceUnits(const DISTANCE_UNIT &units)
{
    internalDistanceUnits_s = units;
}

/* ***************************************************************************** */

void
UnitData::SetInternalTimeUnits(const TIME_UNIT &units)
{
    internalTimeUnits_s = units;
}

/* ***************************************************************************** */

void
UnitData::SetInputReferenceFrame(const REFERENCE_FRAME &frame)
{
    inputReferenceFrame_s = frame;
}

/* ***************************************************************************** */

void
UnitData::SetInputAngleUnits(const ANGLE_UNIT &units)
{
    inputAngleUnits_s = units;
}

/* ***************************************************************************** */

void
UnitData::SetInputDistanceUnits(const DISTANCE_UNIT &units)
{
    inputDistanceUnits_s = units;
}

/* ***************************************************************************** */

void
UnitData::SetInputTimeUnits(const TIME_UNIT &units)
{
    inputTimeUnits_s = units;
}

/* ***************************************************************************** */

void
UnitData::SetOutputReferenceFrame(const REFERENCE_FRAME &frame)
{
    outputReferenceFrame_s = frame;
}

/* ***************************************************************************** */

void
UnitData::SetOutputAngleUnits(const ANGLE_UNIT &units)
{
    outputAngleUnits_s = units;
}

/* ***************************************************************************** */

void
UnitData::SetOutputDistanceUnits(const DISTANCE_UNIT &units)
{
    outputDistanceUnits_s = units;
}

/* ***************************************************************************** */

void
UnitData::SetOutputTimeUnits(const TIME_UNIT &units)
{
    outputTimeUnits_s = units;
}

/* ***************************************************************************** */

REFERENCE_FRAME
UnitData::GetOutputReferenceFrame()
{
    return(outputReferenceFrame_s);
}

/* ***************************************************************************** */

ANGLE_UNIT
UnitData::GetOutputAngleUnits()
{
    return(outputAngleUnits_s);
}

/* ***************************************************************************** */

DISTANCE_UNIT
UnitData::GetOutputDistanceUnits()
{
    return(outputDistanceUnits_s);
}

/* ***************************************************************************** */

TIME_UNIT
UnitData::GetOutputTimeUnits()
{
    return(outputTimeUnits_s);
}

/* ***************************************************************************** */

REFERENCE_FRAME
UnitData::GetInputReferenceFrame()
{
    return(inputReferenceFrame_s);
}

/* ***************************************************************************** */

TIME_UNIT
UnitData::GetInputTimeUnits()
{
    return(inputTimeUnits_s);
}

/* ***************************************************************************** */

ANGLE_UNIT
UnitData::GetInputAngleUnits()
{
    return(inputAngleUnits_s);
}

/* ***************************************************************************** */

DISTANCE_UNIT
UnitData::GetInputDistanceUnits()
{
    return(inputDistanceUnits_s);
}

/* ***************************************************************************** */

string
UnitData::GetAngleUnitString(const ANGLE_UNIT &angle)
{
    string returnString = "";

    if (angle == DEGREES) {
        returnString = "DEG";
    } else if (angle == RADIANS) {
        returnString = "RAD";
    } else {
        returnString = "GRAD";
    }

    return(returnString);
}

/* ***************************************************************************** */

string
UnitData::GetDistanceUnitString(const DISTANCE_UNIT &distance)
{
    string returnString = "";

    if (distance == METERS) {
        returnString = "M";
    } else if (distance == KILOMETERS) {
        returnString = "KM";
    } else if (distance == FEET) {
        returnString = "FT";
    } else if (distance == MILES) {
        returnString = "MI";
    } else {
        returnString = "NMI";
    }

    return(returnString);
}

/* ***************************************************************************** */

string
UnitData::GetTimeUnitString(const TIME_UNIT &time)
{
    string returnString = "";

    if (time == HOURS) {
        returnString = "HR";
    } else if (time == MINUTES) {
        returnString = "MN";
    } else if (time == SECONDS) {
        returnString = "SEC";
    } else if (time == TIME_STEPS) {
        returnString = "TS";
    } else {
        returnString = "UNKNOWN";
    }

    return(returnString);
}

/* ***************************************************************************** */

string
UnitData::GetReferenceFrameString(const REFERENCE_FRAME &frame)
{
    string returnString = "";

    if (frame == EARTH_CENTER_FIXED) {
        returnString = "ECF";
    } else if (frame == EARTH_CENTER_INERTIAL) {
        returnString = "ECI";
    } else if (frame == LAT_LON_ALTITUDE) {
        returnString = "LLA";
    } else if (frame == PQW) {
        returnString = "PQW";
    } else {
        returnString = "UNKNOWN";
    }

    return(returnString);
}

/* ***************************************************************************** */

ANGLE_UNIT
UnitData::GetAngleUnits(const string &valueString)
{
    ANGLE_UNIT angleUnits = UNKNOWN_ANGLE_UNIT;

    if (valueString == "DEG") {
        angleUnits = DEGREES;
    } else if (valueString == "RAD") {
        angleUnits = RADIANS;
    } else if (valueString == "GRAD") {
        angleUnits = GRADIENTS;
    }

    return(angleUnits);
}

/* ***************************************************************************** */

DISTANCE_UNIT
UnitData::GetDistanceUnits(const string &valueString)
{
    DISTANCE_UNIT distanceUnits = UNKNOWN_DISTANCE_UNIT;

    if (valueString == "M") {
        distanceUnits = METERS;
    } else if (valueString == "KM") {
        distanceUnits = KILOMETERS;
    } else if (valueString == "FT") {
        distanceUnits = FEET;
    } else if (valueString == "MI") {
        distanceUnits = MILES;
    } else if (valueString == "NMI") {
        distanceUnits = NAUTICAL_MILES;
    }

    return(distanceUnits);
}

/* ***************************************************************************** */

TIME_UNIT
UnitData::GetTimeUnits(const string &valueString)
{
    TIME_UNIT timeUnits = UNKNOWN_TIME_UNIT;

    if (valueString == "HR") {
        timeUnits = HOURS;
    } else if (valueString == "MN") {
        timeUnits = MINUTES;
    } else if (valueString == "SEC") {
        timeUnits = SECONDS;
    } else if (valueString == "TS") {
        timeUnits = TIME_STEPS;
    }

    return(timeUnits);
}

/* ***************************************************************************** */

REFERENCE_FRAME
UnitData::GetReferenceFrame(const string &valueString)
{
    REFERENCE_FRAME referenceFrame = UNKNOWN_FRAME;

    if (valueString == "ECF") {
        referenceFrame = EARTH_CENTER_FIXED;
    } else if (valueString == "ECI") {
        referenceFrame = EARTH_CENTER_INERTIAL;
    } else if (valueString == "LLA") {
        referenceFrame = LAT_LON_ALTITUDE;
    } else if (valueString == "PQW") {
        referenceFrame = PQW;
    } else if (valueString == "CART") {
        referenceFrame = CARTESIAN;
    }

    return(referenceFrame);
}

/* ***************************************************************************** */

REFERENCE_FRAME
UnitData::GetInternalReferenceFrame()
{
    return(internalReferenceFrame_s);
}

/* ***************************************************************************** */

TIME_UNIT
UnitData::GetInternalTimeUnits()
{
    return(internalTimeUnits_s);
}

/* ***************************************************************************** */

ANGLE_UNIT
UnitData::GetInternalAngleUnits()
{
    return(internalAngleUnits_s);
}

/* ***************************************************************************** */

DISTANCE_UNIT
UnitData::GetInternalDistanceUnits()
{
    return(internalDistanceUnits_s);
}

/* ***************************************************************************** */

double
UnitData::GetOutputAngle(double internalAngle)
{
    return(SimpleMath::ConvertAngle(internalAngle, UnitData::internalAngleUnits_s, outputAngleUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetOutputDistance(double internalDistance)
{
    return(SimpleMath::ConvertDistance(internalDistance, UnitData::internalDistanceUnits_s, outputDistanceUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetOutputTimeStep(int internalTimeIndex)
{
    return(TimePiece::ConvertTime((double)(internalTimeIndex + 1), UnitData::internalTimeUnits_s, outputTimeUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetOutputTimeDuration(int internalDuration)
{
    return(TimePiece::ConvertTime((double)internalDuration, UnitData::internalTimeUnits_s, outputTimeUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetInputAngle(double internalAngle)
{
    return(SimpleMath::ConvertAngle(internalAngle,
                                    internalAngleUnits_s, inputAngleUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetInputDistance(double internalDistance)
{
    return(SimpleMath::ConvertDistance(internalDistance,
                                       internalDistanceUnits_s, inputDistanceUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetInputTimeInstant(int internalIndex)
{
    return(TimePiece::ConvertTime((double)(internalIndex + 1), internalTimeUnits_s,
                                  inputTimeUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetInputTimeDuration(int internalDuration)
{
    return(TimePiece::ConvertTime((double)internalDuration, internalTimeUnits_s,
                                  inputTimeUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetInternalAngle(double inputAngle)
{
    return(SimpleMath::ConvertAngle(inputAngle,
                                    inputAngleUnits_s, internalAngleUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetInternalDistance(double inputDistance)
{
    return(SimpleMath::ConvertDistance(inputDistance,
                                       inputDistanceUnits_s, internalDistanceUnits_s));
}

/* ***************************************************************************** */

int
UnitData::GetInternalTimeIndex(double inputTimeInstant)
{
    return((int)TimePiece::ConvertTime(inputTimeInstant, inputTimeUnits_s,
                                       internalTimeUnits_s) - 1);
}

/* ***************************************************************************** */

int
UnitData::GetInternalTimeDuration(double inputDuration)
{
    return((int)TimePiece::ConvertTime(inputDuration, inputTimeUnits_s,
                                       internalTimeUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetStoreQuarterCircle()
{
    return(SimpleMath::ConvertAngle(HALF_PI, RADIANS, internalAngleUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetStoreHalfCircle()
{
    return(SimpleMath::ConvertAngle(PI, RADIANS, internalAngleUnits_s));
}

/* ***************************************************************************** */

double
UnitData::GetStoreFullCircle()
{
    return(SimpleMath::ConvertAngle(TWO_PI, RADIANS, internalAngleUnits_s));
}

/* ***************************************************************************** */

UnitData::~UnitData()
{
    return;
}
