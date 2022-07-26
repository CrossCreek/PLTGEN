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
 * SpatialSun.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialSun.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialSun

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
//				V3.6-001  08/22/2005   GM  Added equations for solar pressure
//										   acceleration based on Trace/Pokey
//				V3.8-017  05/08/2006   GM  Fixed calculations of eclipse status
//              V4.3-009  2007-12-10  JAM  Deleted V3.6 & 3.8 changes, Pokey
//                                         removed from GenEngr
//              V4.5      2009-07-01  JAM  Deleted unnecessary test for position
//                                         equality in operator==

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//  NOTES :



//****************************************************************************//

//==============================================================================
#ifndef SPATIALSUN_H
#define SPATIALSUN_H	"SpatialSun V4.3-009"

//------------------------------------------------------------------------------
// required headers
#include "SpatialTimeDep.h"
#include "VectorCartesian.h"
#include "Angle.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class SpatialSun : public SpatialTimeDep
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors



    explicit SpatialSun( HandleConst< TimeJ2000 > hdlTime );

    SpatialSun( HandleConst< TimeJ2000 > hdlTime,
                Handle< CoorTransBaseEci > hdlCoorTrans );

    virtual ~SpatialSun();

    //----------------------------------------------------------------------
    // run control interface and reporting functions
    virtual std::istream& Initialize( std::istream& is );

    //----------------------------------------------------------------------
    // time, position & velocity access functions
    virtual const VectorCartesian& GetAcceleration() const;
    //----------------------------------------------------------------------

    const VectorCartesian& GetUnitVector();
    const VectorCartesian& GetEciUnitVector();

    double GetRightAscension();
    double GetDeclination();
    double GetMeanLongitude();
    double GetMeanAnomaly();
    double GetEclipticLongitude();
    double GetEclipticObliquity();

    const VectorCartesian& GetUnitVector() const;
    const VectorCartesian& GetEciUnitVector() const;

    double GetRightAscension() const;
    double GetDeclination() const;
    double GetMeanLongitude() const;
    double GetMeanAnomaly() const;
    double GetEclipticLongitude() const;
    double GetEclipticObliquity() const;

    //----------------------------------------------------------------------
    // operators
    bool operator==( const SpatialSun& spatial ) const;
    bool operator!=( const SpatialSun& spatial ) const;

    //----------------------------------------------------------------------
    // misc functions
    virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    virtual void UpdateState();

    //--------------------------------------------------------------------------
private:
    VectorCartesian  m_eciUnitVector;
    VectorCartesian  m_unitVector;

    VectorCartesian  m_eciUnitVelVector;
    VectorCartesian  m_unitVelVector;

    double m_radius;
    double m_radiusRate;

    Angle m_meanLongitude;
    Angle m_meanAnomaly;
    Angle m_eclipticLongitude;
    Angle m_eclipticObliquity;
    Angle m_declination;
    Angle m_rightAscension;

    Angle m_declinationRate;
    Angle m_rightAscensionRate;
    Angle m_eclipticLongitudeRate;
};

//==============================================================================
inline const VectorCartesian&
SpatialSun::GetUnitVector()
{
    Update();
    return m_unitVector;
}

inline const VectorCartesian&
SpatialSun::GetEciUnitVector()
{
    Update();
    return m_eciUnitVector;
}

inline double
SpatialSun::GetRightAscension()
{
    Update();
    return m_rightAscension;
}

inline double
SpatialSun::GetDeclination()
{
    Update();
    return m_declination;
}

inline double
SpatialSun::GetMeanLongitude()
{
    Update();
    return m_meanLongitude;
}

inline double
SpatialSun::GetMeanAnomaly()
{
    Update();
    return m_meanAnomaly;
}

inline double
SpatialSun::GetEclipticLongitude()
{
    Update();
    return m_eclipticLongitude;
}

inline double
SpatialSun::GetEclipticObliquity()
{
    Update();
    return m_eclipticObliquity;
}

//------------------------------------------------------------------------------
inline const VectorCartesian&
SpatialSun::GetUnitVector() const
{
    if ( IsStale() ) {
        ThrowStaleVector( "GetUnitVector() const" );
    }

    return m_unitVector;
}

inline const VectorCartesian&
SpatialSun::GetEciUnitVector() const
{
    if ( IsStale() ) {
        ThrowStaleVector( "GetEciUnitVector() const" );
    }

    return m_eciUnitVector;
}

inline double
SpatialSun::GetRightAscension() const
{
    if ( IsStale() ) {
        ThrowStaleVector( "GetRightAscension() const" );
    }

    return m_rightAscension;
}

inline double
SpatialSun::GetDeclination() const
{
    if ( IsStale() ) {
        ThrowStaleVector( "GetDeclination() const" );
    }

    return m_declination;
}

inline double
SpatialSun::GetMeanLongitude() const
{
    if ( IsStale() ) {
        ThrowStaleVector( "GetMeanLongitude() const" );
    }

    return m_meanLongitude;
}

inline double
SpatialSun::GetMeanAnomaly() const
{
    if ( IsStale() ) {
        ThrowStaleVector( "GetMeanAnomaly() const" );
    }

    return m_meanAnomaly;
}

inline double
SpatialSun::GetEclipticLongitude() const
{
    if ( IsStale() ) {
        ThrowStaleVector( "GetEclipticLongitude() const" );
    }

    return m_eclipticLongitude;
}

inline double
SpatialSun::GetEclipticObliquity() const
{
    if ( IsStale() ) {
        ThrowStaleVector( "GetEclipticObliquity() const" );
    }

    return m_eclipticObliquity;
}

//------------------------------------------------------------------------------
// misc functions
inline const char* SpatialSun::GetVersion() const
{
    return SPATIALSUN_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // SPATIALSUN_H
