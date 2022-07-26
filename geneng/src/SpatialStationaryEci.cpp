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
 * SpatialStationaryEci.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialStationaryEci.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatTools

//  SUBSYSTEM : GenEngr

//  MODULE(s) : SpatialStationaryEci

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.00-001  10/27/1997  JAM initial release
//              V1.02-003  08/31/1999  JAM s/w moved to the new general
//                                         engineering library
//                                         added auto-update feature
//              V1.02-032  11/16/1999  JAM added const version of GetPosition,
//                                         GetVelocity
//                                         added new Update function to force
//                                         update position & velocity vectors
//                                         without calling the Get functions
//                                         added SetUpdateFrequency and
//                                         GetUpdateFrequecy functions to
//                                         update the vectors independent of
//                                         the clock frequency
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V1.3-016  01/26/2000  JAM  added call to Spatial in update
//                                         function
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :  The default reference system is ECI, use the alternate
//           constructor with the handle to a CoorTrans to change
//           the reference system

//  Contents derived from SE and SST Ver 1.0.



//****************************************************************************//

//==============================================================================
#include "SpatialStationaryEci.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
SpatialStationaryEci
::SpatialStationaryEci()
    : m_vecEciPosition( 0.0, 0.0, 0.0 ),
      m_vecEciVelocity( 0.0, 0.0, 0.0 ),
      m_vecEciAcceleration( 0.0, 0.0, 0.0 ),
      m_radius( m_vecPosition.Magnitude() )
{
    UpdateState();
}

SpatialStationaryEci
::SpatialStationaryEci( Handle< CoorTransBaseEci > hdlCoorTrans )
    : m_vecEciPosition( 0.0, 0.0, 0.0 ),
      m_vecEciVelocity( 0.0, 0.0, 0.0 ),
      m_vecEciAcceleration( 0.0, 0.0, 0.0 ),
      m_radius( m_vecPosition.Magnitude() )
{
    UpdateState();
}

SpatialStationaryEci
::SpatialStationaryEci( const VectorCartesian& vecPosition )
    : m_vecEciPosition( vecPosition ),
      m_vecEciVelocity( 0.0, 0.0, 0.0 ),
      m_vecEciAcceleration( 0.0, 0.0, 0.0 ),
      m_radius( m_vecPosition.Magnitude() )
{
    UpdateState();
}

SpatialStationaryEci
::SpatialStationaryEci( Handle< CoorTransBaseEci > hdlCoorTrans,
                        const VectorCartesian& vecPosition )
    : m_vecEciPosition( vecPosition ),
      m_vecEciVelocity( 0.0, 0.0, 0.0 ),
      m_vecEciAcceleration( 0.0, 0.0, 0.0 ),
      m_radius( m_vecPosition.Magnitude() )
{
    UpdateState();
}

SpatialStationaryEci::~SpatialStationaryEci() { ; }

//------------------------------------------------------------------------------
std::istream& SpatialStationaryEci::Initialize( std::istream& is )
{
    is >> m_vecEciPosition;
    UpdateState();
    return is;
}

//------------------------------------------------------------------------------
double SpatialStationaryEci::GetRadius() const
{
    return m_radius;
}

//------------------------------------------------------------------------------
// operators
bool SpatialStationaryEci::operator==( const SpatialStationaryEci& spatial ) const
{
    return Spatial::operator==( spatial )
           && m_vecEciPosition == spatial.m_vecEciPosition
           && m_vecEciVelocity == spatial.m_vecEciVelocity
           && m_vecPosition    == spatial.m_vecPosition
           && m_vecVelocity    == spatial.m_vecVelocity;
}

bool SpatialStationaryEci::operator!=( const SpatialStationaryEci& spatial ) const
{
    return !operator==( spatial );
}

//------------------------------------------------------------------------------

//CPPCHECK_RELATED
//Removed Initialize() since it wasn't called in code base
/*
void SpatialStationaryEci::Initialize()
{
    if ( m_hdlCoorTrans->GetSourceSystem() == CoorSystem::ECI ) {
        m_vecPosition = m_vecEciPosition;
        m_vecVelocity = m_vecEciVelocity;
    } else {
        Update();
    }

    m_radius = m_vecPosition.Magnitude();
}
*/

void SpatialStationaryEci::UpdateState()
{
    Spatial::UpdateState();
    // the if statement is not neccessary
    // if the coordination system is ECI,
    // this function will only be call once by the constructor
    // update would not call this function since the ECI system
    // is never stale
    // if the coordinate system is not ECI, the the coordinate
    // transformation must be performed
    // if ( m_hdlCoorTrans->GetSourceSystem() != CoorSystem::ECI )
    m_hdlCoorTrans->ToSource( m_vecEciPosition, m_vecEciVelocity,
                              m_vecPosition,    m_vecVelocity );
}

// ============================================================================
}
// A3 namespace end
// ============================================================================

