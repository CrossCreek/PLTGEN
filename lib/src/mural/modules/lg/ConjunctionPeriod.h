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
 * ConjunctionPeriod.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef CONJUNCTION_PERIOD_H
#define CONJUNCTION_PERIOD_H "ConjunctionPeriod"

#include <string>
#include <iostream>
#include <iomanip>


#include "general/utility/Constant.h"
#include "general/data/TimePeriod.h"

using namespace std;

/**
 * Class to represent a calculated or read in conjunction period to be used
 * in the allocation process.
 *
 * Name                           Description (units)
 * -------------                  ---------------------------------------------
 * anyNarrowbandConjunction_s     indicates if any data provider has a
 *                                  narrowband conjunction issue
 * anyWidebandConjunction_s       indicates if any data provider has a
 *                                  wideband conjunction issue
 * numberOfSimultaneousSenders_a  number of senders that, if simultaneously
 *                                  allocated, will violate this conjunction
 *                                  period
 * ptrSender1Designator_a*        designator of first sender (string pointer)
 * ptrSender2Designator_a*        designator of second sender (string pointer)
 * ptrReceiverDesignator_a*       designator of receiver (string pointer)
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */

class ConjunctionPeriod : public TimePeriod
{
public:

    enum CONJUNCTION_TYPE {
        UNKNOWN = -1,
        NB = 0,
        WB = 1
    };

    ConjunctionPeriod();
    ConjunctionPeriod(int startTimeIndex,
                      int endTimeIndex,
                      int simultaneousSenders,
                      const string &newReceiverDesignator,
                      const string &newSender1Designator,
                      const string &newSender2Designator,
                      const CONJUNCTION_TYPE &type);
    ConjunctionPeriod(const ConjunctionPeriod &copyMe);
    virtual ~ConjunctionPeriod();

    ConjunctionPeriod& operator =  (const ConjunctionPeriod &copyMe);
    bool               operator == (const ConjunctionPeriod &compareMe) const;
    bool               operator != (const ConjunctionPeriod &compareMe) const;

    static inline bool AnyNarrowbandConjunction();
    static inline bool AnyWidebandConjunction();

    inline int  GetNumberOfSimultaneousSenders() const;

    inline string GetReceiverDesignator() const;
    inline string GetSender1Designator() const;
    inline string GetSender2Designator() const;

protected:

private:

    static inline string GetClassName();

    static bool         anyNarrowbandConjunction_s;
    static bool         anyWidebandConjunction_s;

    int               numberOfSimultaneousSenders_a;
    string             *ptrReceiverDesignator_a;
    string             *ptrSender1Designator_a;
    string             *ptrSender2Designator_a;
};

inline
bool
ConjunctionPeriod::AnyNarrowbandConjunction()
{
    return(anyNarrowbandConjunction_s);
}

inline
bool
ConjunctionPeriod::AnyWidebandConjunction()
{
    return(anyWidebandConjunction_s);
}

inline
int
ConjunctionPeriod::GetNumberOfSimultaneousSenders() const
{
    return(numberOfSimultaneousSenders_a);
}

inline
string
ConjunctionPeriod::GetReceiverDesignator() const
{
    return(*ptrReceiverDesignator_a);
}

inline
string
ConjunctionPeriod::GetSender1Designator() const
{
    return(*ptrSender1Designator_a);
}

inline
string
ConjunctionPeriod::GetSender2Designator() const
{
    return(*ptrSender2Designator_a);
}

inline
string
ConjunctionPeriod::GetClassName()
{
    return(CONJUNCTION_PERIOD_H);
}

#endif
