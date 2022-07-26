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
 * SpatialGeocentricGrid.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialGeocentricGrid.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialGeocentricGrid

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V01.00  2002-02-20  JAM  Initial Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :



//****************************************************************************//

//==============================================================================
#include "SpatialGeocentricGrid.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
SpatialGeocentricGrid
::SpatialGeocentricGrid( HandleConst< TimeJ2000 > hdlTime )
    : SpatialTimeDep( hdlTime ),
      m_coorTransEciEcf( hdlTime )
{
    Initialize();
}

SpatialGeocentricGrid
::SpatialGeocentricGrid( HandleConst< TimeJ2000 > hdlTime,
                         Angle latMin, Angle latMax, Angle latInc,
                         Angle lonMin, Angle lonMax, Angle lonInc )
    : SpatialTimeDep( hdlTime ),
      m_coorTransEciEcf( hdlTime ),
      m_rngLat( latMin, latMax, latInc ),
      m_rngLon( lonMin, lonMax, lonInc )
{
    Initialize();
}

SpatialGeocentricGrid
::SpatialGeocentricGrid( HandleConst< TimeJ2000 > hdlTime,
                         Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialTimeDep( hdlTime, hdlCoorTrans ),
      m_coorTransEciEcf( hdlTime )
{
    Initialize();
}

SpatialGeocentricGrid
::SpatialGeocentricGrid( HandleConst< TimeJ2000 > hdlTime,
                         Handle< CoorTransBaseEci > hdlCoorTrans,
                         Angle latMin, Angle latMax, Angle latInc,
                         Angle lonMin, Angle lonMax, Angle lonInc )
    : SpatialTimeDep( hdlTime),
      m_coorTransEciEcf( hdlTime ),
      m_rngLat( latMin, latMax, latInc ),
      m_rngLon( lonMin, lonMax, lonInc )
{
    Initialize();
}

SpatialGeocentricGrid::~SpatialGeocentricGrid() { ; }

//------------------------------------------------------------------------------
std::istream& SpatialGeocentricGrid::Initialize( std::istream& is )
{
    // input format: latitude_deg longitude_deg height_nmi
    is >> m_rngLat >> m_rngLon;
    Initialize();
    return is;
}

//------------------------------------------------------------------------------
double SpatialGeocentricGrid::GetRadius() const
{
    return m_vecGeocentricPosition.GetRadius();
}

//------------------------------------------------------------------------------
// operators
bool SpatialGeocentricGrid::operator==( const SpatialGeocentricGrid& spatial ) const
{
    return SpatialTimeDep::operator==( spatial )
           && m_coorTransEciEcf       == spatial.m_coorTransEciEcf
           && m_vecGeocentricPosition == spatial.m_vecGeocentricPosition
           && m_vecEcfPosition        == spatial.m_vecEcfPosition
           && m_rngLat                == spatial.m_rngLat
           && m_rngLon                == spatial.m_rngLon;
}

bool SpatialGeocentricGrid::operator!=( const SpatialGeocentricGrid& spatial ) const
{
    return !operator==( spatial );
}

//------------------------------------------------------------------------------
void SpatialGeocentricGrid::Initialize()
{
    m_vecGeocentricPosition.Set( *(m_rngLat.begin() + m_rngLat.GetSize()/2),
                                 *(m_rngLon.begin() + m_rngLon.GetSize()/2),
                                 EARTH_RADIUS_NMI );
    m_vecEcfPosition = m_vecGeocentricPosition;
    m_vecEcfVelocity.Set( 0.0, 0.0, 0.0 );
    m_vecEcfAcceleration.Set( 0.0, 0.0, 0.0 );

    if ( m_hdlCoorTrans->GetSourceSystem() == CoorSystem::ECF ) {
        m_vecPosition     = m_vecEcfPosition;
        m_vecVelocity     = m_vecEcfVelocity;
        m_vecAcceleration = m_vecEcfAcceleration;
    }

    m_mtxPts = MatrixBase< VectorCartesian >( m_rngLat.GetSize(), m_rngLon.GetSize() );
    VectorCartesian  pos;
    VectorGeocentric llr;
    Range< Angle >::const_iterator itrLat, endLat = m_rngLat.end();
    Range< Angle >::const_iterator itrLon, endLon = m_rngLon.end();
    MatrixBase< VectorCartesian >::iterator itrPt = m_mtxPts.begin();
    double lon;

    //CPPCHECK_FIX Performance - prefix iterator change
    for ( itrLat = m_rngLat.begin(); itrLat != endLat; ++itrLat ) {
        double lat;
        lat = fabs( *itrLat ) < 1.0e-8 ? 0.0 : static_cast<double>(*itrLat);

        //CPPCHECK_FIX Performance - prefix iterator change
        for ( itrLon = m_rngLon.begin(); itrLon != endLon; ++itrLon, ++itrPt ) {
            lon = fabs( *itrLon ) < 1.0e-8 ? 0.0 : static_cast<double>(*itrLon);
            llr.Set( lat, lon, EARTH_RADIUS_NMI );
            pos = llr;
            *itrPt = pos;
        }
    }

    UpdateState();
}

void SpatialGeocentricGrid::UpdateState()
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


