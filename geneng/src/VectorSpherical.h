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
 * VectorSpherical.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
//  FILE      : VectorSpherical.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : VectorSpherical

//  AUTHOR(s) : J. McCormick
//              C. Larsen

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
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

//              V4.5-001  2009-07-01  JAM  Added const qualifier to the
//                                         VectorSpherical argument of the
//                                         insertion operator

//  NOTES :
//  VectorSpherical provides a spherical representation of a 3-space vector and
//  defines common vector algebra functions.  The interface permits the user to
//  treat the vector as two angles, representing direction, and a magnitude.
//  The first angle, alpha, represents an angle measured from the principle
//  axis to the projection of the vector in the fundamental plane,
//  counter-clockwise.  If normalization is enabled, alpha is normalized as
//  PI <= alpha < PI.  The second angle, delta, represents an angle measured
//  from the projection of the vector in the fundamental plane to the vector.
//  If normalization is enabled, delta is normalized as PI/2 <= alpha <= PI/2
//  with alpha adjusted to ensure the direction of the vector is unchanged.
//  The magnitude of the vector is represented by rho.   If normalization is
//  enabled, rho is adjusted to be positive with alpha adjusted to ensure the
//  direction of the vector is unchanged.  By default, normalization is turned
//  on, if VectorSpherical is used to store rates or higher order vectors,
//  normalization should be turned off!  Through implicit casting, VectorCartesian vector algebra
//  operators and functions that are not specialized in VectorSpherical are
//  accessible. VectorSpherical is a virtual class derived from VectorRead3D.

//  UNITS:
//  Function Arg & Return: angles in radians
//  Stream I/O:            angle units controlled by ioAngle manipulators
//  Initialization Files:  N/A

//------------------------------------------------------------------------------
//  Public Class Interface
//  Typedefs
//
//  Constructors
//  VectorSpherical(bool bDoNormalize=true);
//  Default constructor; initializes all elements with 0.0 and sets
//  normalization state, on by default.
//
//  VectorSpherical(double value, bool bDoNormalize=true);
//  Conversion constructor; initializes all elements with value and sets
//  normalization state, on by default.
//
//  VectorSpherical(Angle alpha, Angle delta, double rho,
//                  bool bDoNormalize=true);
//  Alternative constructor; initializes elements with argument list values
//  and sets normalization state, on by default.
//
//  VectorSpherical(const VectorCartesian &cart,
//                  bool bDoNormalize=true);
//  Conversion constructor; initializes elements via a Cartesian to spherical
//  transformation and sets normalization state, on by default.
//
//  The VectorSpherical class doesn’t contain dynamically allocated resources,
//  a defined copy constructor and assignment operator is not necessary.
//
//  Member Functions
//  Angle SetAlpha(Angle alpha);
//  Sets alpha angle to the argument value.  Returns the
//  argument value.
//
//  Angle SetDelta(Angle delta);
//  Sets delta angle to the argument value.  Returns the
//  argument value.
//
//  double SetRho(double rho);
//  Sets rho to the argument value.  Returns the
//  argument value.
//
//  Angle GetAlpha() const;
//  Returns alpha angle.
//
//  Angle GetDelta() const;
//  Returns delta angle.
//
//  double GetRho() const;
//  Return rho.
//
//  virtual void Set(Angle alpha, Angle delta, double rho);
//  Sets alpha and delta angles and rho.
//
//  virtual void Get(Angle& alpha, Angle& delta, double& rho);
//  Assigns alpha and delta angles and rho to argument list
//  parameters.
//
//  double MagnitudeSquared() const;
//  Returns the squared magnitude of this vector.
//
//  double Magnitude() const;
//  Returns the magnitude of this vector.
//
//  VectorSpherical Unit() const;
//  Returns VectorSpherical set to the unit vector derived from this.
//
//  VectorSpherical Negate() const;
//  Returns VectorSpherical set to negative this.
//
//  double DotProduct(const VectorSpherical& sph) const;
//  Returns the dot product of this and sph.
//
//  VectorSpherical CrossProduct(const VectorSpherical& sph) const;
//  Returns the cross product of this and sph.
//
//  Angle SeparationAngle(const VectorSpherical& sph) const;
//  Returns the angle between this and sph.
//
//  bool IsNormalized() const;
//  Returns true if normalization is enable, false otherwise.
//
//  void SetNormalize(bool bDoNormalize);
//  Enables normalization if argument is true, disables normalization
//  otherwise.
//
//  virtual const char *GetVersion() const;
//  Returns class name and version.
//
//  Operators
//  VectorSpherical& operator+=(const VectorSpherical& sph);
//  Adds this vector with sph vector and assigns results to this.  Returns
//  this reference.
//
//  VectorSpherical& operator-=(const VectorSpherical& sph);
//  Subtracts sph vector from this vector and assigns results to this.
//  Returns this reference.
//
//  VectorSpherical& operator*=(double scalar);
//  Multiplies this vector with scalar value and assigns results to this.
//  Returns this reference.
//
//  VectorSpherical& operator/=(double scalar);
//  Divides this vector with scalar value and assigns results to this.
//  Returns this reference.
//
//  VectorSpherical operator+(const VectorSpherical& sph) const;
//  Adds this vector with sph vector and returns the results as a
//  VectorSpherical.
//
//  VectorSpherical operator-(const VectorSpherical& sph) const;
//  Subtracts sph vector from this vector and returns the results as a
//  VectorSpherical.
//
//  VectorSpherical operator*(double scalar) const;
//  Multiplies this vector with scalar value and returns the results as a
//  VectorSpherical.
//
//  VectorSpherical operator/(double scalar) const;
//  Divides this vector with scalar value and returns the results as a
//  VectorSpherical.
//
//  bool operator==(const VectorSpherical& sph) const;
//  Returns true if all element-wise comparisons between this and sph are
//  true, else returns false.
//
//  bool operator!=(const VectorSpherical& sph) const;
//  Returns !operator==(sph).
//
//  bool operator<(const VectorSpherical& sph) const;
//  Returns true if the magnitude of this is less than the magnitude of sph,
//  else returns false
//
//  bool operator<=(const VectorSpherical& sph) const;
//  Returns true if the magnitude of this is less than or equal to the
//  magnitude of sph, else returns false.
//
//  bool operator>(const VectorSpherical& sph) const;
//  Returns true if the magnitude of this is greater than the magnitude of
//  sph, else returns false.
//
//  bool operator>=(const VectorSpherical& sph) const;
//  Returns true if the magnitude of this is greater than or equal to the
//  magnitude of sph, else returns false.
//
//  External Functions
//  Ordinary Functions
//  None.
//
//  Operators
//  VectorSpherical
//  operator+(const VectorSpherical &spherical);
//  Returns VectorSpherical set with spherical.
//
//  VectorSpherical
//  operator-(const VectorSpherical &spherical);
//  Returns VectorSpherical set to negative spherical.
//
//  std::ostream&
//  operator<<(std::ostream& os, const VectorSpherical& spherical);
//  Inserts class elements into the stream in sequence: alpha, delta and rho.
//  Use ioAngle manipulators to control unit conversion.
//
//  std::istream&
//  operator>>(std::istream& is, VectorSpherical& spherical);
//  Assigns extracted elements from the stream to the class in sequence:
//  alpha, delta and rho.  Use ioAngle manipulators to control unit
//  conversion.
//
//  Manipulators
//  ioSeparator, see VectorReal3D.
//
//  Across function interfaces, the angles unit is radians.  For stream I/O,
//  angle units are controlled through the use of ioAngle manipulators.
//  ioAngleDegrees sets a stream to the degree angle unit; on output angles are
//  converted from radians to degrees, on input angles are converted from
//  degrees to radians.  ioAngleRadians sets the stream angle unit to radians,
//  no conversion takes place.  The default angle unit of a stream is radians.
//  ioAngleRestore restores the stream to the previous angle unit.

//------------------------------------------------------------------------------
//  Contents derived from SE and SST Ver 1.0.
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//****************************************************************************//
#ifndef VECTORSPHERICAL_H
#define VECTORSPHERICAL_H  "VectorSpherical V4.3-009  2007-12-10"

#include "VectorReal3D.h"
#include "Angle.h"

// ============================================================================
// A3 namespace start
namespace A3
{
//  forward declarations
class VectorCartesian;
class VectorSpherical;

// inline i/o stream operators
std::istream&
operator>>(std::istream& is, VectorSpherical& spherical);

std::ostream&
operator<<(std::ostream& os, const VectorSpherical& spherical);

VectorSpherical
operator+(const VectorSpherical &spherical);

VectorSpherical
operator-(const VectorSpherical &spherical);

// -------------------------------------------------------------------------
// VectorSpherical class definition
class VectorSpherical : protected VectorReal3D
{
public:
    // constructors



    explicit VectorSpherical(bool bDoNormalize=true);

    VectorSpherical(double value, bool bDoNormalize=true);

    VectorSpherical(Angle alpha, Angle delta, double rho,
                    bool bDoNormalize=true);

    VectorSpherical(const VectorCartesian &cart,
                    bool bDoNormalize=true);

    // access
    Angle SetAlpha(Angle alpha);
    Angle SetDelta(Angle delta);
    double SetRho(double rho);

    Angle GetAlpha() const;
    Angle GetDelta() const;
    double GetRho() const;

    virtual void Set(Angle alpha, Angle delta, double rho);
    virtual void Get(Angle& alpha, Angle& delta, double& rho);

    // operators
    VectorSpherical& operator+=(const VectorSpherical& sph);
    VectorSpherical& operator-=(const VectorSpherical& sph);

    virtual VectorSpherical& operator*=(double scalar);
    virtual VectorSpherical& operator/=(double scalar);

    VectorSpherical operator+(const VectorSpherical& sph) const;
    VectorSpherical operator-(const VectorSpherical& sph) const;

    virtual VectorSpherical operator*(double scalar) const;
    virtual VectorSpherical operator/(double scalar) const;

    bool operator==(const VectorSpherical& sph) const;
    bool operator!=(const VectorSpherical& sph) const;
    bool operator<(const VectorSpherical& sph) const;
    bool operator<=(const VectorSpherical& sph) const;
    bool operator>(const VectorSpherical& sph) const;
    bool operator>=(const VectorSpherical& sph) const;

    // specialized vector functions
    virtual double MagnitudeSquared() const;
    virtual double Magnitude() const;
    virtual VectorSpherical Unit() const;
    virtual VectorSpherical Negate() const;

    double DotProduct(const VectorSpherical &sph) const;
    VectorSpherical CrossProduct(const VectorSpherical &sph) const;
    Angle SeparationAngle(const VectorSpherical &sph) const;

    bool IsNormalized() const;
    void SetNormalize(bool bDoNormalize);

    // general class functions
    virtual const char *GetVersion() const;

protected:
    enum {NORMALIZE_NULL     = 0x00000000,
          NORMALIZE_ENABLED  = 0x00000001,
          ZERO_TWO_PI        = 0x00000010,
          PLUS_MINUS_PI      = 0x00000020,
          NEG_TWO_PI_ZERO    = 0x00000040,
          INTERVAL_BITS      = ZERO_TWO_PI
                               | PLUS_MINUS_PI
                               | NEG_TWO_PI_ZERO,
          ALLOW_NEG_RADIUS   = 0x00000100
         };

    static const double s_SMALL_MAGNITUDE;

    void SetNormalizeInterval(int iNormalize);
    int  GetNormalizeInterval() const;
    void SetNegRadiusAllow(bool bIsNegRadAllowed);
    bool IsNegRadiusAllowed();

private:
    double UnitVectorDotProduct(const VectorSpherical &s) const;
    void ThrowBadUnit() const;
    void Normalize();

    int m_iNormalize;
};

// -------------------------------------------------------------------------
// constructors & destructors
inline
VectorSpherical::VectorSpherical(bool bDoNormalize)
    : m_iNormalize(PLUS_MINUS_PI)
{
    SetNormalize(bDoNormalize);
}

inline
VectorSpherical::VectorSpherical(double value, bool bDoNormalize)
    : VectorReal3D(value, value, value),
      m_iNormalize(PLUS_MINUS_PI)
{
    SetNormalize(bDoNormalize);
}

inline
VectorSpherical::VectorSpherical(Angle alpha, Angle delta, double rho,
                                 bool bDoNormalize)
    : VectorReal3D(alpha, delta, rho),
      m_iNormalize(PLUS_MINUS_PI)
{
    SetNormalize(bDoNormalize);
}

// -------------------------------------------------------------------------
// access functions
inline void
VectorSpherical::Set(Angle alpha, Angle delta, double rho)
{
    VectorReal3D::Set(alpha, delta, rho);
    Normalize();
}

inline void
VectorSpherical::Get(Angle& alpha, Angle& delta, double& rho)
{
    alpha = m_v[0];
    delta = m_v[1];
    rho   = m_v[2];
}

inline Angle
VectorSpherical::SetAlpha(Angle alpha)
{
    m_v[0] = alpha;
    Normalize();
    return m_v[0];
}

inline Angle
VectorSpherical::SetDelta(Angle delta)
{
    m_v[1] = delta;
    Normalize();
    return m_v[1];
}

inline double
VectorSpherical::SetRho(double rho)
{
    m_v[2] = rho;
    Normalize();
    return m_v[2];
}

inline Angle
VectorSpherical::GetAlpha() const
{
    return m_v[0];
}

inline Angle
VectorSpherical::GetDelta() const
{
    return m_v[1];
}

inline double
VectorSpherical::GetRho() const
{
    return m_v[2];
}

// -------------------------------------------------------------------------
// operators
inline VectorSpherical&
VectorSpherical::operator*=(double scalar)
{
    m_v[2] *= scalar;
    Normalize();
    return *this;
}

inline VectorSpherical&
VectorSpherical::operator/=(double scalar)
{
    m_v[2] /= scalar;
    Normalize();
    return *this;
}

inline VectorSpherical
VectorSpherical::operator+(const VectorSpherical &spherical) const
{
    VectorSpherical newSpherical(*this);
    newSpherical += spherical;
    return newSpherical;
}

inline VectorSpherical
VectorSpherical::operator-(const VectorSpherical &spherical) const
{
    VectorSpherical newSpherical(*this);
    newSpherical -= spherical;
    return newSpherical;
}

inline VectorSpherical
VectorSpherical::operator*(double scalar) const
{
    VectorSpherical spherical(*this);
    spherical *= scalar;
    return spherical;
}

inline VectorSpherical
VectorSpherical::operator/(double scalar) const
{
    VectorSpherical spherical(*this);
    spherical /= scalar;
    return spherical;
}

inline VectorSpherical
operator+(const VectorSpherical &spherical)
{
    return spherical;
}

inline VectorSpherical
operator-(const VectorSpherical &spherical)
{
    return spherical.Negate();
}

inline VectorSpherical
operator*(double a, const VectorSpherical &spherical)
{
    return spherical * a;
}

inline bool
VectorSpherical::operator!=(const VectorSpherical& spherical) const
{
    return !operator==(spherical);
}

inline bool
VectorSpherical::operator<(const VectorSpherical& spherical) const
{
    return m_v[2] < spherical.m_v[2];
}

inline bool
VectorSpherical::operator<=(const VectorSpherical& sph) const
{
    return *this < sph || *this == sph;
}

inline bool
VectorSpherical::operator>(const VectorSpherical& sph) const
{
    return !(*this <= sph);
}

inline bool
VectorSpherical::operator>=(const VectorSpherical& sph) const
{
    return !(*this < sph);
}

// -------------------------------------------------------------------------
// specialized vector functions
inline double
VectorSpherical::MagnitudeSquared() const
{
    return m_v[2] * m_v[2];
}

inline double
VectorSpherical::Magnitude() const
{
    return m_v[2];
}

inline VectorSpherical
VectorSpherical::Unit() const
{
    if (m_v[2] < s_SMALL_MAGNITUDE) {
        ThrowBadUnit();
    }

    VectorSpherical unitVec(*this);
    unitVec.m_v[2] = 1.0;
    return unitVec;
}

inline VectorSpherical
VectorSpherical::Negate() const
{
    VectorSpherical unitVec(*this);
    unitVec.m_v[2] *= -1.0;
    unitVec.Normalize();
    return unitVec;
}

// -------------------------------------------------------------------------
// normalization functions
inline void
VectorSpherical::SetNormalize(bool bDoNormalize)
{
    m_iNormalize = bDoNormalize
                   ? m_iNormalize|NORMALIZE_ENABLED
                   : m_iNormalize&~NORMALIZE_ENABLED;
    Normalize();
}

inline void
VectorSpherical::SetNormalizeInterval(int iNormalize)
{
    m_iNormalize &= ~INTERVAL_BITS;
    m_iNormalize |= iNormalize;
    Normalize();
}

inline bool
VectorSpherical::IsNormalized() const
{
    return 0!=(m_iNormalize&NORMALIZE_ENABLED);
}

inline int
VectorSpherical::GetNormalizeInterval() const
{
    return m_iNormalize&INTERVAL_BITS;
}

inline void
VectorSpherical::SetNegRadiusAllow(bool bIsNegRadAllowed)
{
    m_iNormalize = bIsNegRadAllowed
                   ? m_iNormalize|ALLOW_NEG_RADIUS
                   : m_iNormalize&~ALLOW_NEG_RADIUS;
    Normalize();
}

inline bool
VectorSpherical::IsNegRadiusAllowed()
{
    return 0!=(m_iNormalize&ALLOW_NEG_RADIUS);
}

// -------------------------------------------------------------------------
// class version
inline const char*
VectorSpherical::GetVersion() const
{
    return VECTORSPHERICAL_H ;
}
}
// A3 namespace end
// ============================================================================
#endif // VECTORSPHERICAL_H
