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
 * SpatialEphFilePost.h
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
#ifndef SPATIALEPHFILEPOST_H
#define SPATIALEPHFILEPOST_H  "SpatialEphFilePost V2.0"

//------------------------------------------------------------------------------
// required headers
#include "SpatialEphFile.h"
#include "Angle.h"

// ============================================================================
// A3 namespace start
namespace A3
{
//------------------------------------------------------------------------------
class SpatialEphFilePost : public SpatialEphFile
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    // Position & Velocity in ECI



    explicit SpatialEphFilePost( HandleConst< TimeJ2000 > hdlTime );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphFilePost( HandleConst< TimeJ2000 > hdlTime,
                        Handle< CoorTransBaseEci > hdlCoorTrans );

    SpatialEphFilePost( HandleConst< TimeJ2000 > hdlTime,
                        const TimeJ2000& refTime,
                        const std::string& strFileNm );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphFilePost( HandleConst< TimeJ2000 > hdlTime,
                        Handle< CoorTransBaseEci > hdlCoorTrans,
                        const TimeJ2000& refTime,
                        const std::string& strFileNm );

    virtual ~SpatialEphFilePost();

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //----------------------------------------------------------------------
    // file functions

    //WARNING_FIX:
    //1>c:\svn_checkouts\mural_v11.0\geneng\src\SpatialEphFilePost.h(86): error C2555: 'A3::SpatialEphFilePost::InitExtEphFile': overriding virtual function return type differs and is not covariant from 'A3::SpatialEphFile::InitExtEphFile'


    virtual long long int InitExtEphFile();

    //WARNING_FIX:
    //1>c:\svn_checkouts\mural_v11.0\geneng\src\SpatialEphFilePost.h(86): error C2555: 'A3::SpatialEphFilePost::ReadExtEphFile': overriding virtual function return type differs and is not covariant from 'A3::SpatialEphFile::ReadExtEphFile'


    virtual long long int ReadExtEphFile();

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
    // user input date & time which corresponds to
    // the start of the POST data
    TimeJ2000 m_tmPostInertialRefEpoch;
    double    m_dtInertialEpochWrtLaunch;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// misc functions
inline const char* SpatialEphFilePost::GetVersion() const
{
    return SPATIALEPHFILEPOST_H;
}

// ============================================================================
}
// A3 namespace end
//------------------------------------------------------------------------------
#endif

