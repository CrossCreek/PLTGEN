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
 * OutputFileException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "OutputFileException.h"

OutputFileException::OutputFileException()
    : Exception(), ptrFileNameString_a(NULL)
{
    ptrFileNameString_a = new string();
    return;
}

OutputFileException::OutputFileException(const string &currentClassName,
        const string &currentMethodName,
        const string &fileName)
    : Exception(currentClassName, currentMethodName), ptrFileNameString_a(NULL)
{
    ptrFileNameString_a = new string(fileName);
    return;
}

OutputFileException::OutputFileException(const OutputFileException &copyMe)
    : Exception(copyMe), ptrFileNameString_a(NULL)
{
    if (copyMe.ptrFileNameString_a != NULL) {
        ptrFileNameString_a = new string(*(copyMe.ptrFileNameString_a));
    }

    return;
}

OutputFileException&
OutputFileException::operator = (const OutputFileException &copyMe)
{
    Exception::operator = (copyMe);

    if (ptrFileNameString_a != NULL) {
        delete ptrFileNameString_a;
        ptrFileNameString_a = NULL;
    }

    if (copyMe.ptrFileNameString_a != NULL) {
        ptrFileNameString_a = new string(*(copyMe.ptrFileNameString_a));
    }

    return(*this);
}

OutputFileException::~OutputFileException()
{
    if (ptrFileNameString_a != NULL) {
        delete ptrFileNameString_a;
        ptrFileNameString_a = NULL;
    }

    return;
}
