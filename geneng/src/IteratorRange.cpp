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
 * IteratorRange.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : IteratorRange.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : SatTools

//  MODULE(s) : IteratorRange

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V0.1-001  2002-04-29  JAM  Alpha Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: changed distance_type to difference_type, added
//              preprocessor check to use distance_type with VC++ V6
//              Reason: distance_type was non-standard MS implementation, need
//              preprocessor check for VC++ compiler backward compatability
//              Change: changed WIN32 marco to _MSC_VER
//              Reason: newer GNU compilers recognize MS predefined macros
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings



//****************************************************************************//
#include "IteratorRange.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// class statics
// ============================================================================
// explicit instantiation
template class ConstIteratorRange< double >;

// ============================================================================
}
// A3 namespace end
// ============================================================================
