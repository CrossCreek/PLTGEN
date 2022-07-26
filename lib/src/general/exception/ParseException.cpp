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
 * ParseException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "ParseException.h"

ParseException::ParseException()
    : Exception(), ptrParseString_a(NULL)
{
    ptrParseString_a = new string();
    return;
}

ParseException::ParseException(const string &currentClassName,
                               const string &currentMethodName,
                               const string &parseString)
    : Exception(currentClassName, currentMethodName), ptrParseString_a(NULL)
{
    ptrParseString_a = new string(parseString);
    return;
}

ParseException::ParseException(const ParseException &copyMe)
    : Exception(copyMe), ptrParseString_a(NULL)
{
    if (copyMe.ptrParseString_a != NULL) {
        ptrParseString_a = new string(*(copyMe.ptrParseString_a));
    }

    return;
}

ParseException&
ParseException::operator = (const ParseException &copyMe)
{
    Exception::operator = (copyMe);

    if (ptrParseString_a != NULL) {
        delete ptrParseString_a;
        ptrParseString_a = NULL;
    }

    if (copyMe.ptrParseString_a != NULL) {
        ptrParseString_a = new string(*(copyMe.ptrParseString_a));
    }

    return(*this);
}

ParseException::~ParseException()
{
    if (ptrParseString_a != NULL) {
        delete ptrParseString_a;
        ptrParseString_a = NULL;
    }

    return;
}
