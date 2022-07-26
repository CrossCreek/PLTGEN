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
 * SpatialEphFileComplex.cpp
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

//  MODULE(s) : SpatialEphFileComplex

//  AUTHOR(s) : Gary J. Katz

//  UPDATES   : V3.7-001  06/22/2005  GJK  Added Complex file input for
//                                    ephemeris

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced non-standard throw with standard throw,
//              eliminated dependency on unitConverter, eliminated inconsistent
//              use of std scope resolution
//              Reason: dependency in General library resulted in an unnecessary
//              two-way dependency

//              V4.5-001  2009-07-01  JAM
//              added casting to eliminate 64-bit warnings, replaced
//              unnamed namespace makeUpper with A3::makeUpper template

//  NOTES : Read an Ephemeris File in a format that can be written by
//          a COTS package.



//****************************************************************************//

//==============================================================================

#include "SpatialEphFileComplex.h"
#include "CoorTransEciEcf.h"
#include "VectorCartesian.h"

#include "TimeCalendar.h"
#include "TimeJ2000.h"

#include "VectorGeocentric.h"
#include "Handle.h"
#include "CoorTransEciEcf.h"
#include "constants.h"

// #include <UnitConverter.h>

#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>

// ============================================================================
// A3 namespace start
namespace A3
{
//==============================================================================
// constructors & destructors
// Position & Velocity in ECI
SpatialEphFileComplex
::SpatialEphFileComplex( HandleConst< TimeJ2000 > hdlTime )
    : SpatialEphFile( hdlTime ) { ; }

//------------------------------------------------------------------------------
// Coordinate Transformation to be applied to position & velocity
SpatialEphFileComplex
::SpatialEphFileComplex( HandleConst< TimeJ2000 > hdlTime,
                         Handle< CoorTransBaseEci > hdlCoorTrans )
    : SpatialEphFile( hdlTime, hdlCoorTrans ) { ; }

//------------------------------------------------------------------------------
SpatialEphFileComplex
::SpatialEphFileComplex( HandleConst< TimeJ2000 > hdlTime,
                         const std::string& strFileNm )
    : SpatialEphFile( hdlTime, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by " << std::endl
                  << "SpatialEphFileComplex::SpatialEphFileComplex( HandleConst< TimeJ2000 >& hdlTime," << std::endl
                  << "                                            const std::string& strFileNm )"
                  << std::endl << std::endl;
        throw;
    }
}

//------------------------------------------------------------------------------
// Coordinate Transformation to be applied to position & velocity
SpatialEphFileComplex
::SpatialEphFileComplex( HandleConst< TimeJ2000 > hdlTime,
                         Handle< CoorTransBaseEci > hdlCoorTrans,
                         const std::string& strFileNm )
    : SpatialEphFile( hdlTime, hdlCoorTrans, strFileNm )
{
    try {
        InitExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by " << std::endl
                  << "SpatialEphFileComplex::SpatialEphFileComplex( HandleConst< TimeJ2000 >& hdlTime," << std::endl
                  << "                                            Handle< CoorTransBaseEci > hdlCoorTrans,"
                  << "                                            const std::string& strFileNm )"
                  << std::endl << std::endl;
        throw;
    }
}

//------------------------------------------------------------------------------
SpatialEphFileComplex::~SpatialEphFileComplex()
{
    ;
}

//==============================================================================

//WARNING_FIX:
//1>src\SpatialEphFileComplex.cpp(144): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data


long long int SpatialEphFileComplex::InitExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFileComplex.cpp(367): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data
    long long int status = 0;

    try {
        m_numItems = 0;
        m_index    = 0;
        m_offset   = 0;  // continue reading file here (need for paging)???
        //WARNING_FIX:
        //1>src\SpatialEphFileComplex.cpp(367): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data
        status = ReadExtEphFile();
    } catch ( ... ) {
        std::cerr << "Exception caught by "
                  << "SpatialEphFileComplex::InitExtEphFile()"
                  << std::endl << std::endl;
        throw;
    }

    //WARNING_FIX:
    //1>src\SpatialEphFileFreeform.cpp(109): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
    return status;
}

//==============================================================================
// Changed 09-10-05: replaced by GenUtility makeUpper template
//namespace {
//    static const int CAP_DIST('A'-'a');
//
//    template <class T>
//    T makeUpper(T c)
//    {
//        if (static_cast<T>('a')<=c && static_cast<T>('z')>=c)
//                c += static_cast<T>(CAP_DIST);
//
//        return c;
//    }
//
//	template std::string::value_type makeUpper(std::string::value_type);
//}

//WARNING_FIX:
//1>src\SpatialEphFileComplex.cpp(367): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data


long long int SpatialEphFileComplex::ReadExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFileComplex.cpp(158): warning C4244: 'initializing' : conversion from '__int64' to 'long', possible loss of data
    long long int offset = m_offset;
    std::ifstream ephStream;
    TimeJ2000 tmLaunch;
    TimeJ2000 svTime;
    std::string tmp;
    std::string format;
    double latPos, longPos, altPos;
    VectorCartesian pos, vel, posEci, prevPos, velEci;
    VectorGeocentric geoPos;
    CoordSystemTypes coordinateSystem = COORD_SYS_INERTIAL;
    //velEci.Set(0.0, 0.0, 0.0);
    prevPos.Set(0.0, 0.0, 0.0);
    std::string strBuff;

    try {
        double relTime;
        double conversionFactor = 1.0;
        ephStream.exceptions( std::ios_base::failbit | std::ios_base::badbit );
        ephStream.open( m_strFileName.c_str(), std::ios::in);
        ephStream.exceptions( std::ios_base::badbit );
        // Get header information
        ephStream >> tmp;

        while(tmp != "BEGIN" && !ephStream.eof()) {
            ephStream >> tmp;
        }

        if(ephStream.eof()) {
            throw std::runtime_error("SpatialEphFileComplex::ReadExtEphFile() "
                                     "Incorrect data file format.  Missing \"BEGIN\"");
        }

        ephStream >> tmp;

        if(tmp != "Ephemeris")
            throw std::runtime_error("SpatialEphFileComplex::ReadExtEphFile() "
                                     "Incorrect data file format.  Expected \"Ephemeris\" found " + tmp);

        ephStream >> tmp;

        while(!ephStream.eof()) {
            if(tmp == "NumberOfEphemerisPoints") {
                ephStream >> m_numItems;
            } else if(tmp == "ScenarioEpoch") {
                TimeCalendar time;
                int day;
                int year;
                std::string hour;
                std::string minute;
                std::string second;
                std::string month;
                std::string timestr;
                ephStream >> day >> month >> year >> timestr;
                // extract min hour sec
                int hourPos = (int)timestr.find_first_of(':');
                hour = timestr.substr(0, hourPos );
                int minutePos = (int)timestr.find_first_of(':', hourPos + 1);
                minute = timestr.substr(hourPos + 1, minutePos - (hourPos + 1));
                second = timestr.substr(minutePos + 1, timestr.size() - (minutePos + 1));
                time.Set(year, 1, day, atoi(hour.c_str()), atoi(minute.c_str()), atof(second.c_str()));
                time.SetMonth( month );
                // convert to internal time reference (terrestrial time)
                tmLaunch =  timeRefConv(time, A3::TimeRef::UTC, A3::TimeRef::TT);
            } else if( tmp == "DistanceUnit") {
                // UnitConverter converter;
                std::string distanceUnit;
                ephStream >> distanceUnit;
                // Changed 09-10-05 to use GenUtility version of makeUpper to
                // avoid compiler resolution problem
                makeUpper(distanceUnit.begin(), distanceUnit.end());
                //std::transform(distanceUnit.begin(), distanceUnit.end(),
                //               distanceUnit.begin(),
                //               makeUpper<std::string::value_type>);

                if ("NMI" == distanceUnit) {
                    conversionFactor = 1.0;
                } else if ("M" == distanceUnit) {
                    conversionFactor = M_TO_NMI;
                } else if ("KM" == distanceUnit) {
                    conversionFactor = KM_TO_NMI;
                } else if ("FT" == distanceUnit) {
                    conversionFactor = FT_TO_NMI;
                } else {
                    throw std::runtime_error(__FILE__ " invalid unit: "+distanceUnit);
                }
            } else if( tmp == "CentralBody" ) {
                std::string centralBody;
                ephStream >> centralBody;

                if(centralBody != "Earth") {
                    throw std::runtime_error("SpatialEphFileComplex::ReadExtEphFile() "
                                             "Central Body must be \"Earth\" in file : " + m_strFileName );
                }
            } else if( tmp == "CoordinateSystem" ) {
                std::string coordinateSys;
                ephStream >> coordinateSys;

                if( coordinateSys.at(0) == 'F' ) {
                    coordinateSystem = COORD_SYS_FIXED;
                } else if( coordinateSys.at(0) == 'I') {
                    coordinateSystem = COORD_SYS_INERTIAL;
                } else {
                    throw std::runtime_error("SpatialEphFileComplex::ReadExtEphFile() "
                                             "Coordinate System must be \"Fixed\" or \"Intertial\" in file : "
                                             + m_strFileName );
                }
            } else if (tmp == "EphemerisLLATimePos") {
                format = tmp;
                break;
            } else if (tmp == "EphemerisTimePosVel") {
                format = tmp;
                break;
            }

            ephStream >> tmp;
        }

        m_vTime.reserve( m_numItems );
        m_vVecPosition.reserve( m_numItems );
        m_vVecVelocity.reserve( m_numItems );

        if (format == "EphemerisLLATimePos") {
            // Read in Post State Vector data from external File
            while( ephStream >> relTime >> latPos >> longPos >> altPos ) {
                pos = VectorGeocentric(DEG_TO_RAD*latPos, DEG_TO_RAD*longPos,
                                       EARTH_RADIUS_NMI + (conversionFactor * altPos),
                                       false);
                // convert relative time to J2000
                svTime = relTime + tmLaunch;

                if( !m_vTime.empty() && (double)(svTime-m_vTime.back()) > .001) {
                    vel = (pos - prevPos)/(double)(svTime-m_vTime.back());
                } else {
                    vel = (prevPos - pos);
                }

                prevPos = pos;

                // rotate position vector to ECI
                if(coordinateSystem == COORD_SYS_FIXED) {
                    HandleConst< TimeJ2000> hdlTime = Handle< TimeJ2000 >( new TimeJ2000(svTime) );
                    // Base ECI Source ECF
                    CoorTransEciEcf transformer(hdlTime);
                    transformer.ToBase(pos, vel, posEci, velEci);
                } else {
                    posEci = pos;
                    velEci = vel;
                }

                /*if((double)(svTime-m_vTime.back()) > .001)
                  velEci = (posEci - prevPosEci)/(double)(svTime-m_vTime.back());
                else
                  velEci = (prevPosEci - posEci);*/
                m_vTime.push_back( svTime );
                m_vVecPosition.push_back( posEci );
                m_vVecVelocity.push_back( velEci );
                //prevPosEci = posEci;
            }
        } else if (format == "EphemerisTimePosVel") {
            // Read in Post State Vector data from external File
            while( ephStream >> relTime >> pos >> vel ) {
                // convert to NMI
                pos = pos * conversionFactor;
                vel = vel * conversionFactor;
                // convert relative time to J2000
                svTime = relTime + tmLaunch;

                // rotate position vector to ECI
                if(coordinateSystem == COORD_SYS_FIXED) {
                    HandleConst< TimeJ2000> hdlTime = Handle< TimeJ2000 >( new TimeJ2000(svTime) );
                    // Base ECI Source ECF
                    CoorTransEciEcf transformer(hdlTime);
                    transformer.ToBase(pos, vel, posEci, velEci);
                } else {
                    posEci = pos;
                    velEci = vel;
                }

                m_vTime.push_back( svTime );
                m_vVecPosition.push_back( posEci );
                m_vVecVelocity.push_back( velEci );
            }
        } else {
            throw std::runtime_error("SpatialEphFileComplex::ReadExtEphFile() "
                                     "Format must be \"EphemerisLLATimePos\" or \"EphemerisTimePosVel\" in file : " + m_strFileName );
        }

        m_numItems = m_vTime.size();
    } catch ( ... ) {
        if ( ephStream ) {
            ephStream.close();
        }

        std::cerr << "Exception caught by" << std::endl
                  << "SpatialEphFileComplex::ReadExtEphFile()" << std::endl
                  << "ephemeris file name = " << m_strFileName
                  << std::endl << std::endl;
        throw;
    }

    //WARNING_FIX:
    //1>src\SpatialEphFileComplex.cpp(367): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data
    return offset;
}
// ============================================================================
}
// A3 namespace end
// ============================================================================
