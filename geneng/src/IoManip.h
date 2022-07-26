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
 * IoManip.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

//****************************************************************************//
//                         U N C L A S S I F I E D                            //
//****************************************************************************//

//  FILE      : IoManipulator.h

//  NAMESPACE : A3

//  SUBSYSTEM : GenEngr

//  AUTHOR(s) : J. McCormick

//  UPDATES   : V5.0      2010-01-27  JAM
//              Replaces IoManipulator class family with IoManip facets
//              Results in less code, less complexity, greater efficiency

//  Doxygen Documentation
/// @file
/// @brief IoManip: Custom manipulator base class
///
///  Template class used to created user defined manipulator functions
///
/// @author J. McCormick
/// @version V5.0 <BR>
/// Replaces IoManipulator class family with IoManip facets
/// Results in less code, less complexity, greater efficiency
/// @date 2010-01-27
///
/// @par Units
/// - Function Arg & Return: n/a
/// - Stream I/O:            n/a
/// - Initialization Files:  n/a

//=============================================================================
#ifndef IOMANIP_H
#define IOMANIP_H __FILE__ " " __DATE__

#include <locale>
#include <deque>
#include <ios>

namespace A3
{
template <typename FTYP>
class IoManip : public std::locale::facet
{
public:
    typedef typename std::deque<FTYP>::value_type value_type;
    typedef typename std::deque<FTYP>::size_type  size_type;

    IoManip(FTYP dflt, size_t refs=0);
    IoManip(const IoManip& iom);
    IoManip(const IoManip& iom, size_t refs);

    virtual ~IoManip();

    IoManip& operator=(const IoManip& iom);

    // sets, resets and checks formats
    void Set(FTYP fmt) const;
    void Restore() const;
    FTYP Get() const;

protected:
    IoManip();

private:
    typedef std::deque<FTYP> Cntnr;

    Cntnr  *m_pCntnrFrmt;
    size_t *m_pRefCnt;
};

template <typename IOM>
inline std::ios_base&
ioManipSet(std::ios_base& stm, typename IOM::value_type v)
{
    std::locale lcl0(stm.getloc());

    if (std::has_facet<IOM>(lcl0)) {
        std::use_facet<IOM>(lcl0).Set(v);
    } else {
        IOM *pIom(new IOM);
        pIom->Set(v);
        std::locale lcl1(lcl0, pIom);
        stm.imbue(lcl1);
    }

    return stm;
}

template <typename IOM>
inline std::ios_base&
ioManipRestore(std::ios_base& stm)
{
    std::locale lcl(stm.getloc());

    if (std::has_facet<IOM>(lcl)) {
        std::use_facet<IOM>(lcl).Restore();
    }

    return stm;
}

template <typename IOM>
inline typename
IOM::value_type ioManipGet(const std::ios_base& stm)
{
    typename IOM::value_type fmt(IOM::Default());
    std::locale lcl(stm.getloc());

    if (std::has_facet<IOM>(lcl)) {
        fmt = std::use_facet<IOM>(lcl).Get();
    }

    return fmt;
}
} // A3 namespace end

#endif  // IOMANIP_H
