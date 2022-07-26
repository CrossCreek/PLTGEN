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
 * DateFormatException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "DateFormatException.h"

DateFormatException::DateFormatException()
    : Exception(), ptrDateString_a(NULL)
{
    return;
}

DateFormatException::DateFormatException(const string &currentClassName,
        const string &currentMethodName,
        const string &dateString)
    : Exception(currentClassName, currentMethodName), ptrDateString_a(NULL)
{
    ptrDateString_a = new string(dateString);
    return;
}

DateFormatException::DateFormatException(const DateFormatException &copyMe)
    : Exception(copyMe), ptrDateString_a(NULL)
{
    if (copyMe.ptrDateString_a != NULL) {
        ptrDateString_a = new string(*(copyMe.ptrDateString_a));
    }

    return;
}

DateFormatException&
DateFormatException::operator = (const DateFormatException &copyMe)
{
    Exception::operator = (copyMe);

    if (ptrDateString_a != NULL) {
        delete ptrDateString_a;
        ptrDateString_a = NULL;
    }

    if (copyMe.ptrDateString_a != NULL) {
        ptrDateString_a = new string(*(copyMe.ptrDateString_a));
    }

    return(*this);
}

DateFormatException::~DateFormatException()
{
    if (ptrDateString_a != NULL) {
        delete ptrDateString_a;
        ptrDateString_a = NULL;
    }

    return;
}
