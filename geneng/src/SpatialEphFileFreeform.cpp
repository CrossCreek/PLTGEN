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
 * SpatialEphFileFreeform.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  SCCS - Release %R%  Delta %I%  from %G%

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialEphFileFreeform

//  AUTHOR(s) : M. Ell

//  UPDATES   : V3.7-002  06/28/2005  MDE

//              V4.5-001  2009-07-01  JAM
//              added casting to eliminate 64-bit warnings
//				V4.5	12/14/2009 GMB Replaced TimeCalender with stream
//				extraction operator (>>) to correct time reference problem


//  NOTES : Read an Ephemeris File in OPL Freeform Format

//     IS&S` AAA Skill Center - Lockheed Martin Corporation, Valley Forge, PA

//****************************************************************************//

//==============================================================================

#include "SpatialEphFileFreeform.h"
#include "CoorTransEciEcf.h"
#include "VectorCartesian.h"

#include <fstream>
#include <cstdlib>

// ============================================================================
// A3 namespace start
namespace A3
{
//==============================================================================
// constructors & destructors
// Position & Velocity in ECI
SpatialEphFileFreeform
::SpatialEphFileFreeform( HandleConst< TimeJ2000 > hdlTime )
    : SpatialEphFile( hdlTime ) { ; }

//------------------------------------------------------------------------------
// Coordinate Transformation to be applied to position & velocity
SpatialEphFileFreeform
::SpatialEphFileFreeform( HandleConst< TimeJ2000 > hdlTime,
                          Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialEphFile( hdlTime, hdlCoorTrans ) { ; }

//------------------------------------------------------------------------------
SpatialEphFileFreeform
::SpatialEphFileFreeform( HandleConst< TimeJ2000 > hdlTime,
                          const std::string& strFileNm )
    : SpatialEphFile( hdlTime, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by " << std::endl
                  << "SpatialEphFileFreeform::SpatialEphFileFreeform( HandleConst< TimeJ2000 >& hdlTime," << std::endl
                  << "                                            const std::string& strFileNm )"
                  << std::endl << std::endl;
        throw;
    }
}

//------------------------------------------------------------------------------
// Coordinate Transformation to be applied to position & velocity
SpatialEphFileFreeform
::SpatialEphFileFreeform( HandleConst< TimeJ2000 > hdlTime,
                          Handle< CoorTransBaseEci > hdlCoorTrans,
                          const TimeJ2000& refTime,
                          const std::string& strFileNm )
    : SpatialEphFile( hdlTime, hdlCoorTrans, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by " << std::endl
                  << "SpatialEphFileFreeform::SpatialEphFileFreeform( HandleConst< TimeJ2000 >& hdlTime," << std::endl
                  << "                                            Handle< CoorTransBaseEci > hdlCoorTrans,"
                  << "                                            const std::string& strFileNm )"
                  << std::endl << std::endl;
        throw;
    }
}

//------------------------------------------------------------------------------
SpatialEphFileFreeform::~SpatialEphFileFreeform()
{
    ;
}

//==============================================================================

//WARNING_FIX:
//1>src\SpatialEphFileFreeform.cpp(127): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data


long long int SpatialEphFileFreeform::InitExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFileFreeform.cpp(109): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
    long long int status = 0;

    try {
        m_numItems = 0;
        m_index    = 0;
        m_offset   = 0;  // continue reading file here (need for paging)???
        //WARNING_FIX:
        //1>src\SpatialEphFileFreeform.cpp(109): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
        status = ReadExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by "
                  << "SpatialEphFileFreeform::InitExtEphFile()"
                  << std::endl << std::endl;
        throw;
    }

    //WARNING_FIX:
    //1>src\SpatialEphFileFreeform.cpp(127): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data
    return status;
}

//==============================================================================

//WARNING_FIX:
//1>src\SpatialEphFileFreeform.cpp(225): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data


long long int SpatialEphFileFreeform::ReadExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFileFreeform.cpp(123): warning C4244: 'initializing' : conversion from '__int64' to 'long', possible loss of data
    long long int offset = m_offset;
    std::ifstream ephStream;
    // StateVector *ephemeris = NULL;
    TimeJ2000 dateTime;
    VectorCartesian pos, vel, posEci, velEci;
    std::string strBuff;

    try {
        int month;
        int day;
        int year;
        int hour;
        int min;
        double sec;
        char delim;
        int units = 0; // set to 0 ... units will be in feet unless otherwise specified in the header
        m_numItems = 3000;
        //  IF MEMORY REQUIREMENTS ARE EXCESSIVE m_numItems MUST BE DECREASED TO A
        //    REASONABLE(?) NUMBER
        m_vTime.reserve( m_numItems );
        m_vVecPosition.reserve( m_numItems );
        m_vVecVelocity.reserve( m_numItems );
        ephStream.exceptions( std::ios_base::failbit | std::ios_base::badbit );
        ephStream.open( m_strFileName.c_str(), std::ios::in);
        ephStream.exceptions( std::ios_base::badbit );
        // check to see if there is a header or not
        std::getline( ephStream, strBuff );
        // Look to see if the first line contains any letters or stars
        int end = (int)strBuff.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*");

        if (end != -1) {
            // if header, remove the next four lines
            std::getline( ephStream, strBuff );
            std::getline( ephStream, strBuff );
            std::getline( ephStream, strBuff );
            units = (int)strBuff.find_first_of("f"); // See if units are feet or meters
            std::getline( ephStream, strBuff );
        } else {
            ephStream.seekg(std::ios_base::beg);
        }

        // Read in time, date X,Y,Z then Xdot, Ydot, Zdot (in vector cartesian form most likely)
        while( ephStream >> month >> delim >> day >> delim >> year >> hour >> delim >> min >> delim >> sec >> pos >> vel ) {
            std::stringstream timeStrm;
            timeStrm << year << "/" << month << "/" << day << " "
                     << hour << ":" << min << ":" << sec;
            timeStrm >> ioTimeYMDHMS >> ioTimeUTC;
            timeStrm >> dateTime;

            // convert units from ft to nmi or meters to nmi
            if (units == -1) {
                pos *= M_TO_NMI;
                vel *= M_TO_NMI;
            } else {
                pos *= FT_TO_NMI;
                vel *= FT_TO_NMI;
            }

            m_vTime.push_back( dateTime );
            m_vVecPosition.push_back( pos );
            m_vVecVelocity.push_back( vel );
        }

        m_numItems = m_vTime.size();
    } catch ( ... ) {
        if ( ephStream ) {
            ephStream.close();
        }

        std::cerr << "Exception caught by" << std::endl
                  << "SpatialEphFileFreeform::ReadExtEphFile()" << std::endl
                  << "ephemeris file name = " << m_strFileName
                  << std::endl << std::endl;
        throw;
    }

    //WARNING_FIX:
    //1>src\SpatialEphFileFreeform.cpp(225): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data
    return offset;
}
// ============================================================================
}
// A3 namespace end
// ============================================================================
