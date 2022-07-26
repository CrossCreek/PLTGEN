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
 * CoorTransEciNop.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEciNop.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciNop

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
#include "CoorTransEciNop.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
CoorTransEciNop::CoorTransEciNop( Handle< Spatial > hdlSpatial )
    : CoorTransBaseEci( CoorSystem::NOP ),
      m_hdlSpatial( hdlSpatial )
{
    hdlSpatial->RegisterCallBack( this );
    EIClrSet(POS_EI|VEL_EI|ACC_EI);
    UpdateState();
}

//-----------------------------------------------------------------------------
bool CoorTransEciNop::operator==( const CoorTransEciNop& coorTrans ) const
{
    return CoorTransBaseEci::operator==( coorTrans )
           && *m_hdlSpatial == *(coorTrans.m_hdlSpatial);
}

bool CoorTransEciNop::operator!=( const CoorTransEciNop& coorTrans ) const
{
    return !operator==( coorTrans );
}

void
CoorTransEciNop::UpdateState()
{
    static const VectorCartesian northECI( 0.0, 0.0, 1.0 );
    CoorTransBaseEci::UpdateState();
    const VectorCartesian& posECI = m_hdlSpatial->GetPosition();
    const VectorCartesian& velECI = m_hdlSpatial->GetVelocity();
    VectorCartesian posECI0 = posECI;
    VectorCartesian velECI0 = velECI;
    posECI0[2] = 0.0;
    velECI0[2] = 0.0;
    /* compute the direction cosines for the ICR system */
    VectorCartesian nu = -posECI.Unit();
    VectorCartesian ov = northECI.CrossProduct( nu );
    VectorCartesian ou = ov.Unit();
    VectorCartesian pu = nu.CrossProduct( ou );
    std::copy(nu.begin(), nu.end(), Dcm().begin_column(0));
    std::copy(ou.begin(), ou.end(), Dcm().begin_column(1));
    std::copy(pu.begin(), pu.end(), Dcm().begin_column(2));

    if ( IsEnabled( ACCELERATION ) ) {
        const VectorCartesian& accECI = m_hdlSpatial->GetAcceleration();
        VectorCartesian accECI0 = accECI;
        accECI0[2] = 0.0;
        double rsq = posECI0.MagnitudeSquared();
        double vdr = 2.0 * velECI0.DotProduct( posECI0 ) / rsq;
        VectorCartesian angVel( velECI0.CrossProduct( posECI0 ) / rsq );
        VectorCartesian a1( posECI0.CrossProduct( accECI0 ) / rsq );
        VectorCartesian a2( vdr * angVel );
        SetAngularVelocity( angVel );
        SetAngularAcceleration( a1 - a2 );
    } else {
        SetAngularVelocity( velECI0.CrossProduct( posECI0 )
                            / posECI0.MagnitudeSquared() );
    }
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
