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
 * VectorGeodetic.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
//  FILE      : VectorGeodetic.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : VectorGeodetic

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  981021  JAM  alpha development
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.3-009  2007-12-10  JAM  Deleted inline from all class decl
//                                         Reduced use of virtual
//                                         Minor restructuring

//  NOTES :
//  VectorGeodetic represents the location of a point with respect to the
//  WGS-84 ellipsoid.  VectorGeodetic describes the location in terms of
//  latitude, longitude and height.  The latitude angle represents the angle
//  between the plane of the geodetic equator and the ellipsoidal normal at a
//  point.  If normalization is enabled, latitude is normalized as
//  -PI/2 <= latitude <= PI/2 with longitude adjusted to ensure the direction of
//  the vector is unchanged.  The longitude angle represents an angle measured
//  from the principle axis to the projection of the vector in the fundamental
//  plane, counter-clockwise.  If normalization is enabled, longitude is
//  normalized as -PI <= longitude < PI.  Geodetic height, i.e. height above
//  the reference ellipsoid, is represented by height; negative values are
//  permitted.  VectorGeodetic is a virtual class derived from VectorSpherical.

//  UNITS:
//  Function Arg & Return: angles in radians
//  Stream I/O:            angle units controlled by ioAngle manipulators
//  Initialization Files:  N/A

//  Contents derived from SE and SST Ver 1.0.
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//****************************************************************************//
#include "VectorGeodetic.h"
#include "VectorGeocentric.h"
#include "Angle.h"
#include "IoSeparator.h"
#include <math.h>
#include <iostream>
#include <iomanip>

// ============================================================================
// A3 namespace start
namespace A3
{
// conversion constructor--from geocentric to geodetic coordinates
VectorGeodetic::VectorGeodetic(const VectorGeocentric &llr)
    : VectorSpherical(llr.IsNormalized())
{
    SetNegRadiusAllow(true);
    // transform the geocentric llr vector to geodetic coordinates
    // using the modified Gersten approximation developed by Olson
    // Journal of Guidance, Control, and Dynamics
    // Calculation of Geodetic Coordinates from Earth-Centered,
    // Earth-Fixed Coordinates, pp. 188-190
    // Volume 11, Number 2
    // March-April 1988
    SetLongitude(llr.GetLongitude());
    double sinLatC   = sin(llr.GetLatitude());
    double sinLatCSq = sinLatC * sinLatC;
    double cosLatC   = cos(llr.GetLatitude());
    // compute minor radius and z
    double rho = llr.GetRadius() * cosLatC;
    double z   = llr.GetRadius() * sinLatC;
    // compute transformation parameters for Gersten's method
    double eta = EARTH_RADIUS_NMI * EARTH_ECCENTRICITY_SQ / llr.GetRadius();
    double chi = eta * sinLatC * cosLatC;
    double psi = (eta - EARTH_ECCENTRICITY_SQ / 4.0) * sinLatCSq;
    double deltaCos = 1 - chi * chi / 2.0;
    double deltaSin = chi * (1.0 + eta - 2.0 * psi);
    // compute Gersten's latitude and height
    double sinLatG = sinLatC * deltaCos + cosLatC * deltaSin;
    double cosLatG = cosLatC * deltaCos - sinLatC * deltaSin;
    double latG = asin(sinLatG);
    double heightG = llr.GetRadius() - EARTH_RADIUS_NMI
                     + (EARTH_RADIUS_NMI * EARTH_ECCENTRICITY_SQ * sinLatCSq
                        * (1.0 + eta - psi)) / 2.0;
    // compute parameters for the refinement
    double radiusCurv = GetRadiusOfCurvature(sinLatG);
    double rhoG = (radiusCurv + heightG) * cosLatG;
    double zG   = ((1.0 - EARTH_ECCENTRICITY_SQ) * radiusCurv
                   + heightG) * sinLatG;
    double deltaRho = rho - rhoG;
    double deltaZ   = z - zG;
    double rn = (1.0 - EARTH_ECCENTRICITY_SQ) * radiusCurv
                / (1.0 - EARTH_ECCENTRICITY_SQ * sinLatG * sinLatG);
    double deltaLat = (cosLatG * deltaZ - sinLatG * deltaRho)
                      / (rn + heightG);
    double deltaHgt = cosLatG * deltaRho + sinLatG * deltaZ;
    // apply refinement
    SetLatitude(latG + deltaLat);
    SetHeight(heightG + deltaHgt);
    SetNormalize(llr.IsNormalized());
}

VectorGeodetic::VectorGeodetic(const VectorCartesian &cartesian,
                               bool bDoNormalize)
    : VectorSpherical(bDoNormalize)
{
    SetNegRadiusAllow(true);
    VectorGeocentric llr(cartesian, bDoNormalize);
    *this = llr;
}

// -------------------------------------------------------------------------
// calculate the radius of curvature for the member latitude
double VectorGeodetic::GetRadiusOfCurvature() const
{
    double sinLat = sin(GetLatitude());
    return GetRadiusOfCurvature(sinLat);
}

// calculate the radius of curvature given the sine of latitude
// this is a private member used to support the geocentric to
// geodetic conversion constructor and the radius to the
// reference ellipse calculation
double VectorGeodetic::GetRadiusOfCurvature(double sinLat)
{
    return EARTH_RADIUS_NMI
           / sqrt(1.0 - EARTH_ECCENTRICITY_SQ * sinLat * sinLat);
}

// calculate the radius to the reference ellipse for the
// member latitude
double VectorGeodetic::GetRadiusToRefEllipse() const
{
    double cosLat = cos(GetLatitude());
    double sinLat = sin(GetLatitude());
    double eta = (1.0 - EARTH_ECCENTRICITY_SQ) * sinLat;
    double radiusCurvSq = EARTH_RADIUS_SQ_NMI2
                          / (1.0 - EARTH_ECCENTRICITY_SQ * sinLat * sinLat);
    return sqrt(radiusCurvSq * (cosLat * cosLat + eta * eta));
}

// -------------------------------------------------------------------------
// extraction & insertion operators
std::istream&
operator>>(std::istream& is, VectorGeodetic& llh)
{
    Angle angle1, angle2;
    double height;
    is >> angle1 >> angle2 >> height;
    llh.Set(angle1, angle2, height);
    return is;
}

std::ostream&
operator<<(std::ostream& os, const VectorGeodetic& llh)
{
    //WARNING_FIX:
    //1>src\VectorGeodetic.cpp(160): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'int', possible loss of data
    long long int w = os.width();
    os << llh.GetLatitude() << ioSeparator << std::setw(w)
       << llh.GetLongitude() << ioSeparator << std::setw(w)
       << llh.GetHeight();
    return os;
}
}
// A3 namespace end
// ============================================================================
