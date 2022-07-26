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
 * SpatialGeocentricGrid.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialGeocentricGrid.h

//  NAMESPACE : A3

//  SYSTEM    :

//  SUBSYSTEM :

//  MODULE(s) : SpatialGeocentricGrid

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V01.00  2002-02-20  JAM  Initial Release
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.5      2009-07-01  JAM  changed return time of GetSize from
//                                         int to size_type to elimiate conversion
//                                         warning

//  NOTES :



//****************************************************************************//

//==============================================================================
#ifndef SPATIALGEOCENTRICGRID_H
#define SPATIALGEOCENTRICGRID_H	"SpatialGeocentricGrid V2.0"

//------------------------------------------------------------------------------
// required headers
#include "SpatialTimeDep.h"
#include "VectorGeocentric.h"
#include "VectorCartesian.h"
#include "CoorTransEciEcf.h"
#include "Range.h"
#include "MatrixBase.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class SpatialGeocentricGrid : public SpatialTimeDep
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    typedef MatrixBase<VectorCartesian>::const_iterator const_iterator;
    typedef MatrixBase<VectorCartesian>::size_type size_type;

    //----------------------------------------------------------------------
    // constructors & destructors



    explicit SpatialGeocentricGrid( HandleConst< TimeJ2000 > hdlTime );

    SpatialGeocentricGrid( HandleConst< TimeJ2000 > hdlTime,
                           Angle latMin, Angle latMax, Angle latInc,
                           Angle lonMin, Angle lonMax, Angle lonInc );

    SpatialGeocentricGrid( HandleConst< TimeJ2000 > hdlTime,
                           Handle< CoorTransBaseEci > hdlCoorTrans );

    SpatialGeocentricGrid( HandleConst< TimeJ2000 > hdlTime,
                           Handle< CoorTransBaseEci > hdlCoorTrans,
                           Angle latMin, Angle latMax, Angle latInc,
                           Angle lonMin, Angle lonMax, Angle lonInc );

    virtual ~SpatialGeocentricGrid();

    //----------------------------------------------------------------------
    // iterators
    const_iterator begin() const
    {
        return m_mtxPts.begin();
    }
    const_iterator end() const
    {
        return m_mtxPts.end();
    }

    //----------------------------------------------------------------------
    size_type GetSize() const
    {
        return m_mtxPts.GetSize();
    }

    //----------------------------------------------------------------------
    // run control interface and reporting functions
    virtual std::istream& Initialize( std::istream& is );

    //----------------------------------------------------------------------
    // access functions
    const Range< Angle >& GetLatitudeRange() const;
    const Range< Angle >& GetLongitudeRange() const;

    virtual double GetRadius() const;

    //----------------------------------------------------------------------
    inline virtual bool IsStale() const;
    inline const VectorGeocentric& GetGeocentricPosition() const;
    inline const VectorCartesian&  GetECFPosition() const;

    //----------------------------------------------------------------------
    // operators
    bool operator==( const SpatialGeocentricGrid& spatial ) const;
    bool operator!=( const SpatialGeocentricGrid& spatial ) const;

    //----------------------------------------------------------------------
    // misc functions
    virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    virtual void UpdateState();

    //--------------------------------------------------------------------------
private:
    CoorTransEciEcf  m_coorTransEciEcf;

    Range< Angle > m_rngLat;
    Range< Angle > m_rngLon;

    MatrixBase< VectorCartesian > m_mtxPts;

    VectorGeocentric m_vecGeocentricPosition;
    VectorCartesian  m_vecEcfPosition;
    VectorCartesian  m_vecEcfVelocity;
    VectorCartesian  m_vecEcfAcceleration;

    void Initialize();
};

//==============================================================================
inline bool SpatialGeocentricGrid::IsStale() const
{
    return SpatialTimeDep::IsStale() || m_coorTransEciEcf.IsStale();
}

inline const VectorGeocentric&
SpatialGeocentricGrid::GetGeocentricPosition() const
{
    return m_vecGeocentricPosition;
}

inline const VectorCartesian&
SpatialGeocentricGrid::GetECFPosition() const
{
    return m_vecEcfPosition;
}

inline const Range< Angle >&
SpatialGeocentricGrid::GetLatitudeRange() const
{
    return m_rngLat;
}

inline const Range< Angle >&
SpatialGeocentricGrid::GetLongitudeRange() const
{
    return m_rngLon;
}

//------------------------------------------------------------------------------
// misc functions
inline const char* SpatialGeocentricGrid::GetVersion() const
{
    return SPATIALGEOCENTRICGRID_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // SPATIALGEOCENTRICGRID_H
