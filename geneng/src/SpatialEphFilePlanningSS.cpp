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
 * SpatialEphFilePlanningSS.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialEphFilePlanningSS.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialEphFilePlanningSS

//  AUTHOR(s) : D. Bringenberg

//  UPDATES   : V1.0-001  03/19/1998  DCB
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: deleted strstream
//              Reason: unused

//  NOTES : Read an Ephemeris File in Planning ASCII Format.



//****************************************************************************//

//==============================================================================
#include "SpatialEphFilePlanningSS.h"

#include <fstream>
#include <cstdlib>

//------------------------------------------------------------------------------

namespace
{
const unsigned MAX_LINE_LENGTH = 1024;

struct Vector {
    double x;
    double y;
    double z;
};

struct StateVector {
    char date[MAX_LINE_LENGTH];
    char time[MAX_LINE_LENGTH];

    double j2000;

    Vector posNmi;
    Vector velNmiPS;
};
}

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
// Position & Velocity in ECI
SpatialEphFilePlanningSS
::SpatialEphFilePlanningSS( HandleConst< TimeJ2000 > hdlTime )
    : SpatialEphFile( hdlTime ) { ; }

// Coordinate Transformation to be applied to position & velocity
SpatialEphFilePlanningSS
::SpatialEphFilePlanningSS( HandleConst< TimeJ2000 > hdlTime,
                            Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialEphFile( hdlTime, hdlCoorTrans ) { ; }

SpatialEphFilePlanningSS
::SpatialEphFilePlanningSS( HandleConst< TimeJ2000 > hdlTime,
                            const std::string& strFileNm )
    : SpatialEphFile( hdlTime, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFilePlanningSS::SpatialEphFilePlanningSS( "
                  << "HandleConst< TimeJ2000 >& hdlTime,\n"
                  << "                                                "
                  << "const string& strFileNm )\n"
                  << std::endl;
        throw;
    }
}

// Coordinate Transformation to be applied to position & velocity
SpatialEphFilePlanningSS
::SpatialEphFilePlanningSS( HandleConst< TimeJ2000 > hdlTime,
                            Handle< CoorTransBaseEci > hdlCoorTrans,
                            const std::string& strFileNm )
    : SpatialEphFile( hdlTime, hdlCoorTrans, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n" << std::endl
                  << "SpatialEphFilePlanningSS::SpatialEphFilePlanningSS( "
                  << "HandleConst< TimeJ2000 >& hdlTime,\n"
                  << "                                                "
                  << "Handle< CoorTransBaseEci > hdlCoorTrans,\n"
                  << "                                                "
                  << "const string& strFileNm )\n"
                  << std::endl;
        throw;
    }
}

SpatialEphFilePlanningSS::~SpatialEphFilePlanningSS()
{
    ;
}

//==============================================================================

//WARNING_FIX:
//1>src\SpatialEphFilePlanningSS.cpp(129): error C2556: 'long A3::SpatialEphFilePlanningSS::InitExtEphFile(void)' : overloaded function differs only by return type from '__int64 A3::SpatialEphFilePlanningSS::InitExtEphFile(void)'


long long int SpatialEphFilePlanningSS::InitExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFilePlanningSS.cpp(139): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
    long long int status = 0;

    try {
        m_numItems = 0;
        m_index = 0;
        m_offset = 0;  // continue reading file here (need for paging)???
        //WARNING_FIX:
        //1>src\SpatialEphFilePlanningSS.cpp(139): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
        status = ReadExtEphFile();
        time( &m_ssTimer );
    } catch ( ... ) {
        std::cerr << "Exception thrown by "
                  << "SpatialEphFilePlanningSS::InitExtEphFile()"
                  << std::endl << std::endl;
        throw;
    }

    return status;
}

//WARNING_FIX:
//1>src\SpatialEphFilePlanningSS.cpp(149): error C2371: 'A3::SpatialEphFilePlanningSS::ReadExtEphFile' : redefinition; different basic types


long long int SpatialEphFilePlanningSS::ReadExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFilePlanningSS.cpp(206): warning C4244: '=' : conversion from 'std::streamoff' to 'long', possible loss of data
    long long int offset = m_offset;
    std::ifstream ephStream;

    try {
        int          i;
        StateVector  eph;
        TimeJ2000    j2000;
        ephStream.exceptions( std::ios_base::badbit | std::ios_base::failbit );
        ephStream.open( m_strFileName.c_str() );

        if (m_numItems == 0 || m_offset <= 0) {
            char line[MAX_LINE_LENGTH];
            ephStream.getline(line, MAX_LINE_LENGTH);
            //WARNING_FIX:
            //1>src\SpatialEphFilePlanningSS.cpp(170): warning C4244: '=' : conversion from 'std::streamoff' to 'long', possible loss of data
            offset = ephStream.tellg();
            m_numItems = 100;
            //  IF MEMORY REQUIREMENTS ARE EXCESSIVE m_numItems MUST BE
            //  DECREASED TO A REASONABLE(?) NUMBER
            m_vTime.reserve( m_numItems );
            m_vVecPosition.reserve( m_numItems );
            m_vVecVelocity.reserve( m_numItems );
        }

        //  IF MEMORY REQUIREMENTS ARE EXCESSIVE m_numItems MUST BE
        //  DECREASED TO A REASONABLE(?) NUMBER
        //  INITIALIZE PAGING -
        //    SET POINTERS TO START AND END OF PAGE ?
        //    AFTER READ SET PAGE START AND END TIMES
        //    INITIALIZE FILE START RECORD NUMBER TO 0 AND END TO numItems - 1
        //    INITIALIZE INTERPOLATE START POINTER AND TIME TO FIRST
        //    RECORD VALUES
        ephStream.seekg(offset, std::ios::beg);
        i = 0;
        ephStream.exceptions( std::ios_base::badbit );

        while (! ephStream.eof()) {
            ephStream >> eph.date >> eph.time >> eph.j2000
                      >> eph.posNmi.x >> eph.posNmi.y >> eph.posNmi.z
                      >> eph.velNmiPS.x >> eph.velNmiPS.y >> eph.velNmiPS.z;
            m_vTime.push_back( eph.j2000 );
            m_vVecPosition.push_back(VectorCartesian(eph.posNmi.x,
                                     eph.posNmi.y,
                                     eph.posNmi.z));
            m_vVecVelocity.push_back(VectorCartesian(eph.velNmiPS.x,
                                     eph.velNmiPS.y,
                                     eph.velNmiPS.z));
            i++;
        }

        m_numItems = i;
        //WARNING_FIX:
        //1>src\SpatialEphFilePlanningSS.cpp(206): warning C4244: '=' : conversion from 'std::streamoff' to 'long', possible loss of data
        m_offset = ephStream.tellg();
        ephStream.close();
    } catch ( ... ) {
        if ( ephStream ) {
            ephStream.close();
        }

        std::cerr << "Exception thrown by "
                  << "SpatialEphFilePlanningSS::ReadExtEphFile()"
                  << std::endl << std::endl;
        throw;
    }

    //WARNING_FIX:
    //1>src\SpatialEphFilePlanningSS.cpp(233): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data
    return offset;
}

void SpatialEphFilePlanningSS::UpdateState()
{
    static const time_t hour = 3600;
    static const time_t freq = 4 * hour;
    time_t curTime = 0;
    time( &curTime );

    if ( (curTime - m_ssTimer) >= freq ) {
        m_ssTimer = curTime;
        m_vTime.clear();
        m_vVecPosition.clear();
        m_vVecVelocity.clear();
        InitExtEphFile();
    }

    SpatialEphFile::UpdateState();
}


// ============================================================================
}
// A3 namespace end
// ============================================================================
