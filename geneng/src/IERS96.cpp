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
 * IERS96.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// IERS Namespace, Classes & Functions
// Refer to IERS96.h more info.
//===============================================================================
#include "IERS96.h"
#include "JplEph.h"
#include "Normalize.h"
#include "constants.h"

// Astrodynamics, Allocation and Availability
namespace A3
{
// IERS Conventions 1996
namespace IERS96
{
// Precession Angles
void prcang(const TimeJ2000& tmTT,
            Angle& zetaA, Angle& thetaA, Angle& zA)
{
    static const double a1(3.543001009273e-12); // rad/sec
    static const double a2(1.4696072e-25); // rad/sec^2
    static const double a3(2.776433e-36); // rad/sec^3
    static const double b1(3.079186457516e-12); // rad/sec
    static const double b2(2.0770105e-25); // rad/sec^2
    static const double b3(6.453302e-36); // rad/sec^3
    static const double c1(3.543001009273e-12); // rad/sec
    static const double c2(5.3291030e-25); // rad/sec^2
    static const double c3(2.808057e-36); // rad/sec^3
    zetaA = ((a3*tmTT+a2)*tmTT+a1)*tmTT;
    thetaA = ((b3*tmTT+b2)*tmTT+b1)*tmTT;
    zA = ((c3*tmTT+c2)*tmTT+c1)*tmTT;
}

// Greenwich Sidereal Angle
Angle gsa(const TimeJ2000& tmTT)
{
    static const double a0(1.279908e-08);
    static const double a1(3.054326e-10);
    TimeR2000 ut1(ttToUt1(tmTT));
    // double jctt(r2000JC(tmTT));
    Angle nl(Jpl::nutations().Longitude(tmTT));
    Angle me(moe(tmTT));
    Angle ml(mlanm(tmTT));
    Angle ma(UT1::gmsa(ut1));
    return ma + nl * cos(me) + a0 * sin(ml) + a1 * sin(2.0*ml);
}

// Greenwich Mean Sidereal Angle
Angle gmsa(const TimeR2000& tmTT)
{
    return UT1::gmsa(ttToUt1(tmTT));
}

// Mean Longitude of the Ascending Node of the Moon
Angle mlanm(const TimeJ2000& tmTT)
{
    // static const double c0( 2.182439196616); // rad
    // static const double c1(-3.3757044612636e01); // rad/cntry
    // static const double c2( 3.622625e-05); // rad/cntry^2
    // static const double c3( 3.734035e-08); // rad/cntry^3
    // static const double c4(-2.879308e-10); // rad/cntry^4
    // double tmTTJC(r2000JC(tmTT));
    // return Angle((((((c4*tmTTJC+c3)*tmTTJC)+c2)*tmTTJC+c1)
    //             *tmTTJC)+c0);
    static const double c0( 2.1824391966157e+00); // rad
    static const double c1(-1.069696194027e-08); // rad/sec
    static const double c2( 3.6376040e-24); // rad/sec^2
    static const double c3( 1.188137e-36); // rad/sec^3
    static const double c4(-2.903169e-48); // rad/sec^4
    return Angle((((((c4*tmTT+c3)*tmTT)+c2)*tmTT+c1)*tmTT)+c0);
}

// Mean Obliquity of the Ecliptic
Angle moe(const TimeJ2000& tmTT)
{
    // static const double c0( 4.0909280422e-01); // rad
    // static const double c1(-2.26965525e-04); // rad/cntry
    // static const double c2(-2.860401e-09); // rad/cntry^2
    // static const double c3( 8.789672e-09); // rad/cntry^3
    // double tmTTJC(r2000JC(tmTT));
    // return Angle(((((c3*tmTTJC)+c2)*tmTTJC+c1)*tmTTJC)+c0);
    static const double c0( 4.090928042223e-01); // rad
    static const double c1(-7.19210348e-14); // rad/cntry
    static const double c2(-2.872228e-28); // rad/cntry^2
    static const double c3( 2.796796e-37); // rad/cntry^3
    return Angle(((((c3*tmTT)+c2)*tmTT+c1)*tmTT)+c0);
}

// J2000 to Mean Equator, Mean Equinox of Date
// Direction Cosine Matrix
void dcmJ2000ToMod(const TimeJ2000& tmTT, MatrixFP<double>& dcm)
{
    if (!dcm.IsSquare(3)) {
        dcm.Resize(3,3);
    }

    Angle zetaA, thetaA, zA;
    prcang(tmTT, zetaA, thetaA, zA);
    double cze(cos(zetaA)), sze(sin(zetaA));
    double cth(cos(thetaA)), sth(sin(thetaA));
    double czz(cos(zA)), szz(sin(zA));
    MatrixFP<double>::iterator m(dcm.begin());
    m[0] =  cze*cth*czz-sze*szz;
    m[1] = -sze*cth*czz-cze*szz;
    m[2] = -sth*czz;
    m[3] =  cze*cth*szz+sze*czz;
    m[4] = -sze*cth*szz+cze*czz;
    m[5] = -sth*szz;
    m[6] =  cze*sth;
    m[7] = -sze*sth;
    m[8] =  cth;
}

// Mean Equator, Mean Equinox of Date to
// True Equator, True Equinox of Date
// Direction Cosine Matrix
void dcmModToTod(const TimeJ2000& tmTT, MatrixFP<double>& dcm)
{
    if (!dcm.IsSquare(3)) {
        dcm.Resize(3,3);
    }

    Angle nl(Jpl::nutations().Longitude(tmTT));
    Angle ne(Jpl::nutations().Obliquity(tmTT));
    Angle me(moe(tmTT));
    Angle mt(me+ne);
    double cnl(cos(nl)), snl(sin(nl));
    double cme(cos(me)), sme(sin(me));
    double cmt(cos(mt)), smt(sin(mt));
    MatrixFP<double>::iterator m(dcm.begin());
    m[0] =  cnl;
    m[1] = -cme*snl;
    m[2] = -sme*snl;
    m[3] =  cmt*snl;
    m[4] =  cme*cmt*cnl+sme*smt;
    m[5] =  sme*cmt*cnl-cme*smt;
    m[6] =  smt*snl;
    m[7] =  cme*smt*cnl-sme*cmt;
    m[8] =  sme*smt*cnl+cme*cmt;
}

// True Equator, True Equinox of Date to Pseudo Body Fixed
// Direction Cosine Matrix
void dcmTodToPbf(const TimeJ2000& tmTT, MatrixFP<double>& dcm)
{
    if (!dcm.IsSquare(3)) {
        dcm.Resize(3,3);
    }

    Angle a(gsa(tmTT));
    double ca(cos(a)), sa(sin(a));
    dcm.IdentityThis();
    MatrixFP<double>::iterator m(dcm.begin());
    m[0] =  ca;
    m[1] =  sa;
    m[3] = -sa;
    m[4] =  ca;
}

// True Equator, True Equinox of Date to Pseudo Body Fixed
// Direction Cosine Matrix and Rate Matrix
void dcmTodToPbfRM(const TimeJ2000& tmTT,
                   MatrixFP<double>& dcm,
                   MatrixFP<double>& rm)
{
    if (!dcm.IsSquare(3)) {
        dcm.Resize(3,3);
    }

    Angle a(gsa(tmTT));
    double ca(cos(a)), sa(sin(a));
    dcm.IdentityThis();
    MatrixFP<double>::iterator m(dcm.begin());
    m[0] =  ca;
    m[1] =  sa;
    m[3] = -sa;
    m[4] =  ca;
    m = rm.begin();
    m[0] = -sa;
    m[1] =  ca;
    m[3] = -ca;
    m[4] = -sa;
    rm *= EARTH_ROTATION_RATE;
}

// Pseudo Body Fixed to True Equator, True Equinox of Date
// Direction Cosine Matrix and Rate Matrix
void dcmPbfToTodRM(const TimeJ2000& tmTT,
                   MatrixFP<double>& dcm,
                   MatrixFP<double>& rm)
{
    if (!dcm.IsSquare(3)) {
        dcm.Resize(3,3);
    }

    Angle a(gsa(tmTT));
    double ca(cos(a)), sa(sin(a));
    dcm.IdentityThis();
    MatrixFP<double>::iterator m(dcm.begin());
    m[0] =  ca;
    m[1] = -sa;
    m[3] =  sa;
    m[4] =  ca;
    m = rm.begin();
    m[0] = -sa;
    m[1] = -ca;
    m[3] =  ca;
    m[4] = -sa;
    rm *= EARTH_ROTATION_RATE;
}

namespace UT1
{
// @Greenwich Mean Sidereal Angle
Angle gmsa(const TimeR2000& tmUT1)
{
    double fod(0.0);
    double h0d(r2000Day0h(tmUT1, fod));
    double tc0h(h0d/36525);
    double r(JulCenUT1::ratioUS(tc0h));
    Angle a(JulCenUT1::gmsa0h(tc0h));
    return normalizeZeroTwoPi(a + r * fod * TWO_PI);
}

// Greenwich Mean Sidereal Angle at 0hr
Angle gmsa0h(const TimeR2000& tmUT1)
{
    double tc(r2000Day0h(tmUT1)/36525);
    return normalizeZeroTwoPi(JulCenUT1::gmsa0h(tc));
}

// Ratio of Universal to Sidereal Time
double ratioUS(const TimeR2000& tmUT1)
{
    double tc(r2000Day0h(tmUT1)/36525);
    return JulCenUT1::ratioUS(tc);
}
}

namespace JulCenUT1
{
// Greenwich Mean Sidereal Angle at 0hr
Angle gmsa0h(double tmUT1JC0h)
{
    static const double c0( 1.75336855923327); // rad
    static const double c1( 628.331970688841); // rad/cntry
    static const double c2( 0.00000677071394); // rad/cntry^2
    static const double c3(-0.00000000045088); // rad/cntry^3
    return Angle(((((c3*tmUT1JC0h)+c2)*tmUT1JC0h+c1)*tmUT1JC0h)+c0);
}

// Ratio of Universal to Sidereal Time
double ratioUS(double tmUT1JC0h)
{
    static const double c0(1.002737909350795);
    static const double c1(5.9006e-11); // per cntry
    static const double c2(-5.9e-15); // per cntry^2
    return (((c2*tmUT1JC0h+c1)*tmUT1JC0h)+c0);
}

}
}
}
