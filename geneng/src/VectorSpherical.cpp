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
 * VectorSpherical.cpp
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
//  normalization should be turned off!  Spherical coordinates are illustrated
//  in Figure 4.  Through implicit casting, VectorCartesian vector algebra
//  operators and functions that are not specialized in VectorSpherical are
//  accessible. VectorSpherical is a virtual class derived from VectorRead3D.

//  UNITS:
//  Function Arg & Return: angles in radians
//  Stream I/O:            angle units controlled by ioAngle manipulators
//  Initialization Files:  N/A

//  Contents derived from SE and SST Ver 1.0.
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//****************************************************************************//
#include "VectorSpherical.h"
#include "VectorCartesian.h"
#include "TestValue.h"
#include "IoSeparator.h"
#include "Normalize.h"

#include <sstream>
#include <stdexcept>
#include <iomanip>

// ==============================================================================
namespace
{
A3::Normalize< A3::Angle > normalizeNegTwoPiZero( -A3::TWO_PI, 0.0 );
}

// ==============================================================================
// A3 namespace start
namespace A3
{
// class static members
const double VectorSpherical::s_SMALL_MAGNITUDE = 1.0e-30;

// -------------------------------------------------------------------------
// conversion constructor from cartesian coordinates to
// spherical coordinates
VectorSpherical::VectorSpherical(const VectorCartesian &cart,
                                 bool bDoNormalize)
    : m_iNormalize(PLUS_MINUS_PI)
{
    // calculate rho, the radial component
    m_v[2] = cart.Magnitude();

    // if the magnitude of the radial component is sufficient,
    // calculate alpha, the angle in the fundamental plane measured
    // from the principle axis and delta, the angle measured off the
    // fundamental plane toward the normal
    if (m_v[2] >= s_SMALL_MAGNITUDE) {
        m_v[1] = asin(testAsinAcosArg(cart[2] / m_v[2]));
        m_v[0] = atan2(cart[1], cart[0]);
    }
    // if the magnitude of the radial component is too small, set all
    // values to 0
    else {
        m_v[0] = 0.0;
        m_v[1] = 0.0;
        m_v[2] = 0.0;
    }

    SetNormalize(bDoNormalize);
}

// -------------------------------------------------------------------------
// operators
bool
VectorSpherical::operator==(const VectorSpherical& sph) const
{
    bool bIsEq(false);

    if (m_v[2] == sph.m_v[2] && m_v[1] == sph.m_v[1]) {
        bool bBothN(IsNormalized()&&sph.IsNormalized());

        if (bBothN && 0 !=((m_iNormalize^sph.m_iNormalize)&INTERVAL_BITS)) {
            switch (m_iNormalize & INTERVAL_BITS) {
                case PLUS_MINUS_PI:
                    bIsEq = m_v[0] == normalizePlusMinusPi(sph.m_v[0]);
                    break;

                case ZERO_TWO_PI:
                    bIsEq = m_v[0] == normalizeZeroTwoPi(sph.m_v[0]);
                    break;

                case NEG_TWO_PI_ZERO:
                    bIsEq = m_v[0] == normalizeZeroTwoPi(sph.m_v[0]);
                    break;
            }
        } else {
            bIsEq = m_v[0] == sph.m_v[0];
        }
    }

    return bIsEq;
}

VectorSpherical&
VectorSpherical::operator+=(const VectorSpherical &sph)
{
    VectorCartesian cartesian0(*this);
    VectorCartesian cartesian1(sph);
    *this = VectorSpherical(cartesian0 + cartesian1);
    return *this;
}

VectorSpherical&
VectorSpherical::operator-=(const VectorSpherical &sph)
{
    VectorCartesian cartesian0(*this);
    VectorCartesian cartesian1(sph);
    *this = VectorSpherical(cartesian0 - cartesian1);
    return *this;
}

// -------------------------------------------------------------------------
// specialized functions
double
VectorSpherical::DotProduct(const VectorSpherical &spherical) const
{
    try {
        return m_v[2] * spherical.m_v[2] * UnitVectorDotProduct(spherical);
    } catch (...) {
        std::cerr << "\nException caught by" << std::endl
                  << "VectorSpherical::DotProduct(const VectorSpherical &spherical)"
                  << std::endl;
        throw;
    }
}

Angle
VectorSpherical::SeparationAngle(const VectorSpherical &spherical) const
{
    try {
        return acos(UnitVectorDotProduct(spherical));
    } catch (...) {
        std::cerr << "\nException caught by" << std::endl
                  << "VectorSpherical::DotProduct(const VectorSpherical &spherical)"
                  << std::endl;
        throw;
    }
}

VectorSpherical
VectorSpherical::CrossProduct(const VectorSpherical &sph) const
{
    if (m_v[2] < s_SMALL_MAGNITUDE || sph.m_v[2] < s_SMALL_MAGNITUDE) {
        std::ostringstream msg;
        msg << "Function VectorSpherical::CrossProduct("
            << "const VectorSpherical &sph)\n"
            << "The magnitude of at least one vector is too\n"
            << "small for this function.\n"
            << "Magnitude of \"this\" vector  = " << m_v[2] << std::endl
            << "Magnitude of the input vector = "
            << sph.m_v[2] << std::endl << std::ends;
        std::range_error re(msg.str());
        throw re;
    }

    VectorSpherical normalVec;

    try {
        double cosAlpha1 = cos(m_v[0]), cosAlpha2 = cos(sph.m_v[0]);
        double sinAlpha1 = sin(m_v[0]), sinAlpha2 = sin(sph.m_v[0]);
        double cosDelta1 = cos(m_v[1]), cosDelta2 = cos(sph.m_v[1]);
        double sinDelta1 = sin(m_v[1]), sinDelta2 = sin(sph.m_v[1]);
        double cosAlpha21 = cosAlpha2 * cosAlpha1 + sinAlpha2 * sinAlpha1;
        double sinAlpha21 = sinAlpha2 * cosAlpha1 - cosAlpha2 * sinAlpha1;
        double cosDelta21 = cosDelta2 * cosDelta1 + sinDelta2 * sinDelta1;
        double unitDotProd = testAsinAcosArg((cosAlpha21 - 1.0)
                                             * cosDelta2 * cosDelta1 + cosDelta21);
        normalVec.m_v[2] = m_v[2] * sph.m_v[2]
                           * sqrt(1 - unitDotProd * unitDotProd);

        if (normalVec.m_v[2] < s_SMALL_MAGNITUDE
                && normalVec.m_v[2] > -s_SMALL_MAGNITUDE) {
            normalVec.m_v[2] = 0.0;
            normalVec.m_v[1] = 0.0;
            normalVec.m_v[0] = 0.0;
        } else {
            normalVec.m_v[1] = asin(testAsinAcosArg(m_v[2] * sph.m_v[2]
                                                    * cosDelta1 * cosDelta2 * sinAlpha21
                                                    / normalVec.m_v[2]));
            normalVec.m_v[0] = atan2(sinDelta1 * cosDelta2 * cosAlpha2
                                     - sinDelta2 * cosDelta1 * cosAlpha1,
                                     cosDelta1 * sinAlpha1 * sinDelta2
                                     - cosDelta2 * sinAlpha2 * sinDelta1);
        }

        normalVec.Normalize();
    } catch (... ) {
        std::cerr << "\nException caught by\n"
                  << "VectorSpherical::CrossProduct(const VectorSpherical &sph)"
                  << std::endl;
        throw;
    }

    return normalVec;
}

double
VectorSpherical::UnitVectorDotProduct(const VectorSpherical &sph) const
{
    try {
        return testAsinAcosArg((cos(sph.m_v[0] - m_v[0]) - 1.0)
                               * cos(sph.m_v[1]) * cos(m_v[1])
                               + cos(sph.m_v[1] - m_v[1]));
    } catch (...) {
        std::cerr << "\nException caught by\nVectorSpherical::"
                  << "UnitVectorDotProduct(const VectorSpherical &)"
                  << std::endl;
        throw;
    }
}

void
VectorSpherical::Normalize()
{
    if (0==(m_iNormalize&NORMALIZE_ENABLED)) {
        return;
    }

    m_v[1] = normalizePlusMinusPi(m_v[1]);

    if (m_v[1] > HALF_PI) {
        m_v[1] = PI - m_v[1];
        m_v[0] += PI;
    } else if (m_v[1] < -HALF_PI) {
        m_v[1] = -PI - m_v[1];
        m_v[0] += PI;
    }

    if (0 == (m_iNormalize & ALLOW_NEG_RADIUS) && m_v[2] < 0.0) {
        m_v[1] = -m_v[1];
        m_v[0] += PI;
        m_v[2] = -m_v[2];
    }

    switch (m_iNormalize & INTERVAL_BITS) {
        case PLUS_MINUS_PI:
            m_v[0] = normalizePlusMinusPi(m_v[0]);
            break;

        case ZERO_TWO_PI:
            m_v[0] = normalizeZeroTwoPi(m_v[0]);
            break;

        case NEG_TWO_PI_ZERO:
            m_v[0] = normalizeNegTwoPiZero(m_v[0]);
            break;

        default:
            std::string sErr(__FILE__);
            sErr.append(": Invalid Normalize Option!");
            throw std::runtime_error(sErr);
    }
}

// -------------------------------------------------------------------------
// exceptions
void
VectorSpherical::ThrowBadUnit() const
{
    std::ostringstream msg;
    msg << "Function VectorSpherical::Unit()\n"
        << "The magnitude of \"this\" sph vector is too\n"
        << "small for this function.\n"
        << "Magnitude of \"this\" sph vector  = " << m_v[2]
        << std::endl << std::ends;
    std::range_error re(msg.str());
    throw re;
}

// -------------------------------------------------------------------------
// extraction & insertion operators
std::istream&
operator>>(std::istream& is, VectorSpherical& sph)
{
    Angle alpha, delta;
    double rho;
    is >> alpha >> delta >> rho;
    sph.Set(alpha, delta, rho);
    return is;
}

std::ostream&
operator<<(std::ostream& os, const VectorSpherical& sph)
{
    //WARNING_FIX:
    //1>src\VectorSpherical.cpp(340): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'int', possible loss of data
    long long int w = os.width();
    os << sph.GetAlpha() << ioSeparator << std::setw(w)
       << sph.GetDelta() << ioSeparator << std::setw(w)
       << sph.GetRho();
    return os;
}
}
// A3 namespace end
// ============================================================================
