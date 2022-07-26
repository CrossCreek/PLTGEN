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
 * CountryFactor.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "CountryFactor.h"

CountryFactor::CountryFactor(double factor,
                             const Array<string> *ptrCountryCode)
    : countryCodeFactor_a(factor), ptrCountryCodeArray_a(NULL)
{
    ptrCountryCodeArray_a = new Array<string>(*ptrCountryCode);
    return;
}

CountryFactor::~CountryFactor()
{
    if (ptrCountryCodeArray_a != NULL) {
        delete ptrCountryCodeArray_a;
        ptrCountryCodeArray_a = NULL;
    }

    return;
}
