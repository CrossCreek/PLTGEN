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
 * Spatial.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : Spatial.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatTools

//  SUBSYSTEM : GenEngr

//  MODULE(s) : Spatial (abstract class)

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.00-001  10/27/1997  JAM  initial release
//              V1.02-003  08/31/1999  JAM  s/w moved to the new general
//                                          engineering library
//                                          added auto-update feature
//              V1.02-032  11/16/1999  JAM  added const version of GetPosition,
//                                          GetVelocity
//                                          added new Update function to force
//                                          update position & velocity vectors
//                                          without calling the Get functions
//                                          added SetUpdateFrequency and
//                                          GetUpdateFrequecy functions to
//                                          update the vectors independent of
//                                          the clock frequency
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings

//  NOTES :  The default reference system is ECI, use the alternate
//           constructor with the handle to a CoorTrans to change
//           the reference system

//  Contents derived from SE and SST Ver 1.0.



//****************************************************************************//

//==============================================================================
#include "Spatial.h"
#include "VectorCartesian.h"
#include "CoorTransEciEci.h"

#include <sstream>
#include <typeinfo>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// class statics
Handle< CoorTransEciEci > Spatial::s_hdlCoorTransEciEci;

// -----------------------------------------------------------------------------
// constructors & destructors
Spatial::Spatial()
    : m_hdlCoorTrans( s_hdlCoorTransEciEci ),
      m_bIsVelocityEnabled( true ),
      m_bIsAccelerationEnabled( false ),
      m_bIsStale( false )
{
    if ( s_hdlCoorTransEciEci.IsNullObject() ) {
        Handle< CoorTransEciEci > hdlCoorTransEciEci( new CoorTransEciEci );
        s_hdlCoorTransEciEci = hdlCoorTransEciEci;
        m_hdlCoorTrans       = hdlCoorTransEciEci;
    }
}

Spatial::Spatial( Handle< CoorTransBaseEci > hdlCoorTrans )
    : m_hdlCoorTrans( hdlCoorTrans ),
      m_bIsVelocityEnabled( true ),
      m_bIsAccelerationEnabled( false ),
      m_bIsStale( hdlCoorTrans->IsStale() )
{
    m_hdlCoorTrans->RegisterCallBack( this );
}

Spatial::Spatial( const Spatial& spatial )
    : m_vecPosition( spatial.m_vecPosition ),
      m_vecVelocity( spatial.m_vecVelocity ),
      m_vecAcceleration( spatial.m_vecAcceleration ),
      m_hdlCoorTrans( spatial.m_hdlCoorTrans ),
      m_bIsVelocityEnabled( spatial.m_bIsVelocityEnabled ),
      m_bIsAccelerationEnabled( spatial.m_bIsAccelerationEnabled ),
      m_bIsStale( spatial.m_bIsStale )
{
    m_hdlCoorTrans->RegisterCallBack( this );
}

Spatial::~Spatial()
{ ; }

// -----------------------------------------------------------------------------
// time, position & velocity access functions
//------------------------------------------------------------------------------
const VectorCartesian& Spatial::GetPosition()
{
    Update();
    return m_vecPosition;
}

const VectorCartesian& Spatial::GetVelocity()
{
    Update();
    return m_vecVelocity;
}

const VectorCartesian& Spatial::GetAcceleration()
{
    if ( !m_bIsAccelerationEnabled ) {
        ThrowNotEnabled( "GetAcceleration()" );
    }

    Update();
    return m_vecAcceleration;
}

const VectorCartesian& Spatial::GetPosition() const
{
    if ( IsStale() ) {
        ThrowStaleVector( "GetPosition() const" );
    }

    return m_vecPosition;
}

const VectorCartesian& Spatial::GetVelocity() const
{
    if ( IsStale() ) {
        ThrowStaleVector( "GetVelocity() const" );
    }

    return m_vecVelocity;
}

const VectorCartesian& Spatial::GetAcceleration() const
{
    if ( !m_bIsAccelerationEnabled ) {
        ThrowNotEnabled( "GetAcceleration() const" );
    }

    if ( IsStale() ) {
        ThrowStaleVector( "GetAcceleration() const" );
    }

    return m_vecAcceleration;
}

double Spatial::GetRadius()
{
    return GetPosition().Magnitude();
}

double Spatial::GetRadius() const
{
    return GetPosition().Magnitude();
}

// -----------------------------------------------------------------------------
void Spatial::ThrowStaleVector( const std::string& strFunctionName ) const
{
    std::stringstream stmMsg;
    stmMsg << "Exception thrown from\n" << typeid( *this ).name() << std::endl
           << "by function\n"
           << strFunctionName << std::endl
           << "The internal state vector is stale.\n"
           << "The const version of this function was called, non-const\n"
           << "version must be used."
           << std::endl << std::ends;
    std::runtime_error re( stmMsg.str() );
    throw re;
}

void Spatial::ThrowNotEnabled( const std::string& strFunctionName ) const
{
    std::stringstream stmMsg;
    stmMsg << "Exception thrown from\n" << typeid( *this ).name() << std::endl
           << "by function\n"
           << strFunctionName << std::endl
           << "Unexpected call to a disabled function.\n"
           << "This function was disabled to reduce runtime.\n"
           << "To use this function, call the SetEnabled( ... ) "
           << "member function.\n"
           << "To disable again, call SetDisabled( ... ) member function.\n"
           << std::ends;
    std::runtime_error re( stmMsg.str() );
    throw re;
}

void Spatial::ThrowNotImplemented( const std::string& strFunctionName ) const
{
    std::stringstream stmMsg;
    stmMsg << "Exception thrown from\n" << typeid( *this ).name() << std::endl
           << "by function\n"
           << strFunctionName << std::endl
           << "This function is not implemented.\n"
           << std::ends;
    std::runtime_error re( stmMsg.str() );
    throw re;
}

// -----------------------------------------------------------------------------
// operators
bool Spatial::operator==( const Spatial& spatial ) const
{
    return m_hdlCoorTrans.Ref()     == spatial.m_hdlCoorTrans.Ref()
           && m_vecPosition            == spatial.m_vecPosition
           && m_vecVelocity            == spatial.m_vecVelocity
           && m_vecAcceleration        == spatial.m_vecAcceleration
           && m_bIsVelocityEnabled     == spatial.m_bIsVelocityEnabled
           && m_bIsAccelerationEnabled == spatial.m_bIsAccelerationEnabled
           && m_bIsStale               == spatial.m_bIsStale;
}

bool Spatial::operator!=( const Spatial& spatial ) const
{
    return !operator==( spatial );
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
