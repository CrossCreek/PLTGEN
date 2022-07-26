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
 * EulerElemRotation.h
 *
 * MURAL- Multi User Resource Allocator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#ifndef EULERELEMROTATION_H
#define EULERELEMROTATION_H "2003-12-19 V01.00"

#include "Angle.h"

namespace A3
{
class EulerElemRotation
{
public:
    enum Axis {A1=1, A2=2, A3=3};

    EulerElemRotation();
    EulerElemRotation(Axis ax, Angle an);

    Axis  GetAxis() const
    {
        return m_axis;
    }
    Angle GetAngle() const
    {
        return m_angle;
    }

    bool operator==(const EulerElemRotation& eer) const;
    bool operator!=(const EulerElemRotation& eer) const;

private:
    Angle m_angle;
    Axis  m_axis;
};

std::istream& operator>>(std::istream& is, EulerElemRotation& eer);
std::ostream& operator<<(std::ostream& os, const EulerElemRotation& eer);
}

#endif // EULERELEMROTATION_H
