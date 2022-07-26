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
 * anomaly.h
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
//  FUNCTIONS

//  Name:           trueToMeanAnomaly
//  Purpose:        convert true anomaly to mean anomaly
//  Return Type:    double
//  Return Value:   radians
//  Argument Type:  double
//  Argument Value: eccentricity
//  Argument Type:  double
//  Argument Value: true anomaly

//  Name:           meanToTrueAnomaly
//  Purpose:        convert mean anomaly to true anomaly
//  Return Type:    double
//  Return Value:   radians
//  Argument Type:  double
//  Argument Value: eccentricity
//  Argument Type:  double
//  Argument Value: mean anomaly

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  LEGACY COMMENTS
//  Anomaly.cc
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

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

#ifndef ANOMALY_H
#define ANOMALY_H   "anomaly V4.3-009"

// ============================================================================
// A3 namespace start
namespace A3
{

double anomaly(int icod, double eccen, double anminp);

inline double
trueToMeanAnomaly( double eccentricity, double trueAnomaly )
{
    return anomaly( 3, eccentricity, trueAnomaly );
}

inline double
meanToTrueAnomaly( double eccentricity, double meanAnomaly )
{
    return anomaly( 6, eccentricity, meanAnomaly );
}

}
// A3 namespace end
// ============================================================================

#endif
