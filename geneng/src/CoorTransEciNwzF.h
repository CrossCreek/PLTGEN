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
 * CoorTransEciNwzF.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEciNwzF.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciNwzF

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  02/27/1998  JAM  Initial Release
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V5.0      2010-06-01  JAM  deleted unused DCM

//  NOTES :

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

//=============================================================================
#ifndef COORTRANSECINWZF_H
#define COORTRANSECINWZF_H	__FILE__ " " __DATE__

//-----------------------------------------------------------------------------
// required headers
#include "CoorTransEciEcf.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// ECI/ECF tranformations
// base system: ECI
// source system: ECF
class CoorTransEciNwzF : public CoorTransBaseEci
{
    //-------------------------------------------------------------------------
public:
    CoorTransEciNwzF( HandleConst< TimeJ2000 > hdlTime,
                      double geocentricLatitude, double longitude );

    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            VectorCartesian& vecPositionBase );

    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            const VectorCartesian& vecVelocitySource,
            VectorCartesian& vecPositionBase,
            VectorCartesian& vecVelocityBase );

    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            const VectorCartesian& vecVelocitySource,
            const VectorCartesian& vecAccelerationSource,
            VectorCartesian& vecPositionBase,
            VectorCartesian& vecVelocityBase,
            VectorCartesian& vecAccelerationBase );

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              VectorCartesian& vecPositionSource );

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              const VectorCartesian& vecVelocityBase,
              VectorCartesian& vecPositionSource,
              VectorCartesian& vecVelocitySource );

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              const VectorCartesian& vecVelocityBase,
              const VectorCartesian& vecAccelerationBase,
              VectorCartesian& vecPositionSource,
              VectorCartesian& vecVelocitySource,
              VectorCartesian& vecAccelerationSource );

    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            VectorCartesian& vecPositionBase ) const;

    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            const VectorCartesian& vecVelocitySource,
            VectorCartesian& vecPositionBase,
            VectorCartesian& vecVelocityBase ) const;

    virtual void
    ToBase( const VectorCartesian& vecPositionSource,
            const VectorCartesian& vecVelocitySource,
            const VectorCartesian& vecAccelerationSource,
            VectorCartesian& vecPositionBase,
            VectorCartesian& vecVelocityBase,
            VectorCartesian& vecAccelerationBase ) const;

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              VectorCartesian& vecPositionSource ) const;

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              const VectorCartesian& vecVelocityBase,
              VectorCartesian& vecPositionSource,
              VectorCartesian& vecVelocitySource ) const;

    virtual void
    ToSource( const VectorCartesian& vecPositionBase,
              const VectorCartesian& vecVelocityBase,
              const VectorCartesian& vecAccelerationBase,
              VectorCartesian& vecPositionSource,
              VectorCartesian& vecVelocitySource,
              VectorCartesian& vecAccelerationSource ) const;

    virtual bool IsStale() const;

    bool operator==( const CoorTransEciNwzF& coorTrans ) const;
    bool operator!=( const CoorTransEciNwzF& coorTrans ) const;

    const char* GetVersion() const;

protected:
    virtual void UpdateState();

private:
    CoorTransEciEcf m_ctEciEcf;
    double m_geocentricLatitude;
    double m_longitude;
};

//==============================================================================
// inline functions
inline void
CoorTransEciNwzF::UpdateState()
{
    CoorTransBaseEci::UpdateState();
    m_ctEciEcf.Update();
}

inline bool
CoorTransEciNwzF::IsStale() const
{
    return m_ctEciEcf.IsStale() || CoorTrans::IsStale();
}

inline const char*
CoorTransEciNwzF::GetVersion() const
{
    return COORTRANSECINWZF_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // COORTRANSECINWZF_H

