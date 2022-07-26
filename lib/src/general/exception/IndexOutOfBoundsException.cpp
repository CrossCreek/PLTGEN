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
 * IndexOutOfBoundsException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "IndexOutOfBoundsException.h"

IndexOutOfBoundsException::IndexOutOfBoundsException()
    : Exception(), arraySize_a(0), arrayIndex_a(0)
{
    return;
}

IndexOutOfBoundsException::IndexOutOfBoundsException(const string &currentClassName,
        const string &currentMethodName,
        const int &arraySize,
        const int &arrayIndex)
    : Exception(currentClassName, currentMethodName),
      arraySize_a(arraySize), arrayIndex_a(arrayIndex)
{
    return;
}

IndexOutOfBoundsException::IndexOutOfBoundsException(const IndexOutOfBoundsException &copyMe)
    : Exception(copyMe), arraySize_a(copyMe.arraySize_a), arrayIndex_a(copyMe.arrayIndex_a)
{
    return;
}

IndexOutOfBoundsException&
IndexOutOfBoundsException::operator = (const IndexOutOfBoundsException &copyMe)
{
    Exception::operator = (copyMe);
    arraySize_a  = copyMe.arraySize_a;
    arrayIndex_a = copyMe.arrayIndex_a;
    return(*this);
}


bool
IndexOutOfBoundsException::operator == (const IndexOutOfBoundsException &compareMe) const
{
    return( (Exception::operator == (compareMe)) &&
            (arraySize_a == compareMe.arraySize_a) &&
            (arrayIndex_a == compareMe.arrayIndex_a) );
}

IndexOutOfBoundsException::~IndexOutOfBoundsException()
{
    return;
}
