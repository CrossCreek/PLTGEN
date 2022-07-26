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
 * InputException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "InputException.h"

InputException::InputException()
    : Exception(), ptrInputString_a(NULL)
{
    return;
}

InputException::InputException(const string &currentClassName,
                               const string &currentMethodName,
                               string inputString)
    : Exception(currentClassName, currentMethodName), ptrInputString_a(NULL)
{
    if (inputString.size() > 0) {
        ptrInputString_a = new string(inputString);
    }

    return;
}

InputException::InputException(const InputException &copyMe)
    : Exception(copyMe), ptrInputString_a(NULL)
{
    if (copyMe.ptrInputString_a != NULL) {
        ptrInputString_a = new string(*(copyMe.ptrInputString_a));
    }

    return;
}

InputException&
InputException::operator = (const InputException &copyMe)
{
    Exception::operator = (copyMe);

    if (ptrInputString_a != NULL) {
        delete ptrInputString_a;
        ptrInputString_a = NULL;
    }

    if (copyMe.ptrInputString_a != NULL) {
        ptrInputString_a = new string(*(copyMe.ptrInputString_a));
    }

    return(*this);
}

InputException::~InputException()
{
    if (ptrInputString_a != NULL) {
        delete ptrInputString_a;
        ptrInputString_a = NULL;
    }

    return;
}
