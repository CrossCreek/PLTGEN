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
 * SpatialConstantEci.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialConstantEci.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatTools

//  SUBSYSTEM : GenEngr

//  MODULE(s) : SpatialConstantEci

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.00-001  3/18/2001  JAM initial release
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :  The default reference system is ECI, use the alternate
//           constructor with the handle to a CoorTrans to change
//           the reference system

//  Contents derived from SE and SST Ver 1.0.



//****************************************************************************//

//==============================================================================
#include "SpatialConstantEci.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
SpatialConstantEci
::SpatialConstantEci()
    : m_vecEciPosition( 0.0, 0.0, 0.0 ),
      m_vecEciVelocity( 0.0, 0.0, 0.0 ),
      m_vecEciAcceleration( 0.0, 0.0, 0.0 ),
      m_radius( m_vecPosition.Magnitude() )
{
    UpdateState();
}

SpatialConstantEci
::SpatialConstantEci( Handle< CoorTransBaseEci > hdlCoorTrans )
    : Spatial( hdlCoorTrans ),
      m_vecEciPosition( 0.0, 0.0, 0.0 ),
      m_vecEciVelocity( 0.0, 0.0, 0.0 ),
      m_vecEciAcceleration( 0.0, 0.0, 0.0 ),
      m_radius( m_vecPosition.Magnitude() )
{
    UpdateState();
}

SpatialConstantEci
::SpatialConstantEci( const VectorCartesian& vecPosition,
                      const VectorCartesian& vecVelocity )
    : m_vecEciPosition( vecPosition ),
      m_vecEciVelocity( vecVelocity ),
      m_vecEciAcceleration( 0.0, 0.0, 0.0 ),
      m_radius( m_vecPosition.Magnitude() )
{
    UpdateState();
}

SpatialConstantEci
::SpatialConstantEci( const VectorCartesian& vecPosition,
                      const VectorCartesian& vecVelocity,
                      const VectorCartesian& vecAcceleration )
    : m_vecEciPosition( vecPosition ),
      m_vecEciVelocity( vecVelocity ),
      m_vecEciAcceleration( vecAcceleration ),
      m_radius( m_vecPosition.Magnitude() )
{
    UpdateState();
}

SpatialConstantEci
::SpatialConstantEci( Handle< CoorTransBaseEci > hdlCoorTrans,
                      const VectorCartesian& vecPosition,
                      const VectorCartesian& vecVelocity )
    : Spatial( hdlCoorTrans ),
      m_vecEciPosition( vecPosition ),
      m_vecEciVelocity( vecVelocity ),
      m_vecEciAcceleration( 0.0, 0.0, 0.0 ),
      m_radius( m_vecPosition.Magnitude() )
{
    UpdateState();
}

SpatialConstantEci
::SpatialConstantEci( Handle< CoorTransBaseEci > hdlCoorTrans,
                      const VectorCartesian& vecPosition,
                      const VectorCartesian& vecVelocity,
                      const VectorCartesian& vecAcceleration )
    : Spatial( hdlCoorTrans ),
      m_vecEciPosition( vecPosition ),
      m_vecEciVelocity( vecVelocity ),
      m_vecEciAcceleration( vecAcceleration ),
      m_radius( m_vecPosition.Magnitude() )
{
    hdlCoorTrans->Enable( ACCELERATION );
    UpdateState();
}

SpatialConstantEci::~SpatialConstantEci() { ; }

//------------------------------------------------------------------------------
std::istream& SpatialConstantEci::Initialize( std::istream& is )
{
    is >> m_vecEciPosition;
    UpdateState();
    return is;
}

//------------------------------------------------------------------------------
double SpatialConstantEci::GetRadius() const
{
    return m_radius;
}

//------------------------------------------------------------------------------
// operators
bool SpatialConstantEci::operator==( const SpatialConstantEci& spatial ) const
{
    return Spatial::operator==( spatial )
           && m_vecEciPosition     == spatial.m_vecEciPosition
           && m_vecEciVelocity     == spatial.m_vecEciVelocity
           && m_vecEciAcceleration == spatial.m_vecEciAcceleration;
}

bool SpatialConstantEci::operator!=( const SpatialConstantEci& spatial ) const
{
    return !operator==( spatial );
}

//------------------------------------------------------------------------------

//CPPCHECK_RELATED
//Removed Initialize() since it wasn't called in code base
/*
void SpatialConstantEci::Initialize()
{
    if ( m_hdlCoorTrans->GetSourceSystem() == CoorSystem::ECI ) {
        m_vecPosition     = m_vecEciPosition;
        m_vecVelocity     = m_vecEciVelocity;
        m_vecAcceleration = m_vecEciAcceleration;
    } else {
        Update();
    }

    m_radius = m_vecPosition.Magnitude();
}
*/

void SpatialConstantEci::UpdateState()
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

// ============================================================================
}
// A3 namespace end
// ============================================================================

