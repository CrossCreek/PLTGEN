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
 * CoorTransDcm.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : CoorTransDcm.h

//  NAMESPACE : A3

//  SYSTEM    : SatSim 2000

//  SUBSYSTEM : SatTools

//  MODULE(s) : CoorTransDcm

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.0-001  1998-02-27  JAM  Initial Release
//              V1.1-000  2001-04-07  JAM  deleted unused functions
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.5      2009-07-01  JAM  switched to MatrixFP
//              V5.0      2010-07-14  JAM  change CoorTransDcm for typedef
//                                         to class to limit construction
//                                         to a 3x3 (prevents misuse)

//  NOTES : Direction cosine matrix class for coordinate transformations.

//==============================================================================
#ifndef COORTRANSDCM_H
#define COORTRANSDCM_H	"CoorTransDcm V5.0"

#include "MatrixFP.h"

// A3 namespace start
namespace A3
{
// typedef MatrixFP<double> CoorTransDcm;
class CoorTransDcm : public MatrixFP<double>
{
public:
    CoorTransDcm() : MatrixFP<double>(3, 3) {;}
    explicit CoorTransDcm(Option optn) : MatrixFP<double>(3, 3, optn) {;}
    CoorTransDcm(const CoorTransDcm& dcm) : MatrixFP<double>(dcm) {;}
    explicit CoorTransDcm(const MatrixFP<double>& dcm) : MatrixFP<double>(dcm)
    {
        if (!dcm.IsSquare(3)) {
            THROW_RUNTIME_ERROR("DCM must be 3x3 but it's not!");
        }
    }

    explicit CoorTransDcm(const Quaternion& q) : MatrixFP<double>(q) {;}
    explicit CoorTransDcm(const EulerElemRotation& eer) : MatrixFP(eer) {;}

    CoorTransDcm& operator=(const CoorTransDcm& dcm)
    {
        MatrixFP<double>::operator=(dcm);
        return *this;
    }

    CoorTransDcm& operator=(const MatrixFP<double>& dcm)
    {
        if (!dcm.IsSquare(3)) {
            THROW_RUNTIME_ERROR("DCM must be 3x3 but it's not!");
        }

        MatrixFP<double>::operator=(dcm);
        return *this;
    }
};
}
// A3 namespace end
#endif  // COORTRANSDCM_H
