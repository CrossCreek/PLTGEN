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
 * SpatialTimeDep.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Base class for time dependant spatial classes
// Refer to SpatialTimeDep.h for more info.
//==============================================================================
#include "SpatialTimeDep.h"
#include "VectorCartesian.h"
#include "CoorTransEciEci.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
SpatialTimeDep::SpatialTimeDep( HandleConst< TimeJ2000 > hdlTime )
    : m_hdlTime( hdlTime ),
      // m_lastUpdate( TimeJ2000() ), default constructor sufficient
      m_dtFD( 1.0 )
{ ; }

SpatialTimeDep::SpatialTimeDep( HandleConst< TimeJ2000 > hdlTime,
                                Handle< CoorTransBaseEci > hdlCoorTrans )
    : Spatial( hdlCoorTrans ),
      m_hdlTime( hdlTime ),
      m_dtFD( 1.0 )
{ ; }

SpatialTimeDep::~SpatialTimeDep()
{ ; }

// -----------------------------------------------------------------------------
// operators
bool SpatialTimeDep::operator==( const SpatialTimeDep& spatial ) const
{
    return Spatial::operator==( static_cast< const Spatial& >( spatial ) )
           && *m_hdlTime       == *(spatial.m_hdlTime)
           && m_lastUpdate     == spatial.m_lastUpdate;
}

bool SpatialTimeDep::operator!=( const SpatialTimeDep& spatial ) const
{
    return !operator==( spatial );
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
