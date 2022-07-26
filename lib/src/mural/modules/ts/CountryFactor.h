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
 * CountryFactor.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef COUNTRY_FACTOR_H
#define COUNTRY_FACTOR_H "CountryFactor"

#include <string>

#include "general/array/Array.h"

using namespace std;

/**
 * Data class associating a country with a code and a resource factor.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class CountryFactor
{
public:

    CountryFactor(double factor,
                  const Array<string> *ptrCountryCode);




    CountryFactor(const CountryFactor &countryFactor);

    virtual ~CountryFactor();

    inline void   PrintCountryCodes(ostream &outfile) const;

    inline int   GetNumberOfCountries() const;

    inline double  GetFactor() const;

    inline bool   ContainsCountryCode(const string &countryCode) const;

protected:

private:




    double          countryCodeFactor_a;
    Array<string> *ptrCountryCodeArray_a;
};

inline
void
CountryFactor::PrintCountryCodes(ostream &outfile) const
{
    if (ptrCountryCodeArray_a != NULL) {
        ptrCountryCodeArray_a->Print(outfile);
    }

    return;
}

inline
int
CountryFactor::GetNumberOfCountries() const
{
    int numberOfCountries = 0;

    if (ptrCountryCodeArray_a != NULL) {
        numberOfCountries = ptrCountryCodeArray_a->Size();
    }

    return(numberOfCountries);
}

inline
double
CountryFactor::GetFactor() const
{
    return(countryCodeFactor_a);
}

inline
bool
CountryFactor::ContainsCountryCode(const string &countryCode) const
{
    return(ptrCountryCodeArray_a->ValueExists(countryCode));
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
CountryFactor::GetClassName()
{
    return(COUNTRY_FACTOR_H);
}
*/

#endif
