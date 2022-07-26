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
 * Revolution.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef REVOLUTION_H
#define REVOLUTION_H "Revolution"

#include <iostream>
#include <iomanip>
#include <string>


#include "general/data/TimePeriod.h"

using namespace std;

/**
 * This class represents the calculated data from the position generator for an
 * orbit revolution.
 *
 * Name                              Description (units)
 * -------------                     -------------------------------------------
 * revolutionNumber_a                number assigned to this revolution
 * ascendingZeroCrossingLongitude_a  longitude at which this revolution crosses
 *                                     zero latitude (i.e. equator) on the
 *                                     ascending pass
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class Revolution : public TimePeriod
{
public:

    Revolution();
    Revolution(int revNumber,
               int startRevTime,
               int endRevTime,
               double zeroCrossingLongitude);
    Revolution(const Revolution &copy);
    virtual ~Revolution();

    Revolution&     operator =  (const Revolution &copyMe);
    bool            operator == (const Revolution &compareMe);
    bool            operator != (const Revolution &compareMe);

    inline const int  GetRevNumber() const;

    inline const double GetAscendingZeroCrossingLongitude() const;

    inline void SetAscendingZeroCrossingLongitude(double longitude);

protected:

private:

    inline void SetRevNumber(int number);

    static inline string GetClassName();

    int               revolutionNumber_a;
    double              ascendingZeroCrossingLongitude_a;
};

inline
const int
Revolution::GetRevNumber() const
{
    return(revolutionNumber_a);
}

inline
void
Revolution::SetRevNumber(int number)
{
    revolutionNumber_a = number;
}

inline
const double
Revolution::GetAscendingZeroCrossingLongitude() const
{
    return(ascendingZeroCrossingLongitude_a);
}

inline
void
Revolution::SetAscendingZeroCrossingLongitude(double longitude)
{
    ascendingZeroCrossingLongitude_a = longitude;
}

inline
string
Revolution::GetClassName()
{
    return(REVOLUTION_H);
}

#endif
