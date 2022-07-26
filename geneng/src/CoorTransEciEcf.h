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
 * CoorTransEciEcf.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEciEcf.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciEcf

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  02/27/1998  JAM  Initial Release
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.5      2009-07-01  JAM  Added option to use IERS 1996
//                                         Greenwich Sideral Angle, note that
//                                         Greenwich Mean Sideral Angle is
//                                         still the default
//              V5.0      2010-01-18  JAM  deleted namespace constant
//                                         TIME_NOT_SET

//  NOTES :

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

//=============================================================================
#ifndef COORTRANSECIECF_H
#define COORTRANSECIECF_H	__FILE__ " " __DATE__

//-----------------------------------------------------------------------------
// required headers
#include "CoorTransBaseEci.h"
#include "PlanetEarth.h"
#include "IERS96.h"
#include "HandleConst.h"
#include "TimeJ2000.h"
#include "Angle.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// ECI/ECF tranformations
// base system: ECI, TOD
// source system: ECF, PBF
class CoorTransEciEcf : public CoorTransBaseEci
{
public:
    enum GrAngTyp {GMSA92C, GMSA92U, GMSA96, GSA96};

    CoorTransEciEcf(HandleConst<TimeJ2000> hdlTime,
                    GrAngTyp grAngTyp = GMSA96);

    bool operator==( const CoorTransEciEcf& coorTrans ) const;
    bool operator!=( const CoorTransEciEcf& coorTrans ) const;

    Angle GetGreenwichAngle() const;
    Angle RightAscensionToLongitude( Angle ra ) const;
    Angle LongitudeToRightAscension( Angle lon ) const;

    virtual bool IsStale() const;

protected:
    virtual void UpdateState();
    const TimeJ2000& GetTimeOfLastTransformation() const;

    HandleConst< TimeJ2000 > m_hdlTime;
    TimeJ2000 m_timeOfLastTransformation;
    GrAngTyp m_grAngTyp;

private:
};

//-----------------------------------------------------------------------------
inline Angle
CoorTransEciEcf::RightAscensionToLongitude( Angle ra ) const
{
    return ra - GetGreenwichAngle();
}

inline Angle
CoorTransEciEcf::LongitudeToRightAscension( Angle lon ) const
{
    return lon + GetGreenwichAngle();
}

inline const TimeJ2000&
CoorTransEciEcf::GetTimeOfLastTransformation() const
{
    return m_timeOfLastTransformation;
}

inline bool
CoorTransEciEcf::IsStale() const
{
    return m_timeOfLastTransformation != m_hdlTime.Ref()
           || CoorTransBaseEci::IsStale();
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // COORTRANSECIECF_H
