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
 * Angle.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : Angle.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

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
//              V3.5-001  12/31/2004  GJK  added conversion from rad to degrees
//                                         (comment, undo the V3.5-001
//                                          modification, see notes below)

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: added negation operator
//              Reason: eliminated 2 implicit casts

//              V4.5-001   2009-07-01 JAM  Move ioAngleManip to unnamed namelist

//              V5.0       2010-01-28 JAM  Replaced manipulator with new
//                                         template version, deleted V3.5-001
//                                         conversion function--redundant with
//                                         preexisting conversion fnctn

//  UNITS
//  Function Arg & Return: radians
//  Stream I/O:            manipulator controlled, default radians
//  Initialization Files:  N/A

//****************************************************************************//

//=============================================================================
#ifndef ANGLE_H
#define ANGLE_H __FILE__ " " __DATE__

#include <iosfwd>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class Angle
{
public:
    Angle() : m_angle(0.0) {;}
    Angle(const Angle& a) : m_angle(a.m_angle) {;}


    Angle(const double& angle_radians) : m_angle(angle_radians) {;}
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit Angle(const double& angle_radians) : m_angle(angle_radians) {;}

    operator const double&() const
    {
        return m_angle;
    }

    operator double&()
    {
        return m_angle;
    }

private:
    double m_angle;
};

// ----------------------------------------------------------------------------
// i/o operators & manipulators
std::ostream& operator<<( std::ostream& os, Angle angle );
std::istream& operator>>( std::istream& is, Angle& angle );

// degrees and radians manipulators (default is degrees)
std::ios_base& ioAngleDegrees( std::ios_base& stream );
std::ios_base& ioAngleRadians( std::ios_base& stream );
std::ios_base& ioAngleRestore( std::ios_base& stream );

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif // ANGLE_H
