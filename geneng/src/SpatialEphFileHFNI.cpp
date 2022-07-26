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
 * SpatialEphFileHFNI.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialEphFileHFNI.cpp

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialEphFileHFNI

//  AUTHOR(s) : D. Bringenberg

//  UPDATES   : V1.0-001  03/19/1998  DCB
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: deleted strstream
//              Reason: unused
//				V4.5	12/14/2009 GMB Replaced TimeCalender with stream
//				extraction operator (>>) to correct time reference problem

//  NOTES : Read an Ephemeris File in HFNI ASCII Format.



//****************************************************************************//

//==============================================================================
#include "SpatialEphFileHFNI.h"

#include <fstream>
#include <cstdlib>
#include <sstream>

//==============================================================================
namespace
{
struct Vector {
    double x;
    double y;
    double z;
};

struct StateVector {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    double seconds;

    double minutesFromStart;

    Vector posM;
    Vector velMPS;
};
}

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// constructors & destructors
// Position & Velocity in ECI
SpatialEphFileHFNI
::SpatialEphFileHFNI( HandleConst< TimeJ2000 > hdlTime )
    : SpatialEphFile( hdlTime ) { ; }

// Coordinate Transformation to be applied to position & velocity
SpatialEphFileHFNI
::SpatialEphFileHFNI( HandleConst< TimeJ2000 > hdlTime,
                      Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialEphFile( hdlTime, hdlCoorTrans ) { ; }

SpatialEphFileHFNI
::SpatialEphFileHFNI( HandleConst< TimeJ2000 > hdlTime,
                      const std::string& strFileNm )
    : SpatialEphFile( hdlTime, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n"
                  << "SpatialEphFileHFNI::SpatialEphFileHFNI( "
                  << "HandleConst< TimeJ2000 >& hdlTime,\n"
                  << "                                        "
                  << "const string& strFileNm )\n"
                  << std::endl;
        throw;
    }
}

// Coordinate Transformation to be applied to position & velocity
SpatialEphFileHFNI
::SpatialEphFileHFNI( HandleConst< TimeJ2000 > hdlTime,
                      Handle< CoorTransBaseEci > hdlCoorTrans,
                      const std::string& strFileNm )
    : SpatialEphFile( hdlTime, hdlCoorTrans, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by\n" << std::endl
                  << "SpatialEphFileHFNI::SpatialEphFileHFNI( "
                  << "HandleConst< TimeJ2000 >& hdlTime,\n"
                  << "                                        "
                  << "Handle< CoorTransBaseEci > hdlCoorTrans,\n"
                  << "                                        "
                  << "const string& strFileNm )\n"
                  << std::endl;
        throw;
    }
}

SpatialEphFileHFNI::~SpatialEphFileHFNI()
{
    ;
}

//==============================================================================

//WARNING_FIX:
//1>src\SpatialEphFileHFNI.cpp(132): error C2556: 'long A3::SpatialEphFileHFNI::InitExtEphFile(void)' : overloaded function differs only by return type from '__int64 A3::SpatialEphFileHFNI::InitExtEphFile(void)'


long long int SpatialEphFileHFNI::InitExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFileHFNI.cpp(139): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
    long long int status = 0;

    try {
        m_numItems = 0;
        m_index = 0;
        m_offset = 0;  // continue reading file here (need for paging)???
        //WARNING_FIX:
        //1>src\SpatialEphFileHFNI.cpp(139): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
        status = ReadExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by "
                  << "SpatialEphFileHFNI::InitExtEphFile()\n"
                  << std::endl;
        throw;
    }

    return status;
}

//WARNING_FIX:
//1>src\SpatialEphFileHFNI.cpp(240): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data


long long int SpatialEphFileHFNI::ReadExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFileHFNI.cpp(171): warning C4244: '=' : conversion from 'std::streamoff' to 'long', possible loss of data
    long long int offset = 0;
    std::ifstream ephStream;
    // static const unsigned MAX_LINE_LENGTH = 1024;

    try {
        int          i;
        // char         line[MAX_LINE_LENGTH];
        std::string line;
        StateVector  eph;
        TimeJ2000 j2000;
        ephStream.exceptions( std::ios_base::badbit | std::ios_base::failbit );
        ephStream.open( m_strFileName.c_str() );

        if (m_numItems == 0 || m_offset <= 0) {
            // ephStream.getline(line, MAX_LINE_LENGTH);
            std::getline(ephStream, line);
            //WARNING_FIX:
            //1>src\SpatialEphFileHFNI.cpp(171): warning C4244: '=' : conversion from 'std::streamoff' to 'long', possible loss of data
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
        // ephStream.getline(line, MAX_LINE_LENGTH);
        std::getline(ephStream, line);

        while (! ephStream.eof()) {
            // std::istringstream(line, sizeof(line))
            std::istringstream(line)
                    >> eph.year >> eph.month >> eph.day
                    >> eph.hour >> eph.minute >> eph.seconds
                    >> eph.minutesFromStart
                    >> eph.posM.x >> eph.posM.y >> eph.posM.z
                    >> eph.velMPS.x >> eph.velMPS.y >> eph.velMPS.z;
            std::stringstream timeStrm;
            timeStrm << eph.year << "/" << eph.month << "/" << eph.day << " "
                     << eph.hour << ":" << eph.minute << ":" << eph.seconds;
            timeStrm >> ioTimeYMDHMS >> ioTimeUTC >> j2000;
            m_vTime.push_back( j2000 );
            m_vVecPosition.push_back(VectorCartesian(eph.posM.x * M_TO_NMI,
                                     eph.posM.y * M_TO_NMI,
                                     eph.posM.z * M_TO_NMI ));
            m_vVecVelocity.push_back(VectorCartesian(eph.velMPS.x * M_TO_NMI,
                                     eph.velMPS.y * M_TO_NMI,
                                     eph.velMPS.z * M_TO_NMI));
            i++;
            // ephStream.getline(line, MAX_LINE_LENGTH);
            std::getline(ephStream, line);
        }

        m_numItems = i;
        offset = ephStream.tellg();
        ephStream.close();
    } catch ( ... ) {
        if ( ephStream ) {
            ephStream.close();
        }

        std::cerr << "Exception caught by "
                  << "SpatialEphFileHFNI::ReadExtEphFile()\n"
                  << std::endl;
        throw;
    }

    return offset;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
