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
 * IoSeparator.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : IoSeparator.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM   new general engineering library
//              V1.3-006  12/20/1999  JAM  reorganized manipulator decl & defs
//                                         to reduce compile time overhead;
//                                         made IoManipulator more
//                                         flexible; changed ioSeparator from
//                                         global to stream dependent (hence
//                                         more flexible) like other manips
//              V1.3-???  08/01/2000  JAM  moved from global to A3 namespace
//                                         namespace directive no longer
//                                         applied to std
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: added explicited int cast to m_itrCurrentIos->second.size()
//              Reason: stops "warning C4018: '<' : signed/unsigned mismatch" from
//              implicit conversion
//              Change: deleted strstream
//              Reason: no longer needed

//              V4.5-001  2009-07-01  JAM  Added ioSpc reference to ioSeparator
//                                         Moved initialization of ioSeparator
//                                         manipulator to Initialize.cpp

//              V5.0      2010-01-27  JAM
//              Replaced by IoDelim, see IoDelim.h for details

//=============================================================================
#ifndef IOSEPARATOR_H
#define IOSEPARATOR_H   __FILE__ " " __DATE__

#include "IoDelim.h"

#define ioSeparator ioDelim

#endif  // IOSEPARATOR_H
