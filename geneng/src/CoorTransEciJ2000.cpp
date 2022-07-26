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
 * CoorTransEciJ2000.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// ECI(TOD)-J2000 coordinate transformation class
// Refer to CoorTransEciJ2000.h for more info.
// =============================================================================
// required headers
#include "CoorTransEciJ2000.h"
#include "IERS96.h"

// ============================================================================
// A3 namespace start
namespace A3
{
//-----------------------------------------------------------------------------
CoorTransEciJ2000::CoorTransEciJ2000(HandleConst<TimeJ2000> hdlTime)
    : CoorTransBaseEci(CoorSystem::MOD), m_hdlTime(hdlTime)
{
    SetAngularVelocityZero();
    SetAngularAccelerationZero();
    EIClrSet(POS_EI|VEL_EI|ACC_EI);
    UpdateState();
}

//-----------------------------------------------------------------------------
bool
CoorTransEciJ2000::operator==( const CoorTransEciJ2000& coorTrans ) const
{
    return CoorTransBaseEci::operator==( coorTrans )
           && m_timeOfLastTransformation == coorTrans.m_timeOfLastTransformation
           && *m_hdlTime == *(coorTrans.m_hdlTime);
}

bool
CoorTransEciJ2000::operator!=( const CoorTransEciJ2000& coorTrans ) const
{
    return !operator==( coorTrans );
}

void
CoorTransEciJ2000::UpdateState()
{
    static MatrixFP<double> dcmJ2000Mod(3,3,MatrixOption::IDENTITY),
           dcmModTod(3,3,MatrixOption::IDENTITY);
    CoorTransBaseEci::UpdateState();
    m_timeOfLastTransformation = m_hdlTime.Ref();
    IERS96::dcmJ2000ToMod(m_timeOfLastTransformation, dcmJ2000Mod);
    IERS96::dcmModToTod(m_timeOfLastTransformation, dcmModTod);
    dcmModTod.Multiply(dcmJ2000Mod, Dcm());
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
