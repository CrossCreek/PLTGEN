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
 * SpatialEphFile.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialEphFile.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialEphFile (abstract class)

//  AUTHOR(s) : D. Bringenberg, M. Pochet
//              J. McCormick

//  UPDATES   : V1.0-001  03/19/1998  DCB
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

//              V4.4-001  2008-10-03  JAM  added ClearAndReserve fctn to
//                                         support initialization

//              V4.5-001  2009-07-01  JAM
//              Replaced class Lagrangian interpolation with call to library
//              function for improved maintainability and consistency
//              Deleted unused variables

//  NOTES : Read an Ephemeris File and Interpolate Ephemeris Values if needed.



//****************************************************************************//

//   Interpolate uses Nth Order Lagrangian interpolation
//   Interpolate returns an integer error code (all errors are fatal):

//   Error Code   Problem
//   ----------   --------------------------------------------
//    0           normal return, no error
//    1           # of entries < 2
//    2           time < first ephemeris time
//    3           time > last ephemeris time
//    4           first ephemeris time == last ephemeris time
//    5           nOrder < 2 or nOrder > m_MAXORDER or nOrder is odd
//    6           nOrder > # of entries
//    7           could not find nOrder unique times

//==============================================================================

#include "SpatialEphFile.h"
#include "interpolLagrangePoly.h"

// using math.h since Microsoft's cmath is not in the std namespace
#include <math.h>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
#define TOLERANCE 0.001    // Should this be tunable???

const int SpatialEphFile::m_MAXORDER =  20;

// ============================================================================
// constructors & destructors
// Position & Velocity in ECI
SpatialEphFile::SpatialEphFile( HandleConst< TimeJ2000 > hdlTime )
    : SpatialTimeDep( hdlTime ),
      bDoHintInit(true),
      m_numItems( 0 ),
      m_index( 0 ),
      m_offset( -1 ),  // continue reading file here (need for paging)???
      m_order( 6 )
{ ; }

// Coordinate Transformation to be applied to position & velocity
SpatialEphFile::SpatialEphFile( HandleConst< TimeJ2000 > hdlTime,
                                Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialTimeDep( hdlTime, hdlCoorTrans ),
      bDoHintInit(true),
      m_numItems( 0 ),
      m_index( 0 ),
      m_offset( -1 ),  // continue reading file here (need for paging)???
      m_order( 6 )
{ ; }

// Position & Velocity in ECI
SpatialEphFile::SpatialEphFile( HandleConst< TimeJ2000 > hdlTime,
                                const std::string& strFileNm )
    : SpatialTimeDep( hdlTime ),
      bDoHintInit(true),
      m_strFileName( strFileNm ),
      m_numItems( 0 ),
      m_index( 0 ),
      m_offset( -1 ),  // continue reading file here (need for paging)???
      m_order( 6 )
{ ; }

// Coordinate Transformation to be applied to position & velocity
SpatialEphFile::SpatialEphFile( HandleConst< TimeJ2000 > hdlTime,
                                Handle< CoorTransBaseEci > hdlCoorTrans,
                                const std::string& strFileNm )
    : SpatialTimeDep( hdlTime, hdlCoorTrans ),
      bDoHintInit(true),
      m_strFileName( strFileNm ),
      m_numItems( 0 ),
      m_index( 0 ),
      m_offset( -1 ),  // continue reading file here (need for paging)???
      m_order( 6 )
{ ; }

SpatialEphFile::~SpatialEphFile()
{ ; }

//------------------------------------------------------------------------------
// run control interface and reporting functions
std::istream& SpatialEphFile::Initialize( std::istream& is )
{
    try {
        is >> m_strFileName;
        m_offset = InitExtEphFile();
        m_iTimeHint = m_vTime.begin();
        bDoHintInit = false;
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFile::Initialize( istream& is )"
                  << std::endl;
        throw;
    }

    return is;
}

//------------------------------------------------------------------------------
// update fuction
void SpatialEphFile::UpdateState()
{
    try {
        if ( m_bIsAccelerationEnabled ) {
            ThrowNotImplemented( "SpatialEphFile::UpdateState(): ACCELERATION" );
        }

        SpatialTimeDep::UpdateState();
        double time = GetTime();
        VectorCartesian velocity0;

        if ( m_bIsAccelerationEnabled ) {
            // jam note: replace following with call t numerical differentiation
            // function in GenMath to improve acceleration estimate
            TimeJ2000 time0 = time - GetFDDT();

            if ( time0 < m_vTime[0] ) {
                time0 = time + GetFDDT();
            }

            Interpolate( time0 );

            if ( m_hdlCoorTrans->GetSourceSystem() == CoorSystem::ECI ) {
                velocity0 = m_vecEciVelocity;
            } else {
                m_hdlCoorTrans->ToSource( m_vecEciPosition, m_vecEciVelocity,
                                          m_vecPosition,    velocity0 );
            }
        }

        Interpolate( time );

        if ( m_hdlCoorTrans->GetSourceSystem() == CoorSystem::ECI ) {
            m_vecPosition = m_vecEciPosition;
            m_vecVelocity = m_vecEciVelocity;
        } else {
            m_hdlCoorTrans->ToSource( m_vecEciPosition, m_vecEciVelocity,
                                      m_vecPosition,    m_vecVelocity );
        }

        if ( m_bIsAccelerationEnabled )
            // jam note: replace following with call t numerical differentiation
            // function in GenMath to improve acceleration estimate, see above
        {
            m_vecAcceleration = ( m_vecVelocity - velocity0 ) / GetFDDT();
        }
    } catch ( ... ) {
        std::cerr << "Exception caught by SpatialEphFile::Update()\n"
                  << std::endl;
        throw;
    }
}

//------------------------------------------------------------------------------
void SpatialEphFile::Interpolate( double time )  // time in sec
{
    try {
        if (time < GetFirstTime()) {
            throw( "time < first ephemeris time" );
        }

        if (time > GetLastTime()) {
            throw( "time > last ephemeris time" );
        }

        if (bDoHintInit) {
            m_iTimeHint = m_vTime.begin();
            bDoHintInit = false;
        }

        std::vector<TimeJ2000>::const_iterator
        itrT(m_vTime.begin()),
             etrT(m_vTime.end());
        std::vector<VectorCartesian>::const_iterator
        itrP(m_vVecPosition.begin()),
             itrV(m_vVecVelocity.begin());
        m_iTimeHint =
            interpolLagrangePoly(itrT, etrT, m_iTimeHint, itrP,
                                 m_order, time, m_vecEciPosition);
        interpolLagrangePoly(itrT, etrT, m_iTimeHint, itrV,
                             m_order, time, m_vecEciVelocity);
    } catch ( const char* msg ) {
        std::cerr << "Warning generated by SpatialEphFile::Interpolate()\n"
                  << msg << std::endl << std::endl;
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFile::Interpolate()\n"
                  << std::endl;
        throw;
    }
}

//------------------------------------------------------------------------------
void SpatialEphFile::ClearAndReserve()
{
    static const int szReserve = 100;
    m_vTime.clear();
    m_vVecPosition.clear();
    m_vVecVelocity.clear();
    m_vTime.reserve( szReserve );
    m_vVecPosition.reserve( szReserve );
    m_vVecVelocity.reserve( szReserve );
}

//------------------------------------------------------------------------------
void SpatialEphFile::ThrowBadInterpolate( int error )
{
    switch ( error ) {
        case 0:
            break;

        case 1:
            throw std::runtime_error("SpatialEphFile::Interpolate empty array");
            break;

        case 2:
            throw std::runtime_error("SpatialEphFile::Interpolate time too early");
            break;

        case 3:
            throw std::runtime_error("SpatialEphFile::Interpolate time too late");
            break;

        case 4:
            throw std::runtime_error("SpatialEphFile::Interpolate last time is not after first time");
            break;

        case 5:
            throw std::runtime_error("SpatialEphFile::Interpolate invalid order of interpolation");
            break;

        case 6:
            throw std::runtime_error("SpatialEphFile::Interpolate order is greater than the number of records");
            break;

        case 7:
            throw std::runtime_error("SpatialEphFile::Interpolate Could not find enough unique times");
            break;

        default:
            throw std::runtime_error("SpatialEphFile::Interpolate undefined error");
            break;
    }
}

//------------------------------------------------------------------------------
// operators
bool SpatialEphFile::operator==( const SpatialEphFile& spatial ) const
{
    return SpatialTimeDep::operator==( spatial )
           && m_vTime          == spatial.m_vTime
           && m_vVecPosition   == spatial.m_vVecPosition
           && m_vVecVelocity   == spatial.m_vVecVelocity
           && m_strFileName    == spatial.m_strFileName
           && m_numItems       == spatial.m_numItems
           && m_index          == spatial.m_index
           && m_offset         == spatial.m_offset
           && m_vecEciPosition == spatial.m_vecEciPosition
           && m_vecEciVelocity == spatial.m_vecEciVelocity
           && m_vecPosition    == spatial.m_vecPosition
           && m_vecVelocity    == spatial.m_vecVelocity
           && m_order          == spatial.m_order;
}

bool SpatialEphFile::operator!=( const SpatialEphFile& spatial ) const
{
    return !operator==( spatial );
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
