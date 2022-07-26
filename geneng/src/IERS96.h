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
 * IERS96.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
/// @file
/// @brief IERS Namespace, Classes & Functions
///
/// @details Provides transformations between celestial and terrestrial systems
/// as defined in IERS Technical Note 21, IERS Conventions 1996.
///
/// For the purposes of this implementation:
/// J2000 Mean Equator Mean Equinox (J2000) <=> Celestial Reference System (CRS)
///
/// The coordinate transformation from the Terrestrial Reference System
/// (TRS) to the Celestial Reference System (CRS) at some time (t) is:
/// <br><br><em>[CRS] = PN(t)R(t)W(t)[TRS]</em>;
/// <br><br>where
/// <br>@em PN(t) = Celestial Ephemeris Pole (CEP) motion in the CRS,
/// <br>@em R(t) =  rotation of the Earth around the axis of the CEP
/// <br>@em W(t) = polar motion
///
/// @author J. McCormick
/// @version V4.5 Initial release
/// @date 2009-07-01
///
/// @par Units
/// - Function Arg & Return: as defined in Tech Note 21, see arg comments
/// - Stream I/O:            L, T = standard, A = manipulator controlled
/// - Initialization Files:  N/A
// =============================================================================
#ifndef IERS96_H
/// header guard macro
#define IERS96_H   "IERS96 2009-06-17"

#include "TimeJ2000.h"
#include "Angle.h"
#include "MatrixFP.h"
#include <math.h>

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// IERS Conventions 1996
namespace IERS96
{
/// @brief R2000 day at 0 hr
/// @details Rounds down the specified time to 0 hr and returns
/// the results in units of R2000 days
/// E.g., the R2000 day at 0 hr for 1 January 2000 12:00:00
/// is -0.5 which corresponds to 1 January 2000 00:00:00
/// @param [in] tm time, R2000 sec
/// @return J2000 day at 0 hr (e.g., +/-0.5, +/-1.5, ...)
inline double r2000Day0h(const TimeR2000& tm)
{
    double c(floor(tm*SEC_TO_DAY-0.5)+0.5);
    return c;
}

/// @brief R2000 day at 0 hr
/// @details Rounds down the specified time to 0 hr and returns
/// the results in units of R2000 days and fraction of day
/// @param [in] tm time, R2000 sec
/// @param [out] faction of day remaining
/// @return J2000 day at 0 hr (e.g., +/-0.5, +/-1.5, ...)
inline double r2000Day0h(const TimeR2000& tm, double& fracDay)
{
    double t(tm*SEC_TO_DAY);
    double c(floor(t-0.5)+0.5);
    fracDay = t - c;
    return c;
}

/// @brief Julian Centuries w.r.t. R2000
/// @param [in] tm time, R2000 sec
/// @return R2000 centuries
inline double r2000JC(const TimeR2000& tm)
{
    return tm * SEC_TO_DAY / 36525.0;
}

/// @brief Precession Angles
/// @param [in] tmTT time, J2000, TT, sec
/// @param [out] zetaA precession angle, radians
/// @param [out] thetaA precession angle, radians
/// @param [out] zA precession angle, radians
void prcang(const TimeJ2000& tmTT,
            Angle& zetaA, Angle& thetaA, Angle& zA);

/// @brief Greenwich Sidereal Angle
/// @param [in] tmTT time, J2000, TT, sec
/// @return Greenwich Mean Angle, radians
Angle gsa(const TimeJ2000& tmTT);

/// @brief Mean Longitude of the Ascending Node of the Moon
/// @param [in] tmTT time, J2000, TT, sec
/// @return Moon's Mean Longitude of the Ascending Node, radians
Angle mlanm(const TimeJ2000& tmTT);

/// @brief Mean Obliquity of the Ecliptic
/// @param [in] tmTT time, J2000, TT, sec
/// @return Mean Obliquity of the Ecliptic, radians
Angle moe(const TimeJ2000& tmTT);

/// @brief Greenwich Mean Sidereal Angle
/// @param [in] tmTT time, J2000, TT, sec
/// @return Greenwich Mean Sidereal Angle, radians
Angle gmsa(const TimeR2000& tmTT);

/// @brief J2000 to Mean Equator, Mean Equinox of Date
/// Direction Cosine Matrix
/// @param [in] tmTT time, J2000, TT, sec
/// @param [out] dcm J2000 to Mean of Date DCM
void dcmJ2000ToMod(const TimeJ2000& tmTT, MatrixFP<double>& dcm);

/// @brief Mean Equator, Mean Equinox of Date to J2000
/// Direction Cosine Matrix
/// @param [in] tmTT time, J2000, TT, sec
/// @param [out] dcm Mean of Date to J2000 DCM
inline
void dcmModToJ2000(const TimeJ2000& tmTT, MatrixFP<double>& dcm)
{
    dcmJ2000ToMod(tmTT, dcm);
    dcm.TransposeThis();
}

/// @brief Mean Equator, Mean Equinox of Date to
/// True Equator, True Equinox of Date
/// Direction Cosine Matrix
/// @param [in] tmTT time, J2000, TT, sec
/// @param [out] dcm Mean of Date to True of Date DCM
void dcmModToTod(const TimeJ2000& tmTT, MatrixFP<double>& dcm);

/// @brief True Equator, True Equinox of Date to
/// Mean Equator, Mean Equinox of Date
/// Direction Cosine Matrix
/// @param [in] tmTT time, J2000, TT, sec
/// @param [out] dcm True of Date to Mean of Date DCM
inline
void dcmTodToMod(const TimeJ2000& tmTT, MatrixFP<double>& dcm)
{
    dcmModToTod(tmTT, dcm);
    dcm.TransposeThis();
}

/// @brief True Equator, True Equinox of Date to Pseudo Body Fixed
/// Direction Cosine Matrix
/// @param [in] tmTT time, J2000, TT, sec
/// @param [out] dcm True of Date to Pseudo Body Fixed DCM
void dcmTodToPbf(const TimeJ2000& tmTT, MatrixFP<double>& dcm);

/// @brief Pseudo Body Fixed to True Equator, True Equinox of Date
/// Direction Cosine Matrix
/// @param [in] tmTT time, J2000, TT, sec
/// @param [out] dcm Pseudo Body Fixed to True of Date DCM
inline
void dcmPbfToTod(const TimeJ2000& tmTT, MatrixFP<double>& dcm)
{
    dcmTodToPbf(tmTT, dcm);
    dcm.TransposeThis();
}

/// @brief True Equator, True Equinox of Date to Pseudo Body Fixed
/// Direction Cosine Matrix and Rate Matrix
/// @param [in] tmTT time, J2000, TT, sec
/// @param [out] dcm True of Date to Pseudo Body Fixed DCM
/// @param [out] rm Rate Matrix
void dcmTodToPbfRM(const TimeJ2000& tmTT,
                   MatrixFP<double>& dcm,
                   MatrixFP<double>& rm);

/// @brief Pseudo Body Fixed to True Equator, True Equinox of Date
/// Direction Cosine Matrix and Rate Matrix
/// @param [in] tmTT time, J2000, TT, sec
/// @param [out] dcm Pseudo Body Fixed to True of Date DCM
/// @param [out] rm Rate Matrix
void dcmPbfToTodRM(const TimeJ2000& tmTT,
                   MatrixFP<double>& dcm,
                   MatrixFP<double>& rm);

/// UT1
/// @details All functions within this namespace use J2000 UT1
/// for time arguments
namespace UT1
{
/// @brief Greenwich Mean Sidereal Angle
/// @param [in] tmUT1 time, R2000, UT1, sec
/// @return Greenwich Mean Sidereal Angle, radians
Angle gmsa(const TimeR2000& tmUT1);

/// @brief Greenwich Mean Sidereal Angle at 0hr
/// @param [in] tmUT1 time, R2000, UT1, sec
/// @return Greenwich Mean Sidereal Angle, radians
Angle gmsa0h(const TimeR2000& tmUT1);

/// @brief Ratio of Universal to Sidereal Time
/// @param [in] tmUT1 time, R2000, UT1, sec
/// @return universal to sidereal ratio
double ratioUS(const TimeR2000& tmUT1);
} //UT1

/// Julian Centuries UT1
/// @details All functions within this namespace use Julian Centuries
/// w.r.t. J2000 UT1 for time arguments
namespace JulCenUT1
{
/// @brief Greenwich Mean Sidereal Angle at 0hr
/// @details GMST0h UT1 = 6h 41m 50.54841s + 8640184.812866s *
///  T(cut) + 0.093104s T(cut)^2 - 6.2x10^-6s * T(cut)^3 <br>
/// where <br>
/// T(cut) = R2000, UT1 at 0hr, jc
/// @param [in] tmUT1JC0h time, R2000, UT1, jc
/// @return Greenwich Mean Sidereal Angle, radians
Angle gmsa0h(double tmUT1JC0h);

/// @brief Ratio of Universal to Sidereal Time
/// @details r = 1.002737909350795 + 5.9006x10^-11 * TT(cut) -
///  5.9x10^-15 * T(cut)^2
/// T(cut) = R2000, UT1 at 0hr, jc
/// @param [in] tmUT1JC0h time, R2000, UT1, jc
/// @return universal to sidereal ratio
double ratioUS(double tmUT1JC0h);
} // JulCenUT1

} // IERS96
} // A3
#endif  // IERS96_H
