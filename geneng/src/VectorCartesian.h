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
 * VectorCartesian.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
//  FILE      : VectorCartesian.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : VectorCartesian

//  AUTHOR(s) : J. McCormick

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

//  NOTES :
//  VectorCartesian provides a Cartesian representation of a 3-space vector and
//  defines common vector algebra functions.  The interface permits the user to
//  treat the vector as three scalar elements or as an array through the use of
//  the [] operator or iterators.  Cartesian element 0 is the vector projection
//  on the principal axis in the fundamental plane.  Element 1 represents the
//  projection on the subordinate axis in the fundamental plane.  Element 2
//  represents the projection on the normal axis to the fundamental plane.
//  VectorCartesian is a virtual class derived from VectorRead3D.

//  UNITS:
//  Function Arg & Return: any consistent
//  Stream I/O:            any consistent
//  Initialization Files:  N/A

//------------------------------------------------------------------------------
//  Public Class Interface
//  Through inheritance, VectorCartesian exposes the public VectorRead3D
//  interface as public.
//
//  Typedefs
//
//  Constructors
//  VectorCartesian();
//  Default constructor; initializes all elements with 0.0.
//
//  VectorCartesian(double value);
//  Conversion constructor; initializes all elements with value.
//
//  VectorCartesian(double el0, double el1, double el2);
//  Alternative constructor; initializes elements with argument list values.
//
//  VectorCartesian(const VectorSpherical &sph);
//  Conversion constructor; initializes elements via a spherical to Cartesian
//  transformation.
//
//  VectorCartesian(const VectorGeodetic &llh);
//  Conversion constructor; initializes elements via a geodetic to Cartesian
//  transformation.
//
//  The VectorCartesian class doesn’t contain dynamically allocated resources,
//  a defined copy constructor and assignment operator is not necessary.
//
//  Member Functions
//  double SetElement0(double el0);
//  Deprecated function. Sets element 0 to the argument value.  Returns the
//  argument value.
//
//  double SetElement1(double el1);
//  Deprecated function. Sets element 1 to the argument value.  Returns the
//  argument value.
//
//  double SetElement2(double el2);
//  Deprecated function. Sets element 2 to the argument value.  Returns the
//  argument value.
//
//  double GetElement0() const;
//  Deprecated function. Returns element 0.
//
//  double GetElement1() const;
//  Deprecated function. Returns element 1.
//
//  double GetElement2() const;
//  Deprecated function. Returns element 2.
//
//  double MagnitudeSquared() const;
//  Returns the squared magnitude of this vector.
//
//  double Magnitude() const;
//  Returns the magnitude of this vector.
//
//  VectorCartesian Unit() const;
//  Returns VectorCartesian set to the unit vector derived from this.
//
//  VectorCartesian Negate() const;
//  Returns VectorCartesian set to negative this.
//
//  double DotProduct(const VectorCartesian& cart) const;
//  Returns the dot product of this and cart.
//
//  VectorCartesian CrossProduct(const VectorCartesian& cart) const;
//  Returns the cross product of this and cart.
//
//  Angle SeparationAngle(const VectorCartesian& cart) const;
//  Returns the angle between this and cart.
//
//  virtual const char *GetVersion() const;
//  Returns class name and version.
//
//  Operators
//  VectorCartesian& operator+=(const VectorCartesian& cart);
//  Adds this vector with cart vector and assigns results to this.  Returns
//  this reference.
//
//  VectorCartesian& operator-=(const VectorCartesian& cart);
//  Subtracts cart vector from this vector and assigns results to this.
//  Returns this reference.
//
//  VectorCartesian& operator*=(double scalar);
//  Multiplies this vector with scalar value and assigns results to this.
//  Returns this reference.
//
//  VectorCartesian& operator/=(double scalar);
//  Divides this vector with scalar value and assigns results to this.
//  Returns this reference.
//
//  VectorCartesian operator+(const VectorCartesian& cart) const;
//  Adds this vector with cart vector and returns the results as a
//  VectorCartesian.
//
//  VectorCartesian operator-(const VectorCartesian& cart) const;
//  Subtracts cart vector from this vector and returns the results as a
//  VectorCartesian.
//
//  VectorCartesian operator*(double scalar) const;
//  Multiplies this vector with scalar value and returns the results as a
//  VectorCartesian.
//
//  VectorCartesian operator/(double scalar) const;
//  Divides this vector with scalar value and returns the results as a
//  VectorCartesian.
//
//  bool operator<(const VectorCartesian& cart) const;
//  Returns true if the magnitude of this is less than the magnitude of cart,
//  else returns false
//
//  bool operator<=(const VectorCartesian& cart) const;
//  Returns true if the magnitude of this is less than or equal to the
//  magnitude of cart, else returns false.
//
//  bool operator>(const VectorCartesian& cart) const;
//  Returns true if the magnitude of this is greater than the magnitude of
//  cart, else returns false.
//
//  bool operator>=(const VectorCartesian& cart) const;
//  Returns true if the magnitude of this is greater than or equal to the
//  magnitude of cart, else returns false.
//
//  External Functions
//  Ordinary Functions
//  None.
//
//  Operators
//  VectorCartesian
//  operator*(double scalar, const VectorCartesian& cart)
//  Multiplies scalar with cart and returns the results as a VectorCartesian.
//
//  VectorCartesian
//  operator+(const VectorCartesian& cart)
//  Returns VectorCartesian set with cart.
//
//  VectorCartesian
//  operator-(const VectorCartesian& cart)
//  Returns VectorCartesian set to negative cart.
//
//  std::ostream&
//  operator<<(std::ostream& os, const VectorCartesian& v);
//  Inserts class elements into the stream in sequence from front to back.
//
//  std::istream&
//  operator>>(std::istream& os, VectorCartesian& v);
//  Assigns extracted elements from the stream to the class from front to back.
//
//  Manipulators
//  See VectorReal3D.

//------------------------------------------------------------------------------
//  Contents derived from SE and SST Ver 1.0.
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//****************************************************************************//
#ifndef VECTORCARTESIAN_H
#define VECTORCARTESIAN_H  "VectorCartesian V4.3-009  2007-12-10"

#include "VectorReal3D.h"
#include <math.h>

// ============================================================================
// A3 namespace start
namespace A3
{
// forward declarations
class VectorSpherical;
class VectorCartesian;
class VectorGeodetic;
class Angle;

// -------------------------------------------------------------------------
// VectorCartesian Class Definition
class VectorCartesian : public VectorReal3D
{
public:
    // constructors and destructors
    VectorCartesian();
    VectorCartesian(double el0, double el1, double el2);


    VectorCartesian(double value);
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit VectorCartesian(double value);


    VectorCartesian(const VectorSpherical& sph);
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit VectorCartesian(const VectorSpherical& sph);


    VectorCartesian(const VectorGeodetic&  llh);
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit VectorCartesian(const VectorGeodetic&  llh);

    // -----------------------------------------------------------------
    // data access functions
    // SetElement_ & GetElement_ are deprecated
    // use operator[]
    double SetElement0(double el0);
    double SetElement1(double el1);
    double SetElement2(double el2);

    double GetElement0() const;
    double GetElement1() const;
    double GetElement2() const;

    // -----------------------------------------------------------------
    // operators
    VectorCartesian& operator+=(const VectorCartesian& cart);
    VectorCartesian& operator-=(const VectorCartesian& cart);

    VectorCartesian& operator*=(double scalar);
    VectorCartesian& operator/=(double scalar);

    VectorCartesian operator+(const VectorCartesian& cart) const;
    VectorCartesian operator-(const VectorCartesian& cart) const;

    VectorCartesian operator*(double scalar) const;
    VectorCartesian operator/(double scalar) const;

    bool operator<(const VectorCartesian& cart) const;
    bool operator<=(const VectorCartesian& cart) const;
    bool operator>(const VectorCartesian& cart) const;
    bool operator>=(const VectorCartesian& cart) const;

    // -----------------------------------------------------------------
    // common vector functions
    double MagnitudeSquared() const;
    double Magnitude() const;

    VectorCartesian Unit() const;
    VectorCartesian Negate() const;

    double DotProduct(const VectorCartesian& cart) const;
    VectorCartesian CrossProduct(const VectorCartesian& cart) const;
    Angle SeparationAngle(const VectorCartesian& cart) const;

    // -----------------------------------------------------------------
    // class version
    virtual const char *GetVersion() const;
};

// -------------------------------------------------------------------------
// constructors
inline
VectorCartesian::VectorCartesian() {;}

inline
VectorCartesian::VectorCartesian(double el0, double el1, double el2)
    : VectorReal3D(el0, el1, el2) {;}

inline
VectorCartesian::VectorCartesian(double value)
    : VectorReal3D(value) {;}

// -------------------------------------------------------------------------
// access functions
inline double
VectorCartesian::SetElement0(double el0)
{
    m_v[0] = el0;
    return m_v[0];
}

inline double
VectorCartesian::SetElement1(double el1)
{
    m_v[1] = el1;
    return m_v[1];
}

inline double
VectorCartesian::SetElement2(double el2)
{
    m_v[2] = el2;
    return m_v[2];
}

inline double
VectorCartesian::GetElement0() const
{
    return m_v[0];
}

inline double
VectorCartesian::GetElement1() const
{
    return m_v[1];
}

inline double
VectorCartesian::GetElement2() const
{
    return m_v[2];
}

// -------------------------------------------------------------------------
// operators
inline VectorCartesian&
VectorCartesian::operator+=(const VectorCartesian &cart)
{
    m_v[0] += cart.m_v[0];
    m_v[1] += cart.m_v[1];
    m_v[2] += cart.m_v[2];
    return *this;
}

inline VectorCartesian&
VectorCartesian::operator-=(const VectorCartesian &cart)
{
    m_v[0] -= cart.m_v[0];
    m_v[1] -= cart.m_v[1];
    m_v[2] -= cart.m_v[2];
    return *this;
}

inline VectorCartesian&
VectorCartesian::operator*=(double scalar)
{
    m_v[0] *= scalar;
    m_v[1] *= scalar;
    m_v[2] *= scalar;
    return *this;
}

inline VectorCartesian&
VectorCartesian::operator/=(double scalar)
{
    m_v[0] /= scalar;
    m_v[1] /= scalar;
    m_v[2] /= scalar;
    return *this;
}

inline VectorCartesian
VectorCartesian::operator+(const VectorCartesian &cart) const
{
    return VectorCartesian(m_v[0] + cart.m_v[0],
                           m_v[1] + cart.m_v[1],
                           m_v[2] + cart.m_v[2]);
}

inline VectorCartesian
VectorCartesian::operator-(const VectorCartesian &cart) const
{
    return VectorCartesian(m_v[0] - cart.m_v[0],
                           m_v[1] - cart.m_v[1],
                           m_v[2] - cart.m_v[2]);
}

inline VectorCartesian
VectorCartesian::operator*(double scalar) const
{
    return VectorCartesian(m_v[0] * scalar,
                           m_v[1] * scalar,
                           m_v[2] * scalar);
}

inline VectorCartesian
VectorCartesian::operator/(double scalar) const
{
    return VectorCartesian(m_v[0] / scalar,
                           m_v[1] / scalar,
                           m_v[2] / scalar);
}

inline bool
VectorCartesian::operator<(const VectorCartesian& cart) const
{
    return MagnitudeSquared() < cart.MagnitudeSquared();
}

inline bool
VectorCartesian::operator<=(const VectorCartesian& cart) const
{
    return *this < cart || *this == cart;
}

inline bool
VectorCartesian::operator>(const VectorCartesian& cart) const
{
    return !(*this <= cart);
}

inline bool
VectorCartesian::operator>=(const VectorCartesian& cart) const
{
    return !(*this < cart);
}

// -------------------------------------------------------------------------
// useful functions
inline double
VectorCartesian::MagnitudeSquared() const
{
    return m_v[0] * m_v[0] + m_v[1] * m_v[1] + m_v[2] * m_v[2];
}

inline double
VectorCartesian::Magnitude() const
{
    return sqrt(MagnitudeSquared());
}

inline VectorCartesian
VectorCartesian::Unit() const
{
    return *this / Magnitude();
}

inline VectorCartesian
VectorCartesian::Negate() const
{
    return VectorCartesian(-m_v[0], -m_v[1], -m_v[2]);
}

inline double
VectorCartesian::DotProduct(const VectorCartesian &cart) const
{
    return m_v[0] * cart.m_v[0]
           + m_v[1] * cart.m_v[1]
           + m_v[2] * cart.m_v[2];
}

inline VectorCartesian
VectorCartesian::CrossProduct(const VectorCartesian &cart) const
{
    return VectorCartesian(
               m_v[1] * cart.m_v[2] - m_v[2] * cart.m_v[1],
               m_v[2] * cart.m_v[0] - m_v[0] * cart.m_v[2],
               m_v[0] * cart.m_v[1] - m_v[1] * cart.m_v[0]);
}

// -------------------------------------------------------------------------
// class version
inline const char
*VectorCartesian::GetVersion() const
{
    return VECTORCARTESIAN_H;
}

// =========================================================================
inline VectorCartesian
operator+(const VectorCartesian &cart)
{
    return cart;
}

inline VectorCartesian
operator-(const VectorCartesian &cart)
{
    return cart.Negate();
}

inline VectorCartesian
operator*(double a, const VectorCartesian &cart)
{
    return cart * a;
}

// -------------------------------------------------------------------------
// insertion and extraction operators
inline std::ostream& operator<<(std::ostream& os, const VectorCartesian& a)
{
    return os << (const VectorReal3D &)a;
}

inline std::istream& operator>>(std::istream& is, VectorCartesian& a)
{
    return is >> (VectorReal3D &)a;
}
}
// A3 namespace end
// ============================================================================
#endif // VECTORCARTESIAN_H
