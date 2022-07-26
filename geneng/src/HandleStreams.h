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
 * HandleStreams.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : HandleStreams.h

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES : see The C++ Programming Language, 3th Edition by B. Stroustrup
//          pp. 782-786

//****************************************************************************//

#ifndef HANDLESTREAMS_H
#define HANDLESTREAMS_H "HandleStreams Version 2.0"

#include "Handle.h"
#include "HandleConst.h"
#include <iostream>

// ============================================================================
// A3 namespace start
namespace A3
{

// ============================================================================
template < class T >
inline ostream& operator<<( ostream& os, const Handle< T >& hdlT )
{
    return os << *hdlT;
}

template < class T >
inline istream& operator>>( istream& is, Handle< T >& hdlT )
{
    return is >> *hdlT;
}

template < class T >
inline ostream& operator<<( ostream& os, const HandleConst< T >& hdlT )
{
    return os << *hdlT;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif // HANDLESTREAMS_H
