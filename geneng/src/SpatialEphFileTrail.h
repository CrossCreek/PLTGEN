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
 * SpatialEphFileTrail.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialEphFileTrail.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialEphFileTrail

//  AUTHOR(s) : D. Bringenberg

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

//  NOTES : Read an Ephemeris File in Trail ASCII Format.



//****************************************************************************//

//==============================================================================
#ifndef SPATIALEPHFILETRAIL_H
#define SPATIALEPHFILETRAIL_H	"SpatialEphFileTrail VV4.3-009"

//------------------------------------------------------------------------------
// required headers
#include "SpatialEphFile.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class SpatialEphFileTrail : public SpatialEphFile
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    // Position & Velocity in ECI



    explicit SpatialEphFileTrail( HandleConst< TimeJ2000 > hdlTime );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphFileTrail( HandleConst< TimeJ2000 > hdlTime,
                         Handle< CoorTransBaseEci > hdlCoorTrans );

    // Position & Velocity in ECI
    SpatialEphFileTrail( HandleConst< TimeJ2000 > hdlTime,
                         const std::string& strFileNm );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphFileTrail( HandleConst< TimeJ2000 > hdlTime,
                         Handle< CoorTransBaseEci > hdlCoorTrans,
                         const std::string& strFileNm );

    virtual ~SpatialEphFileTrail();

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //----------------------------------------------------------------------
    // file functions

    //WARNING_FIX:
    //1>c:\svn_checkouts\mural_v11.0\geneng\src\SpatialEphFileTrail.h(94): error C2555: 'A3::SpatialEphFileTrail::InitExtEphFile': overriding virtual function return type differs and is not covariant from 'A3::SpatialEphFile::InitExtEphFile'


    virtual long long int InitExtEphFile();

    //WARNING_FIX:
    //1>c:\svn_checkouts\mural_v11.0\geneng\src\SpatialEphFileTrail.h(89): error C2555: 'A3::SpatialEphFileTrail::ReadExtEphFile': overriding virtual function return type differs and is not covariant from 'A3::SpatialEphFile::ReadExtEphFile'


    virtual long long int ReadExtEphFile();

    //--------------------------------------------------------------------------
protected:

    //--------------------------------------------------------------------------
private:
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// misc functions
inline const char* SpatialEphFileTrail::GetVersion() const
{
    return SPATIALEPHFILETRAIL_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
