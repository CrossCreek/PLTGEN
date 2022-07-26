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
 * SpatialConstantEci.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : SpatialConstantEci.h

//  NAMESPACE : A3

//  SYSTEM    : SatTools

//  SUBSYSTEM : GenEngr

//  MODULE(s) : SpatialConstantEci

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.00-001  3/18/2001  JAM initial release
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :  The default reference system is ECI, use the alternate
//           constructor with the handle to a CoorTrans to change
//           the reference system

//  Contents derived from SE and SST Ver 1.0.



//****************************************************************************//

//==============================================================================
#ifndef SPATIALCONSTANTECI_H
#define SPATIALCONSTANTECI_H	"SpatialConstantEci Version 2.0"

//------------------------------------------------------------------------------
// required headers
#include "Spatial.h"
#include "VectorCartesian.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
class SpatialConstantEci : public Spatial
{
    //--------------------------------------------------------------------------
public:
    //----------------------------------------------------------------------
    // constructors & destructors
    SpatialConstantEci();

    SpatialConstantEci( const VectorCartesian& vecPosition,
                        const VectorCartesian& vecVelocity );

    SpatialConstantEci( const VectorCartesian& vecPosition,
                        const VectorCartesian& vecVelocity,
                        const VectorCartesian& vecAcceleration );



    explicit SpatialConstantEci( Handle< CoorTransBaseEci > hdlCoorTrans );

    SpatialConstantEci( Handle< CoorTransBaseEci > hdlCoorTrans,
                        const VectorCartesian& vecPosition,
                        const VectorCartesian& vecVelocity );

    SpatialConstantEci( Handle< CoorTransBaseEci > hdlCoorTrans,
                        const VectorCartesian& vecPosition,
                        const VectorCartesian& vecVelocity,
                        const VectorCartesian& vecAcceleration );

    virtual ~SpatialConstantEci();

    //----------------------------------------------------------------------
    // run control interface and reporting functions
    virtual std::istream& Initialize( std::istream& is );

    //----------------------------------------------------------------------
    // time, position & velocity access functions
    virtual double GetRadius() const;

    //----------------------------------------------------------------------
    // operators
    bool operator==( const SpatialConstantEci& spatial ) const;
    bool operator!=( const SpatialConstantEci& spatial ) const;

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
inline const char* SpatialConstantEci::GetVersion() const
{
    return SPATIALCONSTANTECI_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // SPATIALCONSTANTECI_H
