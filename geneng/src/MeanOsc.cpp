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
 * MeanOsc.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Mean-Osculating Orbit Element Transformations
// Refer to MeanOsc.h for more info.
// =============================================================================
#include "MeanOsc.h"
#include "anomaly.h"
#include "constants.h"
#include "IoSeparator.h"
#include "MatrixFP.h"
#include "CoorTransEciPqw.h"
#include "TestValue.h"
#include "Normalize.h"

#include <math.h>
#include <iomanip>

namespace A3
{
namespace
{
// cos approx for i = PI rad (90 deg) problem, reco degree = 4
double cosApprox(double angle, unsigned degree)
{
    double r(1.0), s(1.0), a(1.0), a2(angle*angle);
    unsigned n(1);

    for (unsigned i=0; degree > i; ++i) {
        s *= -1.0;
        n *= (2*i+1)*(2*i+2);
        a *= a2;
        r += s*a/n;
    }

    return r;
}

// Transform orbit elements from mean to osculating
void meanToOsculating(const MatrixFP<double>& mn, MatrixFP<double>& osc)
{
    static const double small_e(1.0e-12);
    static const double small_i(1.0e-8); // 5.7e-7 deg
    static const double polar_i(PI/2.0); // 90 deg
    static const double retro_i_eq_thr(PI-5.2e-2);
    static const double crit_i_direct(acos(sqrt(0.2))); // 63.435 deg
    static const double crit_i_retro(PI-crit_i_direct); // 116.565 deg
    static const double crit_i_bnd(3.5e-2);  // 2 deg
    static const double crit_i_cos_sq(0.2);
    static const double crit_i_e_ftr(-6.0e3);
    static const double crit_i_s_ftr(4.0e-2);

    if (6 != mn.size())
        throw std::runtime_error(
            "meanToOsculating(const MatrixFP<>&, MatrixFP<>&) bad size");

    MatrixFP<double>::const_iterator imn(mn.begin());
    // mean semimajor axis
    const double &am(imn[0]);
    // mean eccentricity
    // NOTE: eccentricity must be less than 1
    const double &em(imn[1]);
    // mean mean anomaly
    const double &mm(imn[2]);
    // mean right ascension of the ascending node
    const double &ram(imn[3]);
    // mean inclination
    double im(imn[4]);
    // mean argument of perigee
    const double &apm(imn[5]);
    const bool bIsDirec(polar_i > im);
    const double crit_i(bIsDirec ? crit_i_direct : crit_i_retro);
    const double crit_delta(im-crit_i);
    const bool bIsInCrit(crit_i_bnd > fabs(crit_delta) ? true : false);
    const bool bIsRetroEq(retro_i_eq_thr < im);

    // ---------------------------------------------------------------------
    if (6 != osc.size()) {
        osc.Resize(1,6);
    }

    MatrixFP<double>::iterator iosc(osc.begin());
    // references to the output osculation element set
    double &a(iosc[0]);
    double &e(iosc[1]);
    double &l(iosc[2]);
    double &h(iosc[3]);
    double &i(iosc[4]);
    double &g(iosc[5]);
    // ---------------------------------------------------------------------
    // sub 0
    // mean true anomaly
    const double tam(anomaly(6, em, mm));
    // mean eccentric anomaly
    const double eam(anomaly(4, em, mm));
    // mean radian distance
    const double mrd(am*(1.0-em*cos(eam)));
    // mean mean anomaly
    const double &lm(mm);
    // mean true anomaly
    const double &fm(tam);
    // mean right ascension of the ascending node
    const double &hm(ram);
    // mean argument of perigee
    const double &gm(apm);
    const double cfm(cos(fm));
    const double cfm2(cfm*cfm);
    const double cfm3(cfm2*cfm);
    const double c2fm2gm(cos(2.0*(fm+gm)));
    const double c3fm2gm(cos(3.0*fm+2.0*gm));
    const double c1fm2gm(cos(fm+2.0*gm));
    const double sfm(sin(fm));
    // const double sfm2(sfm*sfm);
    const double s2fm2gm(sin(2.0*(fm+gm)));
    const double s3fm2gm(sin(3.0*fm+2.0*gm));
    const double s1fm2gm(sin(fm+2.0*gm));
    const double cgm(cos(gm));
    const double c2gm(cos(2.0*gm));
    const double c3gm(cos(3.0*gm));
    const double sgm(sin(gm));
    const double s2gm(sin(2.0*gm));
    const double s3gm(sin(3.0*gm));
    const double am2(am*am);
    const double am3(am2*am);
    const double am4(am3*am);
    const double am5(am4*am);
    const double em2(em*em);
    const double io2(im/2.0);
    const double cim(cos(im));
    // Note: handles divide by zero below, 4 places
    const double cio2(bIsRetroEq ? cosApprox(io2, 4) : cos(io2) );
    const double sim(sin(im));
    const double sim2(sim*sim);
    const double sio2(sin(io2));
    // Note: Divide by zero for 180 deg inclination, handled by cio
    const double tio2(sio2/cio2);
    const double clm(cos(lm));
    const double slm(sin(lm));
    const double chm(cos(hm));
    const double shm(sin(hm));
    const double &re(EARTH_RADIUS);
    const double &re2(EARTH_RADIUS_SQ);
    const double re3(re2*re);
    const double re4(re3*re);
    const double re5(re4*re);
    const double aor(am/mrd);
    const double aor2(aor*aor);
    const double aor3(aor2*aor);
    // ---------------------------------------------------------------------
    // sub 1
    // const double n0(sqrt(EARTH_GRAV_CONST/am3));
    const double eta(sqrt(1.0-em2));
    const double eta2(eta*eta);
    const double eta3(eta2*eta);
    const double eta4(eta2*eta2);
    const double eta5(eta4*eta);
    const double eta6(eta3*eta3);
    const double eta8(eta4*eta4);
    const double eta10(eta5*eta5);
    const double &theta(cim);
    const double theta2(theta*theta);
    // const double theta3(theta2*theta);
    const double theta4(theta2*theta2);
    const double theta6(theta4*theta2);
    const double k2(EARTH_J2_ZONAL*re2/2.0);
    const double k3(-EARTH_J3_ZONAL*re3);
    const double k4(-3.0*EARTH_J4_ZONAL*re4/8.0);
    const double k5(-EARTH_J5_ZONAL*re5);
    const double gam2(k2/am2);
    const double gam3(k3/am3);
    const double gam4(k4/am4);
    const double gam5(k5/am5);
    const double gam2p(gam2/eta4);
    const double gam3p(gam3/eta6);
    const double gam4p(gam4/eta8);
    const double gam5p(gam5/eta10);
    // ---------------------------------------------------------------------
    // sub 2: a terms
    // NOTE: a1p divide by zero at critical inclinations, handled below
    double a1pdos(0.0);

    if (bIsInCrit) {
        a1pdos = crit_i_s_ftr*exp(crit_i_e_ftr*crit_delta*crit_delta);

        if (crit_i_cos_sq < theta2) {
            a1pdos = -a1pdos;
        }
    }

    const double a1p(1.0/(1.0-5.0*theta2+a1pdos));
    const double a1(gam2p*eta2*(1.0-11.0*theta2-40.0*theta4*a1p)/8.0);
    const double a2p(3.0*theta2+8.0*theta4*a1p);
    const double a2(5.0*gam4p*eta2*(1.0-a2p)/12.0/gam2p);
    const double a3(gam5p*(3.0*em2+4.0)/gam2p);
    const double a4(gam5p*(1.0-3.0*a2p)/gam2p);
    const double a5(a3*(1.0-3.0*a2p));
    const double a6(gam3p/4.0/gam2p);
    const double a7(a6*eta2*sim);
    const double a8p(gam5p*em*(1.0-5.0*theta2-16.0*theta4*a1p)/gam2p);
    const double a8(a8p*em);
    const double a9(eta2*sim);
    const double a10(2.0+em2);
    const double a11(3.0*em2+2.0);
    const double a12(a11*theta2);
    const double a13((5.0*em2+2.0)*theta4*a1p);
    const double a14(em2*theta6*a1p*a1p);
    const double a15(theta2*a1p);
    const double a16(a15*a15);
    const double a17(em*sim);
    const double a18(a17/(1.0+eta));
    // const double a19((1.0+theta)*sim);
    const double a20(em*theta);
    const double a21(em*a20);
    // const double a22(a20*tio2);
    // const double a23(eta2*a17);
    // const double a24(a11+2.0);
    const double a25(16.0*a15+40.0*a16+3.0);
    const double a26(a21*(11.0+200.0*a16+80.0*a15)/8.0);
    // ---------------------------------------------------------------------
    // sub 3: b terms
    const double b1a(eta*(a1-a2));
    const double b1b(gam2p*((a10-400.0*a14-40.0*a13-11.0*a12)/16.0
                            +a21*(11.0+200.0*a16+80.0*a15)/8.0));
    const double b1c(5.0*gam4p*(-80.0*a14-8.0*a13-3.0*a12+2.0*a25*a21+a10)
                     /24.0/gam2p);
    const double b1(b1a-b1b+b1c);
    const double b2a(a6*a18*(2.0+eta-em2));
    const double b2b(5.0*a5*a18*eta2/64.0);
    const double b2c(15.0*a4*a17*eta3/32.0);
    // Note: Divide by zero for 180 deg inclination, handled by cio
    const double b2d(a20*tio2*(5.0*a5/64.0+a6));
    const double b2e(5.0*a4*a17*(9.0*em2+26.0)/64.0);
    const double b2f(15.0*a3*a20*a25*sim*(1.0-theta)/32.0);
    const double b2(b2a+b2b-b2c+b2d+b2e+b2f);
    const double b3a(35.0*gam5p*em*sim*(theta-1.0)*a21*(80.0*a16+5.0+32.0*a15)
                     /576.0/gam2p);
    // Note: Divide by zero for 180 deg inclination, handled by cio
    const double b3b(35.0*a8p*(a21*tio2+(2.0*em2+3.0*(1.0-eta3))*sim)
                     /1152.0);
    const double b3(b3a-b3b);
    // const double b4(eta*em*(a1-a2));
    const double b4(em*b1a);
    const double b5(eta*(5.0*a4*a9*(9.0*em2+4.0)/64.0+a7));
    const double b6(35.0*eta3*a8*sim/384.0);
    const double b7(eta2*a17*a1p*(gam2p*(1.0-15.0*theta2)/8.0
                                  -5.0*gam4p*(1.0-7.0*theta2)/12.0/gam2p));
    const double b8(5.0*a3*eta2*(1.0-9.0*theta2-24.0*theta4*a1p)/64.0
                    +eta2*a6);
    const double b9(35.0*eta2*a8/384.0);
    const double b10(sim*(5.0*gam4p*a21*a25/12.0/gam2p-a26*gam2p));
    const double b11(a21*(5.0*a5/64.0+a6+15.0*a3*a25*sim2/32.0));
    const double b12(-(80.0*a16+32.0*a15+5.0)
                     *(35.0*gam5p*em*sim2*a21/576.0/gam2p)
                     -35.0*a8*a20/1152.0);
    const double b13(em*(a1-a2));
    // const double b14(5.0*a5*eta2*sim/64.0+a7);
    const double b14(5.0*a5*a9/64.0+a7);
    // const double b15(35.0*a8*eta2*sim/384.0);
    const double b15(35.0*a8*a9/384.0);
    // ---------------------------------------------------------------------
    // osculating semimajor axis
    const double a1x(em*eta+em/(1.0+eta)+cfm*(3.0+3.0*em*cfm+em2*cfm2));
    const double a1a((3.0*theta2-1.0)*em*a1x/eta6);
    const double a1b(3.0*sim2*aor3*c2fm2gm);
    a = am*(1.0+gam2*(a1a+a1b));
    // ---------------------------------------------------------------------
    // osculating eccentricity
    const double d1e(b13*c2gm+b14*sgm-b15*s3gm);
    const double emdl2b1((-eta2*aor2-aor+1.0)*s1fm2gm);
    const double emdl2b2((eta2*aor2+aor+1.0/3.0)*s3fm2gm);
    const double emdl2b(3.0* sim2 *(emdl2b1+emdl2b2));
    const double emdl2a(2.0*(3.0*theta2-1.0)*(eta2*aor2+aor+1.0)*sfm);
    const double emdl2(eta3*gam2p*(emdl2a+emdl2b)/4.0);
    const double emdl1(b4*s2gm-b5*cgm+b6*c3gm);
    const double emdl(emdl1-emdl2);
    const double de2c1(em*eta+em/(1.0+eta)+3.0*em*cfm2+3.0*cfm+em2*cfm3);
    const double de2c(gam2*(3.0*theta2-1.0)*de2c1/eta6);
    const double de2b1(3.0*em*cfm2+3.0*cfm+em2*cfm3+em);
    const double de2b(3.0*gam2* sim2 *c2fm2gm*de2b1/eta6);
    const double de2a(gam2p*sim2*(3.0*c1fm2gm+c3fm2gm));
    const double de2(eta2*(de2a-de2b-de2c)/2.0);
    const double de(d1e-de2);
    const double emde(em+de);
    e = sqrt(emde*emde+emdl*emdl);
    // ---------------------------------------------------------------------
    // osculating inclination
    const double sio2dh2a(6.0*(em*sfm-lm+fm));
    const double sio2dh2b(3.0*(s2fm2gm+em*s1fm2gm));
    const double sio2dh2c(em*s3fm2gm);
    const double sio2dh2(gam2p*theta*sim*(sio2dh2a-sio2dh2b-sio2dh2c)/2.0);
    const double sio2dh1(b10*s2gm+b11*cgm+b12*c3gm);
    // Note: Divide by zero for 180 deg inclination, handled by cio
    const double sio2dh((sio2dh1-sio2dh2)/2.0/cio2);
    const double di1a(em*c3fm2gm);
    const double di1b(3.0*(em*c1fm2gm+c2fm2gm));
    const double di1(theta*gam2p*sim*(di1a+di1b)/2.0);
    const double di2(a20*(b7*c2gm+b8*sgm-b9*s3gm)/eta2);
    const double di(di1-di2);
    const double ki(di*cio2/2.0+sio2);
    const double sinHlfIncl(testAsinAcosArg(sqrt(sio2dh*sio2dh+ki*ki)));
    i = 2.0*asin(sinHlfIncl);

    // ---------------------------------------------------------------------
    // osculating mean anomaly
    if (small_e <= e) {
        const double ln(emdl*clm+(em+de)*slm);
        const double ld((em+de)*clm-emdl*slm);
        l = atan2(ln,ld);

        if (0.0 > l) {
            l += TWO_PI;
        }
    } else {
        l = 0.0;
    }

    // ---------------------------------------------------------------------
    // osculating right ascension of the ascending node
    if (small_i <= i) {
        const double hn(sio2dh*chm+ki*shm);
        const double hd(ki*chm-sio2dh*shm);
        h = atan2(hn,hd);

        if (0.0 > h) {
            h += TWO_PI;
        }
    } else {
        h = 0.0;
    }

    // ---------------------------------------------------------------------
    // osculating argument of periapsis
    const double lpgphp(lm+gm+hm+b3*c3gm+b1*s2gm+b2*cgm);
    const double lgh4b(3.0*(s2fm2gm+em*s1fm2gm));
    const double lgh4a(em*s3fm2gm);
    const double lgh4(gam2p*(3.0+2.0*theta-5.0*theta2)*(lgh4a+lgh4b)/4.0);
    const double lgh3(1.5*gam2p*((5.0*theta2-2.0*theta-1.0)*(em*sfm+fm-lm)));
    const double lgh2b(2.0*sfm*(3.0*theta2-1.0)*(1.0+aor2*eta2+aor));
    const double lgh2a2(s1fm2gm*(1.0-aor2*eta2-aor));
    const double lgh2a1(s3fm2gm*(1.0/3.0+aor2*eta2+aor));
    const double lgh2a(3.0* sim2 *(lgh2a1+lgh2a2));
    const double lgh2(0.25*(eta2/(eta+1.0))*em*gam2p*(lgh2a+lgh2b));
    const double lgh(lpgphp+lgh2+lgh3+lgh4);
    g = normalizeZeroTwoPi(lgh-l-h);

    if (small_e > e) {
        l = g;
        g = 0.0;
    }
}
}

void meanToOsculating(const OrbitElKeplerian& mn, OrbitElKeplerian& osc)
{
    MatrixFP<double> a(1, 6, 0.0);
    MatrixFP<double> b(1, 6, 0.0);
    mn.GetElements(a);
    meanToOsculating(a, b);
    osc = OrbitElKeplerian(mn.GetEpoch(), b);
}

// -------------------------------------------------------------------------
//Transform orbit elements from osculating to mean
namespace
{
void elementsToPosVel(const MatrixFP<double>& el,
                      VectorCartesian& p, VectorCartesian& v)
{
    if (6 != el.size()) {
        throw std::runtime_error("elementsToPosVel(...)");
    }

    MatrixFP<double>::const_iterator iel(el.begin());
    // mean semimajor axis
    const double &a(iel[0]);
    // mean eccentricity
    const double &e(iel[1]);
    // mean mean anomaly
    const double &ma(iel[2]);
    // mean right ascension of the ascending node
    const double &ra(iel[3]);
    // mean inclination
    const double &i(iel[4]);
    // mean argument of perigee
    const double &ap(iel[5]);
    const double ta(anomaly(6, e, ma));
    const double slr(a * (1.0 - e*e));
    const double r(slr/(1.0+e*cos(ta)));
    const double s(sqrt(EARTH_GRAV_CONST/slr));
    const double cta(cos(ta));
    const double sta(sin(ta));
    const VectorCartesian pqwP(r*cta, r*sta, 0.0);
    const VectorCartesian pqwV(-s*sta, s*(e+cta), 0.0);
    MatrixFP<double> dcm(3,3);
    A3::dcmPqwToEci(ra, i, ap, dcm);
    p = dcm * pqwP;
    v = dcm * pqwV;
}

void posVelToElements(const VectorCartesian& pos,
                      const VectorCartesian& vel,
                      MatrixFP<double>& el)
{
    static const VectorCartesian north( 0.0, 0.0, 1.0 );

    if (6 != el.size()) {
        el.Resize(1,6);
    }

    MatrixFP<double>::iterator iel(el.begin());
    // mean semimajor axis
    double &a(iel[0]);
    // mean eccentricity
    double &e(iel[1]);
    // mean mean anomaly
    double &ma(iel[2]);
    // mean right ascension of the ascending node
    double &ra(iel[3]);
    // mean inclination
    double &i(iel[4]);
    // mean argument of perigee
    double &ap(iel[5]);
    const double radius = pos.Magnitude();
    const VectorCartesian vAngularMo = pos.CrossProduct(vel);
    const VectorCartesian uAngularMo = vAngularMo.Unit();
    const VectorCartesian vNode(-vAngularMo[1], vAngularMo[0], 0.0);
    VectorCartesian uNode(1.0, 0.0, 0.0);

    if (1.0e-12 < vNode.Magnitude()) {
        uNode = vNode.Unit();
    }

    const VectorCartesian vEcc(((vel.MagnitudeSquared()
                                 - EARTH_GRAV_CONST_NMI3_PER_S2 / radius) * pos
                                - pos.DotProduct(vel) * vel)
                               / EARTH_GRAV_CONST_NMI3_PER_S2);
    e = vEcc.Magnitude();
    VectorCartesian uEcc(1.0, 0.0, 0.0);

    if (1.0e-12 < e) {
        uEcc = vEcc.Unit();
    }

    const double slr = vAngularMo.MagnitudeSquared()
                       / EARTH_GRAV_CONST_NMI3_PER_S2;
    a = slr / (1 - e * e);
    i = acos(uAngularMo[2]);
    ra  = uNode[1] >= 0.0 ? acos(uNode[0]) : TWO_PI - acos(uNode[0]);
    const double cosArgP(testAsinAcosArg(uNode.DotProduct(uEcc)));
    ap  = uEcc[2] >= 0.0 ? acos(cosArgP) : TWO_PI - acos(cosArgP);
    const double cosTA(testAsinAcosArg(uEcc.DotProduct(pos)/radius));
    const double ta(pos.DotProduct(vel)>=0.0 ? acos(cosTA)
                    : TWO_PI-acos(cosTA));
    ma = anomaly(3, e, ta);
}

bool isLessOrEqual(const MatrixFP<double>& a, const MatrixFP<double>& b)
{
    MatrixFP<double>::const_iterator ia(a.begin()), ib(b.begin());
    return ia[0] <= ib[0]
           && ia[1] <= ib[1]
           && ia[2] <= ib[2]
           && ia[3] <= ib[3]
           && ia[4] <= ib[4]
           && ia[5] <= ib[5];
}

bool osculatingToMeanOE(const MatrixFP<double>& osc, MatrixFP<double>& mn)
{
    static const unsigned uItnMx(25);
    static const double pTol[] = {
        0.00001*KM_TO_LEN,
        1.0e-10,
        1.5e-9,  // 8.6e-8 deg
        1.5e-9,  // 8.6e-8 deg
        1.5e-9,  // 8.6e-8 deg
        1.5e-9   // 8.6e-8 deg
    };
    static const MatrixFP<double> tol(pTol, pTol+6, 1, 0);
    MatrixFP<double> osc1(1,6,0.0);
    MatrixFP<double> dOsc(1,6,0.0);

    if (6 != osc.size()) {
        throw std::runtime_error("osculatingToMeanOE(const MatrixFP<>&, MatrixFP<>&)");
    }

    if (6 != mn.size()) {
        mn.Resize(1,6);
    }

    // ---------------------------------------------------------------------
    bool bSuccess(false);
    mn = osc;

    for (unsigned i=0; !bSuccess && uItnMx>i; ++i) {
        meanToOsculating(mn, osc1);
        dOsc = osc1-osc;
        dOsc[0][2] = normalizePlusMinusPi(dOsc[0][2]);
        dOsc[0][3] = normalizePlusMinusPi(dOsc[0][3]);
        dOsc[0][5] = normalizePlusMinusPi(dOsc[0][5]);
        mn -= dOsc;
        mn[0][2] = normalizeZeroTwoPi(mn[0][2]);
        mn[0][3] = normalizeZeroTwoPi(mn[0][3]);
        mn[0][5] = normalizeZeroTwoPi(mn[0][5]);
        bSuccess = isLessOrEqual(dOsc, tol);
    }

    return bSuccess;
}

bool osculatingToMeanPV(const MatrixFP<double>& osc0, MatrixFP<double>& mn)
{
    static const double small_i(3.0e-4);  // 1.7e-2 deg
    static const unsigned uItnMx(25);
    static const double toldrc(1.0e-9);
    static const double toldsc(1.0e-9);
    static const double toldra(1.0e-6);  // 5.7e-5 deg
    static const double toldsa(1.0e-6);  // 5.7e-5 deg
    MatrixFP<double> osc1(1,6,0.0);
    MatrixFP<double> dOsc(1,6,0.0);

    if (6 != osc0.size()) {
        throw std::runtime_error("osculatingToMeanPV(const MatrixFP<>&, MatrixFP<>&)");
    }

    if (6 != mn.size()) {
        mn.Resize(1,6);
    }

    // ---------------------------------------------------------------------
    bool bSuccess(false);
    VectorCartesian po, vo, po1, vo1, pm, vm, dp, dv;
    MatrixFP<double> osc(1,6,0.0);
    elementsToPosVel(osc0, po, vo);
    posVelToElements(po, vo, osc);
    double pom(po.Magnitude()), vom(vo.Magnitude());
    MatrixFP<double>::iterator iosc(osc.begin());
    MatrixFP<double>::iterator imn(mn.begin());

    // ---------------------------------------------------------------------
    if (small_i > iosc[4]) {
        double i0;
        i0 = iosc[4];
        iosc[4] = 1.1*small_i;
        osculatingToMeanPV(osc, mn);
        iosc[4] = i0;
    } else {
        mn = osc;
    }

    // ---------------------------------------------------------------------
    for (unsigned i=0; !bSuccess && uItnMx>i; ++i) {
        double drc;
        double dsc;
        double dra;
        double dsa;
        double ftr;
        meanToOsculating(mn, osc1);
        elementsToPosVel(osc1, po1, vo1);
        dp = po1 - po;
        dv = vo1 - vo;
        elementsToPosVel(mn, pm, vm);
        ftr = 1.0 - 0.8 * static_cast<double>(i)
              / static_cast<double>(uItnMx-1);
        pm -= ftr*dp;
        vm -= ftr*dv;
        posVelToElements(pm, vm, mn);
        drc = fabs(po1.Magnitude() - pom) / pom;
        dsc = fabs(vo1.Magnitude() - vom) / vom;
        dra = po1.SeparationAngle(po);
        dsa = vo1.SeparationAngle(vo);
        bSuccess = toldrc > drc
                   && toldsc > dsc
                   && toldra > dra
                   && toldsa > dsa;
    }

    return bSuccess;
}

bool osculatingToMean(const MatrixFP<double>& osc, MatrixFP<double>& mn)
{
    static const double low_e(0.01);
    static const double low_i(0.1); // 5.7 deg
    bool bSuccess(false);
    MatrixFP<double>::const_iterator iosc(osc.begin());

    if (low_e > iosc[1] || low_i > iosc[4]) {
        bSuccess = osculatingToMeanPV(osc, mn);
    } else {
        bSuccess = osculatingToMeanOE(osc, mn);
    }

    return bSuccess;
}
}

bool osculatingToMean(const OrbitElKeplerian& osc, OrbitElKeplerian& mn)
{
    MatrixFP<double> a(1, 6, 0.0);
    MatrixFP<double> b(1, 6, 0.0);
    bool bSuccess(false);
    osc.GetElements(a);
    bSuccess = osculatingToMean(a, b);
    mn = OrbitElKeplerian(osc.GetEpoch(), b);
    return bSuccess;
}
} // A3
