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
 * Angle.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

// =============================================================================
//                         U N C L A S S I F I E D
// =============================================================================
// Class representing angles
// Refer to Angle.h for more info.
//==============================================================================
#include "Angle.h"
#include "IoManip.h"
#include "constants.h"
#include "Units.h"
#include <iostream>
#include <stdexcept>

// ============================================================================
namespace A3
{
// angle stream format manipulators
namespace
{
class IoManipAngle : public IoManip<unsigned>
{
public:


    IoManipAngle(size_t refs=0);
    //CPPCHECK_FUTURE_IMPLEMENTATION: explicit IoManipAngle(size_t refs=0);

    static unsigned Default()
    {
        return Units::RADIAN;
    }

    static std::locale::id id;
};

std::locale::id IoManipAngle::id;

IoManipAngle::IoManipAngle(size_t refs)
    : IoManip<unsigned>(Default(), refs) {;}
}

// -------------------------------------------------------------------------
// set the streams format to DEGREE
std::ios_base& ioAngleDegrees(std::ios_base& stm)
{
    return ioManipSet<IoManipAngle>(stm, Units::DEGREE);
}

// set the streams format to radians
std::ios_base& ioAngleRadians(std::ios_base& stm)
{
    return ioManipSet<IoManipAngle>(stm, Units::RADIAN);
}

// restore stream to previous angle format
std::ios_base& ioAngleRestore(std::ios_base& stm)
{
    return ioManipRestore<IoManipAngle>(stm);
}

// -------------------------------------------------------------------------
// i/o operators
std::ostream& operator<<(std::ostream& os, Angle angle)
{
    unsigned fmt(ioManipGet<IoManipAngle>(os));

    switch (fmt) {
        case Units::DEGREE:
            os << static_cast<double>(angle) * RAD_TO_DEG;
            break;

        case Units::RADIAN:
            os << static_cast<double>(angle);
            break;

        case Units::GRADIAN:
            os << static_cast<double>(angle) * 200.0 / PI;
            break;

        default:
            throw std::runtime_error(
                "operator<<( ostream&, Angle ) bad format");
    };

    return os;
}

std::istream& operator>>(std::istream& is, Angle& angle)
{
    unsigned fmt(ioManipGet<IoManipAngle>(is));
    double dAngle;
    is >> dAngle;

    switch (fmt) {
        case Units::DEGREE:
            angle = static_cast< Angle >(dAngle * DEG_TO_RAD);
            break;

        case Units::RADIAN:
            angle = static_cast< Angle >(dAngle);
            break;

        case Units::GRADIAN:
            angle = static_cast< Angle >(dAngle * PI / 200.0);
            break;

        default:
            throw std::runtime_error(
                "operator>>( istream&, Angle ) bad format");
    };

    return is;
}
}
// A3 namespace end
