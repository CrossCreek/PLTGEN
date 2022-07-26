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
 * CoorTransEciMod.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// ECI(TOD)-MOD coordinate transformation class
// Refer to CoorTransEciMod.h for more info.
// =============================================================================
// required headers
#include "CoorTransEciMod.h"
#include "IERS96.h"

// ============================================================================
// A3 namespace start
namespace A3
{
//-----------------------------------------------------------------------------
CoorTransEciMod::CoorTransEciMod(HandleConst<TimeJ2000> hdlTime)
    : CoorTransBaseEci(CoorSystem::MOD),
      m_hdlTime(hdlTime)
{
    SetAngularVelocityZero();
    SetAngularAccelerationZero();
    EIClrSet(POS_EI|VEL_EI|ACC_EI);
    UpdateState();
}

//-----------------------------------------------------------------------------
bool
CoorTransEciMod::operator==( const CoorTransEciMod& coorTrans ) const
{
    return CoorTransBaseEci::operator==( coorTrans )
           && m_timeOfLastTransformation == coorTrans.m_timeOfLastTransformation
           && *m_hdlTime == *(coorTrans.m_hdlTime);
}

bool
CoorTransEciMod::operator!=( const CoorTransEciMod& coorTrans ) const
{
    return !operator==( coorTrans );
}

void
CoorTransEciMod::UpdateState()
{
    CoorTransBaseEci::UpdateState();
    m_timeOfLastTransformation = m_hdlTime.Ref();
    IERS96::dcmModToTod(m_timeOfLastTransformation, Dcm());
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
