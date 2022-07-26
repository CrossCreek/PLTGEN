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
 * CoorTransEciIcr.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEciIcr.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciIcr

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
#include "CoorTransEciIcr.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
CoorTransEciIcr::CoorTransEciIcr( Handle< Spatial > hdlSpatial )
    : CoorTransBaseEci( CoorSystem::ICR ),
      m_hdlSpatial( hdlSpatial )
{
    hdlSpatial->RegisterCallBack( this );
    EIClrSet(POS_EI|VEL_EI);
    UpdateState();
}

//-----------------------------------------------------------------------------
bool CoorTransEciIcr::operator==( const CoorTransEciIcr& coorTrans ) const
{
    return CoorTransBaseEci::operator==( coorTrans )
           && *m_hdlSpatial == *(coorTrans.m_hdlSpatial);
}

bool CoorTransEciIcr::operator!=( const CoorTransEciIcr& coorTrans ) const
{
    return !operator==( coorTrans );
}

void
CoorTransEciIcr::UpdateState()
{
    CoorTransBaseEci::UpdateState();
    const VectorCartesian& posECI = m_hdlSpatial->GetPosition();
    const VectorCartesian& velECI = m_hdlSpatial->GetVelocity();
    /* compute the direction cosines for the ICR system */
    VectorCartesian ru = posECI.Unit();
    VectorCartesian cv = posECI.CrossProduct( velECI );
    VectorCartesian cu = cv.Unit();
    VectorCartesian iu = cu.CrossProduct( ru );
    std::copy(iu.begin(), iu.end(), Dcm().begin_column(0));
    std::copy(cu.begin(), cu.end(), Dcm().begin_column(1));
    std::copy(ru.begin(), ru.end(), Dcm().begin_column(2));

    if ( IsEnabled( ACCELERATION ) ) {
        const VectorCartesian& accECI = m_hdlSpatial->GetAcceleration();
        double rsq = posECI.MagnitudeSquared();
        double vdr = 2.0 * velECI.DotProduct( posECI ) / rsq;
        VectorCartesian angVel( posECI.CrossProduct( velECI ) / rsq );
        VectorCartesian a1( posECI.CrossProduct( accECI ) / rsq );
        VectorCartesian a2( vdr * angVel );
        SetAngularVelocity( angVel );
        SetAngularAcceleration( a1 - a2 );
    } else {
        SetAngularVelocity( posECI.CrossProduct( velECI )
                            / posECI.MagnitudeSquared() );
    }
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
