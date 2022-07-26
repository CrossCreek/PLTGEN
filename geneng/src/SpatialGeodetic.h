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
 * SpatialGeodetic.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialGeodetic.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialGeodetic

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  02/26/1999  JAM  Initial Release
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES : Geodetic stationary object.



//****************************************************************************//

//==============================================================================
#ifndef SPATIALGEODETIC_H
#define SPATIALGEODETIC_H	"SpatialGeodetic Version 2.0"

//------------------------------------------------------------------------------
// required headers
#include "SpatialGeocentric.h"
#include "VectorGeodetic.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class SpatialGeodetic : public SpatialGeocentric
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors



    explicit SpatialGeodetic( HandleConst< TimeJ2000 > hdlTime );

    SpatialGeodetic( HandleConst< TimeJ2000 > hdlTime,
                     const VectorGeodetic& vecLocation );

    SpatialGeodetic( HandleConst< TimeJ2000 > hdlTime,
                     Handle< CoorTransBaseEci > hdlCoorTrans );

    SpatialGeodetic( HandleConst< TimeJ2000 > hdlTime,
                     Handle< CoorTransBaseEci > hdlCoorTrans,
                     const VectorGeodetic& vecLocation );

    virtual ~SpatialGeodetic();

    //----------------------------------------------------------------------
    // run control interface and reporting functions
    virtual std::istream& Initialize( std::istream& is );

    //----------------------------------------------------------------------
    inline const VectorGeodetic& GetGeodeticPosition() const;
    //----------------------------------------------------------------------
    // operators
    bool operator==( const SpatialGeodetic& spatial ) const;
    bool operator!=( const SpatialGeodetic& spatial ) const;

    //----------------------------------------------------------------------
    // misc functions
    virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
    VectorGeodetic m_vecGeodeticPosition;
};

//==============================================================================
inline const VectorGeodetic&
SpatialGeodetic::GetGeodeticPosition() const
{
    return m_vecGeodeticPosition;
}

//------------------------------------------------------------------------------
// misc functions
inline const char* SpatialGeodetic::GetVersion() const
{
    return SPATIALGEODETIC_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // SPATIALGEODETIC_H
