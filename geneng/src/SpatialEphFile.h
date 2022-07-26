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
 * SpatialEphFile.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialEphFile.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : Simulation

//  MODULE(s) : SpatialEphFile (abstract class)

//  AUTHOR(s) : J. McCormick, D. Bringenberg

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
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings

//              V4.4-001  2008-10-03  JAM  added ClearAndReserve fctn to
//                                         support initialization
//                                         deleted unused EphFileType

//              V4.5-001  2009-07-01  JAM
//              Replaced class Lagrangian interpolation with call to library
//              function for improved maintainability and consistency
//              Deleted unused variables

//  NOTES : Read an Ephemeris File and Interpolate Ephemeris Values if needed.



//****************************************************************************//

//==============================================================================
#ifndef SPATIALEPHFILE_H
#define SPATIALEPHFILE_H	"SpatialEphFileV4.3-009"

//------------------------------------------------------------------------------
// required headers
#include "SpatialTimeDep.h"
#include <vector>
#include <string>
#include <sstream>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class SpatialEphFile : public SpatialTimeDep
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // enumerations
    // enum EphFileType { UNDEFINED, SATSIM, OPL, TRAIL, PLANNING, HFNI };

    //----------------------------------------------------------------------
    // Position & Velocity in ECI



    explicit SpatialEphFile( HandleConst< TimeJ2000 > hdlTime );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphFile( HandleConst< TimeJ2000 > hdlTime,
                    Handle< CoorTransBaseEci > hdlCoorTrans );

    // Position & Velocity in ECI
    SpatialEphFile( HandleConst< TimeJ2000 > hdlTime,
                    const std::string& strFileNm );

    // Coordinate Transformation to be applied to position & velocity
    SpatialEphFile( HandleConst< TimeJ2000 > hdlTime,
                    Handle< CoorTransBaseEci > hdlCoorTrans,
                    const std::string& strFileNm );

    virtual ~SpatialEphFile();

    //----------------------------------------------------------------------
    // run control interface and reporting functions
    virtual std::istream& Initialize( std::istream& is );

    //----------------------------------------------------------------------
    // identification functions
    inline virtual const std::string& GetEphFileName() const;

    //----------------------------------------------------------------------
    // time, position & velocity access functions
    // Using base class implementation

    //----------------------------------------------------------------------
    inline virtual TimeJ2000   GetFirstTime() const;
    inline virtual TimeJ2000   GetLastTime() const;
    inline virtual double  GetMinimumTimeStepSeconds() const;
    inline virtual double  GetMaximumTimeStepSeconds() const;

    //----------------------------------------------------------------------
    // operators
    bool operator==( const SpatialEphFile& spatial ) const;
    bool operator!=( const SpatialEphFile& spatial ) const;

    //----------------------------------------------------------------------
    // file functions

    //WARNING_FIX:
    //1>src\SpatialEphFileComplex.cpp(126): error C2556: '__int64 A3::SpatialEphFileComplex::InitExtEphFile(void)' : overloaded function differs only by return type from 'long A3::SpatialEphFileComplex::InitExtEphFile(void)'


    virtual long long int InitExtEphFile() = 0;

    //WARNING_FIX:
    //1>src\SpatialEphFileComplex.cpp(367): warning C4244: 'return' : conversion from '__int64' to 'long', possible loss of data


    virtual long long int ReadExtEphFile() = 0;

    //----------------------------------------------------------------------
    // ephemeris interpolation functions
    inline int GetOrderInterpolation() const;
    inline bool SetOrderInterpolation(const int order);

    //----------------------------------------------------------------------
    // misc functions
    inline void SetEphFileName( const std::string& strFileName );

    inline virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    void ClearAndReserve();
    virtual void UpdateState();
    void Interpolate( double timeSec );
    static void ThrowBadInterpolate( int error );

    std::vector< TimeJ2000 >           m_vTime;
    std::vector< TimeJ2000 >::const_iterator m_iTimeHint;
    bool bDoHintInit;

    std::vector< VectorCartesian > m_vVecPosition;
    std::vector< VectorCartesian > m_vVecVelocity;

    std::string   m_strFileName;
    size_t   m_numItems;
    long int m_index;

    //WARNING_FIX:
    //1>src\SpatialEphFilePlanningSS.cpp(170): warning C4244: '=' : conversion from 'std::streamoff' to 'long', possible loss of data

    long long int m_offset;  // continue reading file here (need for paging)???

    static const int m_MAXORDER;
    // m_MAXORDER: Temporary limit???

    //--------------------------------------------------------------------------
private:
    VectorCartesian m_vecEciPosition;
    VectorCartesian m_vecEciVelocity;
    int      m_order;
};

//==============================================================================
// inline functions
//------------------------------------------------------------------------------
// time, position & velocity access functions
// Using base class implementation

//------------------------------------------------------------------------------

inline TimeJ2000 SpatialEphFile::GetFirstTime() const
{
    return m_vTime[0];
}

inline TimeJ2000 SpatialEphFile::GetLastTime() const
{
    return m_vTime.back();
}

inline double SpatialEphFile::GetMinimumTimeStepSeconds() const
{
    if ( m_vTime.size() <= 1 ) {
        std::stringstream stmMsg;
        stmMsg << "Exception thrown by\n"
               << "SpatialEphFile::GetMinimumTimeStepSeconds() const\n"
               << "Ephemeris vector size is too small\n"
               << "size = " << m_vTime.size() << std::ends;
        std::runtime_error except( stmMsg.str() );
        throw except;
    }

    double thisTime = GetFirstTime();
    double lastTime = GetLastTime();
    double step = lastTime - thisTime;

    for (unsigned int i = 1; i < m_vTime.size() - 1; i++) {
        double prevTime;
        double thiep;
        prevTime = thisTime;
        thisTime = m_vTime[i];
        thiep = thisTime - prevTime;

        if (thiep > 0.0 && thiep < step) {
            step = thiep;
        }
    }

    return step;
}

inline double SpatialEphFile::GetMaximumTimeStepSeconds() const
{
    if ( m_vTime.size() <= 1 ) {
        std::stringstream stmMsg;
        stmMsg << "Exception thrown by\n"
               << "SpatialEphFile::GetMaximumTimeStepSeconds() const\n"
               << "Ephemeris vector size is too small\n"
               << "size = " << m_vTime.size() << std::ends;
        std::runtime_error except( stmMsg.str() );
        throw except;
    }

    double thisTime = GetFirstTime();
    double nextTime = m_vTime[1];
    double step = nextTime - thisTime;

    for (unsigned int i = 1; i < m_vTime.size() - 1; i++) {
        double prevTime;
        double thiep;
        prevTime = thisTime;
        thisTime = m_vTime[i];
        thiep = thisTime - prevTime;

        if (thiep > 0.0 && thiep > step) {
            step = thiep;
        }
    }

    return step;
}

//------------------------------------------------------------------------------
// identification functions
inline const std::string& SpatialEphFile::GetEphFileName() const
{
    return m_strFileName;
}

//------------------------------------------------------------------------------
// ephemeris interpolation functions
inline int SpatialEphFile::GetOrderInterpolation() const
{
    return m_order;
}

inline bool SpatialEphFile::SetOrderInterpolation(const int order)
{
    if (order < 2 || order % 2) {
        return false;
    }

//    if (order > m_arTmPosVel.GetSize()) return false; // set before read???
    if (order > m_MAXORDER) {
        return false;    // Temporary limit???
    }

    m_order = order;
    return true;
}

//------------------------------------------------------------------------------
// misc functions
inline const char* SpatialEphFile::GetVersion() const
{
    return SPATIALEPHFILE_H;
}

inline void SpatialEphFile::SetEphFileName( const std::string& strFileName )
{
    m_strFileName = strFileName;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
