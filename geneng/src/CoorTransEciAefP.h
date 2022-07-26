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
 * CoorTransEciAefP.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================

//  FILE      : CoorTransEciAefP.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciAefP

//  AUTHOR(s) : J. McCormick

//  UPDATES   :
//      V5.0    2010-06-01  JAM  initial release

//
/// @file
/// @brief Azimuth-Elevation-Fixed to/from Earth Centered Inertial (TOD)
/// Rotation
///
/// @details Defines the Azimuth Elevation Fixed, AEF, coordinate system with
/// respect to the Earth Centered Inertial (TOD) using geodetic coordinates
/// for the origin and a rotation from the topocentric-horizon system defined
/// in terms of azimuth and elevation.  Refer to "Azimuth Elevation Fixed
/// to/from Topocentric Coordinate System Transformations – Rotation Only"
/// by Joseph McCormick for more details.
///
/// @author J. McCormick
/// @version V5.0  initial release
/// @date 2010-06-01
///
/// @par Units
/// - Function Arg & Return: standard GenEngr (see constants.h)
/// - Stream I/O:            L = n/a, T, A = manipulator controlled
/// - Initialization Files:  N/A
// =============================================================================
#ifndef COORTRANSECIAEFP_H
#define COORTRANSECIAEFP_H	__FILE__ " " __DATE__

#include "CoorTransEciAefZ.h"

namespace A3
{
class CoorTransEciAefP : public CoorTransEciAefZ
{
public:
    CoorTransEciAefP(const HandleConst<TimeJ2000> hdlTime,
                     Angle geodeticLatitude, Angle longitude,
                     Angle azimuth, Angle elevation);
};
} // A3 namespace end

#endif  // COORTRANSECIAEFP_H
