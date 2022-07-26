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
 * TimeException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "TimeException.h"

TimeException::TimeException()
    : Exception(), ptrTimeErrorString_a(NULL)
{
    return;
}

TimeException::TimeException(const string &currentClassName,
                             const string &currentMethodName)
    : Exception(currentClassName, currentMethodName), ptrTimeErrorString_a(NULL)
{
    return;
}

TimeException::TimeException(const string &currentClassName,
                             const string &currentMethodName,
                             const string &timeErrorString)
    : Exception(currentClassName, currentMethodName), ptrTimeErrorString_a(NULL)
{
    if (timeErrorString.size() > 0) {
        ptrTimeErrorString_a = new string(timeErrorString);
    }

    return;
}

TimeException::TimeException(const TimeException &copyMe)
    : Exception(copyMe), ptrTimeErrorString_a(NULL)
{
    if (copyMe.ptrTimeErrorString_a != NULL) {
        ptrTimeErrorString_a = new string(*(copyMe.ptrTimeErrorString_a));
    }

    return;
}

TimeException&
TimeException::operator = (const TimeException &copyMe)
{
    Exception::operator = (copyMe);

    if (copyMe.ptrTimeErrorString_a != NULL) {
        ptrTimeErrorString_a = new string(*(copyMe.ptrTimeErrorString_a));
    }

    return (*this);
}

TimeException::~TimeException()
{
    if (ptrTimeErrorString_a != NULL) {
        delete ptrTimeErrorString_a;
        ptrTimeErrorString_a = NULL;
    }

    return;
}
