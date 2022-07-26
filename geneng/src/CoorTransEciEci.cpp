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
 * CoorTransEciEci.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEciEci.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciEcf

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
// required headers
#include "CoorTransEciEci.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
CoorTransEciEci::CoorTransEciEci()
    : CoorTransBaseEci( CoorSystem::ECI )
{
    EIClrSet(POS_EI|VEL_EI|ACC_EI);
}

//-----------------------------------------------------------------------------
void
CoorTransEciEci::ToBase( const VectorCartesian& vecPositionSource,
                         VectorCartesian& vecPositionBase )
{
    vecPositionBase = vecPositionSource;
}

void
CoorTransEciEci::ToBase( const VectorCartesian& vecPositionSource,
                         const VectorCartesian& vecVelocitySource,
                         VectorCartesian& vecPositionBase,
                         VectorCartesian& vecVelocityBase )
{
    vecPositionBase = vecPositionSource;
    vecVelocityBase = vecVelocitySource;
}

void
CoorTransEciEci::ToBase( const VectorCartesian& vecPositionSource,
                         VectorCartesian& vecPositionBase ) const
{
    vecPositionBase = vecPositionSource;
}

void
CoorTransEciEci::ToBase( const VectorCartesian& vecPositionSource,
                         const VectorCartesian& vecVelocitySource,
                         VectorCartesian& vecPositionBase,
                         VectorCartesian& vecVelocityBase ) const
{
    vecPositionBase = vecPositionSource;
    vecVelocityBase = vecVelocitySource;
}

// acceleration
void
CoorTransEciEci::ToBase( const VectorCartesian& vecPositionSource,
                         const VectorCartesian& vecVelocitySource,
                         const VectorCartesian& vecAccelerationSource,
                         VectorCartesian& vecPositionBase,
                         VectorCartesian& vecVelocityBase,
                         VectorCartesian& vecAccelerationBase )
{
    vecPositionBase     = vecPositionSource;
    vecVelocityBase     = vecVelocitySource;
    vecAccelerationBase = vecAccelerationSource;
}

void
CoorTransEciEci::ToBase( const VectorCartesian& vecPositionSource,
                         const VectorCartesian& vecVelocitySource,
                         const VectorCartesian& vecAccelerationSource,
                         VectorCartesian& vecPositionBase,
                         VectorCartesian& vecVelocityBase,
                         VectorCartesian& vecAccelerationBase ) const
{
    vecPositionBase     = vecPositionSource;
    vecVelocityBase     = vecVelocitySource;
    vecAccelerationBase = vecAccelerationSource;
}

void
CoorTransEciEci::ToSource( const VectorCartesian& vecPositionBase,
                           VectorCartesian& vecPositionSource )
{
    vecPositionSource = vecPositionBase;
}

void
CoorTransEciEci::ToSource( const VectorCartesian& vecPositionBase,
                           const VectorCartesian& vecVelocityBase,
                           VectorCartesian& vecPositionSource,
                           VectorCartesian& vecVelocitySource )
{
    vecPositionSource = vecPositionBase;
    vecVelocitySource = vecVelocityBase;
}

void
CoorTransEciEci::ToSource( const VectorCartesian& vecPositionBase,
                           VectorCartesian& vecPositionSource ) const
{
    vecPositionSource = vecPositionBase;
}

void
CoorTransEciEci::ToSource( const VectorCartesian& vecPositionBase,
                           const VectorCartesian& vecVelocityBase,
                           VectorCartesian& vecPositionSource,
                           VectorCartesian& vecVelocitySource ) const
{
    vecPositionSource = vecPositionBase;
    vecVelocitySource = vecVelocityBase;
}

// acceleration
void
CoorTransEciEci::ToSource( const VectorCartesian& vecPositionBase,
                           const VectorCartesian& vecVelocityBase,
                           const VectorCartesian& vecAccelerationBase,
                           VectorCartesian& vecPositionSource,
                           VectorCartesian& vecVelocitySource,
                           VectorCartesian& vecAccelerationSource  )
{
    vecPositionSource     = vecPositionBase;
    vecVelocitySource     = vecVelocityBase;
    vecAccelerationSource = vecAccelerationBase;
}

void
CoorTransEciEci::ToSource( const VectorCartesian& vecPositionBase,
                           const VectorCartesian& vecVelocityBase,
                           const VectorCartesian& vecAccelerationBase,
                           VectorCartesian& vecPositionSource,
                           VectorCartesian& vecVelocitySource,
                           VectorCartesian& vecAccelerationSource  ) const
{
    vecPositionSource = vecPositionBase;
    vecVelocitySource = vecVelocityBase;
    vecAccelerationSource = vecAccelerationBase;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
