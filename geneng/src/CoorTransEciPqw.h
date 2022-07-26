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
 * CoorTransEciPqw.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransEciPqw.h

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

//              V4.5      2007-07-01  JAM  Made PQW to ECI dcm function external
//                                         to the CoorTransEciPqw class to
//                                         reduce overhead in some calculations

//  NOTES :

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

//=============================================================================
#ifndef COORTRANSECIPQW_H
#define COORTRANSECIPQW_H	"CoorTransEciPqw V4.3-009"

//-----------------------------------------------------------------------------
// required headers
#include "CoorTransBaseEci.h"
#include "HandleConst.h"
#include "TimeJ2000.h"
#include "EulerAnglesKeplerian.h"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
// ECI/PQW tranformations
// base system: ECI
// source system: PQW
class CoorTransEciPqw : public CoorTransBaseEci
{
    //-------------------------------------------------------------------------
public:

    explicit CoorTransEciPqw( const HandleConst< EulerAnglesKeplerian > hdlAngles );

    CoorTransEciPqw( const HandleConst< EulerAnglesKeplerian > hdlAngles,
                     const EulerAnglesKeplerian& eaRates );

    virtual bool IsStale() const;

    bool operator==( const CoorTransEciPqw& coorTrans ) const;
    bool operator!=( const CoorTransEciPqw& coorTrans ) const;

    const char* GetVersion() const;

protected:
    virtual void UpdateState();

private:
    HandleConst< EulerAnglesKeplerian > m_hdlAngles;

    EulerAnglesKeplerian m_eaRates;
    EulerAnglesKeplerian m_anglesOfLastTransformation;
};

//==============================================================================
inline bool
CoorTransEciPqw::IsStale() const
{
    return *m_hdlAngles != m_anglesOfLastTransformation
           || CoorTransBaseEci::IsStale();
}

inline const char* CoorTransEciPqw::GetVersion() const
{
    return COORTRANSECIPQW_H;
}

// ============================================================================
void dcmPqwToEci(const double &ra, const double &i, const double &ap,
                 MatrixFP<double> &dcm);

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif  // COORTRANSECIPQW_H
