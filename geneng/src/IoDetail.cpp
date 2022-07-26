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
 * IoDetail.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Class for specifying debug output level
// Refer to IoDetail.h for more info.
//==============================================================================
#include "IoDetail.h"
#include "IoManip.h"

//=============================================================================
namespace A3
{
using IoOutputDetail::OUTPUTLOW;
using IoOutputDetail::OUTPUTSTD;
using IoOutputDetail::OUTPUTHIGH;
using IoOutputDetail::OUTPUTDEBUG;

namespace
{
class IoManipDetail : public IoManip<unsigned>
{
public:

    explicit IoManipDetail(size_t refs=0);

    static const unsigned Default()
    {
        return IoOutputDetail::OUTPUTSTD;
    }

    static std::locale::id id;
};

std::locale::id IoManipDetail::id;

IoManipDetail::IoManipDetail(size_t refs)
    : IoManip<unsigned>(Default(), refs) {;}
}

std::ios_base& ioOutputDetailLow(std::ios_base& stm)
{
    return ioManipSet<IoManipDetail>(stm, OUTPUTLOW);
}

std::ios_base& ioOutputDetailStandard(std::ios_base& stm)
{
    return ioManipSet<IoManipDetail>(stm, OUTPUTSTD);
}

std::ios_base& ioOutputDetailHigh(std::ios_base& stm)
{
    return ioManipSet<IoManipDetail>(stm, OUTPUTHIGH);
}

std::ios_base& ioOutputDetailDebug(std::ios_base& stm)
{
    return ioManipSet<IoManipDetail>(stm, OUTPUTDEBUG);
}

std::ios_base& ioOutputDetailRestore(std::ios_base& stm)
{
    return ioManipRestore<IoManipDetail>(stm);
}

unsigned ioOutputDetailGetFormat(std::ios_base& stm)
{
    return ioManipGet<IoManipDetail>(stm);
}
}
// A3 namespace end
