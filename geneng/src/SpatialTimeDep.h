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
 * SpatialTimeDep.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialTimeDep.h

//  NAMESPACE : A3

//  SYSTEM    : SatTools

//  SUBSYSTEM : GenEngr

//  MODULE(s) : SpatialTimeDep (abstract class)

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.00-001  10/27/1997  JAM  initial release
//              V1.02-003  08/31/1999  JAM  s/w moved to the new general
//                                          engineering library
//                                          added auto-update feature
//              V1.02-032  11/16/1999  JAM  added const version of GetPosition,
//                                          GetVelocity
//                                          added new Update function to force
//                                          update position & velocity vectors
//                                          without calling the Get functions
//                                          added SetUpdateFrequency and
//                                          GetUpdateFrequecy functions to
//                                          update the vectors independent of
//                                          the clock frequency
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V1.3-016  01/26/2000  JAM  added call to Spatial in update
//                                         function
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V5.0      2010-01-18  JAM  deleted namespace constant
//                                         TIME_NOT_SET

//  NOTES :  The default reference system is ECI, use the alternate
//           constructor with the handle to a CoorTrans to change
//           the reference system

//  Contents derived from SE and SST Ver 1.0.

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  CLASSES

//  Name:       SpatialTimeDep
//  Purpose:    Abstract base class for the spatial class family.
//              This class provides a consistent interface for retrieving the
//              position & velocity of objects.
//              The specific algorithms for computing the position
//              & velocty vectors are implemented in the subclasses.
//              This class family includes an auto-update feature.

//              The constructors must accepts the parameters that are required
//              to initialize the vector propagation algorithms.
//              Required parameters that changed during program execution are
//              accessed through a handle that is passed to the constructor.
//              This approach permits a consistent interface for accessing
//              vectors that is independent of the algorithms used to compute
//              them.  The use of handles for accessing changing required
//              parameters provides a memory safe way of managing these
//              parameters with little overhead.

//              Future implementation will include the acceleration vector.

//  Public Member:  SpatialTimeDep( HandleConst< TimeJ2000 > hdlTime )
//  Purpose:        alternate constructor
//  Return Type:    n/a
//  Return Value:   n/a
//  Argument Type:  HandleConst< TimeJ2000 > hdlTime
//  Argument Value: handle to the time parameter used to compute the
//                  current vector values

//  Public Member:  SpatialTimeDep( HandleConst< TimeJ2000 > hdlTime,
//                           Handle< CoorTransBaseEci > hdlCoorTrans )
//  Purpose:        alternate constructor
//  Return Type:    n/a
//  Return Value:   n/a
//  Argument Type:  HandleConst< TimeJ2000 > hdlTime
//  Argument Value: handle to the time parameter used to compute the
//                  current vector values
//  Argument Type:  HandleConst< TimeJ2000 > hdlCoorTrans
//  Argument Value: handle to the coordinate transformation that is
//                  applied (if necessary) to a vector before returning it

//  Public Member:  HandleConst< TimeJ2000 > GetHdlConstTime() const;
//  Purpose:        get handle to spatial time
//  Return Type:    HandleConst< TimeJ2000 >
//  Return Value:   const handle to time
//  Argument Type:  void
//  Argument Value: n/a

//  Public Member:  TimeJ2000 GetTime() const;
//  Purpose:        get current spatial time
//  Return Type:    TimeJ2000
//  Return Value:   time
//  Argument Type:  void
//  Argument Value: n/a

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



//****************************************************************************//

//=============================================================================
#ifndef SPATIALTIMEDEP_H
#define SPATIALTIMEDEP_H __FILE__ " " __DATE__

//------------------------------------------------------------------------------
// required headers
#include "Spatial.h"
#include "HandleConst.h"
#include "TimeJ2000.h"

// #include <map>

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class SpatialTimeDep : public Spatial
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    // Position & Velocity in ECI



    explicit SpatialTimeDep( HandleConst< TimeJ2000 > hdlTime );

    // Coordinate Transformation to be applied to position & velocity
    SpatialTimeDep( HandleConst< TimeJ2000 > hdlTime,
                    Handle< CoorTransBaseEci > hdlCoorTrans );

    virtual ~SpatialTimeDep();

    //----------------------------------------------------------------------
    // run control interface and reporting functions (to be deleted)
    // virtual std::istream& Initialize( std::istream& is ) = 0;

    //----------------------------------------------------------------------
    // Returns true if the state of the spatial object is stale, the object
    // will automatically update itself the next time a non-const Get
    // function is called
    inline virtual bool IsStale() const;

    //----------------------------------------------------------------------
    // returns a handle to the time object
    inline HandleConst< TimeJ2000 > GetHdlConstTime() const;
    // returns the current time
    inline TimeJ2000 GetTime() const;

    //----------------------------------------------------------------------
    // sets/gets the delta time use for finite difference calculations
    // typically acceleration (but not always)
    // must not be zero
    inline void SetFDDT( double seconds );
    inline double GetFDDT() const;

    //----------------------------------------------------------------------
    // operators
    bool operator==( const SpatialTimeDep& spatial ) const;
    bool operator!=( const SpatialTimeDep& spatial ) const;

    //----------------------------------------------------------------------
    // misc functions
    virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    inline virtual void UpdateState();

    //--------------------------------------------------------------------------
private:
    HandleConst< TimeJ2000 >  m_hdlTime;
    TimeJ2000 m_lastUpdate;
    double    m_dtFD; // seconds, delta time use for finite difference
    // calculations
};

//==============================================================================
inline bool SpatialTimeDep::IsStale() const
{
    return Spatial::IsStale() || *m_hdlTime != m_lastUpdate;
}

inline HandleConst< TimeJ2000 > SpatialTimeDep::GetHdlConstTime() const
{
    return m_hdlTime;
}

inline TimeJ2000 SpatialTimeDep::GetTime() const
{
    return *m_hdlTime;
}

inline void SpatialTimeDep::UpdateState()
{
    Spatial::UpdateState();
    m_lastUpdate = *m_hdlTime;
}

inline void SpatialTimeDep::SetFDDT( double seconds )
{
    if ( 0.0 == m_dtFD ) {
        throw std::runtime_error("SetFDDT(double seconds): must not be zero!");
    }

    m_dtFD = seconds;
}

inline double SpatialTimeDep::GetFDDT() const
{
    return m_dtFD;
}

//------------------------------------------------------------------------------
// misc functions
inline const char* SpatialTimeDep::GetVersion() const
{
    return SPATIALTIMEDEP_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
