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
 * CoorTransEciNwz.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEciNwz.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciNwz

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
#include "CoorTransEciNwz.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
CoorTransEciNwz::CoorTransEciNwz( Handle< Spatial > hdlSpatial )
    : CoorTransBaseEci( CoorSystem::NWZ ),
      m_hdlSpatial( hdlSpatial )
{
    hdlSpatial->RegisterCallBack( this );
    EIClrSet(POS_EI|VEL_EI);
    UpdateState();
}

//-----------------------------------------------------------------------------
bool CoorTransEciNwz::operator==( const CoorTransEciNwz& coorTrans ) const
{
    return CoorTransBaseEci::operator==( coorTrans )
           && *(m_hdlSpatial) == *(coorTrans.m_hdlSpatial);
}

bool CoorTransEciNwz::operator!=( const CoorTransEciNwz& coorTrans ) const
{
    return !operator==( coorTrans );
}

void
CoorTransEciNwz::UpdateState()
{
    static const VectorCartesian ku( 0.0, 0.0, 1.0 );
    CoorTransBaseEci::UpdateState();
    const VectorCartesian& posECI = m_hdlSpatial->GetPosition();
    const VectorCartesian& velECI = m_hdlSpatial->GetVelocity();
    VectorCartesian zu = posECI.Unit();
    VectorCartesian wv = posECI.CrossProduct( ku );
    VectorCartesian wu = wv.Unit();
    VectorCartesian nu = wu.CrossProduct( zu );
    std::copy(nu.begin(), nu.end(), Dcm().begin_column(0));
    std::copy(wu.begin(), wu.end(), Dcm().begin_column(1));
    std::copy(zu.begin(), zu.end(), Dcm().begin_column(2));

    // double rho = sqrt( posECI.GetElement0() * posECI.GetElement0()
    //            + posECI.GetElement1() * posECI.GetElement1() ) ;

    // needs testing, constructor sets acceleration flag to not implemented
    // delete "false ||" below and change accerleration flag to true in
    // the constrcutor to test and release with acceration
    if ( false || IsEnabled( ACCELERATION ) ) {
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
