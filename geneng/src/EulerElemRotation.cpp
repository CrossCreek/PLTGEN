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
 * EulerElemRotation.cpp
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#include "EulerElemRotation.h"
#include "ioDelim.h"
#include "EnumMap.h"
#include <string>
#include <iomanip>

namespace A3
{
namespace
{
typedef EulerElemRotation EER;

class Axes : public EnumMap<EER::Axis>
{
public:
    Axes();
};

Axes::Axes()
{
    /*insert(value_type(EER::A1, std::string("A1")))
    ->second.insert(std::string("Axis1"));
    insert(value_type(EER::A2, std::string("A2")))
    ->second.insert(std::string("Axis2"));
    insert(value_type(EER::A3, std::string("A3")))
    ->second.insert(std::string("Axis3"));*/
}

Axes axes;
}

EulerElemRotation::EulerElemRotation()
    : m_angle(0.0), m_axis(A1) { ; }

EulerElemRotation::EulerElemRotation(Axis ax, Angle an)
    : m_angle(an), m_axis(ax) { ; }

bool EulerElemRotation::operator==(const EulerElemRotation& eer) const
{
    return m_angle == eer.m_angle
           && m_axis  == eer.m_axis;
}

bool EulerElemRotation::operator!=(const EulerElemRotation& eer) const
{
    return !operator==(eer);
}

std::istream&
operator>>(std::istream& is, EulerElemRotation& eer)
{
    std::string sAxis;
    Angle angle;
    is >> sAxis >> angle;
    eer = EulerElemRotation(axes(sAxis), angle);
    return is;
}

std::ostream&
operator<<(std::ostream& os, const EulerElemRotation& eer)
{
    //WARNING_FIX:
    //1>src\EulerElemRotation.cpp(62): warning C4244: 'initializing' : conversion from 'std::streamsize' to 'int', possible loss of data
    long long int w(os.width());
    std::string sAxis(axes(eer.GetAxis()));
    os << sAxis << ioDelim << std::setw(w) << eer.GetAngle();
    return os;
}
}
