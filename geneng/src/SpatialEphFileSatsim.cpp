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
 * SpatialEphFileSatsim.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialEphFileSatsim.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialEphFileSatsim

//  AUTHOR(s) : D. Bringenberg

//  UPDATES   : V1.0-001  03/19/1998  DCB
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it

//              V4.5-001  2009-07-01  JAM
//              added casting to eliminate 64-bit warnings

//  NOTES : Read an Ephemeris File in Satsim Binary Format.



//****************************************************************************//

//==============================================================================

#include "SpatialEphFileSatsim.h"

#include <fstream>
#include <cstdlib>
#include <sstream>

// ============================================================================
namespace
{
const int OBJECT_NAME_LEN = 41;

struct EphHeader {
    double start;
    double end;
    double delta;           // 0 - implies variable step
    int    numRecords;
    char   name[OBJECT_NAME_LEN];
};

struct Vector {
    double x;
    double y;
    double z;
};

struct StateVector {
    double time;
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
SpatialEphFileSatsim
::SpatialEphFileSatsim( HandleConst< TimeJ2000 > hdlTime )
    : SpatialEphFile( hdlTime ) { ; }

// Coordinate Transformation to be applied to position & velocity
SpatialEphFileSatsim
::SpatialEphFileSatsim( HandleConst< TimeJ2000 > hdlTime,
                        Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialEphFile( hdlTime, hdlCoorTrans ) { ; }

SpatialEphFileSatsim
::SpatialEphFileSatsim( HandleConst< TimeJ2000 > hdlTime,
                        const std::string& strFileNm )
    : SpatialEphFile( hdlTime, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFileSatsim::SpatialEphFileSatsim( "
                  << "HandleConst< TimeJ2000 >& hdlTime,\n"
                  << "                                            "
                  << "const string& strFileNm )\n"
                  << std::endl;
        throw;
    }
}

// Coordinate Transformation to be applied to position & velocity
SpatialEphFileSatsim
::SpatialEphFileSatsim( HandleConst< TimeJ2000 > hdlTime,
                        Handle< CoorTransBaseEci > hdlCoorTrans,
                        const std::string& strFileNm )
    : SpatialEphFile( hdlTime, hdlCoorTrans, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFileSatsim::SpatialEphFileSatsim( "
                  << "HandleConst< TimeJ2000 >& hdlTime,\n"
                  << "                                            "
                  << "Handle< CoorTransBaseEci > hdlCoorTrans,\n"
                  << "                                            "
                  << "const string& strFileNm )\n"
                  << std::endl;
        throw;
    }
}

SpatialEphFileSatsim::~SpatialEphFileSatsim()
{
    ;
}

//==============================================================================
//WARNING_FIX:
//1>src\SpatialEphFileSatsim.cpp(134): error C2556: 'long A3::SpatialEphFileSatsim::InitExtEphFile(void)' : overloaded function differs only by return type from '__int64 A3::SpatialEphFileSatsim::InitExtEphFile(void)'


long long int SpatialEphFileSatsim::InitExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFileSatsim.cpp(140): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
    long long int status = 0;

    try {
        m_numItems = 0;
        m_index = 0;
        m_offset = 0;  // continue reading file here (need for paging)???
        //WARNING_FIX:
        //1>src\SpatialEphFileSatsim.cpp(140): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
        status = ReadExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFileSatsim::InitExtEphFile()\n"
                  << std::endl;
        throw;
    }

    return status;
}

//WARNING_FIX:
//1>src\SpatialEphFileSatsim.cpp(231): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data


long long int SpatialEphFileSatsim::ReadExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFileSatsim.cpp(153): warning C4244: 'initializing' : conversion from '__int64' to 'long', possible loss of data
    long long int offset = m_offset;
    std::ifstream ephStream;
    StateVector *ephemeris = NULL;

    try {
        EphHeader    header;
        ephStream.exceptions( std::ios_base::failbit | std::ios_base::badbit );
        ephStream.open( m_strFileName.c_str(), std::ios::in | std::ios::binary );

        //WAS:if (m_numItems <= 0 || m_offset <= 0) {
        if (m_numItems == 0 || m_offset <= 0) {
            ephStream.read((char *)&header, sizeof(EphHeader));
            offset = ephStream.tellg();
            m_numItems = header.numRecords;
            //  IF MEMORY REQUIREMENTS ARE EXCESSIVE m_numItems MUST BE
            //  DECREASED TO A REASONABLE(?) NUMBER
            m_vTime.reserve( m_numItems );
            m_vVecPosition.reserve( m_numItems );
            m_vVecVelocity.reserve( m_numItems );
        }

        //  INITIALIZE PAGING -
        //    SET POINTERS TO START AND END OF PAGE ?
        //    AFTER READ SET PAGE START AND END TIMES
        //    INITIALIZE FILE START RECORD NUMBER TO 0 AND END TO numItems - 1
        //    INITIALIZE INTERPOLATE START POINTER AND TIME TO FIRST
        //    RECORD VALUES
        ephemeris = new StateVector[m_numItems];

        if (ephemeris == NULL) {
            std::ostringstream stmMsg;
            stmMsg << "Exception thrown by\n"
                   << "SpatialEphFileSatsim::ReadExtEphFile()\n"
                   << "Memory allocation failed for the ephemeris array"
                   << std::ends;
            std::runtime_error except( stmMsg.str() );
            throw except;
        }

        ephStream.seekg(offset, std::ios::beg);
        ephStream.exceptions( std::ios_base::failbit );
        ephStream.read((char *)ephemeris,
                       std::streamsize(sizeof(StateVector) * m_numItems));
        offset = ephStream.tellg();
        ephStream.close();
        StateVector* pEph = ephemeris;
        StateVector* pEnd = ephemeris + m_numItems;

        for ( ; pEph != pEnd; pEph++ ) {
            m_vTime.push_back( pEph->time );
            m_vVecPosition.push_back( VectorCartesian( pEph->posNmi.x,
                                      pEph->posNmi.y,
                                      pEph->posNmi.z ) );
            m_vVecVelocity.push_back( VectorCartesian( pEph->velNmiPS.x,
                                      pEph->velNmiPS.y,
                                      pEph->velNmiPS.z ) );
        }

        delete [] ephemeris;
    } catch ( ... ) {
        if ( ephStream ) {
            ephStream.close();
        }

        delete [] ephemeris;
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFileSatsim::ReadExtEphFile()\n"
                  << "ephemeris file name = " << m_strFileName
                  << std::endl << std::endl;
        throw;
    }

    //WARNING_FIX:
    //1>src\SpatialEphFileSatsim.cpp(231): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data
    return offset;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
