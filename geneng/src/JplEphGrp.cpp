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
 * JplEphGrp.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// JphEph namespace, functions and classes for processing JPL ephemeris data
// Refer to JphEphGrp.h for more info.
//===============================================================================
#include "JplEphGrp.h"
#include "GenUtility.h"
#include <iostream>
#include <iterator>
#include <algorithm>

/// Astrodynamics, Allocation and Availability

namespace A3
{
// JPL Ephemeris
namespace Jpl
{
// ---------------------------------------------------------------------
// JPH Ephemeris Group 0000
G0000::G0000() : m_ksize(0), m_ncoeff(0) { ; }

std::istream& G0000::Init(std::istream& is)
{
    std::string sBuf;
    is >> sBuf >> m_ksize;
    is >> sBuf >> m_ncoeff;
    return is;
}

std::ostream& G0000::ReportState(std::ostream& os) const
{
    os << "JPL Ephemeris Group 0000\nKSIZE = " << m_ksize
       << "\nNCOEFF = " << m_ncoeff << '\n' << std::endl;
    return os;
}

// ---------------------------------------------------------------------
// JPH Ephemeris Group 1010
G1010::G1010() { ; }

std::istream& G1010::Init(std::istream& is)
{
    std::string sBuf;
    is >> sBuf >> m_sEphName >> m_sEphStr >> m_sEphEnd;
    std::getline(is, sBuf);
    std::getline(is, m_sEphName);
    std::getline(is, m_sEphStr);
    std::getline(is, m_sEphEnd);
    return is;
}

std::ostream& G1010::ReportState(std::ostream& os) const
{
    os << "JPL Ephemeris Group 1010\n"
       << m_sEphName << '\n'
       << m_sEphStr << '\n'
       << m_sEphEnd << '\n' << std::endl;
    return os;
}

// ---------------------------------------------------------------------
// JPH Ephemeris Group 1030
G1030::G1030()
    : m_jdEphStr(0.0), m_jdEphEnd(0.0), m_intervalDays(0) {;}

std::istream& G1030::Init(std::istream& is)
{
    std::string sBuf;
    is >> sBuf >> m_jdEphStr >> m_jdEphEnd >> m_intervalDays;
    return is;
}

std::ostream& G1030::ReportState(std::ostream& os) const
{
    std::ios_base::fmtflags ff =
        os.setf(std::ios_base::fixed, std::ios_base::floatfield);
    os << "JPL Ephemeris Group 1030\nEphemeris Start: " << m_jdEphStr
       << " JD Days"
       << "\nEphemeris End:   "  << m_jdEphEnd
       << " JD Days" << std::endl;
    os.flags(ff);
    return os;
}

// ---------------------------------------------------------------------
// JPH Ephemeris Group 1040
G1040::G1040() { ; }

std::istream& G1040::Init(std::istream& is)
{
    std::string sBuf;
    unsigned count(0);
    is >> sBuf >> count;
    m_ctConstNm.clear();
    m_ctConstNm.reserve(count);

    for (unsigned i=0; count>i; ++i) {
        is >> sBuf;
        m_ctConstNm.push_back(sBuf);
    }

    return is;
}

std::ostream& G1040::ReportState(std::ostream& os) const
{
    os << "JPL Ephemeris Group 1040\nConstant Name Count: "
       << size() << '\n';
    std::ostream_iterator<std::string> osi(os, "\n");
    std::copy(begin(), end(), osi);
    os << '\n' << std::endl;
    return os;
}

// ---------------------------------------------------------------------
// JPH Ephemeris Group 1041
G1041::G1041() { ; }

std::istream& G1041::Init(std::istream& is)
{
    unsigned count(0);
    std::string sBuf;
    double d(0.0);
    is >> sBuf >> count;
    m_ctConst.clear();
    m_ctConst.reserve(count);

    for (unsigned i=0; count>i; ++i) {
        is >> d;
        m_ctConst.push_back(d);
    }

    return is;
}

std::ostream& G1041::ReportState(std::ostream& os) const
{
    os << "JPL Ephemeris Group 1041\nConstant Count: "
       << size() << '\n';
    std::ostream_iterator<double> osi(os, "\n");
    std::copy(begin(), end(), osi);
    os << '\n' << std::endl;
    return os;
}

// ---------------------------------------------------------------------
// JPH Ephemeris Group 1050
std::istream& G1050::Init(std::istream& is)
{
    const unsigned nObj(13);
    unsigned i(0);
    std::vector<unsigned> ctTblLoc(nObj, 0);
    std::vector<unsigned> ctNCoeff(nObj, 0);
    std::vector<unsigned> ctNGranules(nObj, 0);
    std::string sBuf;
    findString(is, "GROUP", true);
    findString(is, "1050", true);
    is >> sBuf;

    for (i=0; nObj>i; ++i) {
        is >> ctTblLoc[i];
    }

    for (i=0; nObj>i; ++i) {
        is >> ctNCoeff[i];
    }

    for (i=0; nObj>i; ++i) {
        is >> ctNGranules[i];
    }

    m_ctLcg.clear();
    m_ctLcg.reserve(nObj);

    for (i=0; nObj>i; ++i)
        m_ctLcg.push_back(LCG(ctTblLoc[i]-1, // convert from 1 to 0 base
                              ctNCoeff[i],ctNGranules[i]));

    return is;
}

std::ostream& G1050::ReportState(std::ostream& os) const
{
    os << "JPL Ephemeris Group 1050\nLCG Count: "
       << size() << '\n';
    std::ostream_iterator<LCG> osi(os, "\n");
    std::copy(begin(), end(), osi);
    os << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const G1050::LCG& lcg)
{
    os << lcg.m_tblLoc << ' ' << lcg.m_nCoeff << ' ' << lcg.m_nGranules;
    return os;
}

// ---------------------------------------------------------------------
}
}
