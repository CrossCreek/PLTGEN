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
 * SpatialSun.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Sun Spatial Object based on low precision Astro Al formula
// Refer to SpatialJun.h for more info.
//===============================================================================
#include "SpatialSun.h"
#include "Normalize.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
SpatialSun
::SpatialSun( HandleConst< TimeJ2000 > hdlTime )
    : SpatialTimeDep( hdlTime )
{
    Update();
}

SpatialSun
::SpatialSun( HandleConst< TimeJ2000 > hdlTime,
              Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialTimeDep( hdlTime, hdlCoorTrans )
{
    Update();
}

SpatialSun::~SpatialSun() { ; }

//------------------------------------------------------------------------------
std::istream& SpatialSun::Initialize( std::istream& is )
{
    return is;
}

//------------------------------------------------------------------------------
const VectorCartesian& SpatialSun::GetAcceleration() const
{
    throw std::runtime_error( "SpatialSun::GetAcceleration() not implemented" );
    // return m_vecAcceleration;
}

//------------------------------------------------------------------------------
// operators
bool SpatialSun::operator==( const SpatialSun& spatial ) const
{
    return SpatialTimeDep::operator==( spatial )
           && m_eciUnitVector == spatial.m_eciUnitVector;
}

bool SpatialSun::operator!=( const SpatialSun& spatial ) const
{
    return !operator==( spatial );
}

void SpatialSun::UpdateState()
{
    // ------------------------------------------------------------------------
    // update base class
    SpatialTimeDep::UpdateState();
    // ------------------------------------------------------------------------
    // current time in days w.r.t. J2000
    double j2000Day = GetTime() * SEC_TO_DAY;
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Position of the Sun w.r.t. the Earth
    // ------------------------------------------------------------------------
    // calculate the mean longitude of the sun, corrected for aberration
    static const double d_mls = 0.017202792; // rad/day
    static const double c_mls = 4.89495042;  // rad
    m_meanLongitude = normalizeZeroTwoPi( c_mls + d_mls * j2000Day ); // rad
    // ------------------------------------------------------------------------
    // calculate the mean anomaly of the sun
    static const double d_mas = 0.01720197034; // rad/day
    static const double c_mas = 6.240040768;   // rad
    m_meanAnomaly = normalizeZeroTwoPi( c_mas + d_mas * j2000Day ); // rad
    double cos_mas  = cos( m_meanAnomaly );
    double cos_2mas = 2.0 * cos_mas * cos_mas - 1;
    double sin_mas  = sin( m_meanAnomaly );
    double sin_2mas = 2.0 * sin_mas * cos_mas;
    // ------------------------------------------------------------------------
    // calculate the ecliptic longitude
    static const double c0_elng = 0.033423055; // rad
    static const double c1_elng = 0.000349065; // rad
    m_eclipticLongitude = m_meanLongitude
                          + c0_elng * sin_mas
                          + c1_elng * sin_2mas;  // rad
    m_eclipticLongitude = normalizeZeroTwoPi( m_eclipticLongitude );
    double sin_elng = sin( m_eclipticLongitude );
    // ------------------------------------------------------------------------
    // calculate the obliquity of the ecliptic
    static const double d_oblq = 6.9813e-9;   // rad/day
    static const double c_oblq = 0.409087723; // rad
    m_eclipticObliquity = c_oblq + d_oblq * j2000Day; // rad
    double sin_oblq = sin( m_eclipticObliquity );
    // ------------------------------------------------------------------------
    // calculate the declination
    double sin_dec = sin_oblq * sin_elng;
    m_declination  = asin( sin_dec ); // rad
    double cos_dec = cos( m_declination );
    // ------------------------------------------------------------------------
    // calculate the right ascension
    m_rightAscension = atan2( cos(m_eclipticObliquity) * sin_elng,
                              cos(m_eclipticLongitude) );
    double sin_ras = sin( m_rightAscension );
    double cos_ras = cos( m_rightAscension );
    // ------------------------------------------------------------------------
    // compute position unit vector
    m_eciUnitVector.Set( cos_dec * cos_ras, cos_dec * sin_ras, sin_dec );
    // ------------------------------------------------------------------------
    // calculate the radius
    static const double c0_rad = 23458.07467;
    static const double c1_rad = 391.92956;
    static const double c2_rad = 3.28367;
    m_radius = EARTH_RADIUS_NMI * ( c0_rad - c1_rad * cos_mas
                                    - c2_rad * cos_2mas );

    // ------------------------------------------------------------------------
    // rotate the position unit vector to the current coordinate system
    if ( m_hdlCoorTrans->GetSourceSystem() == CoorSystem::ECI ) {
        m_unitVector = m_eciUnitVector;
    } else {
        m_hdlCoorTrans->ToSource( m_eciUnitVector, m_unitVector );
    }

    // ------------------------------------------------------------------------
    // set the position vector
    m_vecPosition = m_unitVector * m_radius;

    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Velocity of the Sun w.r.t. the Earth
    // ------------------------------------------------------------------------
    if ( IsEnabled( VELOCITY ) ) {
        // --------------------------------------------------------------------
        // mean longitude rate
        // static const double meanLongitudeRate = d_mls;
        // --------------------------------------------------------------------
        // mean anomaly rate
        // static const double meanAnomalyRate = d_mas;
        // --------------------------------------------------------------------
        // calculate the ecliptic longitude rate
        m_eclipticLongitudeRate = d_mls + d_mas
                                  * ( c0_elng*cos_mas + 2.0 * c1_elng*cos_2mas );
        // --------------------------------------------------------------------
        // obliquity of the ecliptic rate
        // static const double eclipticObliquityRate = d_oblq;
        // --------------------------------------------------------------------
        // calculate the declination rate
        double cos_elng = cos( m_eclipticLongitude );
        double cos_oblq = cos( m_eclipticObliquity );
        m_declinationRate = ( d_oblq * cos_oblq * sin_elng
                              + m_eclipticLongitudeRate * sin_oblq * cos_elng )
                            / cos_dec;
        // --------------------------------------------------------------------
        // calculate the right ascension rate
        m_rightAscensionRate = cos_ras * cos_ras / cos_elng
                               * ( m_eclipticLongitudeRate * cos_oblq / cos_elng
                                   - d_oblq * sin_oblq * sin_elng );
        // --------------------------------------------------------------------
        // compute velocity unit vector
        double cos_dec_dot = -sin_dec * m_declinationRate;
        double sin_dec_dot =  cos_dec * m_declinationRate;
        double cos_ras_dot = -sin_dec * m_rightAscensionRate;
        double sin_ras_dot =  cos_dec * m_rightAscensionRate;
        m_eciUnitVelVector.Set( cos_dec_dot * cos_ras + cos_dec * cos_ras_dot,
                                cos_dec_dot * sin_ras + cos_dec * sin_ras_dot,
                                sin_dec_dot );
        // --------------------------------------------------------------------
        // calculate the radius rate
        m_radiusRate = EARTH_RADIUS_NMI
                       * ( c1_rad * sin_mas  * d_mas
                           + c2_rad * sin_2mas * 2.0 * d_mas );

        // --------------------------------------------------------------------
        // rotate the velocity unit vector to the current coordinate system
        if ( m_hdlCoorTrans->GetSourceSystem() == CoorSystem::ECI ) {
            m_unitVelVector = m_eciUnitVelVector;
        } else {
            m_hdlCoorTrans->ToSource( m_eciUnitVector, m_eciUnitVelVector,
                                      m_unitVector, m_unitVelVector);
        }

        m_vecVelocity = m_unitVelVector * m_radius
                        + m_unitVector * m_radiusRate;
    }
}

// ============================================================================
}
// A3 namespace end
// ============================================================================


