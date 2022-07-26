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
 * CoorTransEciMod.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEciMod.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciMod

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V4.5      2009-07-01  JAM  Initial release, based on IERS 96
//              V5.0      2010-01-18  JAM  deleted namespace constant
//                                         TIME_NOT_SET

//  NOTES :

//=============================================================================
#ifndef COORTRANSECIMOD_H
#define COORTRANSECIMOD_H	__FILE__ " " __TIME__

//-----------------------------------------------------------------------------
// required headers
#include "CoorTransBaseEci.h"
#include "HandleConst.h"
#include "TimeJ2000.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// ECI/MOD tranformations
// base system: ECI, TOD
// source system: MOD
class CoorTransEciMod : public CoorTransBaseEci
{
public:

    explicit CoorTransEciMod(HandleConst<TimeJ2000> hdlTime);

    bool operator==( const CoorTransEciMod& coorTrans ) const;
    bool operator!=( const CoorTransEciMod& coorTrans ) const;

    virtual bool IsStale() const;

    const char* GetVersion() const;

protected:
    virtual void UpdateState();

    const TimeJ2000& GetTimeOfLastTransformation() const;
    HandleConst< TimeJ2000 > m_hdlTime;
    TimeJ2000 m_timeOfLastTransformation;

private:
};

//-----------------------------------------------------------------------------
inline const TimeJ2000&
CoorTransEciMod::GetTimeOfLastTransformation() const
{
    return m_timeOfLastTransformation;
}

inline bool
CoorTransEciMod::IsStale() const
{
    return m_timeOfLastTransformation != m_hdlTime.Ref()
           || CoorTransBaseEci::IsStale();
}

inline const char* CoorTransEciMod::GetVersion() const
{
    return COORTRANSECIMOD_H;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // COORTRANSECIMOD_H
