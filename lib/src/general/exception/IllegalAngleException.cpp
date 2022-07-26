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
 * IllegalAngleException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "IllegalAngleException.h"

IllegalAngleException::IllegalAngleException()
    : Exception(), minimumAngle_a(0.0), maximumAngle_a(0.0), illegalAngle_a(0.0)
{
    return;
}

IllegalAngleException::IllegalAngleException(const string &currentClassName,
        const string &currentMethodName,
        double minimumAngle,
        double maximumAngle,
        double illegalAngle)
    : Exception(currentClassName, currentMethodName), minimumAngle_a(minimumAngle),
      maximumAngle_a(maximumAngle), illegalAngle_a(illegalAngle)
{
    return;
}

IllegalAngleException::IllegalAngleException(const IllegalAngleException &copyMe)
    : Exception(copyMe), minimumAngle_a(copyMe.minimumAngle_a),
      maximumAngle_a(copyMe.maximumAngle_a), illegalAngle_a(copyMe.illegalAngle_a)
{
    return;
}

IllegalAngleException&
IllegalAngleException::operator = (const IllegalAngleException &copyMe)
{
    Exception::operator = (copyMe);
    minimumAngle_a = copyMe.minimumAngle_a;
    maximumAngle_a = copyMe.maximumAngle_a;
    illegalAngle_a = copyMe.illegalAngle_a;
    return(*this);
}

IllegalAngleException::~IllegalAngleException()
{
    return;
}
