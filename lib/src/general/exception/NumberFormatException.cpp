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
 * NumberFormatException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "NumberFormatException.h"

NumberFormatException::NumberFormatException()
    : Exception(), ptrNumberString_a(NULL)
{
    return;
}

NumberFormatException::NumberFormatException(const string &currentClassName,
        const string &currentMethodName,
        const string &numberString)
    : Exception(currentClassName,currentMethodName), ptrNumberString_a(NULL)
{
    ptrNumberString_a = new string(numberString);
    return;
}

NumberFormatException::NumberFormatException(const NumberFormatException &copyMe)
    : Exception(copyMe), ptrNumberString_a(NULL)
{
    if (copyMe.ptrNumberString_a != NULL) {
        ptrNumberString_a = new string(*(copyMe.ptrNumberString_a));
    }

    return;
}

NumberFormatException&
NumberFormatException::operator = (const NumberFormatException &copyMe)
{
    Exception::operator = (copyMe);

    if (ptrNumberString_a != NULL) {
        delete ptrNumberString_a;
        ptrNumberString_a = NULL;
    }

    if (copyMe.ptrNumberString_a != NULL) {
        ptrNumberString_a = new string(*(copyMe.ptrNumberString_a));
    }

    return(*this);
}

NumberFormatException::~NumberFormatException()
{
    if (ptrNumberString_a != NULL) {
        delete ptrNumberString_a;
        ptrNumberString_a = NULL;
    }

    return;
}
