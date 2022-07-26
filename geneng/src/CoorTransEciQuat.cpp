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
 * CoorTransEciQuat.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Quaternion w.r.t. ECI(TOD) coordinate transformation class
// Refer to CoorTransEciQuat.h for more info.
// =============================================================================
#include "CoorTransEciQuat.h"
#include "GenUtility.h"
#include "interpolQuaternion.h"
#include "ioDelim.h"
#include <fstream>
#include <iostream>
#include <iomanip>

namespace A3
{
CoorTransEciQuat::CoorTransEciQuat(HandleConst<TimeJ2000> hTime,
                                   std::istream& is)
    : CoorTransBaseEci(CoorSystem::QUAT), m_hTime(hTime)
{
    try {
        static const size_t szReserve(180);
        EIClrSet(POS_EI);
        m_vTime.reserve(szReserve);
        m_vQuat.reserve(szReserve);
        TimeJ2000 tm;
        Quaternion q;

        while (is >> tm >> q ) {
            InsertTimeQuat(tm, q);
        }
    } catch (std::exception& e) {
        TRACE_EXCEPT(e);
    }
}

CoorTransEciQuat::CoorTransEciQuat(HandleConst<TimeJ2000> hTime)
    : CoorTransBaseEci(CoorSystem::QUAT), m_hTime(hTime)
{
    try {
        static const size_t szReserve(180);
        EIClrSet(POS_EI);
        m_vTime.reserve(szReserve);
        m_vQuat.reserve(szReserve);
    } catch (std::exception& e) {
        TRACE_EXCEPT(e);
    }
}

CoorTransEciQuat::CoorTransEciQuat(const CoorTransEciQuat& ct)
    : CoorTransBaseEci(ct), m_hTime(ct.m_hTime),
      m_tmUpdate(ct.m_tmUpdate), m_qtrn(ct.m_qtrn),
      m_vTime(ct.m_vTime), m_vQuat(ct.m_vQuat)
{
    m_hint = m_vTime.begin();
}

CoorTransEciQuat& CoorTransEciQuat::operator=(const CoorTransEciQuat& ct)
{
    try {
        if ( this != &ct ) {
            CoorTransBaseEci::operator=(ct);
            m_hTime    = ct.m_hTime;
            m_tmUpdate = ct.m_tmUpdate;
            m_qtrn     = ct.m_qtrn;
            m_vTime    = ct.m_vTime;
            m_vQuat    = ct.m_vQuat;
            m_hint = m_vTime.begin();
        }
    } catch (std::exception& e) {
        TRACE_EXCEPT(e);
    }

    return *this;
}

void CoorTransEciQuat::InsertTimeQuat(const TimeJ2000& tm, const Quaternion& q)
{
    try {
        if (m_vTime.empty() || tm > m_vTime.back()) {
            m_vTime.push_back(tm);
            m_vQuat.push_back(q);
        } else {
            std::vector<TimeJ2000>::iterator
            itr(std::lower_bound(m_vTime.begin(), m_vTime.end(), tm));
            std::vector<TimeJ2000>::difference_type
            dst(std::distance(m_vTime.begin(), itr));

            if (*itr == tm) {
                *itr = tm;
                m_vQuat[dst] = q;
            } else {
                m_vTime.insert(itr, tm);
                m_vQuat.insert(m_vQuat.begin()+dst, q);
            }
        }

        m_hint = m_vTime.begin();
    } catch (std::exception& e) {
        TRACE_EXCEPT(e);
    }
}

void CoorTransEciQuat::UpdateState()
{
    try {
        if (m_vTime.empty()) {
            THROW_RUNTIME_ERROR("Missing quaternions!");
        }

        TimeJ2000 tm0(*m_hTime);

        if (tm0 < m_vTime.front()) {
            m_qtrn = m_vQuat.front();
        } else if (tm0 > m_vTime.back()) {
            m_qtrn = m_vQuat.back();
        } else {
            m_hint = interpolQuaternion(m_vTime.begin(), m_vTime.end(), m_hint,
                                        m_vQuat.begin(), tm0, m_qtrn);
        }

        Dcm() = CoorTransDcm(m_qtrn);
        m_tmUpdate = tm0;
    } catch (std::exception& e) {
        TRACE_EXCEPT(e);
    }
}

bool CoorTransEciQuat::operator==(const CoorTransEciQuat& coorTrans) const
{
    return CoorTransBaseEci::operator==(coorTrans);
}

bool CoorTransEciQuat::operator!=(const CoorTransEciQuat& coorTrans) const
{
    return !operator==(coorTrans);
}

std::ostream& CoorTransEciQuat::DumpTimeQuat(std::ostream& os)
{
    const std::vector<TimeJ2000>::size_type sz(m_vTime.size());
    std::vector<TimeJ2000>::size_type i(0);

    for (; sz > i; ++i)
        os << m_vTime[i] << ioDelim << std::setw(2)
           << m_vQuat[i] << std::endl;

    return os;
}

} // A3 namespace end
