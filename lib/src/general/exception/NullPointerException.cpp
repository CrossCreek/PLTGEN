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
 * NullPointerException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "NullPointerException.h"

NullPointerException::NullPointerException()
    : Exception(), ptrPointerNameString_a(NULL)
{
    return;
}

NullPointerException::NullPointerException(const string &currentClassName,
        const string &currentMethodName,
        string pointerName)
    : Exception(currentClassName, currentMethodName), ptrPointerNameString_a(NULL)
{
    if (pointerName.size() > 0) {
        ptrPointerNameString_a = new string(pointerName);
    }

    return;
}

NullPointerException::NullPointerException(const NullPointerException &copyMe)
    : Exception(copyMe), ptrPointerNameString_a(NULL)
{
    if (copyMe.ptrPointerNameString_a != NULL) {
        ptrPointerNameString_a = new string(*(copyMe.ptrPointerNameString_a));
    }

    return;
}

NullPointerException&
NullPointerException::operator = (const NullPointerException &copyMe)
{
    Exception::operator = (copyMe);

    if (ptrPointerNameString_a != NULL) {
        delete ptrPointerNameString_a;
        ptrPointerNameString_a = NULL;
    }

    if (copyMe.ptrPointerNameString_a != NULL) {
        ptrPointerNameString_a = new string(*(copyMe.ptrPointerNameString_a));
    }

    return(*this);
}

NullPointerException::~NullPointerException()
{
    if (ptrPointerNameString_a != NULL) {
        delete ptrPointerNameString_a;
        ptrPointerNameString_a = NULL;
    }

    return;
}
