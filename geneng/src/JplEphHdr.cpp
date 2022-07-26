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
 * JplEphHdr.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// JphEph namespace, functions and classes for processing JPL ephemeris data
// Refer to JphEphHdr.h for more info.
//===============================================================================
#include "JplEphHdr.h"
#include <ostream>
#include <algorithm>
#include <stdexcept>

// Astrodynamics, Allocation and Availability
namespace A3
{
// JPL Ephemeris
namespace Jpl
{
// EphObjIdMap containing JPL Ephemeris Object Ids
EphObjIdMap::EphObjIdMap()
{
    /*insert(value_type(MERCURY, "Mercury"));
    insert(value_type(VENUS, "Venus"));
    insert(value_type(EMB, "EMB"))
    ->second.insert("Earth-Moon Barycenter");
    insert(value_type(MARS, "Mars"));
    insert(value_type(JUPITER, "Jupiter"));
    insert(value_type(SATURN, "Saturn"));
    insert(value_type(URANUS, "Uranus"));
    insert(value_type(NEPTUNE, "Neptune"));
    insert(value_type(PLUTO, "Pluto"));
    insert(value_type(MOON, "Moon"));
    insert(value_type(SUN, "Sun"));
    insert(value_type(NUTATIONS, "Nutations"));
    insert(value_type(LIBRATIONS, "Librations"));
    insert(value_type(INVALID_OBJECT, "Invalid Object"));*/
};

// ---------------------------------------------------------------------
EphObjDesc::EphObjDesc()
    : m_idObj(INVALID_OBJECT), m_nComponents(0), m_tblLoc(0), m_nCoeff(0),
      m_nGranules(0), m_nIntervalDays(0), m_nGranuleDays(0)
{;}

EphObjDesc::EphObjDesc(EphObjId id, unsigned nComp, unsigned nIntervalDays)
    : m_idObj(id), m_nComponents(nComp), m_tblLoc(0), m_nCoeff(0),
      m_nGranules(0), m_nIntervalDays(nIntervalDays), m_nGranuleDays(0) {;}

void EphObjDesc::Lcg(const G1050::LCG& lcg)
{
    m_tblLoc = lcg.m_tblLoc;
    m_nCoeff = lcg.m_nCoeff;
    m_nGranules = lcg.m_nGranules;

    if (!m_nGranules)
        throw std::runtime_error(
            "EphObjDesc::Lcg(const LCG&): #granules = 0");

    m_nGranuleDays = m_nIntervalDays / m_nGranules;

    if (m_nIntervalDays != m_nGranules*m_nGranuleDays)
        throw std::runtime_error(
            "EphObjDesc::Lcg(const LCG&): granules days not integral");
}

std::ostream& EphObjDesc::ReportState(std::ostream& os) const
{
    os << "JPL Object Description"
       << "\nObject Id:              " << m_idObj
       << "\nObject Name:            " << ephObjIds()(m_idObj)
       << "\nTable Location:         " << m_tblLoc << " (zero base)"
       << "\nNumber of Components:   " << m_nComponents
       << "\nNumber of Coefficients: " << m_nCoeff
       << "\nNumber of Granules:     " << m_nGranules
       << "\nRecord Interval:        " << m_nIntervalDays << " days"
       << "\nGranule Interval:       " << m_nGranuleDays << " days"
       << '\n' << std::endl;
    return os;
}

// ---------------------------------------------------------------------
std::ostream& ReportState(std::ostream& os, const EphObjDescMap& odm)
{
    EphObjDescMap::const_iterator itr(odm.begin());
    EphObjDescMap::const_iterator etr(odm.end());

    while (etr!=itr) {
        (itr++)->second->ReportState(os);
    }

    return os;
}

std::ostream&
operator<<(std::ostream& os, EphConstMap::value_type e)
{
    return os << e.first << ' ' << e.second;
}

std::ostream& ReportState(std::ostream& os, const EphConstMap& ecm)
{
    os << "JPL Ephemeris Group 1041\nConstant Count: "
       << ecm.size() << '\n';
    EphConstMap::const_iterator itr(ecm.begin());

    while (ecm.end() != itr) {
        os << *(itr++) << '\n';
    }

    os << std::endl;
    return os;
}
// ---------------------------------------------------------------------
// EphHdr: JPL Ephemeris Header
EphHdr::EphHdr(const std::string fileNm, EphObjDescMap &odm,
               EphConstMap &ecm)
    : m_fileNm(fileNm)
{
    Handle<std::istream> his(findOpenStdFile(m_fileNm));

    if (!his.IsNullObject()) {
        m_g0000.Init(*his);
        m_g1010.Init(*his);
        m_g1030.Init(*his);
        Init(*his, ecm);
        Init(*his, odm);
        m_ecm = ecm;
        m_odm = odm;
    } else {
        std::stringstream stmMsg;
        stmMsg << "Cannot open JPL header\n"
               << "database name = " << m_fileNm << std::endl;
        THROW_RUNTIME_ERROR(stmMsg.str());
    }
}

std::istream& EphHdr::Init(std::istream& is, EphObjDescMap& odm)
{
    unsigned interval(m_g1030.Interval());
    G1050 g1050;
    g1050.Init(is);
    odm[MERCURY]
        = Handle<EphObjDesc>(new EphObjDesc(MERCURY, 3, interval));
    odm[VENUS]
        = Handle<EphObjDesc>(new EphObjDesc(VENUS, 3, interval));
    odm[EMB]
        = Handle<EphObjDesc>(new EphObjDesc(EMB, 3, interval));
    odm[MARS]
        = Handle<EphObjDesc>(new EphObjDesc(MARS, 3, interval));
    odm[JUPITER]
        = Handle<EphObjDesc>(new EphObjDesc(JUPITER, 3, interval));
    odm[SATURN]
        = Handle<EphObjDesc>(new EphObjDesc(SATURN, 3, interval));
    odm[URANUS]
        = Handle<EphObjDesc>(new EphObjDesc(URANUS, 3, interval));
    odm[NEPTUNE]
        = Handle<EphObjDesc>(new EphObjDesc(NEPTUNE, 3, interval));
    odm[PLUTO]
        = Handle<EphObjDesc>(new EphObjDesc(PLUTO, 3, interval));
    odm[MOON]
        = Handle<EphObjDesc>(new EphObjDesc(MOON, 3, interval));
    odm[SUN]
        = Handle<EphObjDesc>(new EphObjDesc(SUN, 3, interval));
    odm[NUTATIONS]
        = Handle<EphObjDesc>(new EphObjDesc(NUTATIONS, 2, interval));
    odm[LIBRATIONS]
        = Handle<EphObjDesc>(new EphObjDesc(LIBRATIONS, 3, interval));
    EphObjDescMap::iterator itr(odm.begin());
    EphObjDescMap::iterator etr(odm.end());
    G1050::const_iterator itr1050(g1050.begin());
    const G1050::const_iterator etr1050(g1050.end());

    while (etr!=itr && etr1050!=itr1050) {
        (itr++)->second->Lcg(*(itr1050++));
    }

    return is;
}

std::istream& EphHdr::Init(std::istream& is, EphConstMap& ecm)
{
    G1040 g1040;
    G1041 g1041;
    g1040.Init(is);
    g1041.Init(is);

    if (g1040.size() != g1041.size()) {
        throw std::runtime_error("JPL Ephemeris Header bad constants");
    }

    G1040::const_iterator itr0(g1040.begin());
    G1040::const_iterator etr0(g1040.end());
    G1041::const_iterator itr1(g1041.begin());

    for(; etr0 != itr0; ++itr0, ++itr1) {
        ecm[*itr0] = *itr1;
    }

    return is;
}

std::ostream& EphHdr::ReportState(std::ostream& os) const
{
    m_g0000.ReportState(os);
    m_g1010.ReportState(os);
    m_g1030.ReportState(os);
    A3::Jpl::ReportState(os, m_ecm);
    A3::Jpl::ReportState(os, m_odm);
    return os;
}

// ---------------------------------------------------------------------
}
}
