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
 * VectorGeodetic.h
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

//------------------------------------------------------------------------------
//  Public Class Interface
//  Through inheritance, VectorGeodetic hides the public VectorSpherical
//  interface as protected.  Exception, VectorSpherical functions IsNormalized
//  and SetNormalize are exposed as public.
//
//  Typedefs
//  None.
//
//  Constructors
//  VectorGeodetic(bool bDoNormalize=true);
//  Default constructor; initializes all elements with 0.0 and sets
//  normalization state, on by default.
//
//  VectorGeodetic(double value, bool bDoNormalize=true);
//  Conversion constructor; initializes all elements with value and sets
//  normalization state, on by default.
//
//  VectorGeodetic(const VectorCartesian &cartesian,
//                 bool bDoNormalize=true);
//  Conversion constructor; initializes elements via a Cartesian to geocentric
//  transformation and sets normalization state, on by default.
//
//  VectorGeodetic(const VectorGeocentric &geocentricLlr);
//  Conversion constructor; initializes elements via a geocentric to geodetic
//  transformation and the sets normalization state to the state set in
//  geodeticLlr.
//
//  VectorGeodetic(Angle latitude, Angle longitude,
//                 double height, bool bDoNormalize=true);
//  Alternative constructor; initializes elements with argument list values
//  and sets normalization state, on by default.
//
//  The VectorGeodetic class doesn’t contain dynamically allocated resources,
//  a defined copy constructor and assignment operator is not necessary.
//
//  Member Functions
//  Angle SetLatitude(Angle latitude)
//  Sets latitude angle to the argument value.  Returns the argument value.
//
//  Angle SetLongitude(Angle longitude)
//  Sets longitude angle to the argument value.  Returns the argument value.
//
//  double SetHeight(double height);
//  Sets height to the argument value.  Returns the argument value.
//
//  Angle GetLatitude() const;
//  Returns latitude angle.
//
//  Angle GetLongitude() const;
//  Returns longitude angle.
//
//  double GetHeight() const
//  Returns height.
//
//  virtual void Set(Angle latitude, Angle longitude,
//                   double height);
//  Sets latitude and longitude angles and height.
//
//  virtual void Get(Angle& latitude, Angle& longitude,
//                   double& height) const;
//  Assigns latitude and longitude angles and height to argument
//  list parameters.
//
//  virtual const char *GetVersion() const;
//  Returns class name and version.
//
//  double GetRadiusOfCurvature() const;
//  Returns the radius of curvature.
//
//  double GetRadiusToRefEllipse() const;
//  Returns radius to the reference ellipse (WGS-84)
//
//  Operators
//  bool operator==(const VectorGeodetic& llh) const;
//  Returns true if all element-wise comparisons between this and llh are
//  true, else returns false.
//
//  bool operator!=(const VectorGeodetic& llh) const;
//  Returns !operator==(llh).
//  External Functions
//  Ordinary Functions
//  None.
//
//  Operators
//  std::ostream&
//  operator<<(std::ostream& os, VectorGeodetic& llh);
//  Inserts class elements into the stream in sequence: latitude, longitude
//  and height.  Use ioAngle manipulators to control unit conversion.
//
//  std::istream&
//  operator>>(std::istream& is, VectorGeodetic& llh);
//  Assigns extracted elements from the stream to the class in sequence:
//  latitude, longitude and height.  Use ioAngle manipulators to control
//  unit conversion.
//
//  Manipulators
//  See VectorSpherical.

//------------------------------------------------------------------------------
//  Contents derived from SE and SST Ver 1.0.
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//****************************************************************************//
#ifndef VECTORGEODETIC_H
#define VECTORGEODETIC_H  "VectorGeodetic V4.3-009  2007-12-10"

#include "VectorSpherical.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// forward declarations
class VectorGeodetic;
class VectorGeocentric;

// extraction & insertion operators
std::istream&
operator>>(std::istream& is, VectorGeodetic& llh);

std::ostream&
operator<<(std::ostream& os, const VectorGeodetic& llh);

// -------------------------------------------------------------------------
class VectorGeodetic : protected VectorSpherical
{
public:
    // constructors


    VectorGeodetic(bool bDoNormalize=true);
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit VectorGeodetic(bool bDoNormalize=true);

    VectorGeodetic(Angle latitude, Angle longitude, double height,
                   bool bDoNormalize=true);

    VectorGeodetic(const VectorGeodetic &vg) : VectorSpherical(vg) {;}

    VectorGeodetic(double value, bool bDoNormalize=true);

    VectorGeodetic(const VectorCartesian &cartesian,
                   bool bDoNormalize=true);



    VectorGeodetic(const VectorGeocentric &geocentricLlr);
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit VectorGeodetic(const VectorGeocentric &geocentricLlr);

    // access functions
    void Set(Angle latitude, Angle longitude, double height);
    void Get(Angle& latitude, Angle& longitude, double& height) const;
    Angle SetLatitude(Angle latitude);
    Angle SetLongitude(Angle longitude);
    double SetHeight(double height);

    Angle GetLatitude() const;
    Angle GetLongitude() const;
    double GetHeight() const;

    // geodetic functions
    double GetRadiusOfCurvature() const;
    double GetRadiusToRefEllipse() const;

    // operators
    bool operator==(const VectorGeodetic& vec) const
    {
        return !memcmp(m_v, vec.m_v, 3*sizeof(double));
    }

    bool operator!=(const VectorGeodetic& vec) const
    {
        return !operator==(vec);
    }

    // class version
    virtual const char *GetVersion() const;

    // expose VectorSpherical normalization functions
    using VectorSpherical::IsNormalized;
    using VectorSpherical::SetNormalize;

private:
    static double GetRadiusOfCurvature(double sinLat);
};

// -------------------------------------------------------------------------
// constructors
inline
VectorGeodetic::VectorGeodetic(bool bDoNormalize)
    : VectorSpherical(bDoNormalize)
{
    SetNegRadiusAllow(true);
}

inline
VectorGeodetic::VectorGeodetic(double value, bool bDoNormalize)
    : VectorSpherical(value, value, value, bDoNormalize)
{
    SetNegRadiusAllow(true);
}

inline
VectorGeodetic::VectorGeodetic(Angle latitude, Angle longitude,
                               double height, bool bDoNormalize)
    : VectorSpherical(longitude, latitude, height, bDoNormalize)
{
    SetNegRadiusAllow(true);
}

// -------------------------------------------------------------------------
// access functions
inline void
VectorGeodetic::Set(Angle latitude, Angle longitude, double height)
{
    SetLatitude(latitude);
    SetLongitude(longitude);
    SetHeight(height);
}

inline void
VectorGeodetic::Get(Angle& latitude, Angle& longitude, double& height) const
{
    latitude = GetLatitude();
    longitude = GetLongitude();
    height = GetHeight();
}

inline Angle
VectorGeodetic::SetLongitude(Angle longitude)
{
    return SetAlpha(longitude);
}

inline Angle
VectorGeodetic::SetLatitude(Angle latitude)
{
    return SetDelta(latitude);
}

inline double
VectorGeodetic::SetHeight(double height)
{
    return SetRho(height);
}

inline Angle
VectorGeodetic::GetLongitude() const
{
    return GetAlpha();
}

inline Angle
VectorGeodetic::GetLatitude() const
{
    return GetDelta();
}

inline double
VectorGeodetic::GetHeight() const
{
    return GetRho();
}

// -------------------------------------------------------------------------
// class version
inline const char *VectorGeodetic::GetVersion() const
{
    return VECTORGEODETIC_H;
}
}
// A3 namespace end
// ============================================================================
#endif // VECTORGEODETIC_H
