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
 * CoorTransEciPqsA.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEciPqsA.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciPqsA

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
#include "CoorTransEciPqsA.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
CoorTransEciPqsA::CoorTransEciPqsA( Handle< Spatial > hdlSpatial,
                                    double spinAxisRightAscension,
                                    double spinAxisDeclination )
    : CoorTransBaseEci( CoorSystem::PQSA ),
      m_hdlSpatial( hdlSpatial ),
      m_spinAxis( spinAxisRightAscension, spinAxisDeclination, 1.0 )
{
    hdlSpatial->RegisterCallBack( this );
    m_spinAxisCartesian = m_spinAxis;
    EIClrSet(POS_EI|VEL_EI);
    UpdateState();
}

//-----------------------------------------------------------------------------
bool CoorTransEciPqsA::operator==( const CoorTransEciPqsA& coorTrans ) const
{
    return CoorTransBaseEci::operator==( coorTrans )
           && *m_hdlSpatial == *(coorTrans.m_hdlSpatial);
}

bool CoorTransEciPqsA::operator!=( const CoorTransEciPqsA& coorTrans ) const
{
    return !operator==( coorTrans );
}

void
CoorTransEciPqsA::UpdateState()
{
    CoorTransBaseEci::UpdateState();
    const VectorCartesian& posECI = m_hdlSpatial->GetPosition();
    const VectorCartesian& velECI = m_hdlSpatial->GetVelocity();

    if ( IsEnabled( ACCELERATION ) ) {
        ThrowNotImplemented( "CoorTransEciNwz::UpdateState(): Acceleration" );
    }

    VectorCartesian rho( posECI.GetElement0(), posECI.GetElement1(), 0.0 );
    static const VectorCartesian ku( 0.0, 0.0, 1.0 );
    VectorCartesian& su = m_spinAxisCartesian;
    VectorCartesian eu = ku.CrossProduct( rho );
    VectorCartesian pv = m_spinAxis.GetDeclination() > 0.0
                         ? su.CrossProduct( eu )
                         : eu.CrossProduct( m_spinAxis );
    VectorCartesian pu = pv.Unit();
    VectorCartesian qu = su.CrossProduct( pu );
    std::copy(pu.begin(), pu.end(), Dcm().begin_column(0));
    std::copy(qu.begin(), qu.end(), Dcm().begin_column(1));
    std::copy(su.begin(), su.end(), Dcm().begin_column(2));
    VectorCartesian ru = posECI.Unit();

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
