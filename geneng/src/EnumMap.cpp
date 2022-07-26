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
 * EnumMap.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// FlagLbl and EnumMap Classes
// Refer to EnumMap.h for more info.
// =============================================================================
#include "EnumMap.h"
#include <algorithm>

namespace A3
{
// EnumLbls helper functions
namespace
{
bool lesserStrSz(std::string s0, std::string s1)
{
    return s0.size() < s1.size();
}
}

// -------------------------------------------------------------------------
// EnumLbls member function defs
EnumLbls::EnumLbls()
    : m_szLgLbl(0)
{
    m_itrNameDefault = m_contNames.end();
}

EnumLbls::EnumLbls(const EnumLbls& fg)
    : m_contNames(fg.m_contNames), m_szLgLbl(0)
{
    m_itrNameDefault = m_contNames.find(*fg.m_itrNameDefault);
}

EnumLbls::EnumLbls(const value_type& label)
    : m_szLgLbl(label.size())
{
    m_contNames.insert(label);
    m_itrNameDefault = m_contNames.begin();
}

EnumLbls::EnumLbls(const value_type::const_pointer pLabel)
    : m_szLgLbl(0)
{
    m_contNames.insert(pLabel);
    m_itrNameDefault = m_contNames.begin();
    m_szLgLbl = m_itrNameDefault->size();
}

EnumLbls::EnumLbls(iterator beginString, iterator endString,
                   iterator itrNameDef)
    : m_contNames(beginString, endString), m_szLgLbl(0)
{
    m_itrNameDefault = m_contNames.insert(*itrNameDef).first;
    m_szLgLbl = std::max_element(m_contNames.begin(), m_contNames.end(),
                                 lesserStrSz)->size();
}

EnumLbls& EnumLbls::operator=(const EnumLbls& fg)
{
    if (this != &fg) {
        m_itrNameDefault = iterator();
        m_contNames = fg.m_contNames;
        m_itrNameDefault = m_contNames.find(*fg.m_itrNameDefault);
        m_szLgLbl = fg.m_szLgLbl;
    }

    return *this;
}

EnumLbls::const_iterator
EnumLbls::insert(const value_type& label, bool makeDefault)
{
    value_type defaultName = *m_itrNameDefault;
    iterator itr = m_contNames.insert(label).first;

    if (makeDefault) {
        m_itrNameDefault = itr;
    } else {
        m_itrNameDefault = m_contNames.find(defaultName);
    }

    m_szLgLbl = m_szLgLbl<label.size() ? label.size() : m_szLgLbl;
    return itr;
}

EnumLbls::size_type
EnumLbls::erase(const value_type& label)
{
    value_type defaultName = *m_itrNameDefault;
    size_type sz = m_contNames.erase(label);

    if (label == defaultName) {
        m_itrNameDefault = m_contNames.begin();
    } else {
        m_itrNameDefault = m_contNames.find(defaultName);
    }

    if (label.size() == m_szLgLbl)
        m_szLgLbl = std::max_element(m_contNames.begin(), m_contNames.end(),
                                     lesserStrSz)->size();

    return sz;
}

bool EnumLbls::operator==(const EnumLbls& fg) const
{
    return m_contNames == fg.m_contNames;
}

bool EnumLbls::operator==(const value_type& label) const
{
    return m_contNames.end() != m_contNames.find(label);
}

bool EnumLbls::operator<(const EnumLbls& fg) const
{
    return m_contNames < fg.m_contNames;
}

bool EnumLbls::operator<=(const EnumLbls& fg) const
{
    return m_contNames <= fg.m_contNames;
}

// -------------------------------------------------------------------------
// explicit instantiations
// not needed but helps find compilation time errors
template class EnumMap<unsigned int>;
} // A3
