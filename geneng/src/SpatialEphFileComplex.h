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
 * SpatialEphFileComplex.h
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

//  AUTHOR(s) : Gary J Katz

//  UPDATES   : V3.7-001  06/22/2005  GJK  Added Complex file input for
//                                         ephemeris

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced non-standard throw with standard throw,
//              eliminated dependency on unitConverter, eliminated inconsistent
//              use of std scope resolution
//              Reason: dependency in General library resulted in an unnecessary
//              two-way dependency

//              V4.5-001  2009-07-01  JAM
//              added casting to eliminate 64-bit warnings

//  NOTES : Read an Ephemeris File in a format that can be written by
//          a COTS package.
//          Format:
//            (Sample Header will ignore info that is not understood)

//          BEGIN Ephemeris
//          NumberOfEphemerisPoints n          ( where n is the number of points)
//          InterpolationMethod     Lagrange
//          InterpolationOrder      1
//          DistanceUnit            Kilometers ( will convert units )
//          CentralBody             Earth      ( must be earth at this time )
//          CoordinateSystem        Fixed      ( either Fixed or Inertial )

//          EphemerisLLATimePos

//    Time(1)    Lat(1)   Long(1)   Alt(1)
//    Time(2)    Lat(2)   Long(2)   Alt(2)
//    Time(...)  Lat(...) Long(...) Alt(...)
//    Time(n)    Lat(n)   Long(n)   Alt(n)
//



//****************************************************************************//

//==============================================================================
#ifndef SPATIALEPHFILECOMPLEX_H
#define SPATIALEPHFILECOMPLEX_H  "SpatialEphFileComplex V4.3-009"

//------------------------------------------------------------------------------
// required headers
#include "SpatialEphFile.h"

// ============================================================================
// A3 namespace start
namespace A3
{
//------------------------------------------------------------------------------
class SpatialEphFileComplex : public SpatialEphFile
{

    enum CoordSystemTypes {
        COORD_SYS_FIXED,
        COORD_SYS_INERTIAL
    };



    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    // Position & Velocity in ECI



    explicit SpatialEphFileComplex( HandleConst< TimeJ2000 > hdlTime );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphFileComplex( HandleConst< TimeJ2000 > hdlTime,
                           Handle< CoorTransBaseEci > hdlCoorTrans );

    SpatialEphFileComplex( HandleConst< TimeJ2000 > hdlTime,
                           const std::string& strFileNm );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphFileComplex( HandleConst< TimeJ2000 > hdlTime,
                           Handle< CoorTransBaseEci > hdlCoorTrans,
                           const std::string& strFileNm );

    virtual ~SpatialEphFileComplex();

    //----------------------------------------------------------------------
    // misc functions
    inline virtual const char* GetVersion() const;

    //----------------------------------------------------------------------long
    // file functions

    //WARNING_FIX:
    //1>src\SpatialEphFileComplex.cpp(126): error C2556: '__int64 A3::SpatialEphFileComplex::InitExtEphFile(void)' : overloaded function differs only by return type from 'long A3::SpatialEphFileComplex::InitExtEphFile(void)'


    virtual long long int InitExtEphFile();

    //WARNING_FIX:
    //1>src\SpatialEphFileComplex.cpp(367): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data


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
inline const char* SpatialEphFileComplex::GetVersion() const
{
    return SPATIALEPHFILECOMPLEX_H;
}

// ============================================================================
}
// A3 namespace end
//------------------------------------------------------------------------------
#endif

