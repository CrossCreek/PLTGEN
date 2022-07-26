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
 * CoorTransBaseEci.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransBaseEci.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransBaseEci

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  02/27/1998  JAM  Initial Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

//=============================================================================
#ifndef COORTRANSBASEECI_H
#define COORTRANSBASEECI_H	"CoorTransBaseEci V2.0"

//-----------------------------------------------------------------------------
// required headers
#include "CoorTrans.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class CoorTransBaseEci : public CoorTrans
{
    //-------------------------------------------------------------------------
public:

    explicit CoorTransBaseEci( CoorSystem::Type sourceSystem );

    const char* GetVersion() const;

};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline CoorTransBaseEci::CoorTransBaseEci( CoorSystem::Type sourceSystem )
    : CoorTrans( CoorSystem::ECI, sourceSystem ) { ; }

//-----------------------------------------------------------------------------
// access

//-----------------------------------------------------------------------------
// misc functions
inline const char* CoorTransBaseEci::GetVersion() const
{
    return COORTRANSBASEECI_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // COORTRANSBASEECI_H
