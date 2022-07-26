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
 * IoDetail.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : IoDetail.h

//  NAMESPACE : A3

//  FILE      : ioDetail.h

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : Manipulator

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM   new general engineering library
//              V1.3-006  12/20/1999  JAM  reorganized manipulator decl & defs
//                                         to reduce compile time overhead;
//                                         made IoManipulator more
//                                         flexible; changed ioSeparator from
//                                         global to stream dependent (hence
//                                         more flexible) like other manips
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.5-001  2009-07-01  JAM  Move ioDetailManip to unnamed namespace
//              V5.0      2010-01-28  JAM  Replaced IoManipulator with new
//                                         facet based IoManip

//  NOTES :

//****************************************************************************//
#ifndef IODETAIL_H
#define IODETAIL_H  __FILE__ " " __DATE__

#include <ios>

// =============================================================================
// A3 namespace start
namespace A3
{
// g++ workaround
namespace IoOutputDetail
{
enum IOD { OUTPUTLOW   = 0x01,
           OUTPUTSTD   = 0x02,
           OUTPUTHIGH  = 0x04,
           OUTPUTDEBUG = 0x08
         };
}

// class data output detail manipulator (default is standard)
std::ios_base& ioOutputDetailLow(std::ios_base& stm);

std::ios_base& ioOutputDetailStandard(std::ios_base& stm);

std::ios_base& ioOutputDetailHigh(std::ios_base& stm);

std::ios_base& ioOutputDetailDebug(std::ios_base& stm);

std::ios_base& ioOutputDetailRestore(std::ios_base& stm);

unsigned int ioOutputDetailGetFormat(std::ios_base& stm) ;

} // A3 namespace end
#endif  // IODETAIL_H
