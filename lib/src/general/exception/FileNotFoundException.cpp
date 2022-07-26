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
 * FileNotFoundException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "FileNotFoundException.h"

FileNotFoundException::FileNotFoundException()
    : Exception(), ptrFileNameString_a(NULL)
{
    ptrFileNameString_a = new string();
    return;
}

FileNotFoundException::FileNotFoundException(const string &currentClassName,
        const string &currentMethodName,
        const string &fileName)
    : Exception(currentClassName, currentMethodName), ptrFileNameString_a(NULL)
{
    ptrFileNameString_a = new string(fileName);
    return;
}

FileNotFoundException::FileNotFoundException(const FileNotFoundException &copyMe)
    : Exception(copyMe), ptrFileNameString_a(NULL)
{
    if (copyMe.ptrFileNameString_a != NULL) {
        ptrFileNameString_a = new string(*(copyMe.ptrFileNameString_a));
    }

    return;
}

FileNotFoundException&
FileNotFoundException::operator = (const FileNotFoundException &copyMe)
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


bool
FileNotFoundException::operator == (const FileNotFoundException &compareMe) const
{
    return( (Exception::operator == (compareMe)) &&
            (*ptrFileNameString_a == *(compareMe.ptrFileNameString_a)) );
}

bool
FileNotFoundException::operator != (const FileNotFoundException &compareMe) const
{
    return(!(operator == (compareMe)));
}

FileNotFoundException::~FileNotFoundException()
{
    if (ptrFileNameString_a != NULL) {
        delete ptrFileNameString_a;
        ptrFileNameString_a = NULL;
    }

    return;
}
