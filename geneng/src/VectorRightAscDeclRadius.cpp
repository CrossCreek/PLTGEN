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
 * VectorRightAscDeclRadius.cpp
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
//  Contents derived from SE and SST Ver 1.0.
//  M&DS AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA
//****************************************************************************//
#include "VectorRightAscDeclRadius.h"
#include "IoSeparator.h"
#include <iostream>
#include <iomanip>

// ============================================================================
// A3 namespace start
namespace A3
{
// insertion & extraction operator
std::istream&
operator>>(std::istream& is, VectorRightAscDeclRadius& rdr)
{
    Angle ra, de;
    double rd;
    is >> ra >> de >> rd;
    rdr.Set(ra, de, rd);
    return is;
}

std::ostream&
operator<<(std::ostream& os, const VectorRightAscDeclRadius& rdr)
{
    //WARNING_FIX:
    //1>src\VectorRightAscDeclRadius.cpp(76): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'int', possible loss of data
    long long int w = os.width();
    os << rdr.GetRightAscension() << ioSeparator << std::setw(w)
       << rdr.GetDeclination() << ioSeparator << std::setw(w)
       << rdr.GetRadius();
    return os;
}
}
// A3 namespace end
// ============================================================================

