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
 * OpPeriodTpvEclipse.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/
//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : OpPeriodTpvEclipse.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : OpPeriodTpvEclipse

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  980126  JAM  Alpha test
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :



//****************************************************************************//

//==============================================================================
#ifndef OPPERIODTPVECLIPSE_H
#define OPPERIODTPVECLIPSE_H	"OpPeriodTpvEclipse V2.0"

//------------------------------------------------------------------------------
// required headers
#include "OpPeriodTpv.h"
#include "SpatialSun.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class OpPeriodTpvEclipse : public OpPeriodTpv
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    enum EclipseCond {ILLUMINATED = 0x01,
                      PENUMBRA    = 0x02,
                      UMBRA       = 0x04
                     };
    //----------------------------------------------------------------------
    // constructors & destructors
    inline OpPeriodTpvEclipse( Handle< SpatialTimeDep > hdlSpatial,
                               const std::string& strReqLighting );
    inline virtual ~OpPeriodTpvEclipse();

    //----------------------------------------------------------------------
    // test function
    inline virtual bool Test();

    //----------------------------------------------------------------------
    // identification functions
    inline virtual OpPeriodType GetType() const;

    //----------------------------------------------------------------------
    // operators
    inline bool operator==( const OpPeriodTpvEclipse& opPeriod ) const;
    inline bool operator!=( const OpPeriodTpvEclipse& opPeriod ) const;

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
    char m_reqLighting;
    SpatialSun m_spatialSun;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// constructors & destructors
inline OpPeriodTpvEclipse::OpPeriodTpvEclipse( Handle< SpatialTimeDep > hdlSpatial,
        const std::string& strReqLighting )
    : OpPeriodTpv( hdlSpatial ),
      m_reqLighting( 0 ),
      m_spatialSun( hdlSpatial->GetHdlConstTime() )
{
    if ( strReqLighting == "ILLUMINATED" ) {
        m_reqLighting = ILLUMINATED;
    } else if ( strReqLighting == "PENUMBRA" ) {
        m_reqLighting = PENUMBRA;
    } else if ( strReqLighting == "UMBRA" ) {
        m_reqLighting = UMBRA;
    } else if ( strReqLighting == "ILLUMINATED/PENUMBRA" ) {
        m_reqLighting = ILLUMINATED | PENUMBRA;
    } else if ( strReqLighting == "PENUMBRA/UMBRA" ) {
        m_reqLighting = PENUMBRA | UMBRA;
    } else {
        throw std::invalid_argument( "OpPeriodTpvEclipse--invalid lighting: "
                                     + strReqLighting );
    }
}

inline OpPeriodTpvEclipse::~OpPeriodTpvEclipse()
{ ; }

//------------------------------------------------------------------------------
// test function
inline bool OpPeriodTpvEclipse::Test()
{
    static const double DLTRAD = SUN_RADIUS_NMI - EARTH_RADIUS_NMI;
    static const double SUMRAD = SUN_RADIUS_NMI + EARTH_RADIUS_NMI;
    // static SpatialSun spatialSun( m_hdlSpatial->GetHdlConstTime() );
    EclipseCond lightingCondition = ILLUMINATED;
    const VectorCartesian& position = m_hdlSpatial->GetPosition();
    double eAng = acos(EARTH_RADIUS_NMI / position.Magnitude());
    /* angle from the sun line to penumbra */
    double pAng = eAng - SUMRAD / m_spatialSun.GetRadius() + HALF_PI;
    /* angle from the sun line to numbra */
    double uAng = eAng + DLTRAD / m_spatialSun.GetRadius() + HALF_PI;
    /* angle from the sun line to the s/c */
    double sAng = position.SeparationAngle( m_spatialSun.GetUnitVector() );

    if      (sAng > uAng) {
        lightingCondition = UMBRA;
    } else if (sAng > pAng) {
        lightingCondition = PENUMBRA;
    }

    m_bLastStatus = m_reqLighting && lightingCondition;
    return m_bLastStatus;
}

//------------------------------------------------------------------------------
// identification functions
inline OpPeriodTpvEclipse::OpPeriodType OpPeriodTpvEclipse::GetType() const
{
    return TPV_ECLIPSE;
}

//------------------------------------------------------------------------------
// operators
inline bool OpPeriodTpvEclipse::operator==( const OpPeriodTpvEclipse& opPeriod ) const
{
    return OpPeriodTpv::operator==( opPeriod )
           && ( m_reqLighting == opPeriod.m_reqLighting );
}

inline bool OpPeriodTpvEclipse::operator!=( const OpPeriodTpvEclipse& opPeriod ) const
{
    return !operator==( opPeriod );
}

//------------------------------------------------------------------------------
// misc functions
inline const char* OpPeriodTpvEclipse::GetVersion() const
{
    return OPPERIODTPVECLIPSE_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
