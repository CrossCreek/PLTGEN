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
 * EigenValueVector.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : EigenValueVector.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : EigenValueVector

//  AUTHOR(s) : R. Harrison

//  UPDATES   : V4.1-001  06/01/2006   RMH  Initial Release

//  NOTES :     Finds and sorts the eigen values & vectors of symmentric matrix

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//
#include <math.h>
#ifndef EIGENVALUEVECTOR_H
#define EIGENVALUEVECTOR_H "EigenValueVector V4.1-001"
namespace A3
{
//===========================================================================
class EigenValueVector
{
public:

    void tred2(double **a, int n, double *d, double *e);

    void tqli(double *d, double *e, int n, double **z);

    void eigsrt(double *d, double **v, int n);

    inline double pythag(double a, double b);

    inline double sign(double a, double b);

    ~EigenValueVector();

private:

//	EigenValueVector();


};
//-----------------------------------------------------------------------------
inline double EigenValueVector::pythag(double a, double b)
{
    double absa,absb;
    absa=fabs(a);
    absb=fabs(b);

    if (absa > absb) {
        return absa*sqrt(1.0+pow((absb/absa),2));
    } else {
        return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+pow((absa/absb),2)));
    }
}
//-----------------------------------------------------------------------------
inline double EigenValueVector::sign(double a, double b)
{
    if (b>=0) {
        return fabs(a);
    } else {
        return -fabs(a);
    }
}
//-----------------------------------------------------------------------------
}//Namespace end
#endif


