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
 * IoDelim.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Class for inserting and extracting a field delimiters
// Refer to IoDelim.h for more info.
// =============================================================================
#include "IoDelim.h"
#include "IoManip.h"
#include "EnumMap.h"
#include "GenMath.h"

#include <iostream>
#include <ctime>
#include <stdexcept>
#include <typeinfo>
#include <string>
#include <algorithm>
#include <functional>
#include <locale>
#include <string.h>

// A3 namespace start
namespace A3
{
namespace
{
class IoManipDlm : public IoManip<std::string>
{
public:

    explicit IoManipDlm(size_t refs=0);

    static std::string Default()
    {
        return std::string(" ");
    }

    static std::locale::id id;
};

std::locale::id IoManipDlm::id;

IoManipDlm::IoManipDlm(size_t refs)
    : IoManip<std::string>(Default(), refs) {;}

// ---------------------------------------------------------------------
void castMask(const std::string& s, std::ctype_base::mask m,
              std::ctype_base::mask* pTable)
{
    for (size_t i=0; s.size()!=i; ++i) {
        setBitsOn(m, pTable[static_cast<size_t>(s[i])]);
    }
}

void uncastMask(const std::string& s, std::ctype_base::mask m,
                std::ctype_base::mask* pTable)
{
    for (size_t i=0; s.size()!=i; ++i) {
        setBitsOff(m, pTable[static_cast<size_t>(s[i])]);
    }
}

std::ios_base& reclassSpace(std::ios_base& iosb, const std::string& s,
                            void (*pRecast)(const std::string&, std::ctype_base::mask,
                                    std::ctype_base::mask*))
{
    typedef std::ctype<char> cctype;
    static char ctbl[cctype::table_size];
    static size_t i(0);

    for (; cctype::table_size>i; ++i) {
        ctbl[i] = static_cast<char>(i);
    }

    std::ctype_base::mask
    *pMsk(new std::ctype_base::mask[cctype::table_size]);
    std::use_facet<cctype>
    (iosb.getloc()).is(ctbl, ctbl+cctype::table_size, pMsk);
    (*pRecast)(s, std::ctype_base::space, pMsk);
    cctype *pCtype(new cctype(pMsk, true));
    iosb.imbue(std::locale(iosb.getloc(), pCtype));
    return iosb;
}
}

// ------------------------------------------------------------------------
// Cast a character as white spaces
std::ios_base& ioCastWhite(std::ios_base& iosb, char c)
{
    return reclassSpace(iosb, std::string(1,c), castMask);
}

// Cast characters as white spaces
std::ios_base& ioCastWhite(std::ios_base& iosb, const std::string& s)
{
    return reclassSpace(iosb, s, castMask);
}

// Uncast a character as white space
std::ios_base& ioUncastWhite(std::ios_base& iosb, char c)
{
    return reclassSpace(iosb, std::string(1,c), uncastMask);
}

// Uncast characters as white space
std::ios_base& ioUncastWhite(std::ios_base& iosb, const std::string& s)
{
    return reclassSpace(iosb, s, uncastMask);
}

// ------------------------------------------------------------------------
// set a stream's delimiter
std::ios_base&
ioDelimSet(std::ios_base& iosb, const std::string& sDelim)
{
    ioManipSet<IoManipDlm>(iosb, sDelim);
    return iosb;
}

// ------------------------------------------------------------------------
struct IoDelimSet : public std::string {

    explicit IoDelimSet(const std::string& sDelim) : std::string(sDelim) {;}
};

IoDelimSet ioDelimSet(const std::string& sDelim)
{
    return IoDelimSet(sDelim);
}

std::ostream& operator<<(std::ostream& os, const IoDelimSet& iods)
{
    ioDelimSet(os, static_cast<const std::string&>(iods));
    return os;
}

// ------------------------------------------------------------------------
// restores a streams last delimiter
std::ios_base& ioDelimRestore(std::ios_base& iosb)
{
    ioManipRestore<IoManipDlm>(iosb);
    return iosb;
}

// ------------------------------------------------------------------------
// get the stream delimiter
std::string ioDelimGet(const std::ios_base& iosb)
{
    return ioManipGet<IoManipDlm>(iosb);
}

// ------------------------------------------------------------------------
std::ostream& ioDelim(std::ostream& os)
{
    os << ioManipGet<IoManipDlm>(os);
    return os;
}

} // A3 namespace end
