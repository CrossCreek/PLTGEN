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
 * SpatialGeocentric.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialGeocentric.h

//  NAMESPACE : A3

//  SYSTEM    :

//  SUBSYSTEM :

//  MODULE(s) : SpatialGeocentric

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  10/27/1997  JAM  Initial Release
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :  system class.



//****************************************************************************//

//==============================================================================
#ifndef SPATIALGEOCENTRIC_H
#define SPATIALGEOCENTRIC_H	"SpatialGeocentric V2.0"

//------------------------------------------------------------------------------
// required headers
#include "SpatialTimeDep.h"
#include "VectorGeocentric.h"
#include "VectorCartesian.h"
#include "CoorTransEciEcf.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class SpatialGeocentric : public SpatialTimeDep
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors



    explicit SpatialGeocentric( HandleConst< TimeJ2000 > hdlTime );

    SpatialGeocentric( HandleConst< TimeJ2000 > hdlTime,
                       const VectorGeocentric& vecLocation );

    SpatialGeocentric( HandleConst< TimeJ2000 > hdlTime,
                       Handle< CoorTransBaseEci > hdlCoorTrans );

    SpatialGeocentric( HandleConst< TimeJ2000 > hdlTime,
                       Handle< CoorTransBaseEci > hdlCoorTrans,
                       const VectorGeocentric& vecLocation );

    virtual ~SpatialGeocentric();

    //----------------------------------------------------------------------
    // run control interface and reporting functions
    virtual std::istream& Initialize( std::istream& is );

    //----------------------------------------------------------------------
    // access functions
    virtual double GetRadius() const;

    //----------------------------------------------------------------------
    inline virtual bool IsStale() const;
    inline const VectorGeocentric& GetGeocentricPosition() const;
    inline const VectorCartesian&  GetECFPosition() const;

    //----------------------------------------------------------------------
    // operators
    bool operator==( const SpatialGeocentric& spatial ) const;
    bool operator!=( const SpatialGeocentric& spatial ) const;

    //----------------------------------------------------------------------
    // misc functions
    virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    virtual void UpdateState();

    //--------------------------------------------------------------------------
private:
    CoorTransEciEcf  m_coorTransEciEcf;

    VectorGeocentric m_vecGeocentricPosition;
    VectorCartesian  m_vecEcfPosition;
    VectorCartesian  m_vecEcfVelocity;
    VectorCartesian  m_vecEcfAcceleration;

    void Initialize();
};

//==============================================================================
inline bool SpatialGeocentric::IsStale() const
{
    return SpatialTimeDep::IsStale() || m_coorTransEciEcf.IsStale();
}

inline const VectorGeocentric&
SpatialGeocentric::GetGeocentricPosition() const
{
    return m_vecGeocentricPosition;
}

inline const VectorCartesian&
SpatialGeocentric::GetECFPosition() const
{
    return m_vecEcfPosition;
}

//------------------------------------------------------------------------------
// misc functions
inline const char* SpatialGeocentric::GetVersion() const
{
    return SPATIALGEOCENTRIC_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // SPATIALGEOCENTRIC_H
