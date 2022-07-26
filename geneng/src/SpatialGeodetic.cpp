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
 * SpatialGeodetic.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialGeodetic.cpp

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
#include "SpatialGeodetic.h"
#include <sstream>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
SpatialGeodetic
::SpatialGeodetic( HandleConst< TimeJ2000 > hdlTime )
    : SpatialGeocentric( hdlTime ),
      m_vecGeodeticPosition( 0.0, 0.0, 0.0 )
{
    ;
}

SpatialGeodetic
::SpatialGeodetic( HandleConst< TimeJ2000 > hdlTime,
                   Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialGeocentric( hdlTime, hdlCoorTrans ),
      m_vecGeodeticPosition( 0.0, 0.0, 0.0 )
{
    ;
}

SpatialGeodetic
::SpatialGeodetic( HandleConst< TimeJ2000 > hdlTime,
                   const VectorGeodetic& vecPosition )
    : SpatialGeocentric( hdlTime, VectorGeocentric( vecPosition ) ),
      m_vecGeodeticPosition( vecPosition )
{
    ;
}

SpatialGeodetic
::SpatialGeodetic( HandleConst< TimeJ2000 > hdlTime,
                   Handle< CoorTransBaseEci > hdlCoorTrans,
                   const VectorGeodetic& vecPosition )
    : SpatialGeocentric( hdlTime, hdlCoorTrans, VectorGeocentric( vecPosition ) ),
      m_vecGeodeticPosition( vecPosition )
{
    ;
}

SpatialGeodetic::~SpatialGeodetic() { ; }

//------------------------------------------------------------------------------
// operators
bool SpatialGeodetic::operator==( const SpatialGeodetic& spatial ) const
{
    return Spatial::operator==( spatial )
           && m_vecGeodeticPosition == spatial.m_vecGeodeticPosition;
}

bool SpatialGeodetic::operator!=( const SpatialGeodetic& spatial ) const
{
    return !operator==( spatial );
}

//------------------------------------------------------------------------------
std::istream& SpatialGeodetic::Initialize( std::istream& is )
{
    is >> m_vecGeodeticPosition;
    std::stringstream stmTmp;
    stmTmp << ioGeocentricHeight
           << VectorGeocentric( m_vecGeodeticPosition );
    SpatialGeocentric::Initialize( stmTmp );
    return is;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================


