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
 * VectorCartesian.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
//  FILE      : VectorCartesian.cpp

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

//  Contents derived from SE and SST Ver 1.0.

// ============================================================================
#include "VectorCartesian.h"
#include "VectorSpherical.h"
#include "VectorGeodetic.h"
#include "Angle.h"
#include "TestValue.h"

// ============================================================================
// A3 namespace start
namespace A3
{
VectorCartesian::VectorCartesian(const VectorSpherical &sph)
{
    double rhoCosDelta = sph.GetRho() * cos(sph.GetDelta());
    Set(rhoCosDelta  * cos(sph.GetAlpha()),
        rhoCosDelta  * sin(sph.GetAlpha()),
        sph.GetRho() * sin(sph.GetDelta()));
}

VectorCartesian::VectorCartesian(const VectorGeodetic &llh)
{
    // Reference:
    // Calculation of Geodetic Coordinates from Earth-Centered,
    // Journal of Guidance, Control, and Dynamics
    // Earth-Fixed Coordinates, pp. 188-190
    // Volume 11, Number 2
    // March-April 1988
    double cosLat = cos(llh.GetLatitude());
    double radiusCurv = llh.GetRadiusOfCurvature();
    double pradius = radiusCurv + llh.GetHeight();
    Set(pradius * cosLat * cos(llh.GetLongitude()),
        pradius * cosLat * sin(llh.GetLongitude()),
        ((1.0 - EARTH_ECCENTRICITY_SQ) * radiusCurv + llh.GetHeight())
        * sin(llh.GetLatitude()));
}

// -------------------------------------------------------------------------
// SeparationAngle = angle between two vectors
Angle
VectorCartesian::SeparationAngle(const VectorCartesian &cart) const
{
    double magnXMagn = sqrt(MagnitudeSquared() * cart.MagnitudeSquared());
    return (magnXMagn > 0.0
            ? acos(testAsinAcosArg(DotProduct(cart) / magnXMagn)) : 0.0);
}
}
// A3 namespace end
// ============================================================================
