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
 * Units.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
/// @file
/// @brief Units Class and Unit Enumeration
///
/// @details Contains unit enumerations
///
/// Provides functions for finding a unit value
/// given one of it's labels and vice versa
///
/// @author J. McCormick
/// @version V4.5 Initial release
/// @date 2009-07-01
///
/// @par Units
/// - Function Arg & Return: N/A
/// - Stream I/O:            N/A
/// - Initialization Files:  N/A
// =============================================================================
#ifndef UNITS_H
/// Include guard macro
#define UNITS_H   "Units 2009-02-20"

#include "EnumMap.h"

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// Defines units and provides conversion functions
class Units : public EnumMap<unsigned long>
{
public:
    enum {
        // categories
        NONE         = 0x00000000,
        TIME         = 0x00100000,
        LENGTH       = 0x00200000,
        MASS         = 0x02000000,
        ANGLE        = 0x04000000,
        TEMP         = 0x08000000,
        CAT_BITS     = 0x0E300000,

        // time & calendar formats
        SECOND       = 0x00000001 | TIME,
        MINUTE       = 0x00000002 | TIME,
        HOUR         = 0x00000003 | TIME,
        DAY          = 0x00000004 | TIME,
        WEEK         = 0x00000005 | TIME,
        YEAR         = 0x00000006 | TIME,
        REFREL       = 0x00000007 | TIME, // reference time, relative time
        HMS          = 0x00000008 | TIME, // hours, minutes, seconds
        MS           = 0x00000009 | TIME, // minutes, seconds

        // Calendar Formats
        YMDHMS       = 0x0000000A | TIME,
        DMYHMSA      = 0x0000000B | TIME,
        DMYHMSB      = 0x0000000C | TIME,
        DMYHMSC      = 0x0000000D | TIME,
        YMDHMSJ      = 0x0000000E | TIME,
        YMDH         = 0x0000000F | TIME,
        YMD          = 0x00000010 | TIME,
        DAY_EXCEL    = 0x00000020 | TIME,

        // length
        M           = 0x00000090 | LENGTH,
        KM          = 0x000000A0 | LENGTH,
        NMI         = 0x000000B0 | LENGTH,
        FT          = 0x000000C0 | LENGTH,
        INCH        = 0x000000D0 | LENGTH,
        AU          = 0x000000E0 | LENGTH,

        // mass
        KG          = 0x00000100 | MASS,
        G           = 0x00000200 | MASS,
        LBM         = 0x00000300 | MASS,
        SLUG        = 0x00000400 | MASS,

        // angle
        DEGREE      = 0x00000A00 | ANGLE,
        RADIAN      = 0x00000B00 | ANGLE,
        GRADIAN     = 0x00000C00 | ANGLE,

        // temperature
        K           = 0x00001000 | TEMP,
        R           = 0x00002000 | TEMP,
        C           = 0x00003000 | TEMP,
        F           = 0x00004000 | TEMP
    };

    Units();
};

/// Coordinate System Object ID/Name Map
/// @return object containing map of Ephemeris Object ID/Names
inline const Units& units()
{
    static Units u;
    return u;
}

} // A3

#endif  // UNITS_H
