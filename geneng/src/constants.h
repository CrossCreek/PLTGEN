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
 * constants.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : constants.h

//  NAMESPACE : A3

//  SYSTEM    : SATTOOLS

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V1.2-003  08/31/1999  JAM  migration to the new general
//                                         engineering library, this class
//                                         derived from fcntlib and SE
//              V1.3-003  12/20/1999  JAM  changed #includes to reduce
//                                         compile time overhead
//              V1.3-010  01/05/2000  JJO  add conversion constants for graphics
//              V1.3-016  01/26/2000  JAM  added more constants
//              V1.4      07/27/2000  JAM  moved from global to A3 namespace
//              V2.0      04/29/2002  JAM  1st A3 library build
//              V3.3-009  05/06/2004  GJK  Add MSec for use with Reporter
//				V3.6-001  03/24/2005  GM   Added MOD_JUL_EPOCH for use with Eng library
//				V3.6-001  05/25/2005   GM  Added HOURS_TO_RADIANS,
//                                         SECONDS_TO_RADIANS, GM_TO_LBS and
//                                         DAYS_TO_RADIANS for use with Pokey
//										   Atmosphere model. Use caution when
//										   using these variables as Pokey is not
//                                         well documented
//              V3.8-017  4/24/2006   GM   Solar pressure fix for integrator
//              V4.3-009  2007-12-10  JAM  removed V3.6 & V3.8 changes, pokey
//                                         removed from GenEngr
//              V4.5-001   2009-07-01 JAM  constants.cpp is renamed
//                                         constants.cxx and is #include
//                                         into initialization.cpp which
//                                         is used to control global
//                                         initialization order hence
//                                         constants.cxx must be excluded from
//                                         direct compilation
//                                         corrected value of solar radius
//                                         added mass, force, pressure conversion
//                                         establish switch for use of legacy or
//                                         SI units internally

//              V5.0    2010-06-01  JAM    Moved definitions to header, deleted
//                                         constants.cxx

//  NOTES : contains common constants

//          Contents derive from SE and SST Ver 1.0.

//          note: angular units are radians unless indicated otherwise
//                time units are seconds unless indicated otherwise
//                length units are as indicated for constaints

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//  CONSTANTS

//  Math Constants and Conversions
//  Name                Type    Units       Description
//  PI                  double
//  TWO_PI              double
//  FOUR_PI             double
//  HALF_PI             double
//  PI_SQUARED          double
//  RAD_TO_DEG          double  deg/rad
//  DEG_TO_RAD          double  rad/deg

//  DTA_SEC             double  sec         Terrestrial Time - Intl Atomic Time
//  DGA_SEC             double  sec         GPS Time - International Atomic Time
//  DJM_SEC             double  sec         Julian Date - Modified Julian Date
//  DJM_DAY             double  day         Julian Date - Modified Julian Date
//  JD_J2000_SEC        double  sec         Julian Date of Epoch J2000
//  JD_J2000_DAY        double  day         Julian Date of Epoch J2000
//  MJD_J2000_SEC       double  sec         Modified Julian Date of Epoch J2000
//  MJD_J2000_DAY       double  day         Modified Julian Date of Epoch J2000

//  MIN_TO_SEC          double  sec/min
//  SEC_TO_MIN          double  min/sec

//  HR_TO_MSEC          double  milliseconds/hr
//  HR_TO_SEC           double  sec/hr
//  SEC_TO_HR           double  hr/sec
//  HR_TO_MIN           double  min/hr
//  MIN_TO_HR           double  hr/min

//  DAY_TO_SEC          double  sec/day
//  SEC_TO_DAY          double  day/sec
//  DAY_TO_MIN          double  min/day
//  MIN_TO_DAY          double  day/min
//  DAY_TO_HR           double  hr/day
//  HR_TO_DAY           double  day/hr

//  FT_TO_NMI           double  nmi/ft
//  NMI_TO_FT           double  ft/nmi
//  NMI_TO_M            double  m/nmi
//  M_TO_NMI            double  nmi/m
//  NMI_TO_KM           double  km/nmi
//  KM_TO_NMI           double  nmi/km
//  FT_TO_M             double  m/ft
//  M_TO_FT             double  ft/m
//  KM_TO_M             double  m/km
//  M_TO_KM             double  km/m
//  CM_TO_NMI			double nmi/cm
//  NMI_TO_CM			double cm/nmi
//  Added for graphics

//  NMI_TO_INCH         double  inch/nmi
//  KM_TO_FT            double  ft/km
//  KM_TO_INCH          double  inch/km
//  M_TO_INCH           double  inch/m
//  FT_TO_KM            double  km/ft
//  FT_TO_INCH          double  inch/ft
//  INCH_TO_NMI         double  nmi/inch
//  INCH_TO_KM          double  km/inch
//  INCH_TO_M           double  m/inch
//  INCH_TO_FT          double  ft/inch

//  Physical Constants
//  Name                Type    Units       Description
//  EARTH_RADIUS_NMI    double  nmi         earth's equatorial radius
//  EARTH_RADIUS_KM     double  km
//  EARTH_RADIUS_KM     double  m

//
//  EARTH_RADIUS_SQ_NMI2
//                      double  nmi^2       earth's equatorial radius squared
//  EARTH_RADIUS_SQ_KM2 double  km^2
//  EARTH_RADIUS_SQ_M2  double  m^2

//  EARTH_FLATTENING_FACTOR
//                      double              earth's flattening factor

//  EARTH_ECCENTRICITY  double              earth's ellipsoidal eccentricity
//  EARTH_ECCENTRICITY_SQ
//                      double

//  EARTH_POLAR_RADIUS_NMI
//                      double  nmi         earth's polar radius
//  EARTH_RADIUS_KM
//                      double  km
//  EARTH_RADIUS_M
//                      double  m

//  EARTH_ROTATION_RATE double  rad/sec     earth's rotation rate

//  SIDEREAL_DAY        double  sec         sidereal day

//  EARTH_GRAV_CONST_NMI3_PER_S2
//                      double  nmi^3/s^2   earth's gravitational constant (MU)
//  EARTH_GRAV_CONST_KM3_PER_S2
//                      double  km^3/s^2
//  EARTH_GRAV_CONST_M3_PER_S2
//                      double  m^3/s^2

//  EARTH_J2_ZONAL      double              earth's 2nd zonal harmonic
//  EARTH_J3_ZONAL      double              earth's 3nd zonal harmonic
//  EARTH_J4 _ZONAL     double              earth's 4nd zonal harmonic
//  EARTH_J5 _ZONAL     double              earth's 5nd zonal harmonic

//  EARTH_MOON_MASS_RATIO
//                      double              earth moon mass ratio from JPL DE421

//  AU_NMI              double  nmi         astronomical unit
//  AU_KM               double  km
//  AU_M                double  m

//  SUN_EARTH_DISTANCE_NMI
//                      double  nmi         mean distance from earth to sun
//  SUN_EARTH_DISTANCE_KM
//                      double  km
//  SUN_EARTH_DISTANCE_M
//                      double  m

//  SUN_RADIUS_NMI      double  nmi         solar radius
//  SUN_RADIUS_KM       double  km
//  SUN_RADIUS_M        double  m

//  TROPICAL_YEAR       double  sec         tropical year
//  SIDEREAL_YEAR;      double  sec         sidereal year
//  SUN_SIDEREAL_MOTION double  rad/sec     sidereal mean motion
//  ANOMALISTIC_YEAR    double  sec         anomalistic year

//  SPEED_OF_LIGHT_NMI_PER_S
//                      double  nmi/s       speed of light
//  SPEED_OF_LIGHT_KM_PER_S
//                      double  km/s
//  SPEED_OF_LIGHT_M_PER_S
//                      double  m/s
//	SOL_PRES_AT_1AU_LBS_PER_FT2
//						double lbm/ft^2
//	SOL_PRES_AT_1AU_KG_PER_M2
//						double kg/m^2

//  BOLTZMANN_JOULES_PER_K
//                      double  joules/K    Boltzmann's constant
//  BOLTZMANN_DB_JOULES_PER_K
//                      double  (10 * log10(joules/K ))

//  GRAV_CONST_M3_PER_KG_S2
//                      double  m^3/kg/s^2  unverisal gravitational constant

//	MOD_JUL_EPOCH (NOT USED)
//						double				Modifed Julian Epoch

//  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//****************************************************************************//
#ifndef CONSTANTS_H
#define CONSTANTS_H   "Constants: V4.3-009"

// using stddef.h since Microsoft's cstddef is not in the std namespace
#include <stddef.h>

#define U_LEGACY 0
#define U_SI 1

#ifndef U_TYPE
#define U_TYPE U_LEGACY
#endif

// A3 namespace start
namespace A3
{
// math constants and conversions
const double PI         = 3.14159265358979323846;
const double TWO_PI     = 6.28318530717958647692;
const double FOUR_PI    = 12.5663706143591729538;
const double HALF_PI    = 1.57079632679489661923;
const double PI_SQUARED = 9.86960440108935861882;
const double RAD_TO_DEG = 57.2957795130823208768;
const double DEG_TO_RAD = 1.74532925199432957692e-2;

const double MIN_TO_SEC = 60.0;
const double SEC_TO_MIN = 1.66666666666666666667e-2;

const double HR_TO_MSEC = 3600000.0;
const double HR_TO_SEC  = 3600.0;
const double SEC_TO_HR  = 2.77777777777777777778e-4;
const double HR_TO_MIN  = 60.0;
const double MIN_TO_HR  = 1.66666666666666666667e-2;

const double DAY_TO_SEC = 86400.0;
const double SEC_TO_DAY = 1.15740740740740740741e-5;
const double DAY_TO_MIN = 1440.0;
const double MIN_TO_DAY = 6.94444444444444444444e-4;
const double DAY_TO_HR  = 24.0;
const double HR_TO_DAY  = 4.16666666666666666667e-2;

// Terrestrial Time - International Atomic Time
const double DTA_S = 32.184;;
// GPS Time - International Atomic Time
const double DGA_S = 19.0;
// Julian Date - Modified Julian Date
const double DJM_DAY = 2400000.5;
const double DJM_S = DJM_DAY * DAY_TO_SEC;
// Julian Date of Epoch J2000
const double JD_J2000_DAY = 2451545.0;
const double JD_J2000_S = JD_J2000_DAY * DAY_TO_SEC;
// Modified Julian Date of Epoch J2000
const double MJD_J2000_DAY = JD_J2000_DAY - DJM_DAY;
const double MJD_J2000_S = MJD_J2000_DAY * DAY_TO_SEC;
// Excel Date of Epoch J2000
const double EXCEL_J2000_DAY = 36526.5;

const double FT_TO_NMI  = 1.64578833693e-4;
const double NMI_TO_FT  = 6076.11548556;
const double NMI_TO_M   = 1852.0;
const double M_TO_NMI   = 5.39956803455723542117e-4;
const double NMI_TO_KM  = 1.852;
const double KM_TO_NMI  = 0.539956803455723542117;
const double FT_TO_M    = 0.304799999999436;
const double M_TO_FT    = 3.28083989501919;
const double KM_TO_M    = 1000.0;
const double M_TO_KM    = 0.001;
const double CM_TO_NMI  = 5.39956803455723542117e-6;
const double NMI_TO_CM  = 185200.0;

const double NMI_TO_INCH = 72913.38582672;
const double KM_TO_FT    = 3280.839895011919;
const double KM_TO_INCH  = 39370.07874023028;
const double M_TO_INCH   = 39.37007874023028;
const double FT_TO_KM    = 3.04799999999436e-4;
const double FT_TO_INCH  = 12.0;
const double INCH_TO_NMI = 1.37149028275e-5;
const double INCH_TO_KM  = 2.5399999999953e-5;
const double INCH_TO_M   = 2.5399999999953e-2;
const double INCH_TO_FT  = 8.33333333333333333333e-2;

const double SLUG_TO_KG = 1.459390e1;
const double KG_TO_SLUG = 6.852178e-2;
const double LBM_TO_KG  = 4.5359237e-1;
const double KG_TO_LBM  = 2.2046226;
const double G_TO_KG    = 1.0e-03;
const double KG_TO_G    = 1.0e3;

const double LBF_TO_N = 4.448222;
const double N_TO_LBF = 2.248089e-1;

const double PSF_TO_PA = 4.788026e1;
const double PA_TO_PSF = 2.088543e-2;
const double PSI_TO_PA = 6.894757e3;
const double PA_TO_PSI = 1.450377e-4;

// conversion to internal units
// if U_TYPE == U_SI then s, m, kg, N, Pa
// else if U_TYPE == U_LEGACY then s, nmi, slug, lbf, psi
#   if U_TYPE == U_SI
const double M_TO_LEN = 1.0;
const double KM_TO_LEN = KM_TO_M;
const double NMI_TO_LEN = NMI_TO_M;
const double KG_TO_MASS = 1.0;
const double SLUG_TO_MASS = SLUG_TO_KG;
const double LBM_TO_MASS = LBM_TO_KG;
const double N_TO_FORCE = 1.0;
const double LBF_TO_FORCE = LBF_TO_N;
const double PA_TO_PRESSURE = 1.0;
const double PSF_TO_PRESSURE = PSF_TO_PA;
const double PSI_TO_PRESSURE = PSI_TO_PA;
#   else
const double M_TO_LEN = M_TO_NMI;
const double KM_TO_LEN = KM_TO_NMI;
const double NMI_TO_LEN = 1.0;
const double KG_TO_MASS = KG_TO_SLUG;
const double SLUG_TO_MASS = 1.0;
const double LBM_TO_MASS = LBM_TO_KG*KG_TO_SLUG;
const double N_TO_FORCE = N_TO_LBF;
const double LBF_TO_FORCE = 1.0;
const double PA_TO_PRESSURE = PA_TO_PSI;
const double PSF_TO_PRESSURE = PSF_TO_PA*PA_TO_PSI;
const double PSI_TO_PRESSURE = 1.0;
#   endif
// -------------------------------------------------------------------------
// physical constants
// earth's equatorial radius

//const double EARTH_RADIUS_NMI = 3443.918; // nmi
const double EARTH_RADIUS_NMI = 3443.918089; // nmi
const double EARTH_RADIUS_KM  = 6378.137; // km
const double EARTH_RADIUS_M   = 6378137.; // m
#   if U_TYPE == U_SI
const double EARTH_RADIUS = EARTH_RADIUS_M;
#   else
const double EARTH_RADIUS = EARTH_RADIUS_NMI;
#   endif

// earth's equatorial radius squared
const double EARTH_RADIUS_SQ_NMI2 = 11860571.19;   // nmi^2
const double EARTH_RADIUS_SQ_KM2  = 40680631.59;   // km^2
const double EARTH_RADIUS_SQ_M2   = 40680631590769.0; // m^2
#   if U_TYPE == U_SI
const double EARTH_RADIUS_SQ = EARTH_RADIUS_SQ_M2;
#   else
const double EARTH_RADIUS_SQ = EARTH_RADIUS_SQ_NMI2;
#   endif

// earth's flattening factor
const double EARTH_FLATTENING_FACTOR = 0.00335281066475;

// earth's ellipsoidal eccentricity
const double EARTH_ECCENTRICITY    = 0.0818191908426;
const double EARTH_ECCENTRICITY_SQ = 0.00669437999014;

// earth's polar radius
const double EARTH_POLAR_RADIUS_NMI = 3432.371; // nmi
const double EARTH_POLAR_RADIUS_KM  = 6356.752; // km
const double EARTH_POLAR_RADIUS_M   = 6356752.; // m
#   if U_TYPE == U_SI
const double EARTH_POLAR_RADIUS = EARTH_POLAR_RADIUS_M;
#   else
const double EARTH_POLAR_RADIUS = EARTH_POLAR_RADIUS_NMI;
#   endif

// earth's polar rotation rate
const double EARTH_ROTATION_RATE_RAD_PER_S = 7.2921158553e-5;
const double SIDEREAL_DAY_S = 86164.0905309;
const double EARTH_ROTATION_RATE = EARTH_ROTATION_RATE_RAD_PER_S;
const double SIDEREAL_DAY = SIDEREAL_DAY_S;  // sec

// earth's gravitational constant
// Updated to EGM-96 values
const double EARTH_GRAV_CONST_NMI3_PER_S2 = 62750.15867743820;
const double EARTH_GRAV_CONST_KM3_PER_S2  = 398600.4415;
const double EARTH_GRAV_CONST_M3_PER_S2   = 3.986004415e14;
#   if U_TYPE == U_SI
const double EARTH_GRAV_CONST = EARTH_GRAV_CONST_M3_PER_S2;
#   else
const double EARTH_GRAV_CONST = EARTH_GRAV_CONST_NMI3_PER_S2;
#   endif

// unversial gravitational constant, NIST value
const double GRAV_CONST_M3_PER_KG_S2   = 6.67428E-11; // m^3 / (kg * s^2)

const double EARTH_GRAV_SEA_LEVEL_M_PER_S2 = -9.798287623;  //m/s^2
const double EARTH_GRAV_SEA_LEVEL_NMI_PER_S2 = -0.005290652064;  //nmi/s^2
#   if U_TYPE == U_SI
const double EARTH_GRAV_SEA_LEVEL = EARTH_GRAV_SEA_LEVEL_M_PER_S2;
#   else
const double EARTH_GRAV_SEA_LEVEL = EARTH_GRAV_SEA_LEVEL_NMI_PER_S2;
#   endif

// earth's 2nd zonal harmonic
const double EARTH_J2_ZONAL = 1082.63e-6;
const double EARTH_J2_ZONAL_SQUARED = 1.17209e-6;
// earth's 3nd zonal harmonic
const double EARTH_J3_ZONAL = -2.53215e-6;
// earth's 4nd zonal harmonic
const double EARTH_J4_ZONAL = -1.61099e-6;
// earth's 5nd zonal harmonic
const double EARTH_J5_ZONAL = -0.235786e-6;

// earth/moon mass ration from JPL DE421
const double EARTH_MOON_MASS_RATIO = 81.3005690699153;

// astronomical unit
const double AU_NMI = 8.0776388045e7;  // nmi
const double AU_KM  = 1.4959787066e8;  // km
const double AU_M   = 1.4959787066e11; // m
#   if U_TYPE == U_SI
const double AU = AU_M;
#   else
const double AU = AU_NMI;
#   endif

// mean distance from earth to sun
const double SUN_EARTH_DISTANCE_NMI = 8.077639e7;  // nmi
const double SUN_EARTH_DISTANCE_KM  = 1.495979e8;  // km
const double SUN_EARTH_DISTANCE_M   = 1.495979e11; // m
#   if U_TYPE == U_SI
const double SUN_EARTH_DISTANCE = SUN_EARTH_DISTANCE_M;
#   else
const double SUN_EARTH_DISTANCE = SUN_EARTH_DISTANCE_NMI;
#   endif

// solar radius
const double SUN_RADIUS_NMI = 375832.; // nmi
const double SUN_RADIUS_KM  = 696041.; // km
const double SUN_RADIUS_M   = 696041000.;  // m
#   if U_TYPE == U_SI
const double SUN_RADIUS = SUN_RADIUS_M;
#   else
const double SUN_RADIUS = SUN_RADIUS_NMI;
#   endif

// solar year
const double TROPICAL_YEAR       = 31556925.22;     // sec
const double SIDEREAL_YEAR       = 31558149.76;     // sec
const double SUN_SIDEREAL_MOTION = 1.990986593e-07; // rad/sec
const double ANOMALISTIC_YEAR    = 31558432.46;     // sec

// speed of light
const double SPEED_OF_LIGHT_NMI_PER_S = 161874.977; // nmi/s
const double SPEED_OF_LIGHT_KM_PER_S  = 299792.458; // km/s
const double SPEED_OF_LIGHT_M_PER_S   = 299792458.; // m/s
#   if U_TYPE == U_SI
const double SPEED_OF_LIGHT = SPEED_OF_LIGHT_M_PER_S;
#   else
const double SPEED_OF_LIGHT = SPEED_OF_LIGHT_NMI_PER_S;
#   endif

// Boltzmann's constant
const double BOLTZMANN_JOULES_PER_K = 1.380658e-23;   // joules/K
// 10*log10(joules/K)
const double BOLTZMANN_DB_JOULES_PER_K = -228.599138863;

// modified Julian Epoch
// const double MOD_JUL_EPOCH			  = 2400000.5;
} // A3
#endif // CONSTANTS_H
