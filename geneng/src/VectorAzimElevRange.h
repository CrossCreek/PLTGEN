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
 * VectorAzimElevRange.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
//  FILE      : VectorAzimElevRange.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : VectorAzimElevRange

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  1998-10-21  JAM  alpha development
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.3-009  2007-12-10  JAM  Deleted inline from all class decl
//                                         Reduced use of virtual
//                                         Minor restructuring

//  NOTES :
//  VectorAzimElevRange is a specialization of VectorSpherical which provides
//  a spherical representation of a 3-space vector.  VectorAzimElevRange
//  describes the direction of a vector in terms of azimuth and elevation;
//  vector magnitude is described as range.  The azimuth angle represents an
//  angle measured from the principle axis to the projection of the vector in
//  the fundamental plane, clockwise.  If normalization is enabled, azimuth is
//  normalized as 0 <= azmuith < 2*PI.  The second angle, elevation, represents
//  an angle measured from the projection of the vector in the fundamental
//  plane to the vector.  If normalization is enabled, elevation is normalized
//  as PI/2 <= elevation <= PI/2 with alpha adjusted to ensure the direction of
//  the vector is unchanged.  The magnitude of the vector is represented by
//  range.  If normalization is enabled, range is adjusted to be positive with
//  azimuth and elevation adjusted to ensure the direction of the vector is
//  unchanged.  By default, normalization is turned on, if VectorAzimElevRange
//  is used to store rates or higher order vectors, normalization should be
//  turned off!  Through implicit casting, VectorSpherical vector algebra
//  operators and functions are accessible.    VectorAzimElevRange is a virtual
//  class derived from VectorSpherical.

//  Note, internally azimuth is stored as VectorVectorSpherical alpha, an
//  counter clockwise angle (-PI, PI)

//  UNITS:
//  Function Arg & Return: angles in radians
//  Stream I/O:            angle units controlled by ioAngle manipulators
//  Initialization Files:  N/A

//------------------------------------------------------------------------------
//  Public Class Interface
//  Through inheritance, VectorAzimElevRange exposes the public VectorSpherical interface as public.
//
//  Typedefs
//  None.
//
//  Constructors
//  VectorAzimElevRange(bool bDoNormalize=true);
//  Default constructor; initializes all elements with 0.0 and sets
//  normalization state, on by default.
//
//  VectorAzimElevRange(double value, bool bDoNormalize=true);
//  Conversion constructor; initializes all elements with value and sets
//  normalization state, on by default.
//
//  VectorAzimElevRange(const VectorCartesian &cartesian,
//                      bool bDoNormalize=true);
//  Conversion constructor; initializes elements via a Cartesian to
//  Azimuth-Elevation-Range transformation and sets normalization state,
//  on by default.
//
//  VectorAzimElevRange(const VectorSpherical &spherical);
//  Conversion constructor; initializes elements via a Spherical to
//  Azimuth-Elevation-Range transformation and the sets normalization state
//  to the state set in spherical.
//
//  VectorAzimElevRange(Angle aziumth, Angle elevation,
//                      double range, bool bDoNormalize=true);
//  Alternative constructor; initializes elements with argument list values
//  and sets normalization state, on by default.
//
//  The VectorAzimElevRange class doesn’t contain dynamically allocated
//  resources, a defined copy constructor and assignment operator is not
//  necessary.
//
//  Member Functions
//  Angle SetAzimuth(Angle azimuth)
//  Sets azimuth angle to the argument value.  Returns the argument value.
//
//  Angle SetElevation(Angle elevation)
//  Sets elevation angle to the argument value.  Returns the argument value.
//
//  double SetRange(double range);
//  Sets range to the argument value.  Returns the argument value.
//
//  Angle GetAzimuth() const;
//  Returns azimuth angle.
//
//  Angle GetElevation() const;
//  Returns elevation angle.
//
//  double GetRange() const
//  Returns range.
//
//  virtual void Set(Angle azimuth, Angle elevation, double range);
//  Sets azimuth and elevation angles and range.
//
//  virtual void Get(Angle& azimuth, Angle& elevation,
//                   double& range) const;
//  Assigns azimuth and elevation angles and range to argument
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
//  operator<<(std::ostream& os, VectorAzimElevRange& aer);
//  Inserts class elements into the stream in sequence: azimuth, elevation and
//  range.  Use ioAngle manipulators to control unit conversion.
//
//  std::istream&
//  operator>>(std::istream& is, VectorAzimElevRange& aer);
//  Assigns extracted elements from the stream to the class in sequence:
//  azimuth, elevation and range.  Use ioAngle manipulators to control unit
//  conversion.
//
//  Manipulators
//  See VectorSpherical.

//------------------------------------------------------------------------------
//  Contents derived from SE and SST Ver 1.0.
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//****************************************************************************//
#ifndef VECTORAZIMELEVRANGE_H
#define VECTORAZIMELEVRANGE_H "VectorAzimElevRange V4.3-009  2007-12-10"

#include "VectorSpherical.h"

// =============================================================================
// A3 namespace start
namespace A3
{
//  forward declarations
class VectorAzimElevRange;

// extraction & insertion operators
std::istream&
operator>>(std::istream& is, VectorAzimElevRange& azelrg);

std::ostream&
operator<<(std::ostream& os, const VectorAzimElevRange& azelrg);

// -------------------------------------------------------------------------
class VectorAzimElevRange : public VectorSpherical
{
public:
    // constructors



    explicit VectorAzimElevRange(bool bDoNormalize=true);

    VectorAzimElevRange(Angle aziumth, Angle elevation, double range,
                        bool bDoNormalize=true);

    VectorAzimElevRange(double value,
                        bool bDoNormalize=true);

    VectorAzimElevRange(const VectorCartesian &cartesian,
                        bool bDoNormalize=true);



    explicit VectorAzimElevRange(const VectorSpherical &spherical);

    // access functions
    virtual void Set(Angle azimuth, Angle elevation, double range);
    Angle SetAzimuth(Angle azimuth);
    Angle SetElevation(Angle elevation);
    double SetRange(double range);

    virtual void Get(Angle& azimuth, Angle& elevation,
                     double& range) const;
    Angle GetAzimuth() const;
    Angle GetElevation() const;
    double GetRange() const;

    // class version
    virtual const char *GetVersion() const;
};

// -------------------------------------------------------------------------
// constructors
inline
VectorAzimElevRange::VectorAzimElevRange(bool bDoNormalize)
    : VectorSpherical(bDoNormalize)
{
    SetNormalizeInterval(NEG_TWO_PI_ZERO);
}

inline
VectorAzimElevRange::VectorAzimElevRange(Angle aziumth, Angle elevation,
        double range, bool bDoNormalize)
    : VectorSpherical(-aziumth, elevation,  range, bDoNormalize)
{
    SetNormalizeInterval(NEG_TWO_PI_ZERO);
}

inline
VectorAzimElevRange::VectorAzimElevRange(double value, bool bDoNormalize)
    : VectorSpherical(-value,  value,  value, bDoNormalize)
{
    SetNormalizeInterval(NEG_TWO_PI_ZERO);
}

inline
VectorAzimElevRange::VectorAzimElevRange(const VectorCartesian &rec,
        bool bDoNormalize)
    : VectorSpherical(rec, bDoNormalize)
{
    SetNormalizeInterval(NEG_TWO_PI_ZERO);
}

inline
VectorAzimElevRange::VectorAzimElevRange(const VectorSpherical &sph)
    : VectorSpherical(sph)
{
    SetNormalizeInterval(NEG_TWO_PI_ZERO);
}

// -------------------------------------------------------------------------
// access functions
inline void
VectorAzimElevRange::Set(Angle azimuth, Angle elevation, double range)
{
    VectorSpherical::Set(-azimuth, elevation, range);
}

inline void
VectorAzimElevRange::Get(Angle& azimuth, Angle& elevation, double& range) const
{
    azimuth   = GetAzimuth();
    elevation = GetElevation();
    range     = GetRange();
}

inline Angle
VectorAzimElevRange::SetAzimuth(Angle azimuth)
{
    SetAlpha(-azimuth);
    return GetAzimuth();
}

inline Angle
VectorAzimElevRange::SetElevation(Angle elevation)
{
    return SetDelta(elevation);
}

inline double
VectorAzimElevRange::SetRange(double range)
{
    return SetRho(range);
}

inline Angle
VectorAzimElevRange::GetAzimuth() const
{
    return -GetAlpha();
}

inline Angle
VectorAzimElevRange::GetElevation() const
{
    return GetDelta();
}

inline double
VectorAzimElevRange::GetRange() const
{
    return GetRho();
}

// -------------------------------------------------------------------------
// class version
inline const char
*VectorAzimElevRange::GetVersion() const
{
    return VECTORAZIMELEVRANGE_H ;
}
}
// A3 namespace end
// ============================================================================
#endif // VECTORAZIMELEVRANGE_H
