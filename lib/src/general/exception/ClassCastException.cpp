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
 * ClassCastException.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "ClassCastException.h"

ClassCastException::ClassCastException()
    : Exception()
{
    return;
}

ClassCastException::ClassCastException(const string &currentClassName,
                                       const string &currentMethodName)
    : Exception(currentClassName, currentMethodName)
{
    return;
}

ClassCastException::ClassCastException(const ClassCastException &copyMe)
    : Exception(copyMe)
{
    return;
}

ClassCastException&
ClassCastException::operator = (const ClassCastException &copyMe)
{
    Exception::operator = (copyMe);
    return (*this);
}

ClassCastException :: ~ClassCastException()
{
    return;
}
