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
 * attenuation.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : attenuation.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  MODULE(s) : attenuation

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V4.5-001  2009-07-01  JAM  Minor formatting and integer casting
//                                         changes to eliminate compiler
//                                         warnings esp. new 64-bit
//                                         Note:
//                                         Rain attentuation models were based
//                                         on the old Crain rain models as
//                                         implemented in legacy '80s FORTRAN,
//                                         recommend replacing with the newer
//                                         ITU-R P.618
//                                         I've added the deprecated pragma to
//                                         as a reminder

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  Contents derived from SE and SST Ver 1.0.

//****************************************************************************//

#ifndef ATTENUATION_H
#define ATTENUATION_H "attenuation V2.0"

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
enum RainRegion { RR_A,  RR_B,  RR_C, RR_D1,
                  RR_D2, RR_D3, RR_E, RR_F,
                  RR_G,  RR_H,  RR_UNDEFINED
                };

extern const char *const sRainRegion[];

double atmosphericAttenuation( double carrierFrequency, // Hz
                               double stationAltitude,  // nmi
                               double elevationAngle ); // radians

double rainAttenuation( double rainRate,         // if set 0, function will calculate it
                        double availability,     // .98 TO .99999
                        double carrierFrequency, // Hz
                        double stationLatitude,  // radians
                        double stationAltitude,  // nmi
                        RainRegion rainRegion,   // A,B,C,D1,D2,D3,E,F,G,H
                        double elevationAngle ); // radians

double calcRainRate( double percOut, RainRegion rainRegion);

RainRegion stringToRainRegion( const char* strRainRegion );

// ============================================================================
}
// A3 namespace end
// ============================================================================
#endif
