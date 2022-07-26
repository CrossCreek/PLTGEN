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
 * SpatialEphFileSST.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialEphFileSST.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialEphFileSST

//  AUTHOR(s) : D. Bringenberg

//  UPDATES   : V1.0-001  03/19/1998  DCB
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES : Read an Ephemeris File in Satsim Binary Format.



//****************************************************************************//

//==============================================================================

#include "SpatialEphFileSST.h"

#include <fstream>
// #include <cstdlib>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
// Position & Velocity in ECI
SpatialEphFileSST
::SpatialEphFileSST( HandleConst< TimeJ2000 > hdlTime )
    : SpatialEphFile( hdlTime ) { ; }

// Coordinate Transformation to be applied to position & velocity
SpatialEphFileSST
::SpatialEphFileSST( HandleConst< TimeJ2000 > hdlTime,
                     Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialEphFile( hdlTime, hdlCoorTrans ) { ; }

SpatialEphFileSST
::SpatialEphFileSST( HandleConst< TimeJ2000 > hdlTime,
                     const std::string& strFileNm )
    : SpatialEphFile( hdlTime, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFileSST::SpatialEphFileSST( "
                  << "HandleConst< TimeJ2000 >& hdlTime,\n"
                  << "                                            "
                  << "const string& strFileNm )\n"
                  << std::endl;
        throw;
    }
}

// Coordinate Transformation to be applied to position & velocity
SpatialEphFileSST
::SpatialEphFileSST( HandleConst< TimeJ2000 > hdlTime,
                     Handle< CoorTransBaseEci > hdlCoorTrans,
                     const std::string& strFileNm )
    : SpatialEphFile( hdlTime, hdlCoorTrans, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFileSST::SpatialEphFileSST( "
                  << "HandleConst< TimeJ2000 >& hdlTime,\n"
                  << "                                            "
                  << "Handle< CoorTransBaseEci > hdlCoorTrans,\n"
                  << "                                            "
                  << "const string& strFileNm )\n"
                  << std::endl;
        throw;
    }
}

SpatialEphFileSST::~SpatialEphFileSST()
{
    ;
}

//==============================================================================

//WARNING_FIX:
//1>src\SpatialEphFileSST.cpp(99): error C2556: 'long A3::SpatialEphFileSST::InitExtEphFile(void)' : overloaded function differs only by return type from '__int64 A3::SpatialEphFileSST::InitExtEphFile(void)'


long long int SpatialEphFileSST::InitExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFileSST.cpp(104): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
    long long int status = 0;

    try {
        m_numItems = 0;
        m_index = 0;
        m_offset = 0;  // continue reading file here (need for paging)???
        //WARNING_FIX:
        //1>src\SpatialEphFileSST.cpp(104): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
        status = ReadExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFileSST::InitExtEphFile()\n"
                  << std::endl;
        throw;
    }

    return status;
}

//WARNING_FIX:
//1>src\SpatialEphFileSST.cpp(128): error C2556: 'long A3::SpatialEphFileSST::ReadExtEphFile(void)' : overloaded function differs only by return type from '__int64 A3::SpatialEphFileSST::ReadExtEphFile(void)'


long long int SpatialEphFileSST::ReadExtEphFile()
{
    std::ifstream ephStream;
    std::string strBuf;
    VectorCartesian pos, vel;
    TimeJ2000 tm;

    try {
        ephStream.exceptions( std::ios_base::failbit | std::ios_base::badbit );
        ephStream.open( m_strFileName.c_str(), std::ios::in );
        ephStream.exceptions( std::ios_base::badbit );
        ephStream >> ioTimeYMDHMS;
        m_numItems = 0;
        ephStream >> std::ws;

        while ( std::getline( ephStream, strBuf ) ) {
            if ( '#' != *(strBuf.begin()) ) {
                ++m_numItems;
            }

            ephStream >> std::ws;
        }

        ephStream.clear();
        ephStream.seekg( 0, std::ios_base::beg );
        m_vTime.clear();
        m_vVecPosition.clear();
        m_vVecVelocity.clear();
        m_vTime.reserve( m_numItems );
        m_vVecPosition.reserve( m_numItems );
        m_vVecVelocity.reserve( m_numItems );
        ephStream.exceptions( std::ios_base::goodbit );

        while( m_numItems-- ) {
            ephStream >> std::ws;

            if ( '#' != ephStream.peek() ) {
                ephStream >> tm >> pos >> vel;
                m_vTime.push_back( tm );
                m_vVecPosition.push_back( pos );
                m_vVecVelocity.push_back( vel );
            }
        }

        m_numItems = m_vTime.size();
        ephStream.close();
    } catch ( ... ) {
        if ( ephStream ) {
            ephStream.close();
        }

        std::cerr << "Exception caught by\n"
                  << "SpatialEphFileSST::ReadExtEphFile()\n"
                  << "ephemeris file name = " << m_strFileName
                  << std::endl << std::endl;
        throw;
    }

    return 0;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
