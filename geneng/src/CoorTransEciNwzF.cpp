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
 * CoorTransEciNwzF.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// ECI(TOD)-North West Zenith Geocentric coordinate transformation class
// Refer to CoorTransEciNwzF.h for more info.
// =============================================================================
// required headers
#include "CoorTransEciNwzF.h"
#include "VectorGeocentric.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
CoorTransEciNwzF::CoorTransEciNwzF( HandleConst< TimeJ2000 > hdlTime,
                                    double geocentricLatitude, double longitude )
    : CoorTransBaseEci( CoorSystem::NWZF ),
      m_ctEciEcf( hdlTime ),
      m_geocentricLatitude( geocentricLatitude ),
      m_longitude( longitude )
{
    static const VectorCartesian ku( 0.0, 0.0, 1.0 );
    EIClrSet(POS_EI|VEL_EI|ACC_EI);
    VectorGeocentric refGeoC( geocentricLatitude, longitude, 1.0 );
    VectorCartesian zu( refGeoC );
    VectorCartesian wv = zu.CrossProduct( ku );
    VectorCartesian wu = wv.Unit();
    VectorCartesian nu = wu.CrossProduct( zu );
    std::copy(nu.begin(), nu.end(), Dcm().begin_column(0));
    std::copy(wu.begin(), wu.end(), Dcm().begin_column(1));
    std::copy(zu.begin(), zu.end(), Dcm().begin_column(2));
}

//-----------------------------------------------------------------------------
void
CoorTransEciNwzF::ToBase( const VectorCartesian& vecPositionSource,
                          VectorCartesian& vecPositionBase )
{
    VectorCartesian vecPositionEcf;
    CoorTrans::ToBase( vecPositionSource, vecPositionEcf );
    m_ctEciEcf.ToBase( vecPositionEcf, vecPositionBase );
}

void
CoorTransEciNwzF::ToBase( const VectorCartesian& vecPositionSource,
                          const VectorCartesian& vecVelocitySource,
                          VectorCartesian& vecPositionBase,
                          VectorCartesian& vecVelocityBase )
{
    VectorCartesian vecPositionEcf, vecVelocityEcf;
    CoorTrans::ToBase( vecPositionSource, vecVelocitySource,
                       vecPositionEcf,    vecVelocityEcf );
    m_ctEciEcf.ToBase( vecPositionEcf,  vecVelocityEcf,
                       vecPositionBase, vecVelocityBase );
}

void
CoorTransEciNwzF::ToBase( const VectorCartesian& vecPositionSource,
                          const VectorCartesian& vecVelocitySource,
                          const VectorCartesian& vecAccelerationSource,
                          VectorCartesian& vecPositionBase,
                          VectorCartesian& vecVelocityBase,
                          VectorCartesian& vecAccelerationBase )
{
    VectorCartesian vecPositionEcf, vecVelocityEcf, vecAccelerationEcf;
    CoorTrans::ToBase( vecPositionSource, vecVelocitySource,
                       vecAccelerationSource,
                       vecPositionEcf,    vecVelocityEcf,
                       vecAccelerationEcf );
    m_ctEciEcf.ToBase( vecPositionEcf,  vecVelocityEcf,
                       vecAccelerationEcf,
                       vecPositionBase, vecVelocityBase,
                       vecAccelerationBase );
}

void
CoorTransEciNwzF::ToSource( const VectorCartesian& vecPositionBase,
                            VectorCartesian& vecPositionSource )
{
    VectorCartesian vecPositionEcf;
    m_ctEciEcf.ToSource( vecPositionBase, vecPositionEcf );
    CoorTrans::ToSource( vecPositionEcf, vecPositionSource );
}

void
CoorTransEciNwzF::ToSource( const VectorCartesian& vecPositionBase,
                            const VectorCartesian& vecVelocityBase,
                            VectorCartesian& vecPositionSource,
                            VectorCartesian& vecVelocitySource )
{
    VectorCartesian vecPositionEcf, vecVelocityEcf;
    m_ctEciEcf.ToSource( vecPositionBase, vecVelocityBase,
                         vecPositionEcf,  vecVelocityEcf );
    CoorTrans::ToSource( vecPositionEcf,    vecVelocityEcf,
                         vecPositionSource, vecVelocitySource );
}

void
CoorTransEciNwzF::ToSource( const VectorCartesian& vecPositionBase,
                            const VectorCartesian& vecVelocityBase,
                            const VectorCartesian& vecAccelerationBase,
                            VectorCartesian& vecPositionSource,
                            VectorCartesian& vecVelocitySource,
                            VectorCartesian& vecAccelerationSource )
{
    VectorCartesian vecPositionEcf, vecVelocityEcf, vecAccelerationEcf;
    m_ctEciEcf.ToSource( vecPositionBase, vecVelocityBase,
                         vecAccelerationBase,
                         vecPositionEcf,  vecVelocityEcf,
                         vecAccelerationEcf );
    CoorTrans::ToSource( vecPositionEcf,    vecVelocityEcf,
                         vecAccelerationEcf,
                         vecPositionSource, vecVelocitySource,
                         vecAccelerationSource );
}

void
CoorTransEciNwzF::ToBase( const VectorCartesian& vecPositionSource,
                          VectorCartesian& vecPositionBase ) const
{
    VectorCartesian vecPositionEcf;
    CoorTrans::ToBase( vecPositionSource, vecPositionEcf );
    m_ctEciEcf.ToBase( vecPositionEcf, vecPositionBase );
}

void
CoorTransEciNwzF::ToBase( const VectorCartesian& vecPositionSource,
                          const VectorCartesian& vecVelocitySource,
                          VectorCartesian& vecPositionBase,
                          VectorCartesian& vecVelocityBase ) const
{
    VectorCartesian vecPositionEcf, vecVelocityEcf;
    CoorTrans::ToBase( vecPositionSource, vecVelocitySource,
                       vecPositionEcf,    vecVelocityEcf );
    m_ctEciEcf.ToBase( vecPositionEcf,  vecVelocityEcf,
                       vecPositionBase, vecVelocityBase );
}

void
CoorTransEciNwzF::ToBase( const VectorCartesian& vecPositionSource,
                          const VectorCartesian& vecVelocitySource,
                          const VectorCartesian& vecAccelerationSource,
                          VectorCartesian& vecPositionBase,
                          VectorCartesian& vecVelocityBase,
                          VectorCartesian& vecAccelerationBase ) const
{
    VectorCartesian vecPositionEcf, vecVelocityEcf, vecAccelerationEcf;
    CoorTrans::ToBase( vecPositionSource, vecVelocitySource,
                       vecAccelerationSource,
                       vecPositionEcf,    vecVelocityEcf,
                       vecAccelerationEcf );
    m_ctEciEcf.ToBase( vecPositionEcf,  vecVelocityEcf,
                       vecAccelerationEcf,
                       vecPositionBase, vecVelocityBase,
                       vecAccelerationBase );
}

void
CoorTransEciNwzF::ToSource( const VectorCartesian& vecPositionBase,
                            VectorCartesian& vecPositionSource ) const
{
    VectorCartesian vecPositionEcf;
    m_ctEciEcf.ToSource( vecPositionBase, vecPositionEcf );
    CoorTrans::ToSource( vecPositionEcf, vecPositionSource );
}

void
CoorTransEciNwzF::ToSource( const VectorCartesian& vecPositionBase,
                            const VectorCartesian& vecVelocityBase,
                            VectorCartesian& vecPositionSource,
                            VectorCartesian& vecVelocitySource ) const
{
    VectorCartesian vecPositionEcf, vecVelocityEcf;
    m_ctEciEcf.ToSource( vecPositionBase, vecVelocityBase,
                         vecPositionEcf,  vecVelocityEcf );
    CoorTrans::ToSource( vecPositionEcf,    vecVelocityEcf,
                         vecPositionSource, vecVelocitySource );
}

void
CoorTransEciNwzF::ToSource( const VectorCartesian& vecPositionBase,
                            const VectorCartesian& vecVelocityBase,
                            const VectorCartesian& vecAccelerationBase,
                            VectorCartesian& vecPositionSource,
                            VectorCartesian& vecVelocitySource,
                            VectorCartesian& vecAccelerationSource ) const
{
    VectorCartesian vecPositionEcf, vecVelocityEcf, vecAccelerationEcf;
    m_ctEciEcf.ToSource( vecPositionBase, vecVelocityBase,
                         vecAccelerationBase,
                         vecPositionEcf,  vecVelocityEcf,
                         vecAccelerationEcf );
    CoorTrans::ToSource( vecPositionEcf,    vecVelocityEcf,
                         vecAccelerationEcf,
                         vecPositionSource, vecVelocitySource,
                         vecAccelerationSource );
}

bool CoorTransEciNwzF::operator==( const CoorTransEciNwzF& coorTrans ) const
{
    return CoorTransBaseEci::operator==( coorTrans )
           && m_ctEciEcf           == coorTrans.m_ctEciEcf
           && m_geocentricLatitude == coorTrans.m_geocentricLatitude
           && m_longitude          == coorTrans.m_longitude;
}

bool CoorTransEciNwzF::operator!=( const CoorTransEciNwzF& coorTrans ) const
{
    return !operator==( coorTrans );
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
