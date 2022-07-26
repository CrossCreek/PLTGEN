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
 * CoorTransBaseEcf.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransBaseEcf.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransBaseEcf

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  02/27/1998  JAM  Initial Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

//=============================================================================
#ifndef COORTRANSBASEECF_H
#define COORTRANSBASEECF_H	"CoorTransBaseEcf V2.0"

//-----------------------------------------------------------------------------
// required headers
#include "CoorTrans.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class CoorTransBaseEcf : public CoorTrans
{
    //-------------------------------------------------------------------------
public:

    explicit CoorTransBaseEcf( CoorSystem::Type sourceSystem );

    const char* GetVersion() const;

};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline CoorTransBaseEcf::CoorTransBaseEcf( CoorSystem::Type sourceSystem )
    : CoorTrans( CoorSystem::ECF, sourceSystem ) { ; }

//-----------------------------------------------------------------------------
// access

//-----------------------------------------------------------------------------
// misc functions
inline const char* CoorTransBaseEcf::GetVersion() const
{
    return COORTRANSBASEECF_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // CoorTransBaseEcf_H
