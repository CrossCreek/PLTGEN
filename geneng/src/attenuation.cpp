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
 * attenuation.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//


//  FILE      : attenuation.cpp

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
//  Contents derived from SE and SST Ver 1.0

//****************************************************************************//
// using math.h in lieu of cmath since VC++ doesn't put
// cmath in the std namespace
#include <math.h>

#include <string.h>
#include <stdexcept>

#include "attenuation.h"
#include "constants.h"
#include "interpolLagrangePoly.h"

#ifdef _MSC_VER
#   pragma deprecated(atmosphericAttenuation, rainAttenuation)
#endif

// ============================================================================
// A3 namespace start
namespace A3
{
// ============================================================================
const char *const sRainRegion[] = {
    "A",  "B",  "C", "D1",
    "D2", "D3", "E", "F",
    "G",  "H",  "undefined"
};

//=======================================================================
// prototypes of supporting functions
double surfaceProjectedAttenuation( double rainRate, double projSurfPathLength,
                                    double alpha, double beta, double height0DegIso,
                                    double stationAltitude);

void calcRainAttenuationCoeff(double& alpha, double& beta, double freq);

void projectedSurfacePathLength( double& projSurfPathLength,
                                 double& adjProjSurfPathLength, double& adjRainRate,
                                 double& phi, double rainRate,
                                 double rainRateRatio, double percOutage,
                                 double height0DegIso, double elevationAngle,
                                 double stationAltitude, RainRegion rainRegion,
                                 double commEffectiveEarthRadius );

double height0DegIsotherm( double percOutage, double stationLatitude);

//=======================================================================
// reminder, don't use a for loop to resolve an enumeration from a
// string since this approach requires the promotion of a int
// to an enum which may pass the compilation but is undefined by
// the language standard
RainRegion stringToRainRegion( const char* strRainRegion )
{
    RainRegion rainRegion = RR_UNDEFINED;
    size_t nChar(strlen( strRainRegion ));

    if ( nChar == 1 || nChar == 2 ) {
        switch ( strRainRegion[0] ) {
            case 'A':
                rainRegion = RR_A;
                break;

            case 'B':
                rainRegion = RR_B;
                break;

            case 'C':
                rainRegion = RR_C;
                break;

            case 'D':
                if ( nChar == 2 ) {
                    switch ( strRainRegion[1] ) {
                        case '1':
                            rainRegion = RR_D1;
                            break;

                        case '2':
                            rainRegion = RR_D2;
                            break;

                        case '3':
                            rainRegion = RR_D3;
                    }
                }

                break;

            case 'E':
                rainRegion = RR_E;
                break;

            case 'F':
                rainRegion = RR_F;
                break;

            case 'G':
                rainRegion = RR_G;
                break;

            case 'H':
                rainRegion = RR_H;
        }
    }

    return rainRegion;
}

//=======================================================================
// ** atmospheric attenuation function
// converted from Comm Section and QUARC FORTRAN codes to C
// ************************* inputs *************************************
//           carrierfrequency  : carrier frequency (hz)
//           stationaltitude   : station stationAltitude (in nmi)
//           elevationangle    : elevation angle (radians)
// ************************* output *************************************
//           attenuation  : atmospheric attenuationuation in db
// ************************* locals *************************************
double atmosphericAttenuation( double carrierFrequency,
                               double stationAltitude,
                               double elevationAngle )
{
    // static const double EFFECTIVE_HEIGHT = 8.06 * KM_TO_NMI;
    static const double RADRAT = 790.3226;
    static const int NAT = 19;
    // ------------------------------------------------------------------------
    /***
    static double FRQTBL[] =
        { 1.0,  2.0,  4.0,  6.0,  12.0, 15.0, 20.0, 21.7,
          22.7, 25.0, 30.0, 41.0, 54.0, 59.5, 61.5, 66.0,
          84.5, 94.0, 100.0 };

    static double ZENTBL[] =
        { 0.0225, 0.0253, 0.0292, 0.0345, 0.061, 0.08,  0.26,  0.48,
          0.48,   0.245,  0.2,    0.37,   9.0,   242.0, 242.0, 6.2,
          0.7,    0.89,   1.06 };

    static double O2TBL[] =
        { 0.005,  0.006,  0.0065, 0.0067, 0.0071, 0.0078, 0.009,  0.00955,
          0.01,   0.0109, 0.0143, 0.0436, 3.16,   15.42,  14.86,  0.794,
          0.052,  0.0452, 0.0466 };

    static double H2OTBL[] =
        { 5.0E-5, 1.5E-4, 5.0E-4, 1.5E-3, 0.0058, 0.0150, 0.0760, 0.1560,
          0.1610, 0.1096, 0.0592, 0.0575, 0.0912, 0.1070, 0.1150, 0.1380,
          0.2230, 0.2692, 0.3076 };
    ***/
    // natural log of LOGFRQTBL table
    static const double LOGFRQTBL[] = {
        0.000000000, 0.693147181, 1.386294361, 1.791759469,
        2.484906650, 2.708050201, 2.995732274, 3.077312261,
        3.122364924, 3.218875825, 3.401197382, 3.713572067,
        3.988984047, 4.085976313, 4.119037175, 4.189654742,
        4.436751534, 4.543294782, 4.605170186
    };
    // natural log of LOGZENTBL table
    static const double LOGZENTBL[] = {
        -3.794239970, -3.676950883, -3.533586570, -3.366795955,
        -2.796881415, -2.525728644, -1.347073648, -0.733969175,
        -0.733969175, -1.406497068, -1.609437912, -0.994252273,
        2.197224577,  5.488937726,  5.488937726,  1.824549292,
        -0.356674944, -0.116533816,  0.058268908
    };
    // natural log of LOGO2TBL table
    static const double LOGO2TBL[] = {
        -5.298317367, -5.115995810, -5.035953102, -5.005647753,
        -4.947660495, -4.853631545, -4.710530702, -4.651214124,
        -4.605170186, -4.518992490, -4.247495742, -3.132698129,
        1.150572028,  2.735665368,  2.698673039, -0.230671818,
        -2.956511560, -3.096658192, -3.066154738
    };
    // natural log of LOGH20TBL table
    static const double LOGH20TBL[] = {
        -9.903487553, -8.804875264, -7.600902460, -6.502290171,
        -5.149897361, -4.199705078, -2.577021939, -1.857899272,
        -1.826350914, -2.210917904, -2.826833737, -2.855970331,
        -2.394700382, -2.234926445, -2.162823151, -1.980501594,
        -1.500583508, -1.312300681, -1.178955041
    };
    // ------------------------------------------------------------------------
    double attenuation, sinElevAngle;
    double frequencyGHz, logFreq, zenatn, o2atn, h2oatn;
    double sclfac, x2, pts, aveden;
    // **********************************************************************
    // CONVERT FROM (HZ) TO (GHZ)
    frequencyGHz = carrierFrequency * 1.E-9;
    // FIND ZENITH ATTENTUATION (10)
    logFreq = log( frequencyGHz );
    // zenatn = newtonInterpolate3Ext( logFreq, NAT, LOGFRQTBL, LOGZENTBL );
    interpolLagrangePoly( LOGFRQTBL, LOGFRQTBL+NAT, LOGZENTBL, 3, logFreq, zenatn );
    zenatn=exp( zenatn );
    // FIND THE ATTENTUATION COEFFICIENTS FOR O2 AND H2O
    // o2atn = newtonInterpolate3Ext( logFreq, NAT, LOGFRQTBL, LOGO2TBL );
    interpolLagrangePoly( LOGFRQTBL, LOGFRQTBL+NAT, LOGO2TBL, 3, logFreq, o2atn );
    o2atn = exp(o2atn);
    // h2oatn = newtonInterpolate3Ext( logFreq, NAT, LOGFRQTBL, LOGH20TBL );
    interpolLagrangePoly( LOGFRQTBL, LOGFRQTBL+NAT, LOGH20TBL, 3, logFreq, h2oatn );
    h2oatn = exp(h2oatn);
    // CALCULATE THE elevation ANGLE SCALING FACTOR
    sinElevAngle = sin(elevationAngle);
    sclfac = -RADRAT * sinElevAngle
             + sqrt( RADRAT * ( RADRAT * sinElevAngle * sinElevAngle
                                + 2.0) + 1.0 );
    // CALCULATE THE EFFECTIVE PATH LENGTH FROM GROUND 0 TO STATION HEIGHT
    x2 = EARTH_RADIUS_NMI * sinElevAngle;
    pts = -x2 + sqrt( x2 * x2 + stationAltitude
                      * ( stationAltitude + EARTH_RADIUS_NMI + EARTH_RADIUS_NMI ));
    // CALCULATE THE AVEAGE FRACTIONAL PRESSURE/DENSITY ALONG PATH pts
    //      ( PRESSURE HALVES EVERY 5.6 KM )
    aveden = ( pow( 0.5, stationAltitude/3.02376 ) + 1.0 ) * 0.5;
    // CALCULATE THE RESULTING ATTENUATION
    attenuation = zenatn * sclfac
                  - aveden * pts * NMI_TO_KM * ( o2atn + h2oatn );
    return attenuation;
}

//=======================================================================
// ** RAIN ATTENUATION ROUTINE
//
// ************************* INPUTS *************************************
//           freqb  : CARRIER FREQUENCY (HZ)
//           stalat : STATION LATITUDE (radians NORTH OR SOUTH)
//           stationAltitude : STATION HEIGHT (IN nmi)
//           stargn : STATION RAIN REGION (NUMERIC CODE)
//           rpi    : RAIN RATE (IN MM/HR) <IF NON ZERO WILL DETERMIN rp>
//           elev   : ELEVATION ANGLE (radians)
//           avail  : AVAILABILITY (.98 TO .99999)
//   NOTE: avail AND stargn ARE STILL REQUIRED EVEN IF rpi IS SPECIFIED
// **********************************************************************
//
//   OUTPUTS:  atten : RAIN ATTENUATION IN DB
//             rp    : RAIN RATE (IN MM/HR)
// converted from Comm Section and QUARC FORTRAN codes to C
double rainAttenuation( double rainRate, // if set 0, function will calculate it
                        double availability,    double carrierFrequency,
                        double stationLatitude, double stationAltitude,
                        RainRegion rainRegion,  double elevationAngle )
{
    static const double COMM_EFF_EARTH_RADIUS = 8500.0 * KM_TO_NMI;
    double attenuation;
    double height0DegIso;
    double projSurfPathLength, adjProjSurfPathLength, adjRainRate, phi(0.0);
    double alpha, beta, spatn, sltcor;
    double rainRateRatio, computedRainRate;
    // CONVERT FRO HZ TO GHZ
    double frequencyGHz = carrierFrequency * 1.0e-9;
    //  I - DETERMINE THE RAIN RATE
    double percOutage = 100.0 * ( 1.0 - availability );
    computedRainRate = calcRainRate( percOutage, rainRegion );

    // CHECK FOR AN INPUT RAIN RATE TO FORCE rp
    if ( rainRate > 0.0 ) {
        // THEN rainRate FORCES rp...FIND THE RATIO IN CASE rp MUST BE MODIFIED
        rainRateRatio    = rainRate / computedRainRate;
        computedRainRate = rainRate;
    } else {
        // ELSE rp IS GOOD...SET THE SCALING FACTOR TO UNITY (20)
        rainRateRatio = 1.0;
    }

    // II - ESTABLISH THE SURFACE PROJECTED PATH LENGTH
    //      FIND THE 0 DEG ISOTHERM HEIGHT (30)
    height0DegIso = height0DegIsotherm( percOutage, stationLatitude );
    // FIND THE SURFACE PROJECTED PATH LENGTH (AND ADJUST IF NECC.)
    projectedSurfacePathLength( projSurfPathLength,
                                adjProjSurfPathLength, adjRainRate,
                                phi, computedRainRate, rainRateRatio, percOutage,
                                height0DegIso, elevationAngle, stationAltitude,
                                rainRegion, COMM_EFF_EARTH_RADIUS );
    // III - DETERMINE alpha AND beta
    calcRainAttenuationCoeff( alpha, beta, frequencyGHz );
    // IV - CALCULATE SURFACE PROJECTED ATTENUATION
    spatn = surfaceProjectedAttenuation( adjRainRate,
                                         adjProjSurfPathLength, alpha, beta,
                                         height0DegIso, stationAltitude);

    // V - ADJUST FOR HEIGHT ALONG SLANT PATH
    if ( projSurfPathLength <= 0.01 ) {
        //      VERTICLE BEAM
        sltcor = 1.0;
    } else if ( elevationAngle >= (10.0 * DEG_TO_RAD) ) {
        //       elevationAngle >= 10 DEG
        sltcor = 1.0 / cos( elevationAngle );
    } else {
        //       elevationAngle < 10 DEG
        sltcor = sqrt( (COMM_EFF_EARTH_RADIUS
                        * (COMM_EFF_EARTH_RADIUS + height0DegIso + stationAltitude)
                        + height0DegIso * stationAltitude)
                       * phi * phi + (height0DegIso - stationAltitude)
                       * (height0DegIso - stationAltitude) )
                 / projSurfPathLength;
    }

    attenuation = spatn * sltcor;
    return attenuation;
}

//=======================================================================
// SUPPORTING FUNCTIONS
//=======================================================================
// FIND THE RAIN RATE (RP MM/HR) IN REGION 'rainRegion' FOR % OUTAGE 'PO'
// THE REGIONS ARE DEFINED IN CRAIN'S PAPER:
// IEEE TRANS. ON COMM.  VOL COM-28,NO.9 SEPT. 1980
// 0..9 CORRESPOND TO A,B,C,D1,D2,D3,E,F,G,H RESPECTIVELY
// converted from Comm Section and QUARC FORTRAN codes to C
double calcRainRate( double percOut, RainRegion rainRegion)
{
    static const int rainRateValuesPerStation = 12;
    /***
    static const double percOutArray[] = {
         5.0E-5, 0.001, 0.002, 0.005,  0.01,  0.02, 0.05,  0.1,  0.2,  0.5, 1.0, 2.0
    };
    ***/
    static const double LOGPERCOUTARR[] = {
        -9.90349, -6.90776, -6.21461, -5.29832, -4.60517, -3.91202,
        -2.99573, -2.30259, -1.60944, -0.69315,  0.00000,  0.69315
    };
    static const double RAINRATETBL[][rainRateValuesPerStation] = {
        { 45.0,  28.0,  24.0,  19.0,  15.0,  12.0,  8.0,  5.5,  4.0,  2.5, 1.7, 1.1 },
        {115.0,  54.0,  40.0,  26.0,  19.0,  14.0,  9.5,  6.8,  4.8,  2.7, 1.8, 1.2 },
        {158.0,  80.0,  62.0,  41.0,  28.0,  18.0, 11.0,  7.2,  4.8,  2.8, 1.9, 1.2 },
        {168.0,  90.0,  72.0,  50.0,  37.0,  27.0, 16.0, 11.0,  7.5,  4.0, 2.2, 1.3 },
        {171.0, 102.0,  86.0,  64.0,  49.0,  35.0, 22.0, 15.0,  9.5,  5.2, 3.0, 1.8 },
        {213.0, 127.0, 107.0,  81.0,  63.0,  48.0, 31.0, 22.0, 14.0,  7.0, 4.0, 2.5 },
        {250.0, 164.0, 144.0, 117.0,  98.0,  77.0, 52.0, 35.0, 21.0,  8.5, 4.0, 2.0 },
        {131.0,  66.0,  51.0,  34.0,  23.0,  14.0,  8.0,  5.5,  3.2,  1.2, 0.8, 0.4 },
        {215.0, 129.0, 109.0,  85.0,  67.0,  51.0, 33.0, 22.0, 14.0,  7.0, 3.7, 1.6 },
        {385.0, 251.0, 220.0, 178.0, 147.0, 115.0, 77.0, 51.0, 31.0, 13.0, 6.4, 2.8 }
    };
    // double rainRate = newtonInterpolate3Ext( log( percOut ),
    //                   rainRateValuesPerStation,
    //                   LOGPERCOUTARR,
    //                   RAINRATETBL[ (int)rainRegion ] );
    double rainRate = 0.0;
    const double* pRegionRainRate = RAINRATETBL[ (int)rainRegion ];
    double logPercOut = log( percOut );
    interpolLagrangePoly( LOGPERCOUTARR, LOGPERCOUTARR+rainRateValuesPerStation,
                          pRegionRainRate, 3, logPercOut, rainRate );
    return rainRate > 0.0 ? rainRate : 0.0;
}

//=======================================================================
// FIND THE ZERO DEGREE (C) ISOTHERM HEIGHT FOR THE LATITUDE 'stalat'
// in radians AND FOR THE PROBABILITY OF OUTAGE OF 'percOutage' %
// converted from Comm Section and QUARC FORTRAN codes to C
double height0DegIsotherm( double percOutage, double stationLatitude)
{
    static const int NZDI = 12;
    /***
    static const double ZDITBL[] =
          { -10.00   0.00, 10.00,  20.00,
             30.00, 40.00, 50.00,  60.00,
             70.00, 80.00, 90.00, 100.00,
              4.55,  4.60,  4.55,   4.37,
              3.75,  2.38,  1.17,   0.41,
              0.00, -0.17, -0.22,  -0.17,
              5.39,  5.30,  5.39,   5.50,
              5.27,  4.75,  4.07,   3.15,
              2.27,  1.63,  1.40,   1.63 };

    // stationLatitude in degrees
    static const double ZDITBL0[] =
          { -10.00,  0.00, 10.00,  20.00,
             30.00, 40.00, 50.00,  60.00,
             70.00, 80.00, 90.00, 100.00 };
    ***/
    // stationLatitude in radians
    static const double ZDITBL0[] = {
        -0.17453,  0.00000, 0.17453,  0.34907,
        0.52360,  0.69813, 0.87266,  1.04720,
        1.22173,  1.39626, 1.57080,  1.74533
    };
    static const double ZDITBL1[] = {
        4.55,  4.60,  4.55,   4.37,
        3.75,  2.38,  1.17,   0.41,
        0.00, -0.17, -0.22,  -0.17
    };
    static const double ZDITBL2[] = {
        5.39,  5.30,  5.39,   5.50,
        5.27,  4.75,  4.07,   3.15,
        2.27,  1.63,  1.40,   1.63
    };
    double height0DegIso;
    double zdi29, zdi59;
    //******************************************************************
    //
    // FIND THE 99 % ISOTHERM HEIGHT
    // zdi29 = newtonInterpolate3Ext( stationLatitude, NZDI, ZDITBL0, ZDITBL1 );
    interpolLagrangePoly( ZDITBL0, ZDITBL0+NZDI, ZDITBL1, 3, stationLatitude, zdi29 );
    // FIND THE 99.999% ISOTHERM HEIGHT
    // zdi59 = newtonInterpolate3Ext( stationLatitude, NZDI, ZDITBL0, ZDITBL2 );
    interpolLagrangePoly( ZDITBL0, ZDITBL0+NZDI, ZDITBL2, 3, stationLatitude, zdi59 );
    // USE LOGARITHMIC INTERPOLATION BETWEEN 99% AND 99.999%
    //   (EXTRAPOLATE OUTSIDE REGION)---NOTE LOG(1) = 0 AND LOG(.001) = -3
    height0DegIso = (zdi29 - ( zdi59 - zdi29 ) * log10( percOutage ) / 3.0)
                    * KM_TO_NMI;

    if ( height0DegIso < 0.0 ) {
        height0DegIso = 0.0;
    }

    return height0DegIso;
}

//=======================================================================
// CALCULATE THE SURFACE PROJECTED PATH LENGTH
// converted from Comm Section and QUARC FORTRAN codes to C
void projectedSurfacePathLength( double& projSurfPathLength,
                                 double& adjProjSurfPathLength,
                                 double& adjRainRate, double& phi, double rainRate,
                                 double rainRateRatio,
                                 double percOutage,
                                 double height0DegIso,
                                 double elevationAngle,
                                 double stationAltitude, RainRegion rainRegion,
                                 double commEffectiveEarthRadius )
{
    //********************************************************************
    //IF(elevationAngle.GE.10.) GOTO 70
    if ( elevationAngle < 10.0 * DEG_TO_RAD ) {
        double x1;
        double x2;
        double x3;
        double delt;
        delt = height0DegIso-stationAltitude;
        x1 = ( stationAltitude + commEffectiveEarthRadius )
             * sin( elevationAngle );
        x2 = x1 * x1 + ( delt < 0.0 ? 0.0 : delt )
             * ( commEffectiveEarthRadius + commEffectiveEarthRadius
                 + height0DegIso + stationAltitude );
        x3 = cos( elevationAngle ) / ( height0DegIso
                                       + commEffectiveEarthRadius ) * ( sqrt(x2) - x1 );
        phi  = asin( x3 );
        projSurfPathLength = phi * commEffectiveEarthRadius;
    }
    // elevationAngle >= 10 DEG (70)
    else {
        projSurfPathLength = ( height0DegIso - stationAltitude )
                             / tan ( elevationAngle );
    }

    // CALCULATE THE ADJUSTED PATH LENGHT (IF NECC.) (80)
    if ( projSurfPathLength <= 12.149 ) { // nmi
        // THEN SURFACE PROJECTED PATH LENGTH IS LESS THAN 22.5 KM
        adjRainRate   = rainRate;
        adjProjSurfPathLength = projSurfPathLength;
    }
    // ELSE projSurfPathLength > 22.5 KM   ... ADJUST (90)
    else {
        double apo;
        // adjProjSurfPathLength = 22.5;   // km
        adjProjSurfPathLength = 12.149; // nmi
        apo   = percOutage * adjProjSurfPathLength / projSurfPathLength;
        adjRainRate   = calcRainRate( apo, rainRegion );
        adjRainRate   = adjRainRate*rainRateRatio;
    }
}

//=======================================================================
// DETERMINE THE RAIN ATTENUATION VS. freq PARAMETERS
// converted from Comm Section and QUARC FORTRAN codes to C
void calcRainAttenuationCoeff(double& alpha, double& beta, double freq)
{
    // ********************** LOCALS ************************************
    static const int NALP = 18;
    static double FRQTBL[] = {
        1.0,  4.0,  5.0,  6.0,
        7.5,  10.0, 15.0, 20.0,
        25.0, 30.0, 35.0, 40.0,
        50.0, 60.0, 70.0, 80.0,
        90.0, 100.0
    };
    // natural log of the frequency table
    static double LOGFRQTBL[] = {
        0.000000000, 1.386294361, 1.609437912, 1.791759469,
        2.014903021, 2.302585093, 2.708050201, 2.995732274,
        3.218875825, 3.401197382, 3.555348061, 3.688879454,
        3.912023005, 4.094344562, 4.248495242, 4.382026635,
        4.499809670, 4.605170186
    };
    /***
    static double ALPTBL[] =
      { 0.00015, 0.0008, 0.00138, 0.0025,
        0.00482, 0.0125, 0.0357,  0.0699,
        0.113,   0.17,   0.242,   0.325,
        0.485,   0.65,   0.78,    0.875,
        0.935,   0.965 };
    ***/
    // natural log of the LOGALPHATBL table
    static double LOGALPHATBL[] = {
        -8.804875264, -7.130898830, -6.585671780, -5.991464547,
        -5.334981351, -4.382026635, -3.332604590, -2.66068963,
        -2.180367460, -1.771956842, -1.418817553, -1.123930097,
        -0.723606388, -0.430782916, -0.248461359, -0.133531393,
        -0.067208750, -0.035627178
    };
    static double BETATBL[] = {
        0.95, 1.17,  1.24, 1.28,
        1.25, 1.18,  1.12, 1.1,
        1.09, 1.075, 1.04, 0.99,
        0.9,  0.84,  0.79, 0.753,
        0.73, 0.715
    };
    double logFreq = log(freq);
    // alpha = newtonInterpolate3Ext( logFreq, NALP, LOGFRQTBL, LOGALPHATBL );
    interpolLagrangePoly( LOGFRQTBL, LOGFRQTBL+NALP, LOGALPHATBL, 3, logFreq, alpha );
    alpha = exp(alpha);
    // beta = newtonInterpolate3Ext( freq, NALP, FRQTBL, BETATBL );
    interpolLagrangePoly( FRQTBL, FRQTBL+NALP, BETATBL, 3, freq, beta );
}

//=======================================================================
// CALCULATE THE SURFACE PROJECTED ATTENUATION
// converted from Comm Section and QUARC FORTRAN codes to C
double surfaceProjectedAttenuation( double rainRate,
                                    double projSurfPathLength,
                                    double alpha, double beta, // >>>> beta must be unitless
                                    double height0DegIso,
                                    double stationAltitude)
{
    double spatn;
    double B, C, D, U, X1;
    B = 2.3 * pow( rainRate, -0.17 );   // >>>> B must be unitless
    C = 0.026 - 0.03 * log( rainRate );
    D = 20.0 * C + 3.28;
    C /= KM_TO_NMI;     // >>>> C must be units of 1/length
    D *= KM_TO_NMI;     // >>>> D must be units of length
    U = log(B) / D + C; // >>>> U must be units of 1/length

    // if ( projSurfPathLength <= 0.01 ) { // 0.01 is in km
    if ( projSurfPathLength <= 0.0054 ) { // 0.0054 is nmi
        //      THEN VERTICLE BEAM (ELEV = 90 DEG)
        X1 = height0DegIso-stationAltitude;  // >>>> X1 must be length
    } else if ( D > projSurfPathLength ) {
        X1=( exp( U * beta * projSurfPathLength ) - 1.0 )/( U * beta);
        //CPPCHECK_FUTURE_IMPLEMENTATION: X1= expm1( U * beta * projSurfPathLength ) /( U * beta);
    }
    //       ELSE D < projSurfPathLength
    else {
        X1 = ( exp( U * D * beta) - 1.0 ) / ( U * beta ) + pow( B, beta )
             / ( C * beta ) * ( exp( C * beta * projSurfPathLength ) - exp( C * beta * D ) );
        //CPPCHECK_FUTURE_IMPLEMENTATION: X1 = expm1( U * D * beta) / ( U * beta ) + pow( B, beta )
        //CPPCHECK_FUTURE_IMPLEMENTATION:      / ( C * beta ) * ( exp( C * beta * projSurfPathLength ) - exp( C * beta * D ) );
    }

    spatn = alpha * pow( rainRate, beta ) * X1 * NMI_TO_KM;
    return spatn;
}

// ============================================================================
}
// A3 namespace end
// ============================================================================

