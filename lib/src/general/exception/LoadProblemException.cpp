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
 * LoadProblemException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "LoadProblemException.h"

LoadProblemException::LoadProblemException()
    : Exception(), ptrLoadProblemErrorString_a(NULL)
{
    return;
}

LoadProblemException::LoadProblemException(const string &currentClassName,
        const string &currentMethodName,
        const string &loadErrorString)
    : Exception(currentClassName, currentMethodName),
      ptrLoadProblemErrorString_a(NULL)
{
    if (loadErrorString.size() > 0) {
        ptrLoadProblemErrorString_a = new string(loadErrorString);
    }

    return;
}

LoadProblemException::LoadProblemException(const LoadProblemException &copyMe)
    : Exception(copyMe), ptrLoadProblemErrorString_a(NULL)
{
    if (copyMe.ptrLoadProblemErrorString_a != NULL) {
        ptrLoadProblemErrorString_a = new
        string(*(copyMe.ptrLoadProblemErrorString_a));
    }

    return;
}

LoadProblemException&
LoadProblemException::operator = (const LoadProblemException &copyMe)
{
    Exception::operator = (copyMe);

    if (copyMe.ptrLoadProblemErrorString_a != NULL) {
        ptrLoadProblemErrorString_a = new
        string(*(copyMe.ptrLoadProblemErrorString_a));
    }

    return(*this);
}

LoadProblemException::~LoadProblemException()
{
    if (ptrLoadProblemErrorString_a != NULL) {
        delete ptrLoadProblemErrorString_a;
        ptrLoadProblemErrorString_a = NULL;
    }

    return;
}
