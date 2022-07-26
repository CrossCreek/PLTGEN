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
 * IoDelim.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================

//  FILE      : IoDelim.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V5.0 2010-01-27 JAM Initial release
//              Replaces IoSeparator, provides delimiter insertion and
//              casting of characters to white spaces for extraction (new);
//              provides set, get and restore functions

//  Doxygen Documentation
/// @file
/// @brief IoDelim: delimiter insertion and extraction
///
/// The ioDelim & castWhite/uncastWhite functions provide a mechanism for
/// inserting delimiters into a stream and the casting of characters to white
/// spaces to support treating delimiters as white spaces on extraction.
/// The ioDelim function is used after an insertion operator to insert
/// a delimiter.  The ioDelimSet functions specifies the delimiter to be used.
///
/// For example:
/// <em>ioDelim.Set(std::cout, ", ");</em>
/// <em>std::cout << 'a' << ioDelim << 'b' << std::endl;</em>
/// yields
/// <em>a, b</em>
/// Use castWhite & uncastWhite to cast/uncast a character as whitespace for
/// extraction. Note that each stream has its own delimiters
///
/// @author J. McCormick
/// @version V5.0 <BR>
/// Replaces IoSeparator, provides delimiter insertion and
/// casting of characters to white spaces for extraction (new);
/// provides set, get and restore functions
/// @date 2010-01-27
///
/// @par Units
/// - Function Arg & Return: n/a
/// - Stream I/O:            n/a
/// - Initialization Files:  n/a

//=============================================================================
#ifndef IODELIMITER_H
#define IODELIMITER_H   __FILE__ " " __DATE__

#include <iosfwd>
#include <string>

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// Assign a delimiter to a stream
/// @param[in,out] iosb stream associated with the delimiter,
/// typically an ostream
/// @param[in] sDelim delimiter to be assigned
/// @return stream reference
std::ios_base& ioDelimSet(std::ios_base& iosb, const std::string& sDelim);

/// Assign a delimiter to a stream
/// @details Usage:
/// <em>os << ... << ioDelimSet("your delimiter") << ...</em>
/// @param[in] sDelim delimiter to be assigned
/// @return IoDelimSet object to be inserted into a stream
struct IoDelimSet;
IoDelimSet ioDelimSet(const std::string& sDelim);
std::ostream& operator<<(std::ostream& os, const IoDelimSet& iods);

/// Return the delimiter assigned to the output stream
/// @param[in] iosb stream associated with the delimiter,
/// typically an ostream
/// @return delimiter
std::string ioDelimGet(const std::ios_base& iosb);

/// Replace the current delimiter with the previous delimiter
/// @details Maximum of 10 previous delimiters are saved
/// using Restore pops the current delimiter off the saved
/// delimiter stack hence reduces the stack size by one
/// @param[in, out] iosb stream associated with the delimiter,
/// typically an ostream
/// @return stream reference
std::ios_base& ioDelimRestore(std::ios_base& iosb);

/// Cast a character as white space
/// @param[in, out] iosb stream receiving reclassified character,
/// typically an istream
/// @param[in] c character to be casted as a whitespace
/// @return stream
std::ios_base& ioCastWhite(std::ios_base& iosb, char c);

/// Cast characters as white space
/// @param[in, out] iosb stream receiving reclassified character
/// typically an istream
/// @param[in] s string containing characters to be casted as a whitespace
/// @return stream
std::ios_base& ioCastWhite(std::ios_base& iosb, const std::string& s);

/// Uncast a character as white space
/// @param[in, out] iosb stream receiving reclassified character
/// typically an istream
/// @param[in] c character to be uncasted as a whitespace
/// @return stream
std::ios_base& ioUncastWhite(std::ios_base& iosb, char c);

/// Uncast characters as white space
/// @param[in, out] iosb stream receiving reclassified character
/// typically an istream
/// @param[in] s character to be uncasted as a whitespace
/// @return stream
std::ios_base& ioUncastWhite(std::ios_base& iosb, const std::string& s);

/// Delimiter insertion
/// @details Example:
/// <em>std::cout << 'a' << ioDelim << 'b' << std::endl;</em>
/// yields
/// <em>a, b</em>
/// @param[in, out] os output stream receiving the delimiter
/// @param[in] delim delimiter to be inserted
/// @return output stream
std::ostream& ioDelim(std::ostream& os);
}
// A3 namespace end
// ============================================================================
#endif  // IODELIMITER_H
