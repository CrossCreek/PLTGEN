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
 * IoManip.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Manipulator base class class
// Refer to IoManip.h for more info.
// =============================================================================
#include "IoManip.h"
#include <string>

// A3 namespace start
namespace A3
{
template <typename FTYP>
IoManip<FTYP>::IoManip()
    : std::locale::facet(0),
      m_pCntnrFrmt(new Cntnr),
      m_pRefCnt(new size_t(1)) {;}

template <typename FTYP>
IoManip<FTYP>::IoManip(FTYP dflt, size_t refs)
    : std::locale::facet(refs),
      m_pCntnrFrmt(new Cntnr(1, dflt)),
      m_pRefCnt(new size_t(1)) {;}

template <typename FTYP>
IoManip<FTYP>::IoManip(const IoManip& iom)
    : std::locale::facet(0),
      m_pCntnrFrmt(iom.m_pCntnrFrmt),
      m_pRefCnt(iom.m_pRefCnt)
{
    ++(*m_pRefCnt);
}

template <typename FTYP>
IoManip<FTYP>::IoManip(const IoManip& iom, size_t refs)
    : std::locale::facet(refs),
      m_pCntnrFrmt(iom.m_pCntnrFrmt),
      m_pRefCnt(iom.m_pRefCnt)
{
    ++(*m_pRefCnt);
}

template <typename FTYP>
IoManip<FTYP>::~IoManip()
{
    if (1 > --(*m_pRefCnt)) {
        delete m_pCntnrFrmt;
        delete m_pRefCnt;
    }
}

template <typename FTYP>
IoManip<FTYP>& IoManip<FTYP>::operator=(const IoManip<FTYP>& iom)
{
    if (&iom != this) {
        if (1 > --(*m_pRefCnt)) {
            delete m_pCntnrFrmt;
            delete m_pRefCnt;
        }

        m_pCntnrFrmt = iom.m_pCntnrFrmt;
        m_pRefCnt = iom.m_pRefCnt;
        ++m_pRefCnt;
    }

    return *this;
}

// -------------------------------------------------------------------------
// set a streams format
template <typename FTYP>
void IoManip<FTYP>::Set(FTYP fmt) const
{
    static const size_type MAX_UNDO(10);

    if (MAX_UNDO <= m_pCntnrFrmt->size()) {
        m_pCntnrFrmt->pop_front();
    }

    m_pCntnrFrmt->push_back(fmt);
}

// -------------------------------------------------------------------------
// restores a streams last format
template <typename FTYP>
void IoManip<FTYP>::Restore() const
{
    if (1 < m_pCntnrFrmt->size()) {
        m_pCntnrFrmt->pop_back();
    }
}

// -------------------------------------------------------------------------
// check a streams format
template <typename FTYP>
FTYP IoManip<FTYP>::Get() const
{
    return m_pCntnrFrmt->back();
}

// -------------------------------------------------------------------------
// explicit instantiation
template class IoManip<char>;
template class IoManip<unsigned char>;
template class IoManip<short>;
template class IoManip<unsigned short>;
template class IoManip<int>;
template class IoManip<unsigned>;
template class IoManip<std::string>;

} // A3 namespace end
