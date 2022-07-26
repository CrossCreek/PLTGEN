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
 * CoorSystem.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
/// @file
/// @brief CoorSystem namespace and enumeration
///
/// @details Defines the CoorSystem namespace, coordinate system enumeration
/// and FlapMap object coorSystem for converting to/from coordinate system
/// enumerators and labels.
///
/// @author J. McCormick
/// @version V5.0  Added AEFZ and AEFP coordinate frames and Quaternion
/// @date 2010-06-01
///
/// @author J. McCormick
/// @version V4.5  CoorSystem uses EnumMap class to reduce redundancy
/// <BR> J2000 coordinate frame implementation added to GenEngr
/// @date 2009-07-01
///
/// @author J. McCormick
/// @version V4.3-009 Compiler compatibility update, VS 6 through 2005 and
/// gcc 3.4.2
/// <BR> Change: replaced strstream with stringstream
/// <BR> Reason: strstream is deprecated, older compilers didn't support
/// stringstream, newer compilers support it
/// @date 2007-12-10
///
/// @author GM
/// @version V3.6-001 Add J2000 coordinate frame enumerator but
/// J2000 implementation is in the PRO library, not GenEngr
/// @date 2005-08-23
///
/// @author J. McCormick
/// @version V2.0 A3 library build
/// @date 2002-04-29
///
/// @author J. McCormick
/// @version V1.0 Initial release
/// @date 1998-02-08
///
/// @par Units
/// - Function Arg & Return: N/A
/// - Stream I/O:            N/A
/// - Initialization Files:  N/A
// =============================================================================
#ifndef COORSYSTEM_H
/// Include guard macro
#define COORSYSTEM_H    __FILE__ " " __DATE__

#include "EnumMap.h"

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// Maps coordinate systems enumerators to labels
class CoorSystem : public EnumMap<unsigned char>
{
public:
    typedef unsigned char Type;

    enum {ECI,  ECF,  PQW,  NWZ,  NWZF, NWP,
          NWPF, ICR,  PQSA, PQSB, PQSC, NOP,
          NES,  NIC,  POST, J2000, MOD, TOD,
          PBF,  AEFZ, AEFP, QUAT, PCI, PAT
         };

    CoorSystem();
};

/// Coordinate System Object ID/Name Map
/// @return object containing map of Ephemeris Object ID/Names
inline const CoorSystem& coorSystem()
{
    static CoorSystem csi;
    return csi;
}

} // A3
#endif  // COORSYSTEM_H

