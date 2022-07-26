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
 * CoordinateFrame.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include <iostream>
#include <iomanip>
#include "CoordinateFrame.h"
#include "SimpleMath.h"
#include "general/exception/ArithmeticException.h"
#include "general/utility/Constant.h"

/* ***************************************************************************** */

CoordinateFrame::CoordinateFrame(REFERENCE_FRAME frame)
    : ptrXAxis_a(new SpaceVector(1.0, 0.0, 0.0, frame)),
      ptrYAxis_a(new SpaceVector(0.0, 1.0, 0.0, frame)),
      ptrZAxis_a(new SpaceVector(0.0, 0.0, 1.0, frame))
{
    return;
}

/* ***************************************************************************** */

CoordinateFrame::CoordinateFrame(const SpaceVector &xAxis,
                                 const SpaceVector &yAxis,
                                 const SpaceVector &zAxis,
                                 REFERENCE_FRAME frame)
    : ptrXAxis_a(new SpaceVector(xAxis.GetUnitVector())),
      ptrYAxis_a(new SpaceVector(yAxis.GetUnitVector())),
      ptrZAxis_a(new SpaceVector(zAxis.GetUnitVector()))
{
    return;
}

/* ***************************************************************************** */

CoordinateFrame::CoordinateFrame(double azimuthAngle,
                                 double elevationAngle)
    : ptrXAxis_a(NULL), ptrYAxis_a(NULL), ptrZAxis_a(NULL)
{
    const double cosAzimuth = cos(azimuthAngle);
    const double sinAzimuth = sin(azimuthAngle);
    const double cosElevation = cos(elevationAngle);
    const double sinElevation = sin(elevationAngle);
    ptrXAxis_a = shared_ptr<SpaceVector>(new SpaceVector((cosElevation * cosAzimuth), (cosElevation * sinAzimuth), -sinElevation));
    ptrYAxis_a = shared_ptr<SpaceVector>(new SpaceVector(-sinAzimuth, cosAzimuth, 0.0));
    ptrZAxis_a = shared_ptr<SpaceVector>(new SpaceVector((sinElevation * cosAzimuth), (sinElevation * sinAzimuth), cosElevation));
    return;
}

/* ***************************************************************************** */

CoordinateFrame::CoordinateFrame(const CoordinateFrame &copyMe)
    : ptrXAxis_a(new SpaceVector(copyMe.GetXAxis())),
      ptrYAxis_a(new SpaceVector(copyMe.GetYAxis())),
      ptrZAxis_a(new SpaceVector(copyMe.GetZAxis()))
{
    return;
}

/* ***************************************************************************** */

CoordinateFrame&
CoordinateFrame::operator = (const CoordinateFrame &copyMe)
{
    ptrXAxis_a = shared_ptr<SpaceVector>(new SpaceVector(copyMe.GetXAxis()));
    ptrYAxis_a = shared_ptr<SpaceVector>(new SpaceVector(copyMe.GetYAxis()));
    ptrZAxis_a = shared_ptr<SpaceVector>(new SpaceVector(copyMe.GetZAxis()));
    return(*this);
}

/* ***************************************************************************** */

bool
CoordinateFrame::operator == (const CoordinateFrame &compareMe) const
{
    return( (*ptrXAxis_a == compareMe.GetXAxis()) &&
            (*ptrYAxis_a == compareMe.GetYAxis()) &&
            (*ptrZAxis_a == compareMe.GetZAxis()) );
}

/* ***************************************************************************** */

bool
CoordinateFrame::operator != (const CoordinateFrame &compareMe) const
{
    return(!(operator == (compareMe)));
}

/* ***************************************************************************** */

void
CoordinateFrame::Print(ostream &outfile) const
{
    outfile.setf(ios::fixed);
    outfile << " " << setw(8) << setprecision(5) << ptrXAxis_a->GetX()
            << " " << setw(8) << setprecision(5) << ptrXAxis_a->GetY()
            << " " << setw(8) << setprecision(5) << ptrXAxis_a->GetZ() << NEW_LINE;
    outfile << " " << setw(8) << setprecision(5) << ptrYAxis_a->GetX()
            << " " << setw(8) << setprecision(5) << ptrYAxis_a->GetY()
            << " " << setw(8) << setprecision(5) << ptrYAxis_a->GetZ() << NEW_LINE;
    outfile << " " << setw(8) << setprecision(5) << ptrZAxis_a->GetX()
            << " " << setw(8) << setprecision(5) << ptrZAxis_a->GetY()
            << " " << setw(8) << setprecision(5) << ptrZAxis_a->GetZ() << NEW_LINE;
    return;
}

/* ***************************************************************************** */

void
CoordinateFrame::EulerTransformAboutX(double rotationAngle)
{
    double       cosineRotationAngle = 0.00;
    double       sineRotationAngle = 0.00;
    double       useRotationAngle = rotationAngle;
    const double quarterCircle = UnitData::GetStoreQuarterCircle();
    SpaceVector  yAxis;
    SpaceVector  zAxis;

    if (rotationAngle < 0.0) {
        useRotationAngle += (4.0 * quarterCircle);
    }

    cosineRotationAngle = cos(useRotationAngle);
    sineRotationAngle = sin(useRotationAngle);
    // Rotated Y Axis
    yAxis = SpaceVector(((cosineRotationAngle * ptrYAxis_a->GetX()) +
                         (sineRotationAngle * ptrZAxis_a->GetX())),
                        ((cosineRotationAngle * ptrYAxis_a->GetY()) +
                         (sineRotationAngle * ptrZAxis_a->GetY())),
                        ((cosineRotationAngle * ptrYAxis_a->GetZ()) +
                         (sineRotationAngle * ptrZAxis_a->GetZ())),
                        ptrYAxis_a->GetReferenceFrame());
    // Rotated Z Axis
    zAxis = SpaceVector(((cosineRotationAngle * ptrZAxis_a->GetX()) -
                         (sineRotationAngle * ptrYAxis_a->GetX())),
                        ((cosineRotationAngle * ptrZAxis_a->GetY()) -
                         (sineRotationAngle * ptrYAxis_a->GetY())),
                        ((cosineRotationAngle * ptrZAxis_a->GetZ()) -
                         (sineRotationAngle * ptrYAxis_a->GetZ())),
                        ptrZAxis_a->GetReferenceFrame());

    if (SimpleMath::Abs(ptrXAxis_a->GetSeparationAngle(yAxis) - quarterCircle) > ANGLE_TOLERANCE) {
        throw new ArithmeticException(GetClassName(),
                                      "EulerTransformAboutX() -> X and Y Axes are not perpendicular");
    }

    if (SimpleMath::Abs(ptrXAxis_a->GetSeparationAngle(zAxis) - quarterCircle) > ANGLE_TOLERANCE) {
        throw new ArithmeticException(GetClassName(),
                                      "EulerTransformAboutX() -> X and Z Axes are not perpendicular");
    }

    if (SimpleMath::Abs(yAxis.GetSeparationAngle(zAxis) - quarterCircle) > ANGLE_TOLERANCE) {
        throw new ArithmeticException(GetClassName(),
                                      "EulerTransformAboutX() -> Y and Z Axes are not perpendicular");
    }

    ptrYAxis_a = shared_ptr<SpaceVector>(new SpaceVector(yAxis));
    ptrZAxis_a = shared_ptr<SpaceVector>(new SpaceVector(zAxis));
}

/* ***************************************************************************** */

void
CoordinateFrame::EulerTransformAboutY(double rotationAngle)
{
    double       cosineRotationAngle = 0.00;
    double       sineRotationAngle = 0.00;
    double       useRotationAngle = rotationAngle;
    const double quarterCircle = UnitData::GetStoreQuarterCircle();
    SpaceVector  xAxis;
    SpaceVector  zAxis;

    if (rotationAngle < 0.0) {
        useRotationAngle += (4.0 * quarterCircle);
    }

    cosineRotationAngle = cos(useRotationAngle);
    sineRotationAngle = sin(useRotationAngle);
    // Rotated X Axis
    xAxis = SpaceVector(((cosineRotationAngle * ptrXAxis_a->GetX()) -
                         (sineRotationAngle * ptrZAxis_a->GetX())),
                        ((cosineRotationAngle * ptrXAxis_a->GetY()) -
                         (sineRotationAngle * ptrZAxis_a->GetY())),
                        ((cosineRotationAngle * ptrXAxis_a->GetZ()) -
                         (sineRotationAngle * ptrZAxis_a->GetZ())),
                        ptrXAxis_a->GetReferenceFrame());
    // Rotated Z Axis
    zAxis = SpaceVector(((sineRotationAngle * ptrXAxis_a->GetX()) +
                         (cosineRotationAngle * ptrZAxis_a->GetX())),
                        ((sineRotationAngle * ptrXAxis_a->GetY()) +
                         (cosineRotationAngle * ptrZAxis_a->GetY())),
                        ((sineRotationAngle * ptrXAxis_a->GetZ()) +
                         (cosineRotationAngle * ptrZAxis_a->GetZ())),
                        ptrZAxis_a->GetReferenceFrame());

    if (SimpleMath::Abs(ptrYAxis_a->GetSeparationAngle(xAxis) - quarterCircle) > ANGLE_TOLERANCE) {
        throw new ArithmeticException(GetClassName(),
                                      "EulerTransformAboutY() -> X and Y Axes are not perpendicular");
    }

    if (SimpleMath::Abs(xAxis.GetSeparationAngle(zAxis) - quarterCircle) > ANGLE_TOLERANCE) {
        throw new ArithmeticException(GetClassName(),
                                      "EulerTransformAboutY() -> X and Z Axes are not perpendicular");
    }

    if (SimpleMath::Abs(ptrYAxis_a->GetSeparationAngle(zAxis) - quarterCircle) > ANGLE_TOLERANCE) {
        throw new ArithmeticException(GetClassName(),
                                      "EulerTransformAboutY() -> Y and Z Axes are not perpendicular");
    }

    ptrXAxis_a = shared_ptr<SpaceVector>(new SpaceVector(xAxis));
    ptrZAxis_a = shared_ptr<SpaceVector>(new SpaceVector(zAxis));
}

/* ***************************************************************************** */

void
CoordinateFrame::EulerTransformAboutZ(double rotationAngle)
{
    double       cosineRotationAngle = 0.00;
    double       sineRotationAngle = 0.00;
    double       useRotationAngle = rotationAngle;
    const double quarterCircle = UnitData::GetStoreQuarterCircle();
    SpaceVector  xAxis;
    SpaceVector  yAxis;

    if (rotationAngle < 0.0) {
        useRotationAngle += (4.0 * quarterCircle);
    }

    cosineRotationAngle = cos(useRotationAngle);
    sineRotationAngle = sin(useRotationAngle);
    // Rotated X Axis
    xAxis = SpaceVector(((cosineRotationAngle * ptrXAxis_a->GetX()) +
                         (sineRotationAngle * ptrYAxis_a->GetX())),
                        ((cosineRotationAngle * ptrXAxis_a->GetY()) +
                         (sineRotationAngle * ptrYAxis_a->GetY())),
                        ((cosineRotationAngle * ptrXAxis_a->GetZ()) +
                         (sineRotationAngle * ptrYAxis_a->GetZ())),
                        ptrXAxis_a->GetReferenceFrame());
    // Rotated Y Axis
    yAxis = SpaceVector(((cosineRotationAngle * ptrYAxis_a->GetX()) -
                         (sineRotationAngle * ptrXAxis_a->GetX())),
                        ((cosineRotationAngle * ptrYAxis_a->GetY()) -
                         (sineRotationAngle * ptrXAxis_a->GetY())),
                        ((cosineRotationAngle * ptrYAxis_a->GetZ()) -
                         (sineRotationAngle * ptrXAxis_a->GetZ())),
                        ptrYAxis_a->GetReferenceFrame());

    if (SimpleMath::Abs(xAxis.GetSeparationAngle(yAxis) - quarterCircle) > ANGLE_TOLERANCE) {
        throw new ArithmeticException(GetClassName(),
                                      "EulerTransformAboutZ() -> X and Y Axes are not perpendicular");
    }

    if (SimpleMath::Abs(ptrZAxis_a->GetSeparationAngle(xAxis) - quarterCircle) > ANGLE_TOLERANCE) {
        throw new ArithmeticException(GetClassName(),
                                      "EulerTransformAboutZ() -> X and Z Axes are not perpendicular");
    }

    if (SimpleMath::Abs(ptrZAxis_a->GetSeparationAngle(yAxis) - quarterCircle) > ANGLE_TOLERANCE) {
        throw new ArithmeticException(GetClassName(),
                                      "EulerTransformAboutZ() -> Y and Z Axes are not perpendicular");
    }

    ptrXAxis_a = shared_ptr<SpaceVector>(new SpaceVector(xAxis));
    ptrYAxis_a = shared_ptr<SpaceVector>(new SpaceVector(yAxis));
}

/* ***************************************************************************** */

CoordinateFrame
CoordinateFrame::MultiplyBy(const CoordinateFrame &multiplyMatrix)
{
    SpaceVector       newX;
    SpaceVector       newY;
    SpaceVector       newZ;
    const SpaceVector multiplyX = multiplyMatrix.GetXAxis();
    const SpaceVector multiplyY = multiplyMatrix.GetYAxis();
    const SpaceVector multiplyZ = multiplyMatrix.GetZAxis();
    newX = SpaceVector(((ptrXAxis_a->GetX()*multiplyX.GetX()) +
                        (ptrXAxis_a->GetY()*multiplyY.GetX()) +
                        (ptrXAxis_a->GetZ()*multiplyZ.GetX())),
                       ((ptrXAxis_a->GetX()*multiplyX.GetY()) +
                        (ptrXAxis_a->GetY()*multiplyY.GetY()) +
                        (ptrXAxis_a->GetZ()*multiplyZ.GetY())),
                       ((ptrXAxis_a->GetX()*multiplyX.GetZ()) +
                        (ptrXAxis_a->GetY()*multiplyY.GetZ()) +
                        (ptrXAxis_a->GetZ()*multiplyZ.GetZ())));
    newY = SpaceVector(((ptrYAxis_a->GetX()*multiplyX.GetX()) +
                        (ptrYAxis_a->GetY()*multiplyY.GetX()) +
                        (ptrYAxis_a->GetZ()*multiplyZ.GetX())),
                       ((ptrYAxis_a->GetX()*multiplyX.GetY()) +
                        (ptrYAxis_a->GetY()*multiplyY.GetY()) +
                        (ptrYAxis_a->GetZ()*multiplyZ.GetY())),
                       ((ptrYAxis_a->GetX()*multiplyX.GetZ()) +
                        (ptrYAxis_a->GetY()*multiplyY.GetZ()) +
                        (ptrYAxis_a->GetZ()*multiplyZ.GetZ())));
    newZ = SpaceVector(((ptrZAxis_a->GetX()*multiplyX.GetX()) +
                        (ptrZAxis_a->GetY()*multiplyY.GetX()) +
                        (ptrZAxis_a->GetZ()*multiplyZ.GetX())),
                       ((ptrZAxis_a->GetX()*multiplyX.GetY()) +
                        (ptrZAxis_a->GetY()*multiplyY.GetY()) +
                        (ptrZAxis_a->GetZ()*multiplyZ.GetY())),
                       ((ptrZAxis_a->GetX()*multiplyX.GetZ()) +
                        (ptrZAxis_a->GetY()*multiplyY.GetZ()) +
                        (ptrZAxis_a->GetZ()*multiplyZ.GetZ())));
    return(CoordinateFrame(newX, newY, newZ));
}

/* ***************************************************************************** */

SpaceVector
CoordinateFrame::MultiplyBy(const SpaceVector &multiplyVector)
{
    const double b1 = multiplyVector.GetX();
    const double b2 = multiplyVector.GetY();
    const double b3 = multiplyVector.GetZ();
    const double newX = (ptrXAxis_a->GetX()*b1) + (ptrXAxis_a->GetY()*b2) + (ptrXAxis_a->GetZ()*b3);
    const double newY = (ptrYAxis_a->GetX()*b1) + (ptrYAxis_a->GetY()*b2) + (ptrYAxis_a->GetZ()*b3);
    const double newZ = (ptrZAxis_a->GetX()*b1) + (ptrZAxis_a->GetY()*b2) + (ptrZAxis_a->GetZ()*b3);
    return(SpaceVector(newX, newY, newZ));
}

/* ***************************************************************************** */

const SpaceVector
CoordinateFrame::GetXAxis() const
{
    return(*ptrXAxis_a);
}

/* ***************************************************************************** */

const SpaceVector
CoordinateFrame::GetYAxis() const
{
    return(*ptrYAxis_a);
}

/* ***************************************************************************** */

const SpaceVector
CoordinateFrame::GetZAxis() const
{
    return(*ptrZAxis_a);
}

/* ***************************************************************************** */

CoordinateFrame::~CoordinateFrame()
{
    return;
}

