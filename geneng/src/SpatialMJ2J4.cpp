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
 * SpatialMJ2J4.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialMJ2J4.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialMJ2J4

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.



//****************************************************************************//

//==============================================================================
#include "SpatialMJ2J4.h"
#include "Normalize.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
SpatialMJ2J4
::SpatialMJ2J4( HandleConst< TimeJ2000 > hdlTime )
    : SpatialTimeDep( hdlTime ),
      m_anomalisticMeanMotion( 0.0 ),
      m_nodePrecessionRate( 0.0 ),
      m_apsidalRotationRate( 0.0 )
{
    CoorTransEciPqw* pCoorTransEciPqw
        = new CoorTransEciPqw( m_curOrbitElements.GetHdlConstEulerAngles() );
    m_hCoorTransEciPqw = Handle< CoorTransEciPqw >( pCoorTransEciPqw );
}

SpatialMJ2J4
::SpatialMJ2J4( HandleConst< TimeJ2000 > hdlTime,
                Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialTimeDep( hdlTime, hdlCoorTrans ),
      m_anomalisticMeanMotion( 0.0 ),
      m_nodePrecessionRate( 0.0 ),
      m_apsidalRotationRate( 0.0 )
{
    CoorTransEciPqw* pCoorTransEciPqw
        = new CoorTransEciPqw( m_curOrbitElements.GetHdlConstEulerAngles() );
    m_hCoorTransEciPqw = Handle< CoorTransEciPqw >( pCoorTransEciPqw );
}

SpatialMJ2J4
::SpatialMJ2J4( HandleConst< TimeJ2000 > hdlTime,
                const OrbitElKeplerian& icOrbitElements )
    : SpatialTimeDep( hdlTime ),
      m_icOrbitElements( icOrbitElements ),
      m_curOrbitElements( icOrbitElements )
{
    InitializeElementRates();
    EulerAnglesKeplerian eaRates(  m_nodePrecessionRate, 0.0, m_apsidalRotationRate );
    CoorTransEciPqw* pCoorTransEciPqw
        = new CoorTransEciPqw( m_curOrbitElements.GetHdlConstEulerAngles(), eaRates );
    pCoorTransEciPqw->Enable( ACCELERATION );
    m_hCoorTransEciPqw = Handle< CoorTransEciPqw >( pCoorTransEciPqw );
    UpdateState();
}

SpatialMJ2J4
::SpatialMJ2J4( HandleConst< TimeJ2000 > hdlTime,
                Handle< CoorTransBaseEci > hdlCoorTrans,
                const OrbitElKeplerian& icOrbitElements )
    : SpatialTimeDep( hdlTime, hdlCoorTrans ),
      m_icOrbitElements( icOrbitElements ),
      m_curOrbitElements( icOrbitElements )
{
    InitializeElementRates();
    EulerAnglesKeplerian eaRates(  m_nodePrecessionRate, 0.0, m_apsidalRotationRate );
    CoorTransEciPqw* pCoorTransEciPqw
        = new CoorTransEciPqw( m_curOrbitElements.GetHdlConstEulerAngles(), eaRates );
    pCoorTransEciPqw->Enable( ACCELERATION );
    m_hCoorTransEciPqw = Handle< CoorTransEciPqw >( pCoorTransEciPqw );
    UpdateState();
}

SpatialMJ2J4::~SpatialMJ2J4()
{ ; }

//------------------------------------------------------------------------------
std::istream& SpatialMJ2J4::Initialize( std::istream& is )
{
    is >> m_icOrbitElements;
    m_curOrbitElements = m_icOrbitElements;
    InitializeElementRates();
    EulerAnglesKeplerian eaRates(  m_nodePrecessionRate, 0.0, m_apsidalRotationRate );
    CoorTransEciPqw*  pCoorTransEciPqw
        = new CoorTransEciPqw( m_curOrbitElements.GetHdlConstEulerAngles(), eaRates );
    pCoorTransEciPqw->Enable( ACCELERATION );
    m_hCoorTransEciPqw = Handle< CoorTransEciPqw >( pCoorTransEciPqw );
    UpdateState();
    return is;
}

//------------------------------------------------------------------------------
// operators
bool SpatialMJ2J4::operator==( const SpatialMJ2J4& spatial ) const
{
    return SpatialTimeDep::operator==( spatial )
           && m_icOrbitElements == spatial.m_icOrbitElements
           && m_curOrbitElements == spatial.m_curOrbitElements
           && m_vecPqwPosition == spatial.m_vecPqwPosition
           && m_vecPqwVelocity == spatial.m_vecPqwVelocity
           && m_vecPqwAcceleration == spatial.m_vecPqwAcceleration
           && m_vecEciPosition == spatial.m_vecEciPosition
           && m_vecEciVelocity == spatial.m_vecEciVelocity
           && m_vecEciAcceleration == spatial.m_vecEciAcceleration
           && m_anomalisticMeanMotion == spatial.m_anomalisticMeanMotion
           && m_nodePrecessionRate == spatial.m_nodePrecessionRate
           && m_apsidalRotationRate == spatial.m_apsidalRotationRate
           && m_hCoorTransEciPqw == spatial.m_hCoorTransEciPqw;
}

bool SpatialMJ2J4::operator!=( const SpatialMJ2J4& spatial ) const
{
    return !operator==( spatial );
}

//------------------------------------------------------------------------------
void SpatialMJ2J4::InitializeElementRates()
{
    m_semiMajorAxisCubed = m_icOrbitElements.GetSemiMajorAxis()
                           * m_icOrbitElements.GetSemiMajorAxis()
                           * m_icOrbitElements.GetSemiMajorAxis();
    double e2 = m_icOrbitElements.GetEccentricity()
                * m_icOrbitElements.GetEccentricity();
    double n0 = sqrt( EARTH_GRAV_CONST_NMI3_PER_S2 / m_semiMajorAxisCubed );
    double b0 = 1.0 - e2;
    double b1 = sqrt(b0);
    double rop2 = EARTH_RADIUS_SQ_NMI2
                  / m_icOrbitElements.GetSemiLatusRectum()
                  / m_icOrbitElements.GetSemiLatusRectum();
    double rop4 = rop2 * rop2;
    double si   = sin( m_icOrbitElements.GetInclination() );
    double siSq = si * si;
    double ci   = cos( m_icOrbitElements.GetInclination() );
    double ciSq = ci * ci;
    double c0 = 1.5 * EARTH_J2_ZONAL * rop2 * ( 1.0 - 1.5 * siSq );
    double c1a1 = 105.0 + 144.0 * b1 + 25.0 * b0;
    double c1a2 = 30.0 - 96 * b1 - 90.0 * b0;
    double c1b1 = 16.0 * b1 + 25.0 * b0 - 15.0 + c1a2 * ciSq + c1a1 * ciSq * ciSq;
    double c1   = 3.0 * EARTH_J2_ZONAL_SQUARED * c1b1;
    double c2 = -45.0 * e2 * EARTH_J4_ZONAL
                * ( 3.0 - 30.0 * ciSq + 35.0 * ciSq * ciSq );
    double c3 = c0 + ( c1 + c2 ) * rop4 / 128.0;
    // calculate anomalistic mean motion
    m_anomalisticMeanMotion = n0 * ( 1 + b1 * c3 );
    // calculate mean right ascension rate
    double d1a1 = 12.0 - 21.0 * siSq;
    double d1a2 = 1.0 + 1.5 * e2;
    double d1a3 = 2.5 - 5.0 * e2 / 24.0 - 3.0 * b1;
    double d1b1 = 1.5 + e2 / 6.0 - 2.0 * b1 - d1a3 * siSq;
    double d1c1 = 1.0 + 1.5 * EARTH_J2_ZONAL * d1b1 * rop2;
    m_nodePrecessionRate = -1.5 * EARTH_J2_ZONAL * m_anomalisticMeanMotion
                           * rop2 * ci * d1c1
                           - 0.3125 * EARTH_J4_ZONAL
                           * n0 * d1a2 * d1a1 * ci * rop4;
    // calculate mean argument of perigee rate
    double f1a1 = 27.0 / 14.0 - 6.75 * siSq + 5.0625 * siSq * siSq;
    double f1a2 = 43.0 / 24.0 - e2 / 48.0 - 3.0 * b1;
    double f1a3 = 2.0 - 2.5 * siSq;
    double f1b1 = 12.0 / 7.0 - 93.0 / 14.0 * siSq + 5.25 * siSq * siSq + e2 * f1a1;
    double f1b2 = 2.0 + e2 / 2.0 - 2.0 * b1 - f1a2 * siSq;
    double f1c1 = 1.0 + 1.5 * EARTH_J2_ZONAL * f1b2 * rop2;
    m_apsidalRotationRate = 1.5 * EARTH_J2_ZONAL * m_anomalisticMeanMotion
                            * f1a3 * f1c1 * rop2
                            - 1.25 * EARTH_J2_ZONAL_SQUARED
                            * e2 * n0 * ciSq * ciSq * rop4
                            - 4.375 * EARTH_J4_ZONAL * n0 * f1b1 * rop4;
}

void SpatialMJ2J4::UpdateElements()
{
    double dt = GetTime() - m_icOrbitElements.GetEpoch();
    m_curOrbitElements.SetRAAscendingNode(
        normalizeZeroTwoPi( m_icOrbitElements.GetRAAscendingNode()
                            + dt * m_nodePrecessionRate ) );
    m_curOrbitElements.SetArgumentOfPeriapsis(
        normalizeZeroTwoPi( m_icOrbitElements.GetArgumentOfPeriapsis()
                            + dt * m_apsidalRotationRate ) );
    m_curOrbitElements.SetMeanAnomaly(
        normalizeZeroTwoPi( m_icOrbitElements.GetMeanAnomaly()
                            + dt * m_anomalisticMeanMotion ) );
    m_curOrbitElements.SetEpoch( GetTime() );
}

void SpatialMJ2J4::UpdateState()
{
    SpatialTimeDep::UpdateState();
    UpdateElements();
    // alias to reduce statement length
    OrbitElKeplerian& oe = m_curOrbitElements;
    // advance the satellite
    double ta  = oe.GetTrueAnomaly();
    double cta = cos(ta);
    double sta = sin(ta);
    m_radius = oe.GetSemiLatusRectum()
               / ( 1.0 + oe.GetEccentricity() * cta );
    double vfactor = m_anomalisticMeanMotion
                     * sqrt( m_semiMajorAxisCubed / oe.GetSemiLatusRectum() );
    // position and velocity in the PQW system
    m_vecPqwPosition.Set( m_radius * cta, m_radius * sta, 0.0 );
    m_vecPqwVelocity.Set( -vfactor * sta,
                          vfactor * ( oe.GetEccentricity() + cta ),
                          0.0 );

    // ------------------------------------------------------------------------
    // acceleration
    if ( IsEnabled( ACCELERATION ) ) {
        double mu_eff = m_anomalisticMeanMotion * m_anomalisticMeanMotion
                        * m_semiMajorAxisCubed;
        m_vecPqwAcceleration = VectorCartesian( -cta, -sta, 0 )
                               * mu_eff / m_radius / m_radius;
    }

    // ------------------------------------------------------------------------
    if ( m_hdlCoorTrans->GetSourceSystem() == CoorSystem::PQW ) {
        m_vecPosition = m_vecPqwPosition;
        m_vecVelocity = m_vecPqwVelocity;

        if ( IsEnabled( ACCELERATION ) ) {
            m_vecAcceleration = m_vecPqwAcceleration;
        }
    } else {
        if ( IsEnabled( ACCELERATION ) )
            m_hCoorTransEciPqw->ToBase( m_vecPqwPosition, m_vecPqwVelocity,
                                        m_vecPqwAcceleration,
                                        m_vecEciPosition, m_vecEciVelocity,
                                        m_vecEciAcceleration );
        else
            m_hCoorTransEciPqw->ToBase( m_vecPqwPosition, m_vecPqwVelocity,
                                        m_vecEciPosition, m_vecEciVelocity );

        if ( m_hdlCoorTrans->GetSourceSystem() == CoorSystem::ECI ) {
            m_vecPosition     = m_vecEciPosition;
            m_vecVelocity     = m_vecEciVelocity;
            m_vecAcceleration = m_vecEciAcceleration;
        } else {
            if ( IsEnabled( ACCELERATION ) ) {
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
}

// ============================================================================
}
// A3 namespace end
// ============================================================================

