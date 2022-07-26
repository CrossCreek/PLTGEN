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
 * SpatialEphFileTrail.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialEphFileTrail.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialEphFileTrail

//  AUTHOR(s) : D. Bringenberg

//  UPDATES   : V1.0-001  03/19/1998  DCB
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//				V4.5	12/14/2009 GMB Convert input time from UTC to TT

//  NOTES : Read an Ephemeris File in Trail ASCII Format.

//****************************************************************************//

//==============================================================================
#include "SpatialEphFileTrail.h"
#include "TimeCalendar.h"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>

// ============================================================================
namespace
{
const unsigned MAX_LINE_LENGTH = 1024;

struct Vector {
    double x;
    double y;
    double z;
};


struct StateVector {




    Vector posKm;
    Vector velKmPS;

    int year;
    int dayOfYear;
    int hour;
    int minute;
    double seconds;
};


}

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
// Position & Velocity in ECI
SpatialEphFileTrail
::SpatialEphFileTrail( HandleConst< TimeJ2000 > hdlTime )
    : SpatialEphFile( hdlTime ) { ; }

// Coordinate Transformation to be applied to position & velocity
SpatialEphFileTrail
::SpatialEphFileTrail( HandleConst< TimeJ2000 > hdlTime,
                       Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialEphFile( hdlTime, hdlCoorTrans ) { ; }

SpatialEphFileTrail
::SpatialEphFileTrail( HandleConst< TimeJ2000 > hdlTime,
                       const std::string& strFileNm )
    : SpatialEphFile( hdlTime, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFileTrail::SpatialEphFileTrail( "
                  << "HandleConst< TimeJ2000 >& hdlTime,\n"
                  << "                                          "
                  << "const string& strFileNm )\n"
                  << std::endl;
        throw;
    }
}

// Coordinate Transformation to be applied to position & velocity
SpatialEphFileTrail
::SpatialEphFileTrail( HandleConst< TimeJ2000 > hdlTime,
                       Handle< CoorTransBaseEci > hdlCoorTrans,
                       const std::string& strFileNm )
    : SpatialEphFile( hdlTime, hdlCoorTrans, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFileTrail::SpatialEphFileTrail(  "
                  << "HandleConst< TimeJ2000 >& hdlTime,\n"
                  << "                                           "
                  << "Handle< CoorTransBaseEci > hdlCoorTrans,\n"
                  << "                                           "
                  << "const string& strFileNm )\n"
                  << std::endl;
        throw;
    }
}

SpatialEphFileTrail::~SpatialEphFileTrail()
{
    ;
}


//==============================================================================

//WARNING_FIX:
//1>src\SpatialEphFileTrail.cpp(162): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data


long long int SpatialEphFileTrail::InitExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFileTrail.cpp(144): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
    long long int status = 0;

    try {
        m_numItems = 0;
        m_index = 0;
        m_offset = 0;  // continue reading file here (need for paging)???
        //WARNING_FIX:
        //1>src\SpatialEphFileTrail.cpp(144): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
        status = ReadExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception thrown by\n"
                  << "SpatialEphFileTrail::InitExtEphFile()\n"
                  << std::endl;
        throw;
    }

    //WARNING_FIX:
    //1>src\SpatialEphFileTrail.cpp(162): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data
    return status;
}

bool FindStateVector( std::ifstream& ephStream, char line[MAX_LINE_LENGTH] )
{
    if (strstr(line, "OSCUL:") != NULL ||
            strstr(line, "LAST POINT") != NULL) {
        return true;
    }

    while (! ephStream.eof()) {
        ephStream.getline(line, MAX_LINE_LENGTH);

        if (strstr(line, "OSCUL:") != NULL ||
                strstr(line, "LAST POINT") != NULL) {
            return true;
        }
    }

    return false;
}


bool ReadStateVector( std::ifstream& ephStream,
                      char line[MAX_LINE_LENGTH],
                      StateVector& eph )
{
    std::string cHour(3,' '), cMinute(3,' ');
    double minutesSinceEpoch;
    ephStream >> minutesSinceEpoch;

    if (! ephStream.fail()) {
        char slash;
        ephStream >> eph.posKm.x   >> eph.posKm.y   >> eph.posKm.z
                  >> eph.velKmPS.x >> eph.velKmPS.y >> eph.velKmPS.z
                  >> eph.year      >> slash         >> eph.dayOfYear
                  >> std::ws       >> cHour[0]      >> cHour[1]
                  >> cMinute[0]    >> cMinute[1]    >> eph.seconds;

        if (cHour[0] == '0') {
            cHour[0] = ' ';
        }

        if (cMinute[0] == '0') {
            cMinute[0] = ' ';
        }

        cHour[2] = cMinute[2] = '\0';
        std::istringstream(cHour) >> eph.hour;
        std::istringstream(cMinute) >> eph.minute;
        ephStream.getline(line, MAX_LINE_LENGTH);
        ephStream.getline(line, MAX_LINE_LENGTH);
        ephStream.getline(line, MAX_LINE_LENGTH);
        return true;
    }

    ephStream.clear();
    return false;
}

//WARNING_FIX:
//1>src\SpatialEphFileTrail.cpp(299): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data


long long int SpatialEphFileTrail::ReadExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFileTrail.cpp(234): warning C4244: '=' : conversion from 'std::streamoff' to 'long', possible loss of data
    long long int offset = m_offset;
    std::ifstream ephStream;

    try {
        int          i;
        int          JANUARY = 1;
        char         line[MAX_LINE_LENGTH];
        StateVector  eph;
        TimeJ2000    j2000;
        ephStream.exceptions( std::ios_base::badbit | std::ios_base::failbit );
        ephStream.open( m_strFileName.c_str() );

        if (m_numItems == 0 || m_offset <= 0) {
            ephStream.getline(line, MAX_LINE_LENGTH);
            //WARNING_FIX:
            //1>src\SpatialEphFileTrail.cpp(234): warning C4244: '=' : conversion from 'std::streamoff' to 'long', possible loss of data
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
        ephStream.exceptions( std::ios_base::badbit );
        i = 0;

        while (! ephStream.eof()) {
            while (FindStateVector(ephStream, line)) {
                while (ReadStateVector(ephStream, line, eph)) {
                    j2000 = TimeCalendar(eph.year, JANUARY, eph.dayOfYear,
                                         eph.hour, eph.minute, eph.seconds);
                    // convert UTC time to internal TT time reference
                    j2000 = timeRefConv(j2000, A3::TimeRef::UTC, A3::TimeRef::TT);
                    m_vTime.push_back( j2000 );
                    m_vVecPosition.push_back( VectorCartesian(
                                                  eph.posKm.x * KM_TO_NMI,
                                                  eph.posKm.y * KM_TO_NMI,
                                                  eph.posKm.z * KM_TO_NMI ) );
                    m_vVecVelocity.push_back( VectorCartesian(
                                                  eph.velKmPS.x * KM_TO_NMI,
                                                  eph.velKmPS.y * KM_TO_NMI,
                                                  eph.velKmPS.z * KM_TO_NMI ) );
                    i++;
                }
            }
        }

        m_numItems = i;
        offset = ephStream.tellg();
        ephStream.close();
    } catch ( ... ) {
        if ( ephStream ) {
            ephStream.close();
        }

        std::cerr << "Exception thrown by "
                  << "SpatialEphFileTrail::ReadExtEphFile()\n"
                  << std::endl;
        throw;
    }

    //WARNING_FIX:
    //1>src\SpatialEphFileTrail.cpp(299): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data
    return offset;
}
// ============================================================================
}
// A3 namespace end
// ============================================================================
