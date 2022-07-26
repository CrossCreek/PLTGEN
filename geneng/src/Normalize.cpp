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
 * Normalize.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : Normalize.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) :

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V1.4      08/25/2000  JAM  moved from global to A3 namespace
//                                         namespace directive no longer
//                                         applied to std
//                                         applied 80 character line limit
//                                         moved includes from header to source
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: changed template argument for Angle to double
//              Reason: using class Angle for units of degrees is deprecated

//              V4.5-001   2009-07-01 JAM
//              Moved the initialization of Normalize functions objects
//              from Normalize.cpp to Initialize.cpp

//  NOTES : base source file for the GenEngr library
//          contains common contants

//          note: angular units are radians
//                time units are seconds
//                length units are as indicated for constaints
//                length units are argument dependent for functions

//          Contents derive from SE and SST Ver 1.0.



//****************************************************************************//
// headers
#include "Normalize.h"
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>


// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// Normalize class members
// ----------------------------------------------------------------------------
// member functions
template < class FPT >
void Normalize< FPT >::ThrowBadBounds( FPT lowerBound, FPT upperBound )
{
    std::ostringstream stmMessage;
    stmMessage << "<<normalize>> argument range error\n"
               << "\nlower bound = " << lowerBound
               << "\nupper bound = " << upperBound << '\n' << std::ends;
    std::out_of_range oor( stmMessage.str() );
    throw oor;
}

template class Normalize<double>;
template class Normalize<Angle>;

// ============================================================================
}
// A3 namespace end
// ============================================================================

