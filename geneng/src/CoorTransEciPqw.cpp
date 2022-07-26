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
 * CoorTransEciPqw.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEciPqw.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransEciPqw

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  02/27/1998  JAM  Initial Release
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: deleted unused variable ap
//              Reason: stops unused variable warning

//  NOTES :

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

//=============================================================================
// required headers
#include "CoorTransEciPqw.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
CoorTransEciPqw
::CoorTransEciPqw( const HandleConst< EulerAnglesKeplerian > hdlAngles  )
    : CoorTransBaseEci( CoorSystem::PQW ),
      m_hdlAngles( hdlAngles )
{
    SetAngularVelocityZero();
    SetAngularAccelerationZero();
    EIClrSet(POS_EI|VEL_EI|ACC_EI);
    UpdateState();
}

CoorTransEciPqw
::CoorTransEciPqw( const HandleConst< EulerAnglesKeplerian > hdlAngles,
                   const EulerAnglesKeplerian& eaRates  )
    : CoorTransBaseEci( CoorSystem::PQW ),
      m_hdlAngles( hdlAngles ),
      m_eaRates( eaRates )
{
    EIClrSet(POS_EI|VEL_EI|ACC_EI);
    UpdateState();
    double i   = m_hdlAngles->GetInclination();
    double ra  = m_hdlAngles->GetRAAscendingNode();
    // double ap  = m_hdlAngles->GetArgumentOfPeriapsis();
    double di  = m_eaRates.GetInclination();
    double dra = m_eaRates.GetRAAscendingNode();
    double dap = m_eaRates.GetArgumentOfPeriapsis();
    double cra = cos( ra );
    double sra = sin( ra );
    double ci  = cos( i );
    double si  = sin( i );
    double av0 = di * cra + dap * si * sra;
    double av1 = di * sra - dap * si * cra;
    double av2 = dra + dap * ci;
    double aa0 = -di * dra * sra + dap * di * ci * sra + dap * dra * si * cra;
    double aa1 =  di * dra * cra - dap * di * ci * sra + dap * dra * si * sra;
    double aa2 = -dap * di * si;
    SetAngularVelocity( VectorCartesian(av0, av1, av2) );
    SetAngularAcceleration( VectorCartesian(aa0, aa1, aa2) );
}

//-----------------------------------------------------------------------------
bool CoorTransEciPqw::operator==( const CoorTransEciPqw& coorTrans ) const
{
    return CoorTransBaseEci::operator==( coorTrans )
           && *m_hdlAngles                 == *(coorTrans.m_hdlAngles)
           && m_anglesOfLastTransformation == coorTrans.m_anglesOfLastTransformation;
}

bool CoorTransEciPqw::operator!=( const CoorTransEciPqw& coorTrans ) const
{
    return !operator==( coorTrans );
}

void CoorTransEciPqw::UpdateState()
{
    CoorTransBaseEci::UpdateState();
    dcmPqwToEci(m_hdlAngles->GetRAAscendingNode(),
                m_hdlAngles->GetInclination(),
                m_hdlAngles->GetArgumentOfPeriapsis(),
                Dcm() );
    m_anglesOfLastTransformation = *m_hdlAngles;
}

void dcmPqwToEci(const double &ra, const double &i, const double &ap,
                 MatrixFP<double> &dcm)
{
    const double cosRA(cos(ra));
    const double cosAP(cos(ap));
    const double cosI(cos(i));
    const double sinRA(sin(ra));
    const double sinAP(sin(ap));
    const double sinI(sin(i));

    if (!dcm.IsSquare(3)) {
        dcm.Resize(3,3);
    }

    CoorTransDcm::iterator a(dcm.begin());
    a[0] =  cosRA*cosAP - sinRA*sinAP*cosI;
    a[1] = -cosRA*sinAP - sinRA*cosAP*cosI;
    a[2] =  sinRA*sinI;
    a[3] =  sinRA*cosAP + cosRA*sinAP*cosI;
    a[4] = -sinRA*sinAP + cosRA*cosAP*cosI;
    a[5] = -cosRA*sinI;
    a[6] =  sinAP*sinI;
    a[7] =  cosAP*sinI;
    a[8] =  cosI;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
