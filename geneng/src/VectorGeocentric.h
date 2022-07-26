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
 * VectorGeocentric.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
//  FILE      : VectorGeocentric.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : VectorGeocentric

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  981021      JAM  Initial Release
//              V1.3-006  12/20/1999  JAM  reorganized manipulator decl & defs
//                                         to reduce compile time overhead;
//                                         made IoManipulator more
//                                         flexible; changed ioSeparator from
//                                         global to stream dependent (hence
//                                         more flexible) like other manips
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.3-009  2007-12-10  JAM  Deleted inline from all class decl
//                                         Reduced use of virtual
//                                         Minor restructuring
//              V4.5-001   2009-07-01 JAM  Move ioGeocentricManip inst. to
//                                         unnamed namespace
//                                         Added stdexcept include to cpp file
//              V5.0       2010-01-28 JAM  Replaced manipulator with new
//                                         template version

//  NOTES :
//  VectorGeocentric is a specialization of VectorSpherical which provides a
//  spherical representation of a 3-space vector.  VectorGeocentric describes
//  the direction of a vector in terms of latitude and longitude; vector
//  magnitude is described as radius.  The latitude angle represents an angle
//  measured from the projection of the vector in the fundamental plane, i.e.
//  equator, to the vector.  If normalization is enabled, latitude is
//  normalized as -PI/2 < latitude < PI/2 with longitude adjusted to ensure
//  the direction of the vector is unchanged.  The longitude angle represents
//  an angle measured from the principle axis to the projection of the vector
//  in the fundamental plane, counter-clockwise.  If normalization is enabled,
//  longitude is normalized as -PI < longitude < PI.  The magnitude of the
//  vector is represented by radius.  If normalization is enabled, radius is
//  adjusted to be positive with latitude and longitude adjusted to ensure
//  the direction of the vector is unchanged.  By default, normalization is
//  turned on, if VectorGeocentric is used to store rates or higher order
//  vectors, normalization should be turned off!  Through implicit casting,
//  VectorSpherical vector algebra operators and functions are accessible.
//  VectorGeocentric is a virtual class derived from VectorSpherical.

//  UNITS:
//  Function Arg & Return: angles in radians
//                         length in nautical miles
//  Stream I/O:            angle units controlled by ioAngle manipulators
//                         length units are nautical miles, length reference
//                         controlled by ioGeocentric (radius, approx
//                         geodetic height, height w.r.t. equatorial radius)
//  Initialization Files:  N/A

//------------------------------------------------------------------------------
//  Public Class Interface
//  Through inheritance, VectorGeocentric exposes the public VectorSpherical
//  interface as public.
//
//  Typedefs
//  None.
//
//  Constructors
//
//  VectorGeocentric(bool bDoNormalize=true);
//  Default constructor; initializes all elements with 0.0 and sets
//  normalization state, on by default.
//
//  VectorGeocentric(double value, bool bDoNormalize=true);
//  Conversion constructor; initializes all elements with value and sets
//  normalization state, on by default.
//
//  VectorGeocentric(const VectorCartesian &cartesian,
//                   bool bDoNormalize=true);
//  Conversion constructor; initializes elements via a Cartesian to geocentric
//  transformation and sets normalization state, on by default.
//
//  VectorGeocentric(const VectorSpherical &spherical);
//  Conversion constructor; initializes elements via a spherical to geocentric
//  transformation and the sets normalization state to the state set in
//  spherical.
//
//  VectorGeocentric(const VectorGeodetic& geodeticLlh);
//  Conversion constructor; initializes elements via a geodetic to geocentric
//  transformation and the sets normalization state to the state set in
//  geodeticLlh.
//
//  VectorGeocentric(Angle latitude, Angle longitude,
//                   double radius, bool bDoNormalize=true);
//  Alternative constructor; initializes elements with argument list values
//  and sets normalization state, on by default.
//
//  The VectorGeocentric class doesn’t contain dynamically allocated
//  resources, a defined copy constructor and assignment operator is not necessary.
//
//  Member Functions
//  Angle SetLatitude(Angle latitude)
//  Sets latitude angle to the argument value.  Returns the argument value.
//
//  Angle SetLongitude(Angle longitude)
//  Sets longitude angle to the argument value.  Returns the argument value.
//
//  double SetRadius(double radius);
//  Sets radius to the argument value.  Returns the argument value.
//
//  Angle GetLatitude() const;
//  Returns latitude angle.
//
//  Angle GetLongitude() const;
//  Returns longitude angle.
//
//  double GetRadius() const
//  Returns radius.
//
//  virtual void Set(Angle latitude, Angle longitude, double radius);
//  Sets latitude and longitude angles and radius.
//
//  virtual void Get(Angle& latitude, Angle& longitude,
//                   double& radius) const;
//  Assigns latitude and longitude angles and radius to argument
//  list parameters.
//
//  virtual const char *GetVersion() const;
//  Returns class name and version.
//
//  Operators
//  None.
//  External Functions
//  Ordinary Functions
//  None.
//
//  Operators
//  std::ostream&
//  operator<<(std::ostream& os, VectorGeocentric& llr);
//  Inserts class elements into the stream in sequence: latitude, longitude
//  and radius.  Use ioAngle manipulators to control unit conversion.  Use
//  ioGeocentric manipulators to control magnitude reference.
//
//  std::istream&
//  operator>>(std::istream& is, VectorGeocentric& llr);
//  Assigns extracted elements from the stream to the class in sequence:
//  latitude, longitude and radius.  Use ioAngle manipulators to control unit
//  conversion.  Use ioGeocentric manipulators to control magnitude reference.
//
//  Manipulators
//  ioSeparator and ioAngle, see VectorSpherical.
//
//  Across function interfaces, the vector magnitude element is geocentric
//  radius.  For stream I/O, magnitude representation is controlled through
//  the use of ioGeocentric manipulators.  ioGeocentricRadius sets the stream
//  to geocentric radius, no I/O conversion takes place.  ioGeocentricHeightEq
//  sets a stream to height w.r.t. to the equatorial radius; on output, the
//  equatorial radius is subtracted from the geocentric radius; on input, the
//  equatorial radius is added to the input height.  ioGeocentricHeight sets a
//  stream to pseudo-geodetic height; on output, radius is converted from
//  geocentric radius to pseudo-geodetic height; on input, pseudo-geodetic
//  height is converted to geocentric radius.  ioGeocentricRestore restores the
//  stream to the previous representation.  Pseudo-geodetic height is geodetic
//  height computed geocentric radius.  For true geodetic height, cast the
//  geocentric vector as a geodetic vector.

//------------------------------------------------------------------------------
//  Contents derived from SE and SST Ver 1.0.
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//****************************************************************************//
#ifndef VECTORGEOCENTRIC_H
#define VECTORGEOCENTRIC_H  "VectorGeocentric V4.3-009  2007-12-10"

#include "VectorSpherical.h"

// =============================================================================
// A3 namespace start
namespace A3
{
//  forward declarations
class VectorGeocentric;
class VectorGeodetic;

// extraction & insertion operators
std::istream& operator>>(std::istream& is, VectorGeocentric& llr);
std::ostream& operator<<(std::ostream& os, const VectorGeocentric& llr);

// manipulators
std::ios_base& ioGeocentricHeight(std::ios_base& stream);
std::ios_base& ioGeocentricHeightEq(std::ios_base& stream);
std::ios_base& ioGeocentricRadius(std::ios_base& stream);
std::ios_base& ioGeocentricRestore(std::ios_base& stream);

// -------------------------------------------------------------------------
// VectorGeocentric class definition
class VectorGeocentric : public VectorSpherical
{
public:
    // constructors



    explicit VectorGeocentric(bool bDoNormalize=true);

    VectorGeocentric(double value, bool bDoNormalize=true);

    VectorGeocentric(Angle latitude, Angle longitude,
                     double radius, bool bDoNormalize=true);

    VectorGeocentric(const VectorCartesian& cartesian,
                     bool bDoNormalize=true);



    explicit VectorGeocentric(const VectorSpherical& spherical);



    explicit VectorGeocentric(const VectorGeodetic& geodeticLlh);

    // access functions
    virtual void Set(Angle latitude, Angle longitude, double radius);

    virtual void Get(Angle& latitude, Angle& longitude,
                     double& radius) const;

    Angle SetLatitude(Angle latitude);
    Angle SetLongitude(Angle longitude);
    double SetRadius(double radius);

    Angle GetLatitude() const;
    Angle GetLongitude() const;
    double GetRadius() const;

    // class version
    virtual const char *GetVersion() const;
};

// -------------------------------------------------------------------------
// constructors
inline
VectorGeocentric::VectorGeocentric(bool bDoNormalize)
    : VectorSpherical(bDoNormalize) { ; }

inline
VectorGeocentric::VectorGeocentric(double value, bool bDoNormalize)
    : VectorSpherical(value, bDoNormalize) { ; }

inline
VectorGeocentric::VectorGeocentric(Angle latitude, Angle longitude,
                                   double radius, bool bDoNormalize)
    : VectorSpherical(longitude, latitude, radius, bDoNormalize) { ; }

inline
VectorGeocentric::VectorGeocentric(const VectorSpherical& spherical)
    : VectorSpherical(spherical) { ; }

// conversion constructor from cartesian coordinates to geocentric
// coordinates--the spherical conversion constructor is used for
// the actual transformation since geocentric coordinates are really
// spherical coordinates
inline
VectorGeocentric::VectorGeocentric(const VectorCartesian& cartesian,
                                   bool bDoNormalize)
    : VectorSpherical(cartesian, bDoNormalize) { ; }

// -------------------------------------------------------------------------
// access functions
inline void
VectorGeocentric::Set(Angle latitude, Angle longitude, double radius)
{
    SetLatitude(latitude);
    SetLongitude(longitude);
    SetRadius(radius);
}

inline void
VectorGeocentric::Get(Angle& latitude, Angle& longitude,
                      double& radius) const
{
    latitude  = GetLatitude();
    longitude = GetLongitude();
    radius    = GetRadius();
}

inline Angle
VectorGeocentric::SetLongitude(Angle longitude)
{
    return SetAlpha(longitude);
}

inline Angle
VectorGeocentric::SetLatitude(Angle latitude)
{
    return SetDelta(latitude);
}

inline double
VectorGeocentric::SetRadius(double radius)
{
    return SetRho(radius);
}

inline Angle
VectorGeocentric::GetLongitude() const
{
    return GetAlpha();
}

inline Angle
VectorGeocentric::GetLatitude() const
{
    return GetDelta();
}

inline double
VectorGeocentric::GetRadius() const
{
    return GetRho();
}

// -------------------------------------------------------------------------
// class version
inline const char *VectorGeocentric::GetVersion() const
{
    return VECTORGEOCENTRIC_H;
}
}
// A3 namespace end
// =============================================================================
#endif // VECTORGEOCENTRIC_H
