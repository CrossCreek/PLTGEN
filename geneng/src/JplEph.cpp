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
 * JplEph.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// JphEph namespace, functions and classes for processing JPL ephemeris data
// Refer to JphEph.h for more info.
//===============================================================================
#include "JplEph.h"
#include "GenUtility.h"
#include "interpolChebyshevPoly.h"
//#include <iostream>
#include <iterator>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// JPL Ephemeris
namespace Jpl
{
// ---------------------------------------------------------------------
EphRec::EphRec()
    : m_pCoeff(NULL), m_sz(0) {;}

EphRec::EphRec(const EphRec& er)
    : m_tmStr(er.m_tmStr), m_tmEnd(er.m_tmEnd),
      m_pCoeff(new double[er.m_sz]), m_sz(er.m_sz)
{
    std::copy(er.m_pCoeff, er.m_pCoeff+m_sz, m_pCoeff);
}

EphRec::EphRec(unsigned sz, std::istream& is )
    : m_pCoeff(NULL), m_sz(sz)
{
    m_pCoeff = new double[m_sz];
    is.read(reinterpret_cast<char*>(m_pCoeff), sz*sizeof(double));
    m_tmStr = DAY_TO_SEC * (m_pCoeff[0] - JD_J2000_DAY);
    m_tmEnd = DAY_TO_SEC * (m_pCoeff[1] - JD_J2000_DAY);
}

EphRec::~EphRec()
{
    delete [] m_pCoeff;
}

EphRec& EphRec::operator=(const EphRec& er)
{
    if ( this != &er ) {
        if (er.m_sz != m_sz) {
            delete [] m_pCoeff;
            m_sz = er.m_sz;
            m_pCoeff = new double[m_sz];
        }

        std::copy(er.m_pCoeff, er.m_pCoeff+m_sz, m_pCoeff);
        m_tmStr = er.m_tmStr;
        m_tmEnd = er.m_tmEnd;
    }

    return *this;
}

bool EphRec::IsInInterval(const TimeJ2000& tm) const
{
    return m_tmStr <= tm && m_tmEnd >= tm;
}

// ---------------------------------------------------------------------
EphCoeff::EphCoeff()
    : m_hDesc(new EphObjDesc), m_dt(0.0), m_pCoeff(NULL) {;}

EphCoeff::EphCoeff(const EphCoeff& ec)
    : m_hDesc(ec.m_hDesc), m_tmStr(ec.m_tmStr), m_dt(ec.m_dt),
      m_pCoeff(new double[m_hDesc->TCoefficients()])
{
    std::copy(ec.m_pCoeff, ec.m_pCoeff+TCoefficients(), m_pCoeff);
}

EphCoeff::EphCoeff(Handle<EphObjDesc> hDesc,
                   const TimeJ2000& tmStr, const TimeJ2000& tmEnd,
                   EphRec::const_iterator pCoeff)
    : m_hDesc(hDesc), m_tmStr(tmStr), m_dt(tmEnd-tmStr),
      m_pCoeff(new double[hDesc->TCoefficients()])
{
    std::copy(pCoeff, pCoeff+TCoefficients(), m_pCoeff);
}

EphCoeff::~EphCoeff()
{
    delete [] m_pCoeff;
}

EphCoeff& EphCoeff::operator=(const EphCoeff& ec)
{
    if (this != &ec) {
        if (ec.TCoefficients() != TCoefficients()) {
            delete [] m_pCoeff;
            m_pCoeff = new double[ec.TCoefficients()];
        }

        m_hDesc = ec.m_hDesc;
        std::copy(ec.m_pCoeff, ec.m_pCoeff+TCoefficients(), m_pCoeff);
        m_tmStr = ec.m_tmStr;
        m_dt = ec.m_dt;
    }

    return *this;
}

template <typename T>
void EphCoeff::Component(const TimeJ2000& tm, unsigned index,
                         T& s) const
{
    double scaled(2.0*(tm-m_tmStr)/m_dt - 1.0);
    interpolChebyshevPoly(begin_component(index), end_component(index),
                          scaled, s);
}

template <typename T>
void EphCoeff::Component(const TimeJ2000& tm, unsigned index,
                         T& s, T& r) const
{
    double scaled(2.0*(tm-m_tmStr)/m_dt - 1.0);
    interpolChebyshevPolyD(begin_component(index), end_component(index),
                           scaled, s, r);
    r *= 2 / m_dt;
}

void EphCoeff::Component(const TimeJ2000& tm, VectorCartesian& s) const
{
    if (NComponents() < s.size())
        throw std::range_error(
            "EphCoeff::Component: too few components");

    const unsigned l(static_cast<unsigned>(s.size()));

    for (unsigned i = 0; l != i; ++i) {
        Component(tm, i, s[i]);
    }
}

void EphCoeff::Component(const TimeJ2000& tm,
                         VectorCartesian& s, VectorCartesian& r) const
{
    if (NComponents() < s.size())
        throw std::range_error(
            "EphCoeff::Component: too few components");

    const unsigned l(static_cast<unsigned>(s.size()));

    for (unsigned i = 0; l != i; ++i) {
        Component(tm, i, s[i], r[i]);
    }
}

EphCoeff::const_iterator EphCoeff::begin_component(unsigned index) const
{
    if (NComponents() <= index)
        throw std::range_error(
            "EphCoeff::begin_component: invalid index");

    unsigned offset(index*NCoefficients());
    return m_pCoeff+offset;
}

EphCoeff::const_iterator EphCoeff::end_component(unsigned index) const
{
    if (NComponents() <= index)
        throw std::range_error(
            "EphCoeff::end_component: invalid index");

    unsigned offset(++index*NCoefficients());
    return m_pCoeff+offset;
}

std::ostream& EphCoeff::ReportState(std::ostream& os) const
{
    os << "\nEphemeris Coefficients State\n"
       << "----------------------------\n";
    m_hDesc->ReportState(os);
    os << "\nTime Period Start: " << m_tmStr
       << "\nTime Period End:   " << TimeJ2000(m_tmStr+m_dt)
       << "\nTime Period, days: " << m_dt*SEC_TO_DAY
       << "\n\nCoefficients\n------------\n";
    const_iterator *pItr(new const_iterator[NComponents()]);
    const_iterator *pEtr(new const_iterator[NComponents()]);
    unsigned i(0);

    do {
        pItr[i] = begin_component(i);
        pEtr[i] = end_component(i);
    } while (NComponents() != ++i);

    do {
        for (i=0; NComponents()!=i; ++i) {
            os << std::setw(16) << *(pItr[i]++) << ((NComponents()-1u-i) ? '\t' : '\n');
        }
    } while(pEtr[0] != pItr[0]);

    delete [] pItr;
    delete [] pEtr;
    return os;
}

// ---------------------------------------------------------------------
EphInterval::EphInterval() : m_hDesc(new EphObjDesc) {;}

EphInterval::EphInterval(Handle<EphObjDesc> hDesc, const EphRec& rec)
    : m_hDesc(hDesc), m_tmStr(rec.TmStart()), m_tmEnd(rec.TmEnd())
{
    EphObjDesc &od(*m_hDesc);
    unsigned sz(od.TCoefficients());
    double t(m_tmStr), dt(od.GranuleInterval()*DAY_TO_SEC);
    EphRec::const_iterator itr(rec.begin()+od.Location());
    EphRec::const_iterator etr(itr+sz);
    EphCoeff ec;

    do {
        ec = EphCoeff(m_hDesc, t, t+dt, itr);
        m_ctCoeff[t] = ec;
        t+=dt;
        itr += sz;
    } while(etr != itr);
}

EphInterval::EphInterval(Handle<EphObjDesc> hDesc,
                         EphRecVec::const_iterator itr, EphRecVec::const_iterator etr)
    : m_hDesc(hDesc), m_tmStr(itr->TmStart()), m_tmEnd((etr-1)->TmEnd())
{
    EphObjDesc &od(*m_hDesc);
    EphRec::const_iterator itrER, etrER;
    unsigned sz(od.TCoefficients()*od.NGranules());
    double dt(od.GranuleInterval()*DAY_TO_SEC);

    do {
        itrER = itr->begin()+od.Location();
        etrER = itrER+sz;
        AddCoefficients(itr->TmStart(), dt, itrER, etrER );
    } while (etr != ++itr);
}

void EphInterval::AddCoefficients(double t, double dt,
                                  EphRec::const_iterator itr, EphRec::const_iterator etr)
{
    unsigned sz(m_hDesc->TCoefficients());
    EphCoeff ec;

    // std::cout << "\n\nName:        " << m_hDesc->Id()
    //           << "\nDistance:    " << std::distance(itr, etr)
    //           << "\nTotal Coeff: " << sz
    //           << "\nd/t =        " << std::distance(itr, etr)/sz
    //           << std::endl;

    do {
        ec = EphCoeff(m_hDesc, t, t+dt, itr);
        m_ctCoeff[t] = ec;
        t+=dt;
        itr += sz;
    } while(etr != itr);
}

void EphInterval::Coefficients(const TimeJ2000& tm, EphCoeff& ec) const
{
    if (m_tmStr > tm || m_tmEnd < tm)
        throw std::range_error(
            "EphInterval::Coefficients: time out of range");

    std::map<TimeJ2000, EphCoeff>::const_iterator
    itr = --m_ctCoeff.upper_bound(tm);
    // DEBUG START
    // std::cout << "\nRequested Time:\n" << tm
    //           << "\nMap Iterator Time:\n" << itr->first
    //           << "\nCoefficient Period:\n"
    //           << itr->second.TimePeriod().first << '\n'
    //           << itr->second.TimePeriod().second << std::endl;
    // if (m_ctCoeff.end() != ++itr) {
    //     std::cout << "\nUpper Bound Map Iterator Time:\n" << itr->first
    //               << "\nUpper Bound Coefficient Period:\n"
    //               << itr->second.TimePeriod().first << '\n'
    //               << itr->second.TimePeriod().second << std::endl;
    //     --itr;
    // }
    // DEBUG END
    ec = itr->second;
}

std::ostream& EphInterval::ReportState(std::ostream& os) const
{
    os << "\nEphemeris Interval State\n"
       << "------------------------\n";
    m_hDesc->ReportState(os);
    EphRecVec::size_type
    tGranules(Eph::NRecBuf()*m_hDesc->NGranules());
    os << "\nTime Interval Start: " << m_tmStr
       << "\nTime Interval End:   " << m_tmEnd
       << "\nTime Interval, days: " << (m_tmEnd-m_tmStr)*SEC_TO_DAY
       << "\nRecords in Interval: " << Eph::NRecBuf()
       << "\nTotal Granules:      " << tGranules;
    const_iterator itr(m_ctCoeff.begin());
    const_iterator etr(m_ctCoeff.end());

    do {
        os << "\n\n[" << int(tGranules)-int(std::distance(itr, etr))
           << "] Ephemeris Coefficient Key Time: "
           << itr->first << '\n';
        itr->second.ReportState(os);
    } while (etr != ++itr);

    return os;
}

// ---------------------------------------------------------------------
const EphRecVec::size_type Eph::s_nRec(4);
const int                  Eph::s_nRecBack(1);

Eph::Eph() : m_ctRec(s_nRec), m_fileNm("bin.421")
    // Eph::Eph() : m_ctRec(s_nRec), m_fileNm("bin.405")
{
    ;
}

void Eph::Init()
{
    EphHdr hdr("header.421e", m_odm, m_ecm);
    // EphHdr hdr("header.405e", m_odm, m_ecm);
    // hdr.ReportState(std::cout);
}

const EphRec& Eph::Record( const TimeJ2000& tm)
{
    ChkInit();
    Handle<std::istream> his(findOpenStdFile(m_fileNm));

    if (!his.IsNullObject()) {
        double jd(tm/DAY_TO_SEC+JD_J2000_DAY);
        std::istream& is(*his);
        is.read(reinterpret_cast<char*>(&m_hdr), sizeof(CoeffHdr));
        // ReportHeader(std::cout << '\n') << std::endl;
        const unsigned recBytes(m_hdr.recLen*sizeof(double));
        unsigned curRecNo(static_cast<unsigned>(jd-m_hdr.t0)
                          /m_hdr.intervalDays);
        std::streampos pos;

        // std::streampos pos(m_hdr.posRec1 + curRecNo*recBytes);
        // his->seekg(pos);
        // m_hCurRec = new EphRec(m_hdr.recLen, *his);

        if (curRecNo+s_nRec-s_nRecBack > m_hdr.recCnt) {
            curRecNo = m_hdr.recCnt - s_nRec + s_nRecBack;
        }

        for (EphRecVec::size_type i=0; s_nRec > i; ++i) {
            pos = m_hdr.posRec1 + (curRecNo+i-s_nRecBack)*recBytes;
            his->seekg(pos);
            m_ctRec[i] = EphRec(m_hdr.recLen, *his);
        }
    } else {
        std::stringstream stmMsg;
        stmMsg << "Cannot open JPL ephemeris file\n"
               << "database name = " << m_fileNm << std::endl;
        THROW_RUNTIME_ERROR(stmMsg.str());
    }

    return m_ctRec[s_nRecBack];
}

void Eph::Interval(EphObjId id, const TimeJ2000& tm, EphInterval& itvl)
{
    // if ( !m_hCurRec->IsInInterval(tm) ) Record(tm);
    // itvl = EphInterval(m_odm[id], *m_hCurRec);
    if (m_ctRec.front().TmStart()>tm || m_ctRec.back().TmEnd()<tm) {
        Record(tm);
    }

    itvl = EphInterval(m_odm[id], m_ctRec.begin(), m_ctRec.end());
}

std::ostream& Eph::ReportHeader(std::ostream& os)
{
    os << "Coefficients Header\n-------------------"
       << "\nName:                 " << m_hdr.name
       << "\nVersion:              " << m_hdr.version
       << "\nTime Start:           " << m_hdr.t0
       << "\nTime End:             " << m_hdr.t1
       << "\nInterval, days:       " << m_hdr.intervalDays
       << "\nRecord Length:        " << m_hdr.recLen
       << "\nRecord Count:         " << m_hdr.recCnt
       << "\nPosition of Record 1: " << m_hdr.posRec1 << '\n';
    return os;
}

// ---------------------------------------------------------------------
void EphObjBase::CondUpdate(const TimeJ2000 &tm)
{
    if (!m_coeff.IsInSpan(tm)) {
        if (!m_itvl.IsInSpan(tm)) {
            eph().Interval(m_id, tm, m_itvl);
        }

        m_itvl.Coefficients(tm, m_coeff);
    }
}

// ---------------------------------------------------------------------
Angle Nutations::Longitude(const TimeJ2000 &tm)
{
    if (m_tmL != tm) {
        CondUpdate(tm);
        m_coeff.Component(tm, 0, m_long);
        m_tmL = tm;
    }

    return m_long;
}

Angle Nutations::Obliquity(const TimeJ2000 &tm)
{
    if (m_tmQ != tm) {
        CondUpdate(tm);
        m_coeff.Component(tm, 1, m_oblq);
        m_tmQ = tm;
    }

    return m_oblq;
}

void Nutations::LongRate(const TimeJ2000 &tm, Angle &lon, Angle &rate)
{
    CondUpdate(tm);
    m_coeff.Component(tm, 0, m_long, rate);
    m_tmL = tm;
    lon = m_long;
}

void Nutations::OblqRate(const TimeJ2000 &tm, Angle &oblq, Angle &rate)
{
    CondUpdate(tm);
    m_coeff.Component(tm, 1, m_oblq, rate);
    m_tmQ = tm;
    oblq = m_oblq;
}

// ---------------------------------------------------------------------
template <EphObjId id>
void EphObjSSB<id>::Position(const TimeJ2000 &tm, VectorCartesian& p)
{
    if (m_tmP != tm) {
        CondUpdate(tm);
        m_coeff.Component(tm, m_pos);
        m_pos *= KM_TO_LEN;
        m_tmP = tm;
    }

    p = m_pos;
}

template <EphObjId id>
void EphObjSSB<id>::Velocity(const TimeJ2000 &tm, VectorCartesian& v)
{
    if (m_tmV != tm) {
        UpdatePosVel(tm);
    }

    v = m_vel;
}

template <EphObjId id>
void EphObjSSB<id>::PosVel(const TimeJ2000 &tm,
                           VectorCartesian& p, VectorCartesian& v)
{
    if (m_tmV != tm || m_tmP != tm) {
        UpdatePosVel(tm);
    }

    p = m_pos;
    v = m_vel;
}

template <EphObjId id>
void EphObjSSB<id>::UpdatePosVel(const TimeJ2000 &tm)
{
    CondUpdate(tm);
    m_coeff.Component(tm, m_pos, m_vel);
    m_pos *= KM_TO_LEN;
    m_vel *= KM_TO_LEN;
    m_tmP = tm;
    m_tmV = tm;
}

template class EphObjSSB<MERCURY>;
template class EphObjSSB<VENUS>;
template class EphObjSSB<EMB>;
template class EphObjSSB<MARS>;
template class EphObjSSB<JUPITER>;
template class EphObjSSB<SATURN>;
template class EphObjSSB<URANUS>;
template class EphObjSSB<NEPTUNE>;
template class EphObjSSB<PLUTO>;
// template class EphObjSSB<MOON>;
template class EphObjSSB<SUN>;

void EphObjSSB<EARTH>::Position(const TimeJ2000 &tm, VectorCartesian& p)
{
    if (m_tmP != tm) {
        CondUpdate(tm);
        m_coeff.Component(tm, m_pos);
        m_pos *= KM_TO_LEN;
        VectorCartesian posMoon;
        m_moon.Position(tm, posMoon);
        // m_pos -= s_EMRAT1 * posMoon;
        m_pos -= posMoon / (1.0+EARTH_MOON_MASS_RATIO);
        m_tmP = tm;
    }

    p = m_pos;
}

void EphObjSSB<EARTH>::Velocity(const TimeJ2000 &tm, VectorCartesian& v)
{
    if (m_tmV != tm) {
        UpdatePosVel(tm);
    }

    v = m_vel;
}

void EphObjSSB<EARTH>::PosVel(const TimeJ2000 &tm,
                              VectorCartesian& p, VectorCartesian& v)
{
    if (m_tmV != tm || m_tmP != tm) {
        UpdatePosVel(tm);
    }

    p = m_pos;
    v = m_vel;
}

void EphObjSSB<EARTH>::UpdatePosVel(const TimeJ2000 &tm)
{
    CondUpdate(tm);
    m_coeff.Component(tm, m_pos, m_vel);
    m_pos *= KM_TO_LEN;
    m_vel *= KM_TO_LEN;
    VectorCartesian posMoon, velMoon;
    m_moon.PosVel(tm, posMoon, velMoon);
    // m_pos -= s_EMRAT1 * posMoon;
    // m_vel -= s_EMRAT1 * velMoon;
    m_pos -= posMoon /(1.0+EARTH_MOON_MASS_RATIO);
    m_vel -= velMoon /(1.0+EARTH_MOON_MASS_RATIO);
    m_tmP = tm;
    m_tmV = tm;
}

// template class EphObjSSB< EARTH >;

void EphObjSSB<EARTH>::MoonGC
::Position(const TimeJ2000 &tm, VectorCartesian& p)
{
    if (m_tmP != tm) {
        CondUpdate(tm);
        m_coeff.Component(tm, m_pos);
        m_pos *= KM_TO_LEN;
        m_tmP = tm;
    }

    p = m_pos;
}

void EphObjSSB<EARTH>::MoonGC
::PosVel(const TimeJ2000 &tm,
         VectorCartesian& p, VectorCartesian& v)
{
    if (m_tmV != tm || m_tmP != tm) {
        CondUpdate(tm);
        m_coeff.Component(tm, m_pos, m_vel);
        m_pos *= KM_TO_LEN;
        m_vel *= KM_TO_LEN;
        m_tmP = tm;
        m_tmV = tm;
    }

    p = m_pos;
    v = m_vel;
}

// ---------------------------------------------------------------------
template <EphObjId id>
void EphObjGC<id>::Position(const TimeJ2000 &tm, VectorCartesian& p)
{
    if (m_tmP != tm) {
        CondUpdate(tm);
        m_coeff.Component(tm, m_pos);
        m_pos *= KM_TO_LEN;
        VectorCartesian posEarth;
        m_earth.Position(tm, posEarth);
        m_pos -= posEarth;
        m_tmP = tm;
    }

    p = m_pos;
}

template <EphObjId id>
void EphObjGC<id>::Velocity(const TimeJ2000 &tm, VectorCartesian& v)
{
    if (m_tmV != tm) {
        UpdatePosVel(tm);
    }

    v = m_vel;
}

template <EphObjId id>
void EphObjGC<id>::PosVel(const TimeJ2000 &tm,
                          VectorCartesian& p, VectorCartesian& v)
{
    if (m_tmV != tm || m_tmP != tm) {
        UpdatePosVel(tm);
    }

    p = m_pos;
    v = m_vel;
}

template <EphObjId id>
void EphObjGC<id>::UpdatePosVel(const TimeJ2000 &tm)
{
    CondUpdate(tm);
    m_coeff.Component(tm, m_pos, m_vel);
    m_pos *= KM_TO_LEN;
    m_vel *= KM_TO_LEN;
    VectorCartesian posEarth, velEarth;
    m_earth.PosVel(tm, posEarth, velEarth);
    m_pos -= posEarth;
    m_vel -= velEarth;
    m_tmP = tm;
    m_tmV = tm;
}

template class EphObjGC<MERCURY>;
template class EphObjGC<VENUS>;
template class EphObjGC<EMB>;
template class EphObjGC<MARS>;
template class EphObjGC<JUPITER>;
template class EphObjGC<SATURN>;
template class EphObjGC<URANUS>;
template class EphObjGC<NEPTUNE>;
template class EphObjGC<PLUTO>;
template class EphObjGC<SUN>;

void EphObjGC<MOON>::Position(const TimeJ2000 &tm, VectorCartesian& p)
{
    if (m_tmP != tm) {
        CondUpdate(tm);
        m_coeff.Component(tm, m_pos);
        m_pos *= KM_TO_LEN;
        m_tmP = tm;
    }

    p = m_pos;
}

void EphObjGC<MOON>::Velocity(const TimeJ2000 &tm, VectorCartesian& v)
{
    if (m_tmV != tm) {
        UpdatePosVel(tm);
    }

    v = m_vel;
}

void EphObjGC<MOON>::PosVel(const TimeJ2000 &tm,
                            VectorCartesian& p, VectorCartesian& v)
{
    if (m_tmV != tm || m_tmP != tm) {
        UpdatePosVel(tm);
    }

    p = m_pos;
    v = m_vel;
}

void EphObjGC<MOON>::UpdatePosVel(const TimeJ2000 &tm)
{
    CondUpdate(tm);
    m_coeff.Component(tm, m_pos, m_vel);
    m_pos *= KM_TO_LEN;
    m_vel *= KM_TO_LEN;
    m_tmP = tm;
    m_tmV = tm;
}

// template class EphObjGC< MOON >;

}
}
