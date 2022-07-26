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
 * CoorTransEciEcf.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// ECI-ECF coordinate transformation class
// Refer to CoorTransEciEcf.h for more info.
// =============================================================================
// required headers
#include "CoorTransEciEcf.h"

// ============================================================================
// A3 namespace start
namespace A3
{
//-----------------------------------------------------------------------------
CoorTransEciEcf::CoorTransEciEcf(HandleConst<TimeJ2000> hdlTime,
                                 GrAngTyp grAngTyp)
    : CoorTransBaseEci(CoorSystem::ECF),
      m_hdlTime(hdlTime),
      // m_timeOfLastTransformation(TimeJ2000()), default constructor sufficient
      m_grAngTyp(grAngTyp)
{
    SetAngularVelocity( VectorCartesian(0.0, 0.0, EARTH_ROTATION_RATE) );
    EIClrSet(POS_EI|VEL_EI|ACC_EI);
    UpdateState();
}

//-----------------------------------------------------------------------------
Angle CoorTransEciEcf::GetGreenwichAngle() const
{
    TimeJ2000 tmTT(*m_hdlTime);
    Angle ga;

    switch (m_grAngTyp) {
        case GMSA92U:
            ga = greenwichAngle(ttToUt1(tmTT));
            break;

        case GMSA92C:
            ga = greenwichAngle(ttToUtc(tmTT));
            break;

        case GMSA96:
            ga = IERS96::gmsa(tmTT);
            break;

        case GSA96:
        default:
            ga = IERS96::gsa(tmTT);
            break;
    }

    return ga;
}

//-----------------------------------------------------------------------------
bool
CoorTransEciEcf::operator==( const CoorTransEciEcf& coorTrans ) const
{
    return CoorTransBaseEci::operator==( coorTrans )
           && m_timeOfLastTransformation == coorTrans.m_timeOfLastTransformation
           && *m_hdlTime == *(coorTrans.m_hdlTime);
}

bool
CoorTransEciEcf::operator!=( const CoorTransEciEcf& coorTrans ) const
{
    return !operator==( coorTrans );
}

void
CoorTransEciEcf::UpdateState()
{
    CoorTransBaseEci::UpdateState();
    double theta = GetGreenwichAngle();
    double csTheta = cos(theta);
    double snTheta = sin(theta);
    CoorTransDcm &a(Dcm());
    a.IdentityThis();
    a[0][0] =  csTheta;
    a[0][1] = -snTheta;
    a[1][0] =  snTheta;
    a[1][1] =  csTheta;
    m_timeOfLastTransformation = m_hdlTime.Ref();
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
