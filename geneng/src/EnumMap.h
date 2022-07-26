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
 * EnumMap.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
/// @file
/// @brief EnumLbls and EnumMap Classes
///
/// @details EnumLbls contains string labels and descriptions of enumerators
/// and provides methods for accessing the descriptions.
///
/// EnumMap associates enumerators with EnumLbls and provides methods for
/// casting a flag to a default string and vice versa.
///
/// @author J. McCormick
/// @version V4.5 Initial release
/// @date 2009-07-01
///
/// @par Units
/// - Function Arg & Return: N/A
/// - Stream I/O:            N/A
/// - Initialization Files:  N/A
// =============================================================================
#ifndef ENUMMAP_H
/// Include guard macro
#define ENUMMAP_H   "EnumMap 2009-02-20"

#include "GenUtility.h"
#include <string>
#include <set>
#include <map>
#include <typeinfo>
#include <stdexcept>
#include <sstream>
#include <ostream>
#include <iomanip>

/// Astrodynamics, Allocation and Availability
namespace A3
{
/// Contains string flag labels and descriptions
class EnumLbls
{
public:
    typedef std::set<std::string>::value_type value_type;
    typedef std::set<std::string>::size_type size_type;
    typedef std::set<std::string>::iterator iterator;
    typedef std::set<std::string>::const_iterator const_iterator;

    EnumLbls();

    /// Copy constructor
    /// @param [in] EnumLbls object to be copied
    EnumLbls(const EnumLbls& fg);

    /// Conversion constructor sets default label, insert 1st label
    /// @param [in] label stored and assigned as default

    explicit EnumLbls(const value_type& label);

    /// Conversion constructor sets default label, insert 1st label
    ///
    /// @param [in] pLabel dereferenced, stored and assigned as default

    explicit EnumLbls(value_type::const_pointer pLabel);

    /// Initialize label container with labels/descriptions
    /// @param [in] beginString iterator to 1st label to be stored
    /// @param [in] endString one past the last label iterator
    /// @param [in] itrNameDef default label iterator, doesn't need to
    /// be in the [beginString, endString) range
    EnumLbls(iterator beginString, iterator endString,
             iterator itrNameDef);

    virtual ~EnumLbls() {;}

    /// Add label to container
    /// @param [in] label stored
    /// @param [in] makeDefault if true, label set as default,
    ///  else default is not changed
    /// @return iterator to the inserted label
    const_iterator
    insert(const value_type& label, bool makeDefault=false);

    /// Remove label from container
    /// @param [in] label removed
    /// @return remaining labels count
    size_type erase(const value_type& label);

    /// Default label
    /// @return default label reference
    operator const value_type&() const
    {
        return *m_itrNameDefault;
    }

    /// Label container begin iterator
    /// @return 1st label iterator
    const_iterator begin() const
    {
        return m_contNames.begin();
    }

    /// Label container end iterator
    /// @return past the last label iterator
    const_iterator end() const
    {
        return m_contNames.end();
    }

    /// Label container size
    /// @return label count
    size_type size() const
    {
        return m_contNames.size();
    }

    /// Length of longest label
    /// @return longest label size
    value_type::size_type size_longest_label() const
    {
        return m_szLgLbl;
    }

    /// Assignment operator
    /// @param [in] fg copied EnumLbls object
    /// @return reference to *this
    EnumLbls& operator=(const EnumLbls& fg);

    /// Equality operator, compares all labels
    /// @param [in] fg evaluated EnumLbls object
    /// @return true if equal else false
    bool operator==(const EnumLbls& fg) const;

    /// Equality operator, searches for label in container
    /// @param [in] label evaluated string
    /// @return true if container contains label else false
    bool operator==(const value_type& label) const;

    /// Inequality operator, compares all labels
    /// @param [in] fg evaluated EnumLbls object
    /// @return true if not equal else false
    bool operator!=(const EnumLbls& fg) const
    {
        return !operator==(fg);
    }

    /// Inequality operator, searches for label in container
    /// @param [in] label evaluated string
    /// @return true if container doesn't contain label else false
    bool operator!=(const value_type& label) const
    {
        return !operator==(label);
    }

    /// Less than operator, uses STL implementation
    /// @param [in] fg evaluated EnumLbls object
    /// @return true if *this is less than fg
    bool operator<(const EnumLbls& fg) const;

    /// Less than or equal operator, uses STL implementation
    /// @param [in] fg evaluated EnumLbls object
    /// @return true if *this is less than or equal to fg
    bool operator<=(const EnumLbls& fg) const;

    /// Greater than operator, uses STL implementation
    /// @param [in] fg evaluated EnumLbls object
    /// @return true if *this is greater than fg
    bool operator>(const EnumLbls& fg) const
    {
        return !operator<=(fg);
    }

    /// Grater than or equal operator, uses STL implementation
    /// @param [in] fg evaluated EnumLbls object
    /// @return true if *this is less than fg
    /// @return true if *this is greater than or equal to fg
    bool operator>=(const EnumLbls& fg) const
    {
        return !operator<(fg);
    };

protected:

private:
    std::set<std::string> m_contNames;
    const_iterator m_itrNameDefault;
    value_type::size_type m_szLgLbl;
};

// -------------------------------------------------------------------------
/// @brief Associates enumerators with EnumLbls
///
/// Provides methods for casting a flag to a default string and vice versa
/// @tparam I_TYPE is an unsigned integral type
template <typename I_TYPE, typename LESS = std::less<I_TYPE> >
class EnumMap : public std::map<I_TYPE, EnumLbls, LESS>
{
public:
    typedef typename std::map<I_TYPE, EnumLbls, LESS> container_type;
    typedef typename container_type::key_type       key_type;
    typedef typename container_type::value_type     value_type;
    typedef typename container_type::size_type      size_type;
    typedef typename container_type::iterator       iterator;
    typedef typename container_type::const_iterator const_iterator;

    EnumMap() {;}
    virtual ~EnumMap() {;}

    /// Find flag
    /// @param [in] fg key for search
    /// @return iterator to [flag, EnumLbls] pair that matches flag
    /// or end() for no match
    const_iterator find(const key_type& fg) const
    {
        return std::map<I_TYPE, EnumLbls, LESS>::find(fg);
    }

    /// Find label
    /// @param [in] label search target
    /// @return iterator to [flag, EnumLbls] pair that matches label
    /// or end() for no match
    const_iterator find(const std::string& label) const;

    /// Find values whos keys are subsets of specified bit pattern
    /// @param [in] bp bit pattern
    /// @param [out] em EnumMap to contain [flag, EnumLbls] pairs
    /// satisfying bit pattern constraint; if a pair with a matching
    /// key already exist in the EnumMap argument (em), the value of
    /// the pair will be replaced
    void find_subset(const key_type& bp, EnumMap& em);

    /// Find values whos keys have at least one bit in the specified
    /// bit pattern
    /// @param [in] bp bit pattern
    /// @param [out] em EnumMap to contain [flag, EnumLbls] pairs
    /// satisfying bit pattern constraint; if a pair with a matching
    /// key already exist in the EnumMap argument (em), the value of
    /// the pair will be replaced
    void find_anybit(const key_type& bp, EnumMap& em);

    /// Insert flag with labels
    /// @param [in] prFgNm [flag, EnumLbls] pair to be stored
    /// @return iterator to inserted [flag, EnumLbls] pair
    iterator insert(value_type prFgNm)
    {
        return std::map<I_TYPE, EnumLbls, LESS>::insert(prFgNm).first;
    }

    /// Conversion operator, label to flag
    /// @param [in] s label to be converted
    /// @return flag
    key_type operator()(const std::string& s) const;

    /// Conversion operator, flag to default label
    /// @param [in] f flag to be converted
    /// @return label
    const std::string& operator()(const key_type& f) const;

    /// Write contained enumerators and labels to stream,
    /// returns stream
    /// @param [in,out] os output stream
    /// @return reference to output stream
    std::ostream& ReportState(std::ostream& os) const;

protected:
private:
};

// -------------------------------------------------------------------------
template <typename I_TYPE, typename LESS>
inline typename EnumMap<I_TYPE, LESS>::const_iterator
EnumMap<I_TYPE, LESS>::find(const std::string& label) const
{
    const_iterator i(this->begin());
    const_iterator e(this->end());

    for (; e != i && i->second != label; ++i) ;

    return i;
}

template <typename I_TYPE, typename LESS>
inline void
EnumMap<I_TYPE, LESS>::find_subset(const key_type& bp, EnumMap<I_TYPE, LESS>& em)
{
    const_iterator i(this->begin());
    const_iterator e(this->end());

    for (; e != i; ++i)
        if ((bp & i->first) == i->first) {
            em.insert(*i);
        }
}

template <typename I_TYPE, typename LESS>
inline void
EnumMap<I_TYPE, LESS>::find_anybit(const key_type& bp, EnumMap<I_TYPE, LESS>& em)
{
    const_iterator i(this->begin());
    const_iterator e(this->end());

    for (; e != i; ++i)
        if ((bp & i->first) != 0) {
            em.insert(*i);
        }
}

template <typename I_TYPE, typename LESS>
inline std::ostream&
EnumMap<I_TYPE, LESS>::ReportState(std::ostream& os) const
{
    static const std::string s02(2, ' '), s10(10, ' ');
    std::ios::fmtflags fg =
        os.setf(std::ios::hex|std::ios::showbase,
                std::ios::basefield);
    std::ios::char_type cf = os.fill('0');
    const_iterator itr;
    EnumLbls::const_iterator itrfn;

    for (itr=this->begin(); this->end() != itr; ++itr) {
        os << std::setw(8) << (itr)->first;

        for (itrfn=itr->second.begin(); itr->second.end()!=itrfn; ++itrfn) {
            os << (itr->second.begin()==itrfn ? s02 : s10)
               << std::string(*itrfn) << '\n';
        }
    }

    os.fill(cf);
    os.flags(fg);
    return os;
}

template <typename I_TYPE, typename LESS>
inline typename EnumMap<I_TYPE, LESS>::key_type
EnumMap<I_TYPE, LESS>::operator()(const std::string& s) const
{
    const_iterator itr(find(s));

    if (this->end() == itr) {
        std::string msg(typeid(*this).name());
        msg += "\nCannot find flag matching " + s + "\n";
        throw std::out_of_range(msg);
    }

    return itr->first;
}

template <typename I_TYPE, typename LESS>
inline const std::string&
EnumMap<I_TYPE, LESS>::operator()(const key_type& f) const
{
    const_iterator itr(find(f));

    if (this->end() == itr) {
        std::ostringstream oss;
        oss << typeid(*this).name()
            << "\nCannot find flag matching "
            << key_type(f) << '\n' << std::ends;
        throw std::out_of_range(oss.str());
    }

    return itr->second;
}
} // A3
#endif  // ENUMMAP_H
