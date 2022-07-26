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
 * SpatialMJ2J4.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialMJ2J4.h

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
#ifndef SPATIALMJ2J4_H
#define SPATIALMJ2J4_H	"SpatialMJ2J4 Version 2.0"

//------------------------------------------------------------------------------
// required headers
#include "SpatialTimeDep.h"
#include "VectorCartesian.h"
#include "OrbitElKeplerian.h"
#include "CoorTransEciPqw.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class SpatialMJ2J4 : public SpatialTimeDep
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors



    explicit SpatialMJ2J4( HandleConst< TimeJ2000 > hdlTime );

    SpatialMJ2J4( HandleConst< TimeJ2000 > hdlTime,
                  const OrbitElKeplerian& icOrbitElements );

    SpatialMJ2J4( HandleConst< TimeJ2000 > hdlTime,
                  Handle< CoorTransBaseEci > hdlCoorTrans );

    SpatialMJ2J4( HandleConst< TimeJ2000 > hdlTime,
                  Handle< CoorTransBaseEci > hdlCoorTrans,
                  const OrbitElKeplerian& icOrbitElements );

    virtual ~SpatialMJ2J4();

    //----------------------------------------------------------------------
    // run control interface and reporting functions
    virtual std::istream& Initialize( std::istream& is );

    //----------------------------------------------------------------------
    inline const OrbitElKeplerian& GetOriginalOrbitElements() const;
    inline const OrbitElKeplerian& GetCurrentOrbitElements();

    inline double GetAnomalisticMeanMotion() const;
    inline double GetNodePrecessionRate() const;
    inline double GetApsidalRotationRate() const;

    inline double GetTimeSincePerigee();
    inline double GetTimeSinceApogee();

    //----------------------------------------------------------------------
    // operators
    bool operator==( const SpatialMJ2J4& spatial ) const;
    bool operator!=( const SpatialMJ2J4& spatial ) const;

    //----------------------------------------------------------------------
    // misc functions
    virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    virtual void UpdateState();

    //--------------------------------------------------------------------------
private:
    OrbitElKeplerian m_icOrbitElements;
    OrbitElKeplerian m_curOrbitElements;
    VectorCartesian  m_vecPqwPosition;
    VectorCartesian  m_vecPqwVelocity;
    VectorCartesian  m_vecPqwAcceleration;
    VectorCartesian  m_vecEciPosition;
    VectorCartesian  m_vecEciVelocity;
    VectorCartesian  m_vecEciAcceleration;

    double m_anomalisticMeanMotion; // rad/sec
    double m_nodePrecessionRate;        // rad/sec
    double m_apsidalRotationRate;       // rad/sec
    double m_radius;
    double m_semiMajorAxisCubed;

    Handle< CoorTransEciPqw > m_hCoorTransEciPqw;

    void InitializeElementRates();
    void UpdateElements();
};

//==============================================================================
inline const OrbitElKeplerian&
SpatialMJ2J4::GetOriginalOrbitElements() const
{
    return m_icOrbitElements;
}

inline const OrbitElKeplerian&
SpatialMJ2J4::GetCurrentOrbitElements()
{
    if ( m_curOrbitElements.GetEpoch() != GetTime() ) {
        UpdateElements();
    }

    return m_curOrbitElements;
}

inline double SpatialMJ2J4::GetAnomalisticMeanMotion() const
{
    return m_anomalisticMeanMotion;
}

inline double SpatialMJ2J4::GetNodePrecessionRate() const
{
    return m_nodePrecessionRate;
}

inline double SpatialMJ2J4::GetApsidalRotationRate() const
{
    return m_apsidalRotationRate;
}

inline double SpatialMJ2J4::GetTimeSincePerigee()
{
    return GetCurrentOrbitElements().GetMeanAnomaly()
           / m_anomalisticMeanMotion;
}

inline double SpatialMJ2J4::GetTimeSinceApogee()
{
    double angleFromApogee = GetCurrentOrbitElements().GetMeanAnomaly() - PI;

    if ( angleFromApogee < 0.0 ) {
        angleFromApogee += TWO_PI;
    }

    return angleFromApogee / m_anomalisticMeanMotion;
}

//------------------------------------------------------------------------------
// misc functions
inline const char* SpatialMJ2J4::GetVersion() const
{
    return SPATIALMJ2J4_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // SPATIALMJ2J4_H
