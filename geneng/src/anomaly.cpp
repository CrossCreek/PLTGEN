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
 * anomaly.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : anomaly.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : anomaly

//  AUTHOR(s) : A. Kelch
//              J. McCormick
//              J. Moffit

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V1.4      07/27/2000  JAM  moved from global to A3 namespace
//                                         namespace directive no longer
//                                         applied to std
//                                         applied 80 character line limit
//                                         moved includes from header to source
//              V2.0      04/29/2002  JAM  1st A3 library build

//              V4.3-009  2007-12-10  JAM
//              compiler compatibility update, VS 6 through 2005 and gcc 3.4.2
//              Change: replaced strstream with stringstream
//              Reason: strstream is deprecated, older compilers didn't
//                      support stringstream, newer compilers support it
//              Change: added #include <stdexcept>
//              Reason: eliminates propagation of include order dependencies

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

#include "constants.h"
#include <sstream>
#include <stdexcept>

// using math.h since Microsoft's cmath is not in the std namespace
#include <math.h>

// ============================================================================
// A3 namespace start
namespace A3
{

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  LEGACY COMMENTS
//  This function returns a specified orbital anomaly given any other
//  orbital anomaly.  Input and output anomalies may be in either radians or
//  degrees.  All returned output anomalies will be in the range 0 to 2*pi
//  radians or 0 to 360 degrees.
//  This function may also be used to adjust any angle in radians to the
//  range 0 to 2*pi (icod = 0).
//
//  Based on a FORTRAN function by A. Kelch.
//
//  Calling Sequence:  double Anomaly = anomaly(icod, eccen, animp);
//
//  SYMBOL   I/O   TYPE    UNITS   DESCRIPTION
//  icod      I    int             Anomaly conversion type code:
//                     Code    Input   Output    Units
//                   0  any any radians
//                   1  true    eccen   radians
//                   2  eccen   mean    radians
//                   3  true    mean    radians
//                   4  mean    eccen   radians
//                   5  eccen   true    radians
//                   6  mean    true    radians
//                  -1  true    eccen   degrees
//                  -2  eccen   mean    degrees
//                  -3  true    mean    degrees
//                  -4  mean    eccen   degrees
//                  -5  eccen   true    degrees
//                  -6  mean    true    degrees
//  eccen     I    int             Orbital eccentricity (0 <= eccen < 1)
//  anminp    I   double   deg/rad Input anomaly angle

double anomaly(int icod, double eccen, double anminp)
{
    /* data initialization */
    static const double SIXPI  = 6.0 * PI;
    static const double DELMIN = 1.0e-7;
    static const int    NMAX   = 10;
    /* local variables */
    int kcod;
    double anom, term, temp;
    /* algorithm flags */
    static const int iflg[3][7] = {
        {0, 0, 0, 0,-1, 0,-1},
        {0, 1, 0, 1, 0,-1,-1},
        {0, 0, 1, 1, 0, 0, 0}
    };
    /* initialize internal code */
    kcod = icod < 0 ? -icod : icod;

    /* check validity of input code and eccentricity */
    if ( kcod > 6 ) {
        std::ostringstream stmMsg;
        stmMsg << "Function: double anomaly("
               << "int icod, double eccen, double anminp)"
               << std::endl
               << "Invalid icod, must be between -6 and 6 inclusive"
               << std::endl
               << "icod = " << icod
               << std::endl << std::ends;
        std::invalid_argument except( stmMsg.str() );
        throw except;
    }

    if ( eccen < 0.0 || eccen >= 1.0 ) {
        std::ostringstream stmMsg;
        stmMsg << "Function: double anomaly("
               << "int icod, double eccen, double anminp)"
               << std::endl
               << "Invalid eccen, must be 0 <= eccen < 1"
               << std::endl
               << "eccen = " << eccen
               << std::endl << std::ends;
        std::invalid_argument except( stmMsg.str() );
        throw except;
    }

    anom = anminp;

    if (icod < 0) {
        anom *= DEG_TO_RAD;
    }

    /* ensure the correct range for input anomaly */
    anom = fmod(anom + SIXPI, TWO_PI);

    /* if mean to eccentric anomaly conversion is required */
    if (iflg[0][kcod] != 0) {
        int n;
        n = 0;
        term = anom;
        anom = term + eccen*sin(term);

        do {
            n = n + 1;
            temp = anom;
            anom = anom-(anom-eccen*sin(anom)-term)/(1.0-eccen*cos(anom));
        } while ((n < NMAX) && fabs(anom - temp) > DELMIN);
    }

    /* if true to eccentric or eccentric to true conversion is required */
    if ( iflg[1][kcod] != 0 ) {
        if (iflg[1][kcod] > 0) {
            term = (1.0 - eccen)/(1.0 + eccen);
        } else {
            term = (1.0 + eccen)/(1.0 - eccen);
        }

        term = sqrt(term);
        temp = anom - PI;

        if (fabs(temp) < 1.0e-5)
            // corrected 980323
            // anom = temp/term - (temp >= 0.0 ? PI : PI);
        {
            anom = temp/term - (temp >= 0.0 ? PI : -PI);
        } else {
            anom = 2.0*atan(term*tan(0.5*anom));
        }
    }

    /* if eccentric to mean conversion is required */
    if (iflg[2][kcod] != 0) {
        anom = anom - eccen*sin(anom);
    }

    /* ensure the correct range for output anomaly */
    if (anom < 0.0) {
        anom = anom + TWO_PI;
    }

    if (icod < 0) {
        anom = anom/DEG_TO_RAD;
    }

    /* return output anomaly */
    return anom;
}

}
// A3 namespace end
// ============================================================================
