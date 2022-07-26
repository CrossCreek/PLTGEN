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
 * UpdateException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "UpdateException.h"

UpdateException::UpdateException()
    : Exception(), ptrUpdateErrorString_a(NULL)
{
    return;
}

UpdateException::UpdateException(const string &currentClassName,
                                 const string &currentMethodName,
                                 string updateErrorString)
    : Exception(currentClassName, currentMethodName), ptrUpdateErrorString_a(NULL)
{
    if (updateErrorString.size() > 0) {
        ptrUpdateErrorString_a = new string(updateErrorString);
    }

    return;
}

UpdateException::UpdateException(const UpdateException &copyMe)
    : Exception(copyMe), ptrUpdateErrorString_a(NULL)
{
    if (copyMe.ptrUpdateErrorString_a != NULL) {
        ptrUpdateErrorString_a = new string(*(copyMe.ptrUpdateErrorString_a));
    }

    return;
}

UpdateException&
UpdateException::operator = (const UpdateException &copyMe)
{
    Exception::operator = (copyMe);

    if (copyMe.ptrUpdateErrorString_a != NULL) {
        ptrUpdateErrorString_a = new string(*(copyMe.ptrUpdateErrorString_a));
    }

    return(*this);
}

UpdateException::~UpdateException()
{
    if (ptrUpdateErrorString_a != NULL) {
        delete ptrUpdateErrorString_a;
        ptrUpdateErrorString_a = NULL;
    }

    return;
}
