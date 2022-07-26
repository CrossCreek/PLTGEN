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
 * SpatialEphFilePost.cpp
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

//  MODULE(s) : SpatialEphFilePost

//  AUTHOR(s) : D. Bringenberg, L. DiGrasso, J. McCormick

//  UPDATES   : V1.3      08/29/2001  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build


//  NOTES : Read an Ephemeris File in Satsim Binary Format.



//****************************************************************************//

//==============================================================================

#include "SpatialEphFilePost.h"
#include "CoorTransEciPost.h"
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
SpatialEphFilePost
::SpatialEphFilePost(HandleConst< TimeJ2000 > hdlTime)
    : SpatialEphFile(hdlTime) { ; }

//------------------------------------------------------------------------------
// Coordinate Transformation to be applied to position & velocity
SpatialEphFilePost
::SpatialEphFilePost(HandleConst< TimeJ2000 > hdlTime,
                     Handle< CoorTransBaseEci > hdlCoorTrans)
    : SpatialEphFile(hdlTime, hdlCoorTrans) { ; }

//------------------------------------------------------------------------------
SpatialEphFilePost
::SpatialEphFilePost(HandleConst< TimeJ2000 > hdlTime,
                     const TimeJ2000& refTime,
                     const std::string& strFileNm)
    : SpatialEphFile(hdlTime, strFileNm),
      m_tmPostInertialRefEpoch(refTime)
{
    try {
        InitExtEphFile();
    } catch (...) {
        std::cerr << "Exception caught by " << std::endl
                  << "SpatialEphFilePost::SpatialEphFilePost(HandleConst< TimeJ2000 >& hdlTime," << std::endl
                  << "                                            const std::string& strFileNm)"
                  << std::endl << std::endl;
        throw;
    }
}

//------------------------------------------------------------------------------
// Coordinate Transformation to be applied to position & velocity
SpatialEphFilePost
::SpatialEphFilePost(HandleConst< TimeJ2000 > hdlTime,
                     Handle< CoorTransBaseEci > hdlCoorTrans,
                     const TimeJ2000& refTime,
                     const std::string& strFileNm)
    : SpatialEphFile(hdlTime, hdlCoorTrans, strFileNm),
      m_tmPostInertialRefEpoch(refTime)
{
    try {
        InitExtEphFile();
    } catch (...) {
        std::cerr << "Exception caught by " << std::endl
                  << "SpatialEphFilePost::SpatialEphFilePost(HandleConst< TimeJ2000 >& hdlTime," << std::endl
                  << "                                            Handle< CoorTransBaseEci > hdlCoorTrans,"
                  << "                                            const std::string& strFileNm)"
                  << std::endl << std::endl;
        throw;
    }
}

//------------------------------------------------------------------------------
SpatialEphFilePost::~SpatialEphFilePost()
{
    ;
}

//==============================================================================

//WARNING_FIX:
//1>src\SpatialEphFilePost.cpp(103): error C2556: 'long A3::SpatialEphFilePost::InitExtEphFile(void)' : overloaded function differs only by return type from '__int64 A3::SpatialEphFilePost::InitExtEphFile(void)'


long long int SpatialEphFilePost::InitExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFilePost.cpp(115): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
    long long int status = 0;

    try {
        m_numItems = 0;
        m_index    = 0;
        m_offset   = 0;  // continue reading file here (need for paging)???
        //WARNING_FIX:
        //1>src\SpatialEphFilePost.cpp(115): warning C4244: '=' : conversion from '__int64' to 'long', possible loss of data
        status = ReadExtEphFile();
    } catch (...) {
        std::cerr << "Exception caught by "
                  << "SpatialEphFilePost::InitExtEphFile()"
                  << std::endl << std::endl;
        throw;
    }

    return status;
}

//==============================================================================

//WARNING_FIX:
//>src\SpatialEphFilePost.cpp(190): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data


long long int SpatialEphFilePost::ReadExtEphFile()
{
    //WARNING_FIX:
    //1>src\SpatialEphFilePost.cpp(124): warning C4244: 'initializing' : conversion from '__int64' to 'long', possible loss of data
    long long int offset = m_offset;
    std::ifstream ephStream;
    // StateVector *ephemeris = NULL;
    TimeJ2000 tmLaunch;
    TimeJ2000 svTime;
    VectorCartesian pos, vel, posEci, velEci;

    try {
        double relTime;
        CoorTransEciPci xEciPci(m_tmPostInertialRefEpoch);
        m_numItems = 3000;
        //  IF MEMORY REQUIREMENTS ARE EXCESSIVE m_numItems MUST BE DECREASED TO A
        //    REASONABLE(?) NUMBER
        m_vTime.reserve(m_numItems);
        m_vVecPosition.reserve(m_numItems);
        m_vVecVelocity.reserve(m_numItems);
        ephStream.exceptions(std::ios_base::failbit | std::ios_base::badbit);
        ephStream.open(m_strFileName.c_str(), std::ios::in);
        ephStream.exceptions(std::ios_base::badbit);
        // read post header
        PostHeader ph(ephStream);
        m_dtInertialEpochWrtLaunch = ph.Time0();
        tmLaunch = m_tmPostInertialRefEpoch - m_dtInertialEpochWrtLaunch;

        // Read in Post State Vector data from external File
        while (ephStream >> relTime >> pos >> vel) {
            // convert relative time to J2000
            svTime = relTime + tmLaunch;

            // skip to next line if current entry is a duplicate of previous entry
            if (m_vTime.empty() || svTime != m_vTime.back()) {
                // convert units from ft to nmi
                pos *= FT_TO_NMI;
                vel *= FT_TO_NMI;
                // rotate position and velocity vector to ECI
                xEciPci.ToBase(pos, vel, posEci, velEci);
                m_vTime.push_back(svTime);
                m_vVecPosition.push_back(posEci);
                m_vVecVelocity.push_back(velEci);
            }
        }

        m_numItems = m_vTime.size();
    } catch (...) {
        if (ephStream) {
            ephStream.close();
        }

        std::cerr << "Exception caught by" << std::endl
                  << "SpatialEphFilePost::ReadExtEphFile()" << std::endl
                  << "ephemeris file name = " << m_strFileName
                  << std::endl << std::endl;
        throw;
    }

    //WARNING_FIX:
    //>src\SpatialEphFilePost.cpp(190): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data
    return offset;
}
// ============================================================================
}
// A3 namespace end
// ============================================================================
