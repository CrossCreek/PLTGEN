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
 * VectorRightAscDeclRadius.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
//  FILE      : VectorRightAscDeclRadius.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : VectorRightAscDeclRadius

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  981021  JAM  alpha development
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.3-009  2007-12-10  JAM  Deleted inline from all class decl
//                                         Reduced use of virtual
//                                         Minor restructuring

//  NOTES :
//  VectorRightAscDeclRadius is a specialization of VectorSpherical which
//  provides a spherical representation of a 3-space vector.
//  VectorRightAscDeclRadius describes the direction of a vector in terms of
//  right ascension and declination; vector magnitude is described as range.
//  The right ascension angle represents an angle measured from the principle
//  axis to the projection of the vector in the fundamental plane,
//  counter-clockwise.  If normalization is enabled, right ascension is
//  normalized as 0 <= right ascension < 2*PI.  The declination angle
//  represents an angle measured from the projection of the vector in the
//  fundamental plane to the vector.  If normalization is enabled, declination
//  is normalized as -PI/2 <= declination <= PI/2 with right ascension
//  adjusted to ensure the direction of the vector is unchanged.  The
//  magnitude of the vector is represented by range.  If normalization is
//  enabled, range is adjusted to be positive with right ascension and
//  declination adjusted to ensure the direction of the vector is unchanged.
//  By default, normalization is turned on, if VectorRightAscDeclRadius is
//  used to store rates or higher order vectors, normalization should be
//  turned off!  Through implicit casting, VectorSpherical vector algebra
//  operators and functions are accessible.  VectorRightAscDeclRadius is a
//  virtual class derived from VectorSpherical.

//  UNITS:
//  Function Arg & Return: angles in radians
//  Stream I/O:            angle units controlled by ioAngle manipulators
//  Initialization Files:  N/A

//------------------------------------------------------------------------------
//  Public Class Interface
//  Through inheritance, VectorRightAscDeclRadius exposes the public
//  VectorSpherical interface as public.
//
//  Typedefs
//  None.
//
//  Constructors
//  VectorRightAscDeclRadius(bool bDoNormalize=true);
//  Default constructor; initializes all elements with 0.0 and sets
//  normalization state, on by default.
//
//  VectorRightAscDeclRadius(double value, bool bDoNormalize=true);
//  Conversion constructor; initializes all elements with value and sets
//  normalization state, on by default.
//
//  VectorRightAscDeclRadius(const VectorCartesian &cartesian,
//                      bool bDoNormalize=true);
//  Conversion constructor; initializes elements via a Cartesian to Right
//  Ascension-Declination-Radius transformation and sets normalization state,
//  on by default.
//
//  VectorRightAscDeclRadius(const VectorSpherical &spherical);
//  Conversion constructor; initializes elements via a spherical to Right
//  Ascension-Declination-Radius transformation and the sets normalization
//  state to the state set in spherical.
//
//  VectorRightAscDeclRadius(Angle rightAsc, Angle declination,
//                     double radius, bool doNormalize = true);
//  Alternative constructor; initializes elements with argument list values
//  and sets normalization state, on by default.
//
//  The VectorRightAscDeclRadius class doesn’t contain dynamically
//  allocated resources, a defined copy constructor and assignment operator
//  is not necessary.
//
//  Member Functions
//  Angle SetRightAscension(Angle rightAsc)
//  Sets rightAsc angle to the argument value.  Returns the argument value.
//
//  Angle SetDeclination(Angle declination)
//  Sets declination angle to the argument value.  Returns the argument value.
//
//  double SetRadius(double radius);
//  Sets radius to the argument value.  Returns the argument value.
//
//  Angle GetRightAscension() const;
//  Returns right ascension angle.
//
//  Angle GetDeclination() const;
//  Returns declination angle.
//
//  double GetRadius() const
//  Returns radius.
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
//  operator<<(std::ostream& os, VectorRightAscDeclRadius& rdr);
//  Inserts class elements into the stream in sequence: right ascension,
//  declination and radius.  Use ioAngle manipulators to control unit
//  conversion.
//
//  std::istream&
//  operator>>(std::istream& is, VectorRightAscDeclRadius& rdr);
//  Assigns extracted elements from the stream to the class in sequence: right
//  ascension, declination and radius.  Use ioAngle manipulators to control
//  unit conversion.
//
//  Manipulators
//  See VectorSpherical.

//------------------------------------------------------------------------------
//  Contents derived from SE and SST Ver 1.0.
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//****************************************************************************//
#ifndef VECTORRIGHTASCDECLRADIUS_H
#define VECTORRIGHTASCDECLRADIUS_H \
    "VectorRightAscDeclRadius V4.3-009  2007-12-10"

#include "VectorSpherical.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// forward declarations
class VectorRightAscDeclRadius;

// extraction & insertion stream operators
std::istream&
operator>>(std::istream& is, VectorRightAscDeclRadius& rdr);

std::ostream&
operator<<(std::ostream& os, const VectorRightAscDeclRadius& rdr);

// -------------------------------------------------------------------------
class VectorRightAscDeclRadius : public VectorSpherical
{
public:
    // constructors



    explicit VectorRightAscDeclRadius(bool doNormalize = true);

    VectorRightAscDeclRadius(Angle rightAsc, Angle declination,
                             double radius, bool doNormalize = true);

    VectorRightAscDeclRadius(const VectorCartesian &cartesian,
                             bool doNormalize = true );



    explicit VectorRightAscDeclRadius(const VectorSpherical &spherical);

    Angle SetRightAscension(Angle rightAscension);
    Angle SetDeclination(Angle declination);
    double SetRadius(double radius);

    Angle GetRightAscension() const;
    Angle GetDeclination() const;
    double GetRadius() const;

    // general class functions
    virtual const char *GetVersion() const;
};

// -------------------------------------------------------------------------
inline
VectorRightAscDeclRadius::
VectorRightAscDeclRadius(bool doNormalize)
    : VectorSpherical(doNormalize)
{
    SetNormalizeInterval(ZERO_TWO_PI);
}

inline
VectorRightAscDeclRadius::
VectorRightAscDeclRadius(Angle rightAscension, Angle declination,
                         double radius, bool doNormalize)
    : VectorSpherical(rightAscension, declination, radius, doNormalize)
{
    SetNormalizeInterval(ZERO_TWO_PI);
}

inline
VectorRightAscDeclRadius::
VectorRightAscDeclRadius(const VectorCartesian &cartesian, bool doNormalize)
    : VectorSpherical(cartesian, doNormalize)
{
    SetNormalizeInterval(ZERO_TWO_PI);
}

inline
VectorRightAscDeclRadius::
VectorRightAscDeclRadius(const VectorSpherical &spherical)
    : VectorSpherical(spherical)
{
    SetNormalizeInterval(ZERO_TWO_PI);
}

// -------------------------------------------------------------------------
inline Angle
VectorRightAscDeclRadius::SetRightAscension(Angle rightAscension)
{
    return SetAlpha(rightAscension);
}

inline Angle
VectorRightAscDeclRadius::SetDeclination(Angle declination)
{
    return SetDelta(declination);
}

inline double
VectorRightAscDeclRadius::SetRadius(double radius)
{
    return SetRho(radius);
}

inline Angle
VectorRightAscDeclRadius::GetRightAscension() const
{
    return GetAlpha();
}

inline Angle
VectorRightAscDeclRadius::GetDeclination() const
{
    return GetDelta();
}

inline double
VectorRightAscDeclRadius::GetRadius() const
{
    return GetRho();
}

// -------------------------------------------------------------------------
// class version
inline const char *VectorRightAscDeclRadius::GetVersion() const
{
    return VECTORRIGHTASCDECLRADIUS_H;
}
}
// A3 namespace end
// ============================================================================
#endif // VECTORRIGHTASCDECLRADIUS_H
