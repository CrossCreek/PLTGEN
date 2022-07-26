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
 * CoorTransEcfNwzF.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEcfNwzF.cpp

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : CoorTransNwzF

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  02/27/1998  JAM  Initial Release
//              V1.3-009  12/20/1999  JAM  implemented AutoUpdate base class
//                                         to support the Spatial and CoorTrans
//                                         class families
//              V2.0      04/29/2002  JAM  1st A3 library build

//  NOTES :

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

//=============================================================================
// required headers
#include "CoorTransEcfNwzF.h"
#include "VectorGeocentric.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
CoorTransEcfNwzF::CoorTransEcfNwzF( double geocentricLatitude, double longitude )
    : CoorTransBaseEcf( CoorSystem::NWZF ),
      m_geocentricLatitude( geocentricLatitude ),
      m_longitude( longitude )
{
    Set(geocentricLatitude, longitude);
    EIClrSet(POS_EI|VEL_EI|ACC_EI);
}

void CoorTransEcfNwzF::Set( double geocentricLatitude, double longitude )
{
    static const VectorCartesian ku( 0.0, 0.0, 1.0 );
    m_geocentricLatitude = geocentricLatitude;
    m_longitude          = longitude;
    VectorGeocentric refGeoC( geocentricLatitude, longitude, 1.0 );
    VectorCartesian zu( refGeoC );
    VectorCartesian wv = zu.CrossProduct( ku );
    VectorCartesian wu = wv.Unit();
    VectorCartesian nu = wu.CrossProduct( zu );
    std::copy(nu.begin(), nu.end(), Dcm().begin_column(0));
    std::copy(wu.begin(), wu.end(), Dcm().begin_column(1));
    std::copy(zu.begin(), zu.end(), Dcm().begin_column(2));
}

bool CoorTransEcfNwzF::operator==( const CoorTransEcfNwzF& coorTrans ) const
{
    return CoorTransBaseEcf::operator==( coorTrans )
           && m_geocentricLatitude == coorTrans.m_geocentricLatitude
           && m_longitude          == coorTrans.m_longitude;
    // && m_dcmNwzfToEcf       == coorTrans.m_dcmNwzfToEcf;
}

bool CoorTransEcfNwzF::operator!=( const CoorTransEcfNwzF& coorTrans ) const
{
    return !operator==( coorTrans );
}

// ============================================================================
}
// A3 namespace end
// ============================================================================
