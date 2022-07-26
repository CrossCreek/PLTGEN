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
 * SpatialEphOplFile.h
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

//  MODULE(s) : SpatialEphOplFile

//  AUTHOR(s) : M. Ell

//  UPDATES   : V3.7-002  06/28/2005  MDE  SATSIM now accepts OPL files

//  NOTES : Read an Ephemeris File in OPL Format



//****************************************************************************//

//==============================================================================
#ifndef SpatialEphOplFile_H
#define SpatialEphOplFile_H  "SpatialEphOplFile V2.0"

//------------------------------------------------------------------------------
// required headers
#include "SpatialEphFile.h"

// ============================================================================
// A3 namespace start
namespace A3
{
//------------------------------------------------------------------------------
class SpatialEphOplFile : public SpatialEphFile
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    // Position & Velocity in ECI



    explicit SpatialEphOplFile( HandleConst< TimeJ2000 > hdlTime );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphOplFile( HandleConst< TimeJ2000 > hdlTime,
                       Handle< CoorTransBaseEci > hdlCoorTrans );

    SpatialEphOplFile( HandleConst< TimeJ2000 > hdlTime,
                       const std::string& strFileNm );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphOplFile( HandleConst< TimeJ2000 > hdlTime,
                       Handle< CoorTransBaseEci > hdlCoorTrans,
                       const TimeJ2000& refTime,
                       const std::string& strFileNm );

    virtual ~SpatialEphOplFile();

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //----------------------------------------------------------------------
    // file functions

    //WARNING_FIX:
    //1>c:\svn_checkouts\mural_v11.0\geneng\src\SpatialEphOplFile.h(77): error C2555: 'A3::SpatialEphOplFile::InitExtEphFile': overriding virtual function return type differs and is not covariant from 'A3::SpatialEphFile::InitExtEphFile'


    virtual long long int InitExtEphFile();

    //WARNING_FIX:
    //1>c:\svn_checkouts\mural_v11.0\geneng\src\SpatialEphOplFile.h(77): error C2555: 'A3::SpatialEphOplFile::ReadExtEphFile': overriding virtual function return type differs and is not covariant from 'A3::SpatialEphFile::ReadExtEphFile'


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
inline const char* SpatialEphOplFile::GetVersion() const
{
    return SpatialEphOplFile_H;
}

// ============================================================================
}
// A3 namespace end
//------------------------------------------------------------------------------
#endif

