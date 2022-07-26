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
 * CoorTransEciNes.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEciNes.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciNes

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  02/27/1998  JAM  Initial Release
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

//=============================================================================
#ifndef COORTRANSECINES_H
#define COORTRANSECINES_H	"CoorTransEciNes V2.0"

//-----------------------------------------------------------------------------
// required headers
#include "CoorTransBaseEci.h"
#include "HandleConst.h"
#include "Spatial.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// ECI/NES tranformations
// base system: ECI
// source system: NES
class CoorTransEciNes : public CoorTransBaseEci
{
    //-------------------------------------------------------------------------
public:

    explicit CoorTransEciNes( Handle< Spatial > hdlSpatial );

    bool operator==( const CoorTransEciNes& coorTrans ) const;
    bool operator!=( const CoorTransEciNes& coorTrans ) const;

    const char* GetVersion() const;

protected:
    virtual void UpdateState();
    virtual void CallBack( const AutoUpdate* pUpdatedObject );

private:
    Handle< Spatial > m_hdlSpatial;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
inline void
CoorTransEciNes::CallBack( const AutoUpdate* ) // pUpdatedObject
{
    MakeStale();
}

inline const char* CoorTransEciNes::GetVersion() const
{
    return COORTRANSECINES_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // COORTRANSECINES_H

