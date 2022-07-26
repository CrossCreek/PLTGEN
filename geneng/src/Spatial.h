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
 * Spatial.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : Spatial.h

//  NAMESPACE : A3

//  SYSTEM    : SatTools

//  SUBSYSTEM : GenEngr

//  MODULE(s) : Spatial (abstract class)

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
//              V1.3-016  01/26/2000  JAM  added Update function
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//              support stringstream, newer compilers support it
//              Change: reordered data members
//              Reason: stops pointless initialized here warnings

//  NOTES :  The default reference system is ECI, use the alternate
//           constructor with the handle to a CoorTrans to change
//           the reference system

//  Contents derived from SE and SST Ver 1.0.

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  CLASSES

//  Name:       Spatial
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

//  Public Member:  Spatial()
//  Purpose:        default constructor
//  Return Type:    n/a
//  Return Value:   n/a
//  Argument Type:  void
//  Argument Value: n/a

//  Public Member:  Spatial( Handle< CoorTransBaseEci > hdlCoorTrans )
//  Purpose:        alternate constructor
//  Return Type:    n/a
//  Return Value:   n/a
//  Argument Type:  HandleConst< TimeJ2000 > hdlCoorTrans
//  Argument Value: handle to the coordinate transformation that is
//                  applied (if necessary) to a vector before returning it

//  Public Member:  virtual const VectorCartesian& GetPosition() = 0;
//  Public Member:  virtual const VectorCartesian& GetPosition() const = 0;
//  Purpose:        return const reference to the position vector
//  Return Type:    const VectorCartesian&
//  Return Value:   const reference to the position vector, units: nmi
//  Argument Type:  void
//  Argument Value: n/a

//  Public Member:  virtual const VectorCartesian& GetVelocity() = 0;
//  Public Member:  virtual const VectorCartesian& GetVelocity() const = 0;
//  Purpose:        return const reference to the position vector
//  Return Type:    const VectorCartesian&
//  Return Value:   const reference to the position vector, units: nmi & sec
//  Argument Type:  void
//  Argument Value: n/a

//  Public Member:  virtual double GetRadius();
//  Public Member:  virtual double GetRadius() const;
//  Purpose:        return radius
//  Return Type:    const VectorCartesian&
//  Return Value:   the radius, units: nmi
//  Argument Type:  void
//  Argument Value: n/a

//  Public Member:  virtual void Update();
//  Purpose:        update class values without explicitly calling GetPosition
//                  and GetVelocity
//  Return Type:    void
//  Return Value:   n/a
//  Argument Type:  void
//  Argument Value: n/a

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  OPERATORS

//  Name:           operator==( const Spatial& )
//  Purpose:        test for equality
//  Return Type:    bool
//  Return Value:   true if vectors match, otherwise false
//  Argument Type:  const Spatial&
//  Argument Value: object to be compared to

//  Name:           operator!=( const Spatial& )
//  Purpose:        test for inequality
//  Return Type:    bool
//  Return Value:   true if vectors don't match, otherwise false
//  Argument Type:  const Spatial&
//  Argument Value: object to be compared to

//  Name:           operator<<( ostream&, const Spatial& )
//  Purpose:        insertion operator, write the three elements of the vector
//  Return Type:    ostream &
//  Return Value:   ostream object pass as argument
//  Argument Type:  ostream &
//  Argument Value: output stream
//  Argument Type:  const Spatial&
//  Argument Value: object to be inserted into the stream

//  Name:           operator>>( istream&, Spatial& )
//  Purpose:        extraction operator, store three values from the
//                  stream in the vector
//  Return Type:    istream &
//  Return Value:   istream object pass as argument
//  Argument Type:  istream &
//  Argument Value: input stream
//  Argument Type:  Spatial&
//  Argument Value: object that stores three values from the stream

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



//****************************************************************************//

//=============================================================================
#ifndef SPATIAL_H
#define SPATIAL_H	"Spatial V4.3-009"

//------------------------------------------------------------------------------
// required headers
#include "AutoUpdate.h"
#include "Handle.h"
#include "CoorTransEciEci.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class Spatial : public AutoUpdate
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    Spatial();

    // Coordinate Transformation to be applied to position & velocity



    explicit Spatial( Handle< CoorTransBaseEci > hdlCoorTrans );

    Spatial( const Spatial& spatial );

    virtual ~Spatial();

    //----------------------------------------------------------------------
    // enumerated types to enable/disable Get functions
    // disabling a Get function may reduce run time
    // position is always enable
    // by default, velocity function is enabled;
    // by default, acceleration is disabled
    // if acceleration is enabled, velocity is automatically enabled
    // if velocity is disabled, acceleration is automatically disabled
    // enum VecType { POSITION, VELOCITY, ACCELERATION };

    inline void Enable( VecType vecType );
    inline void Disable( VecType vecType );
    inline bool IsEnabled( VecType vecType ) const;

    //----------------------------------------------------------------------
    // position, velocity, and acceleration access functions
    virtual const VectorCartesian& GetPosition();
    virtual const VectorCartesian& GetVelocity();
    virtual const VectorCartesian& GetAcceleration();

    virtual const VectorCartesian& GetPosition() const;
    virtual const VectorCartesian& GetVelocity() const;
    virtual const VectorCartesian& GetAcceleration() const;

    virtual double GetRadius();
    virtual double GetRadius() const;

    //----------------------------------------------------------------------
    // Returns true if the state of the spatial object is stale, the object
    // will automatically update itself the next time a non-const Get
    // function is called
    inline virtual bool IsStale() const;

    //----------------------------------------------------------------------
    // operators
    bool operator==( const Spatial& spatial ) const;
    bool operator!=( const Spatial& spatial ) const;

    //----------------------------------------------------------------------
    // misc functions
    virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Shared class family data members for position, velocity, and
    // acceleration.  Each derived class MUST set these values when
    // UpdateState() is called!!!
    VectorCartesian  m_vecPosition;
    VectorCartesian  m_vecVelocity;
    VectorCartesian  m_vecAcceleration;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // AutoUpdate calls this function when the state of the spatial object
    // needs to be recomputed.  Derived classes must implement this
    // function such that the base class UpdateState class is called and
    // then recompute is data members
    inline virtual void UpdateState();

    // AutoUpdata calls this function when an object that this class
    // depends on changes state
    inline virtual void CallBack( const AutoUpdate* pUpdatedObject );

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Handle to a coordination tranformation object to be applied to
    // position and velocity vectors.  If the target system is not ECI,
    // calling the GetAcceleration function, the derived class must decide
    // to use finite differences or throw an exception
    Handle< CoorTransBaseEci >  m_hdlCoorTrans;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Enabled flags
    bool m_bIsVelocityEnabled;
    bool m_bIsAccelerationEnabled;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Exception handling functions
    void ThrowStaleVector( const std::string& strFunctionName ) const;
    void ThrowNotEnabled( const std::string& strFunctionName ) const;
    void ThrowNotImplemented( const std::string& strFunctionName ) const;

    //--------------------------------------------------------------------------
private:
    static Handle< CoorTransEciEci > s_hdlCoorTransEciEci;
    bool m_bIsStale;
};

//==============================================================================
// inline functions
inline void
Spatial::UpdateState()
{
    m_bIsStale = false;
}

inline void
Spatial::CallBack( const AutoUpdate* ) // pUpdatedObject
{
    m_bIsStale = true;
}

inline bool
Spatial::IsStale() const
{
    return m_bIsStale;
}

//------------------------------------------------------------------------------
inline void Spatial::Enable( VecType vecType )
{
    if ( ACCELERATION == vecType && !m_bIsAccelerationEnabled ) {
        m_bIsAccelerationEnabled = true;
        m_bIsVelocityEnabled = true;
        m_bIsStale = true;
    } else if (VELOCITY == vecType && !m_bIsVelocityEnabled ) {
        m_bIsVelocityEnabled = true;
        m_bIsStale = true;
    }

    m_hdlCoorTrans->Enable( vecType );
}

inline void Spatial::Disable( VecType vecType )
{
    switch ( vecType ) {
        case POSITION:
            throw std::invalid_argument(
                "Spatial::Disable(VecType) VecType POSITION invalid");

        case VELOCITY:
            m_bIsVelocityEnabled = false;

        case ACCELERATION:
            m_bIsAccelerationEnabled = false;
    }
}

inline bool Spatial::IsEnabled( VecType vecType ) const
{
    if ( VELOCITY == vecType ) {
        return m_bIsVelocityEnabled;
    }

    return m_bIsAccelerationEnabled;
}

inline const char* Spatial::GetVersion() const
{
    return SPATIAL_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
