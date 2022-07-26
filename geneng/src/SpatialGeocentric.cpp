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
 * SpatialGeocentric.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialGeocentric.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialGeocentric

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  10/27/1997  JAM  Initial Release
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES : Geocentric stationary object.



//****************************************************************************//

//==============================================================================
#include "SpatialGeocentric.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
SpatialGeocentric
::SpatialGeocentric( HandleConst< TimeJ2000 > hdlTime )
    : SpatialTimeDep( hdlTime ),
      m_coorTransEciEcf( hdlTime ),
      m_vecGeocentricPosition( 0.0, 0.0, EARTH_RADIUS_NMI )
{
    Initialize();
}

SpatialGeocentric
::SpatialGeocentric( HandleConst< TimeJ2000 > hdlTime,
                     Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialTimeDep( hdlTime, hdlCoorTrans ),
      m_coorTransEciEcf( hdlTime ),
      m_vecGeocentricPosition( 0.0, 0.0, EARTH_RADIUS_NMI )
{
    Initialize();
}

SpatialGeocentric
::SpatialGeocentric( HandleConst< TimeJ2000 > hdlTime,
                     const VectorGeocentric& vecPosition )
    : SpatialTimeDep( hdlTime ),
      m_coorTransEciEcf( hdlTime ),
      m_vecGeocentricPosition( vecPosition )
{
    Initialize();
}

SpatialGeocentric
::SpatialGeocentric( HandleConst< TimeJ2000 > hdlTime,
                     Handle< CoorTransBaseEci > hdlCoorTrans,
                     const VectorGeocentric& vecPosition )
    : SpatialTimeDep( hdlTime, hdlCoorTrans ),
      m_coorTransEciEcf( hdlTime ),
      m_vecGeocentricPosition( vecPosition )
{
    Initialize();
}

SpatialGeocentric::~SpatialGeocentric() { ; }

//------------------------------------------------------------------------------
std::istream& SpatialGeocentric::Initialize( std::istream& is )
{
    // input format: latitude_deg longitude_deg height_nmi
    is >> ioGeocentricHeight
       >> m_vecGeocentricPosition
       >> ioGeocentricRestore;
    Initialize();
    return is;
}

//------------------------------------------------------------------------------
double SpatialGeocentric::GetRadius() const
{
    return m_vecGeocentricPosition.GetRadius();
}

//------------------------------------------------------------------------------
// operators
bool SpatialGeocentric::operator==( const SpatialGeocentric& spatial ) const
{
    return SpatialTimeDep::operator==( spatial )
           && m_coorTransEciEcf       == spatial.m_coorTransEciEcf
           && m_vecGeocentricPosition == spatial.m_vecGeocentricPosition
           && m_vecEcfPosition        == spatial.m_vecEcfPosition;
}

bool SpatialGeocentric::operator!=( const SpatialGeocentric& spatial ) const
{
    return !operator==( spatial );
}

//------------------------------------------------------------------------------
void SpatialGeocentric::Initialize()
{
    m_vecEcfPosition = m_vecGeocentricPosition;
    m_vecEcfVelocity.Set( 0.0, 0.0, 0.0 );
    m_vecEcfAcceleration.Set( 0.0, 0.0, 0.0 );

    if ( m_hdlCoorTrans->GetSourceSystem() == CoorSystem::ECF ) {
        m_vecPosition     = m_vecEcfPosition;
        m_vecVelocity     = m_vecEcfVelocity;
        m_vecAcceleration = m_vecEcfAcceleration;
    }

    UpdateState();
}

void SpatialGeocentric::UpdateState()
{
    SpatialTimeDep::UpdateState();

    if ( CoorSystem::ECF == m_hdlCoorTrans->GetSourceSystem() ) {
        return;
    }

    if ( IsEnabled( ACCELERATION ) ) {
        m_coorTransEciEcf.ToBase( m_vecEcfPosition, m_vecEcfVelocity,
                                  m_vecEcfAcceleration,
                                  m_vecPosition,    m_vecVelocity,
                                  m_vecAcceleration );
    } else {
        m_coorTransEciEcf.ToBase( m_vecEcfPosition, m_vecEcfVelocity,
                                  m_vecPosition,    m_vecVelocity );
    }

    if ( CoorSystem::ECI != m_hdlCoorTrans->GetSourceSystem() ) {
        VectorCartesian m_vecEciPosition( m_vecPosition );
        VectorCartesian m_vecEciVelocity( m_vecVelocity );

        if ( IsEnabled( ACCELERATION ) ) {
            VectorCartesian m_vecEciAcceleration( m_vecAcceleration );
            m_hdlCoorTrans->ToSource( m_vecEciPosition, m_vecEciVelocity,
                                      m_vecEciAcceleration,
                                      m_vecPosition,    m_vecVelocity,
                                      m_vecAcceleration );
        } else {
            m_hdlCoorTrans->ToSource( m_vecEciPosition, m_vecEciVelocity,
                                      m_vecPosition,    m_vecVelocity );
        }
    }
}

// ============================================================================
}
// A3 namespace end
// ============================================================================


