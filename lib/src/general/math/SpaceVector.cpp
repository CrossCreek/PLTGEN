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
 * SpaceVector.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "SpaceVector.h"
#include "SimpleMath.h"
#include "general/time/TimePiece.h"
#include "general/exception/ArithmeticException.h"
#include "general/exception/DivideByZeroException.h"
#include "general/utility/Constant.h"

/* ***************************************************************************** */

SpaceVector::SpaceVector(REFERENCE_FRAME frame)
    : firstProjection_a(0.0), secondProjection_a(0.0), thirdProjection_a(0.0),
      referenceFrame_a(frame)
{
    return;
}

/* ***************************************************************************** */

SpaceVector::SpaceVector(double firstProjection,
                         double secondProjection,
                         double thirdProjection,
                         REFERENCE_FRAME frame,
                         ANGLE_UNIT angle,
                         DISTANCE_UNIT distance)
    : firstProjection_a(firstProjection), secondProjection_a(secondProjection),
      thirdProjection_a(thirdProjection), referenceFrame_a(frame)
{
    const ANGLE_UNIT    storeAngle = UnitData::GetInternalAngleUnits();
    const DISTANCE_UNIT storeDistance = UnitData::GetInternalDistanceUnits();

    if ( (frame != EARTH_CENTER_FIXED) && (frame != EARTH_CENTER_INERTIAL) &&
            (frame != LAT_LON_ALTITUDE) && (frame != PQW) && (frame != CARTESIAN)) {
        throw new InputException(GetClassName(),
                                 "SpaceVector(const double&,const double&,const double&,const REFERENCE_FRAME&)"
                                 "Reference frame must be either ECF, ECI, LLA, PQW, or CART!");
    }

    if (distance != storeDistance) {
        if (frame != LAT_LON_ALTITUDE) {
            firstProjection_a = SimpleMath::ConvertDistance(firstProjection_a, distance, storeDistance);
            secondProjection_a = SimpleMath::ConvertDistance(secondProjection_a, distance, storeDistance);
        }

        thirdProjection_a = SimpleMath::ConvertDistance(thirdProjection_a, distance, storeDistance);
    }

    if ((angle != storeAngle) && (frame == LAT_LON_ALTITUDE)) {
        firstProjection_a = SimpleMath::ConvertAngle(firstProjection_a, angle, storeAngle);
        secondProjection_a = SimpleMath::ConvertAngle(secondProjection_a, angle, storeAngle);
    }

    return;
}

/* ***************************************************************************** */

SpaceVector::SpaceVector(const SpaceVector &copyMe)
    : firstProjection_a(copyMe.firstProjection_a),
      secondProjection_a(copyMe.secondProjection_a),
      thirdProjection_a(copyMe.thirdProjection_a),
      referenceFrame_a(copyMe.referenceFrame_a)
{
    return;
}

/* ***************************************************************************** */

SpaceVector&
SpaceVector::operator = (const SpaceVector &copyMe)
{
    firstProjection_a  = copyMe.firstProjection_a;
    secondProjection_a = copyMe.secondProjection_a;
    thirdProjection_a  = copyMe.thirdProjection_a;
    referenceFrame_a   = copyMe.referenceFrame_a;
    return(*this);
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::operator + (const SpaceVector &addMe) const
{
    if (referenceFrame_a != addMe.GetReferenceFrame()) {
        throw new ArithmeticException(GetClassName(), "operator + (const SpaceVector&)");
    }

    return(SpaceVector((firstProjection_a + addMe.GetX()),
                       (secondProjection_a + addMe.GetY()),
                       (thirdProjection_a + addMe.GetZ()),
                       referenceFrame_a));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::operator - (const SpaceVector &subtractMe) const
{
    if (referenceFrame_a != subtractMe.GetReferenceFrame()) {
        throw new ArithmeticException(GetClassName(), "operator - (const SpaceVector&)");
    }

    return(SpaceVector((firstProjection_a - subtractMe.GetX()),
                       (secondProjection_a - subtractMe.GetY()),
                       (thirdProjection_a - subtractMe.GetZ()),
                       referenceFrame_a));
}

/* ***************************************************************************** */

SpaceVector&
SpaceVector::operator += (const SpaceVector &addMe)
{
    if (referenceFrame_a != addMe.GetReferenceFrame()) {
        throw new ArithmeticException(GetClassName(), "operator += (const SpaceVector&)");
    }

    firstProjection_a  += addMe.GetX();
    secondProjection_a += addMe.GetY();
    thirdProjection_a  += addMe.GetZ();
    return(*this);
}

/* ***************************************************************************** */

SpaceVector&
SpaceVector::operator -= (const SpaceVector &subtractMe)
{
    if (referenceFrame_a != subtractMe.GetReferenceFrame()) {
        throw new ArithmeticException(GetClassName(), "operator -= (const SpaceVector&)");
    }

    firstProjection_a  -= subtractMe.GetX();
    secondProjection_a -= subtractMe.GetY();
    thirdProjection_a  -= subtractMe.GetZ();
    return(*this);
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::operator * (double multiplyMe) const
{
    return(SpaceVector((firstProjection_a * multiplyMe),
                       (secondProjection_a * multiplyMe),
                       (thirdProjection_a * multiplyMe),
                       referenceFrame_a));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::operator / (double divideByMe) const
{
    if (divideByMe == 0.0) {
        throw new DivideByZeroException(GetClassName(), "operator / (const double&)");
    }

    return(SpaceVector((firstProjection_a / divideByMe),
                       (secondProjection_a / divideByMe),
                       (thirdProjection_a / divideByMe),
                       referenceFrame_a));
}

/* ***************************************************************************** */

SpaceVector&
SpaceVector::operator *= (double multiplyMe)
{
    firstProjection_a  *= multiplyMe;
    secondProjection_a *= multiplyMe;
    thirdProjection_a  *= multiplyMe;
    return(*this);
}

/* ***************************************************************************** */

SpaceVector&
SpaceVector::operator /= (double divideByMe)
{
    if (divideByMe == 0.0) {
        throw new DivideByZeroException(GetClassName(), "operator /= (const double&)");
    }

    firstProjection_a  /= divideByMe;
    secondProjection_a /= divideByMe;
    thirdProjection_a  /= divideByMe;
    return(*this);
}

/* ***************************************************************************** */

double
SpaceVector::operator * (const SpaceVector &dotMe) const
{
    if (referenceFrame_a != dotMe.GetReferenceFrame()) {
        throw new ArithmeticException(GetClassName(), "operator * (const SpaceVector&)");
    }

    return(DotProduct(*this, dotMe));
}

/* ***************************************************************************** */

bool
SpaceVector::operator == (const SpaceVector &compareMe) const
{
    return( (firstProjection_a == compareMe.firstProjection_a) &&
            (secondProjection_a == compareMe.secondProjection_a) &&
            (thirdProjection_a == compareMe.thirdProjection_a) &&
            (referenceFrame_a == compareMe.referenceFrame_a) );
}

/* ***************************************************************************** */

bool
SpaceVector::operator != (const SpaceVector &compareMe) const
{
    return(!(operator == (compareMe)));
}

/* ***************************************************************************** */

bool
SpaceVector::operator < (const SpaceVector &compareMe) const
{
    return(GetMagnitudeSquared() < compareMe.GetMagnitudeSquared());
}

/* ***************************************************************************** */

bool
SpaceVector::CompareXYZ(const SpaceVector &compareMe) const
{
    return( (firstProjection_a == compareMe.GetX()) &&
            (secondProjection_a == compareMe.GetY()) &&
            (thirdProjection_a == compareMe.GetZ()) );
}

/* ***************************************************************************** */

void
SpaceVector::ConvertToUnitVector()
{
    const double magnitude = GetMagnitude();
    firstProjection_a /= magnitude;
    secondProjection_a /= magnitude;
    thirdProjection_a /= magnitude;
    return;
}

/* ***************************************************************************** */

void
SpaceVector::EraseVector()
{
    firstProjection_a = 0.0;
    secondProjection_a = 0.0;
    thirdProjection_a = 0.0;
    return;
}

/* ***************************************************************************** */

double
SpaceVector::DotProduct(const SpaceVector &dotMe) const
{
    return(DotProduct(*this, dotMe));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::ConvertVector(const SpaceVector &inputVector,
                           const REFERENCE_FRAME &fromFrame,
                           const REFERENCE_FRAME &toFrame,
                           double j2000Time)
{
    double convertedX = inputVector.GetX();
    double convertedY = inputVector.GetY();
    double convertedZ = inputVector.GetZ();

    if (fromFrame != toFrame) {
        double cosTheta = 0.0;
        double sinTheta = 0.0;
        double interimX = 0.0;
        double interimY = 0.0;
        double interimZ = 0.0;

        if ((fromFrame == EARTH_CENTER_INERTIAL) || (toFrame == EARTH_CENTER_INERTIAL)) {
            double theta;
            theta = Calendar::CalculateGreenwichSiderealAngle(j2000Time);
            cosTheta = cos(theta);
            sinTheta = sin(theta);
        }

        if (fromFrame == EARTH_CENTER_FIXED) {
            if (toFrame == EARTH_CENTER_INERTIAL) {
                convertedX = (inputVector.GetX() * cosTheta) - (inputVector.GetY() * sinTheta);
                convertedY = (inputVector.GetX() * sinTheta) + (inputVector.GetY() * cosTheta);
                convertedZ = inputVector.GetZ();
            } else if (toFrame == LAT_LON_ALTITUDE) {
                convertedZ = SimpleMath::SquareRoot((inputVector.GetX() * inputVector.GetX())
                                                    + (inputVector.GetY() * inputVector.GetY())
                                                    + (inputVector.GetZ() * inputVector.GetZ()));
                convertedY = SimpleMath::ArcTangent(inputVector.GetY(), inputVector.GetX());
                convertedX = 0.0;

                if (convertedZ != 0.0) {
                    convertedX = SimpleMath::ArcSine(inputVector.GetZ() / convertedZ);
                }
            }
        } else if (fromFrame == EARTH_CENTER_INERTIAL) {
            convertedX = (inputVector.GetX() * cosTheta) + (inputVector.GetY() * sinTheta);
            convertedY = (-inputVector.GetX() * sinTheta) + (inputVector.GetY() * cosTheta);
            convertedZ = inputVector.GetZ();

            if (toFrame == LAT_LON_ALTITUDE) {
                interimX = convertedX;
                interimY = convertedY;
                interimZ = convertedZ;
                convertedZ = SimpleMath::SquareRoot((interimX * interimX)
                                                    + (interimY * interimY)
                                                    + (interimZ * interimZ));
                convertedY = SimpleMath::ArcTangent(interimY, interimX);
                convertedX = 0.0;

                if (convertedZ != 0.0) {
                    convertedX = SimpleMath::ArcSine(interimZ / convertedZ);
                }
            }
        } else if (fromFrame == LAT_LON_ALTITUDE) {
            convertedX = inputVector.GetZ() * (cos(inputVector.GetX())) * (cos(inputVector.GetY()));
            convertedY = inputVector.GetZ() * (cos(inputVector.GetX())) * (sin(inputVector.GetY()));
            convertedZ = inputVector.GetZ() * (sin(inputVector.GetX()));

            if (toFrame == EARTH_CENTER_INERTIAL) {
                interimX = convertedX;
                interimY = convertedY;
                convertedX = (interimX * cosTheta) - (interimY * sinTheta);
                convertedY = (interimX * sinTheta) + (interimY * cosTheta);
            }
        }
    }

    return(SpaceVector(convertedX, convertedY, convertedZ, toFrame));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::ConvertTo(const REFERENCE_FRAME &toFrame,
                       int timeIndex) const
{
    double j2000Time = 0.0;

    if ((referenceFrame_a == EARTH_CENTER_INERTIAL) || (toFrame == EARTH_CENTER_INERTIAL)) {
        if (timeIndex < 0) {
            throw new InputException(GetClassName(),
                                     "ConvertTo(const REFERENCE_FRAME&, int)",
                                     "Must input valid time step when converting from/to ECI SpaceVector");
        }

        j2000Time = TimePiece::GetJ2000Time(timeIndex);
    }

    return(ConvertVector(*this, referenceFrame_a, toFrame, j2000Time));
}

SpaceVector
SpaceVector::ConvertTo(const REFERENCE_FRAME &toFrame,
                       double j2000Time) const
{
    return(ConvertVector(*this, referenceFrame_a, toFrame, j2000Time));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::GetInputSpaceVector(int timeIndex) const
{
    double          j2000Time = 0.0;
    double          convertedX = 0.0;
    double          convertedY = 0.0;
    double          convertedZ = 0.0;
    REFERENCE_FRAME toFrame = UnitData::GetInputReferenceFrame();
    SpaceVector     convertedVector;

    if ((referenceFrame_a == EARTH_CENTER_INERTIAL) || (toFrame == EARTH_CENTER_INERTIAL)) {
        if (timeIndex < 0) {
            throw new InputException(GetClassName(), "GetInputSpaceVector(int)",
                                     "Must input valid time step when converting from/to ECI SpaceVector");
        }

        j2000Time = TimePiece::GetJ2000Time(timeIndex);
    }

    convertedVector = ConvertVector(*this, referenceFrame_a, toFrame, j2000Time);

    if (toFrame != LAT_LON_ALTITUDE) {
        convertedX = UnitData::GetInputDistance(convertedVector.GetX());
        convertedY = UnitData::GetInputDistance(convertedVector.GetY());
    } else {
        convertedX = UnitData::GetInputAngle(convertedVector.GetX());
        convertedY = UnitData::GetInputAngle(convertedVector.GetY());
    }

    convertedZ = UnitData::GetInputDistance(convertedVector.GetZ());
    return(SpaceVector(convertedX, convertedY, convertedZ, toFrame));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::GetOutputSpaceVector(int timeIndex) const
{
    double          j2000Time = 0.0;
    double          convertedX = 0.0;
    double          convertedY = 0.0;
    double          convertedZ = 0.0;
    REFERENCE_FRAME toFrame = UnitData::GetOutputReferenceFrame();
    SpaceVector     convertedVector;

    if ((referenceFrame_a == EARTH_CENTER_INERTIAL) || (toFrame == EARTH_CENTER_INERTIAL)) {
        if (timeIndex < 0) {
            throw new InputException(GetClassName(), "GetOutputSpaceVector(int)",
                                     "Must input valid time step when converting from/to ECI SpaceVector");
        }

        j2000Time = TimePiece::GetJ2000Time(timeIndex);
    }

    convertedVector = ConvertVector(*this, referenceFrame_a, toFrame, j2000Time);

    if (toFrame != LAT_LON_ALTITUDE) {
        convertedX = UnitData::GetOutputDistance(convertedVector.GetX());
        convertedY = UnitData::GetOutputDistance(convertedVector.GetY());
    } else {
        convertedX = UnitData::GetOutputAngle(convertedVector.GetX());
        convertedY = UnitData::GetOutputAngle(convertedVector.GetY());
    }

    convertedZ = UnitData::GetOutputDistance(convertedVector.GetZ());
    return(SpaceVector(convertedX, convertedY, convertedZ, toFrame));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::GetStoreSpaceVector(int timeIndex) const
{
    double          j2000Time = 0.0;
    REFERENCE_FRAME toFrame = UnitData::GetInternalReferenceFrame();

    if ((referenceFrame_a == EARTH_CENTER_INERTIAL) || (toFrame == EARTH_CENTER_INERTIAL)) {
        if (timeIndex < 0) {
            throw new InputException(GetClassName(), "GetOutputSpaceVector(int)",
                                     "Must input valid time step when converting from/to ECI SpaceVector");
        }

        j2000Time = TimePiece::GetJ2000Time(timeIndex);
    }

    return(ConvertVector(*this, referenceFrame_a, toFrame, j2000Time));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::CrossProduct(const SpaceVector &crossMe) const
{
    if (referenceFrame_a != crossMe.GetReferenceFrame()) {
        throw new ArithmeticException(GetClassName(), "CrossProduct(const SpaceVector&)");
    }

    return(SpaceVector((GetY() * crossMe.GetZ()) - (GetZ() * crossMe.GetY()),
                       (GetZ() * crossMe.GetX()) - (GetX() * crossMe.GetZ()),
                       (GetX() * crossMe.GetY()) - (GetY() * crossMe.GetX()),
                       referenceFrame_a));
}

/* ***************************************************************************** */

double
SpaceVector::GetSeparationAngle(const SpaceVector &compareMe,
                                ANGLE_UNIT angle) const
{
    if (referenceFrame_a != compareMe.GetReferenceFrame()) {
        throw new ArithmeticException(GetClassName(), "GetSeparationAngle(const SpaceVector&, ANGLE_UNIT)");
    }

    return(AngleBetweenVectors(*this, compareMe, angle));
}

/* ***************************************************************************** */

double
SpaceVector::GetSeparationDistance(const SpaceVector &compareMe,
                                   DISTANCE_UNIT distance) const
{
    if (referenceFrame_a != compareMe.GetReferenceFrame()) {
        throw new ArithmeticException(GetClassName(), "GetSeparationDistance(const SpaceVector&, DISTANCE_UNIT)");
    }

    return(DistanceBetweenVectors(*this, compareMe, distance));
}

/* ***************************************************************************** */

double
SpaceVector::GetMagnitudeSquared() const
{
    return((firstProjection_a * firstProjection_a)
           + (secondProjection_a * secondProjection_a)
           + (thirdProjection_a * thirdProjection_a));
}

/* ***************************************************************************** */

double
SpaceVector::GetMagnitude() const
{
    return(SimpleMath::SquareRoot(GetMagnitudeSquared()));
}

/* ***************************************************************************** */

double
SpaceVector::GetLatitude(ANGLE_UNIT angle) const
{
    double latitude = ConvertTo(LAT_LON_ALTITUDE).GetX();
    return(SimpleMath::ConvertAngle(latitude, UnitData::GetInternalAngleUnits(), angle));
}

/* ***************************************************************************** */

double
SpaceVector::GetLongitude(ANGLE_UNIT angle) const
{
    double longitude = ConvertTo(LAT_LON_ALTITUDE).GetY();
    return(SimpleMath::ConvertAngle(longitude, UnitData::GetInternalAngleUnits(), angle));
}

/* ***************************************************************************** */

double
SpaceVector::GetAltitude(DISTANCE_UNIT distance) const
{
    double altitude = ConvertTo(LAT_LON_ALTITUDE).GetZ();
    return(SimpleMath::ConvertDistance(altitude, UnitData::GetInternalDistanceUnits(), distance));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::GetUnitVector() const
{
    return(SpaceVector(*this / GetMagnitude()));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::Negate() const
{
    return(SpaceVector(*this * (-1.0)));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::ToEarthsSurface(int timeIndex) const
{
    SpaceVector vectorLLA;

    try {
        vectorLLA = ConvertTo(LAT_LON_ALTITUDE, timeIndex);
        //vectorLLA.SetZ(Earth::GetStoreRadius());
        vectorLLA.SetZ(UnitData::GetInternalDistance(EARTH_RADIUS_NMI_s));
    } catch (Exception *ptrError) {
        ptrError->AddMethod(GetClassName(), "ToEarthsSurface(int)");
        throw;
    }

    return(ConvertVector(vectorLLA, LAT_LON_ALTITUDE, referenceFrame_a,
                         TimePiece::GetJ2000Time(timeIndex)));
}

/* ***************************************************************************** */

double
SpaceVector::ProjectionOnto(const SpaceVector &projectedOnto) const
{
    return(DotProduct(projectedOnto) / (projectedOnto.GetMagnitude() * projectedOnto.GetMagnitude()));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::ProjectIntoPlane(const SpaceVector &planeVector) const
{
    return(ProjectIntoPlane(*this, planeVector));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::TransformCoordinates(const SpaceVector &xFrame,
                                  const SpaceVector &yFrame,
                                  const SpaceVector &zFrame) const
{
    return(TransformCoordinates(*this, xFrame, yFrame, zFrame));
}

/* ***************************************************************************** */

double
SpaceVector::AngleBetweenVectors(const SpaceVector &vector1,
                                 const SpaceVector &vector2,
                                 const ANGLE_UNIT &returnAngleUnits)
{
    const double angleBetween = SimpleMath::ArcCosine(DotProduct(vector1, vector2)
                                / (vector1.GetMagnitude() * vector2.GetMagnitude()));
    return(SimpleMath::ConvertAngle(angleBetween,
                                    UnitData::GetInternalAngleUnits(), returnAngleUnits));
}

/* ***************************************************************************** */

double
SpaceVector::DistanceBetweenVectors(const SpaceVector &vector1,
                                    const SpaceVector &vector2,
                                    const DISTANCE_UNIT &distance)
{
    SpaceVector  differenceVector = vector1 - vector2;
    const double distanceBetween = differenceVector.GetMagnitude();
    return(SimpleMath::ConvertDistance(distanceBetween,
                                       UnitData::GetInternalDistanceUnits(), distance));
}

/* ***************************************************************************** */

double
SpaceVector::DotProduct(const SpaceVector &vector1,
                        const SpaceVector &vector2)
{
    return((vector1.GetX() * vector2.GetX()) + (vector1.GetY() * vector2.GetY())
           + (vector1.GetZ() * vector2.GetZ()));
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::ProjectIntoPlane(const SpaceVector &projectVector,
                              const SpaceVector &planeVector)
{
    SpaceVector planeVectorNorm = planeVector.GetUnitVector();
    SpaceVector projectVectorNorm = planeVectorNorm
                                    * DotProduct(projectVector, planeVectorNorm);
    return(projectVector - projectVectorNorm);
}

/* ***************************************************************************** */

SpaceVector
SpaceVector::TransformCoordinates(const SpaceVector &transformVector,
                                  const SpaceVector &xFrame,
                                  const SpaceVector &yFrame,
                                  const SpaceVector &zFrame)
{
    const double transformedX = DotProduct(xFrame,transformVector);
    const double transformedY = DotProduct(yFrame,transformVector);
    const double transformedZ = DotProduct(zFrame,transformVector);
    return(SpaceVector(transformedX, transformedY, transformedZ,
                       transformVector.GetReferenceFrame()));
}

/* ***************************************************************************** */

double
SpaceVector::GetX() const
{
    return(firstProjection_a);
}

/* ***************************************************************************** */

double
SpaceVector::GetY() const
{
    return(secondProjection_a);
}

/* ***************************************************************************** */

double
SpaceVector::GetZ() const
{
    return(thirdProjection_a);
}

/* ***************************************************************************** */

REFERENCE_FRAME
SpaceVector::GetReferenceFrame() const
{
    return(referenceFrame_a);
}

/* ***************************************************************************** */

void
SpaceVector::SetX(double newX)
{
    firstProjection_a = newX;
}

/* ***************************************************************************** */

void
SpaceVector::SetY(double newY)
{
    secondProjection_a = newY;
}

/* ***************************************************************************** */

void
SpaceVector::SetZ(double newZ)
{
    thirdProjection_a = newZ;
}

/* ***************************************************************************** */

void
SpaceVector::SetReferenceFrame(const REFERENCE_FRAME &newFrame)
{
    referenceFrame_a = newFrame;
}

/* ***************************************************************************** */

void
SpaceVector::Set(double newX,
                 double newY,
                 double newZ)
{
    SetX(newX);
    SetY(newY);
    SetZ(newZ);
}

/* ***************************************************************************** */

SpaceVector::~SpaceVector()
{
    return;
}

