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
 * TimeBase.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//
//  FILE      : TimeBase.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.3-006  12/20/1999  JAM  reorganized manipulator decl & defs
//                                         to reduce compile time overhead;
//                                         made IoManipulator more
//                                         flexible; changed ioSeparator from
//                                         global to stream dependent (hence
//                                         more flexible) like other manips
//              V2.0      04/29/2002  JAM  1st A3 namespace build
//              V4.5      2009-07-01  JAM  Move ioTimeManip to unnamed namespace
//                                         Added new time conversions
//                                         Added new time conversions, added
//                                         time reference enumeration, added
//                                         time reference manipulators
//              V5.0      2010-01-18  JAM  Replaced A3::timeRef global with
//                                         reference
//                                         Replaced manipulator with new
//                                         template version

//  Doxygen Documentation
/// @file
/// @brief Time manipulators and system time

/// @author J. McCormick
/// @version V5.0 <BR>
/// Replaced A3::timeRef global with reference
/// @date 2010-01-18

/// @author J. McCormick
/// @version V4.5 <BR>
/// Moved ioTimeManip to unnamed namespace, added new time
/// conversions, added new time conversions, added time reference enumeration,
/// added time reference manipulators
/// @date 2009-07-01

/// @author J. McCormick
/// @version V2.0 <BR>
/// 1st A3 namespace build
/// @date 04/29/2002

/// @author J. McCormick
/// @version V1.3-016 <BR>
/// Reorganized manipulator decl & defs to reduce compile time
/// overhead; made IoManipulator more flexible; changed ioSeparator from
/// global to stream dependent (hence more flexible) like other manips
/// @date 12/20/1999

/// @par Units
/// - Function Arg & Return: n/a
/// - Stream I/O:            n/a
/// - Initialization Files:  n/a

// =============================================================================

#ifndef TIMEBASE_H
#define TIMEBASE_H   "TimeBase.h V5.0"

#include "EnumMap.h"

#include <iostream>
#include <string>
#include <map>

// =============================================================================
// A3 namespace start
namespace A3
{
/// time reference enumeration & labels
class TimeRef : public EnumMap<unsigned>
{
public :
    enum {TT, UTC, UT1, TAI, TGP};
    TimeRef();
};

/// Time Reference ID/Name Map
/// @return object containing map of time reference ID/Names
inline const TimeRef& timeRef()
{
    static TimeRef tr;
    return tr;
}

// =========================================================================
/// Time manipulators (default is seconds)
std::ios_base& ioTimeSeconds(std::ios_base& stream);
std::ios_base& ioTimeMinutes(std::ios_base& stream);
std::ios_base& ioTimeHours(std::ios_base& stream);
std::ios_base& ioTimeDays(std::ios_base& stream);
std::ios_base& ioTimeYMDHMS(std::ios_base& stream);
std::ios_base& ioTimeYMDH(std::ios_base& stream);
std::ios_base& ioTimeExcelDays(std::ios_base& stream);
std::ios_base& ioTimeRestore(std::ios_base& stream);

/// Time reference manipulators (default is UTC)
std::ios_base& ioTimeUTC(std::ios_base& stream);
std::ios_base& ioTimeTT(std::ios_base& stream);
std::ios_base& ioTimeUT1(std::ios_base& stream);
std::ios_base& ioTimeTAI(std::ios_base& stream);
std::ios_base& ioTimeGPS(std::ios_base& stream);
std::ios_base& ioTimeRefRestore(std::ios_base& stream);
std::ios_base&
ioTimeRefSetFormat(std::ios_base& stream, const std::string& fmt);

/// return formatted time
std::ostream& ioCurrentTime(std::ostream& os);

/// return formatted time with message
const std::string&
ioCurrentTime(const std::string& message);

// provided for backward compatibility
extern const std::string&
(*printCurrentTime)(const std::string& message);

} // A3 namespace end

#endif // TIMEBASE_H
