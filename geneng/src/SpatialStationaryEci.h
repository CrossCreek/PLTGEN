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
 * SpatialStationaryEci.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialStationaryEci.h

//  NAMESPACE : A3

//  SYSTEM    : SatTools

//  SUBSYSTEM : GenEngr

//  MODULE(s) : SpatialStationaryEci

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.00-001  10/27/1997  JAM initial release
//              V1.02-003  08/31/1999  JAM s/w moved to the new general
//                                         engineering library
//                                         added auto-update feature
//              V1.02-032  11/16/1999  JAM added const version of GetPosition,
//                                         GetVelocity
//                                         added new Update function to force
//                                         update position & velocity vectors
//                                         without calling the Get functions
//                                         added SetUpdateFrequency and
//                                         GetUpdateFrequecy functions to
//                                         update the vectors independent of
//                                         the clock frequency
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :  The default reference system is ECI, use the alternate
//           constructor with the handle to a CoorTrans to change
//           the reference system

//  Contents derived from SE and SST Ver 1.0.



//****************************************************************************//

//==============================================================================
#ifndef SPATIALSTATIONARYECI_H
#define SPATIALSTATIONARYECI_H	"SpatialStationaryEci Version 2.0"

//------------------------------------------------------------------------------
// required headers
#include "Spatial.h"
#include "VectorCartesian.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class SpatialStationaryEci : public Spatial
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    SpatialStationaryEci();



    explicit SpatialStationaryEci( const VectorCartesian& vecLocation );


    //WASL SpatialStationaryEci( Handle< CoorTransBaseEci > hdlCoorTrans );
    explicit SpatialStationaryEci( Handle< CoorTransBaseEci > hdlCoorTrans );

    SpatialStationaryEci( Handle< CoorTransBaseEci > hdlCoorTrans,
                          const VectorCartesian& vecLocation );

    virtual ~SpatialStationaryEci();

    //----------------------------------------------------------------------
    // run control interface and reporting functions
    virtual std::istream& Initialize( std::istream& is );

    //----------------------------------------------------------------------
    // time, position & velocity access functions
    virtual double GetRadius() const;

    //----------------------------------------------------------------------
    // operators
    bool operator==( const SpatialStationaryEci& spatial ) const;
    bool operator!=( const SpatialStationaryEci& spatial ) const;

    //----------------------------------------------------------------------
    // misc functions
    virtual const char* GetVersion() const;

    //--------------------------------------------------------------------------
protected:
    virtual void UpdateState();


    //--------------------------------------------------------------------------
private:
    VectorCartesian  m_vecEciPosition;
    VectorCartesian  m_vecEciVelocity;
    VectorCartesian  m_vecEciAcceleration;

    double m_radius;



};

//==============================================================================
// misc functions
inline const char* SpatialStationaryEci::GetVersion() const
{
    return SPATIALSTATIONARYECI_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // SPATIALSTATIONARYECI_H
