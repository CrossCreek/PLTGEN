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
 * ConjunctionPeriod.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "ConjunctionPeriod.h"


#include "general/time/TimePiece.h"

bool ConjunctionPeriod::anyNarrowbandConjunction_s = false;
bool ConjunctionPeriod::anyWidebandConjunction_s = false;

ConjunctionPeriod::ConjunctionPeriod()
    : TimePeriod(), numberOfSimultaneousSenders_a(0), ptrReceiverDesignator_a(NULL),
      ptrSender1Designator_a(NULL), ptrSender2Designator_a(NULL)
{
    ptrReceiverDesignator_a = new string("UNKNOWN");
    ptrSender1Designator_a = new string("UNKNOWN");
    ptrSender2Designator_a = new string("UNKNOWN");
    return;
}

ConjunctionPeriod::ConjunctionPeriod(int startTimeIndex,
                                     int endTimeIndex,
                                     int simultaneousSenders,
                                     const string &newReceiverType,
                                     const string &newSender1Type,
                                     const string &newSender2Type,
                                     const CONJUNCTION_TYPE &type)
    : TimePeriod(startTimeIndex, endTimeIndex),
      numberOfSimultaneousSenders_a(simultaneousSenders), ptrReceiverDesignator_a(NULL),
      ptrSender1Designator_a(NULL), ptrSender2Designator_a(NULL)
{
    ptrReceiverDesignator_a = new string(newReceiverType);
    ptrSender1Designator_a = new string(newSender1Type);
    ptrSender2Designator_a = new string(newSender2Type);

    if (type == NB) {
        anyNarrowbandConjunction_s = true;
    } else if (type == WB) {
        anyWidebandConjunction_s = true;
    }

    return;
}

ConjunctionPeriod::ConjunctionPeriod(const ConjunctionPeriod &copyMe)
    : TimePeriod(copyMe),
      numberOfSimultaneousSenders_a(copyMe.numberOfSimultaneousSenders_a),
      ptrReceiverDesignator_a(NULL), ptrSender1Designator_a(NULL),
      ptrSender2Designator_a(NULL)
{
    ptrReceiverDesignator_a
        = new string(*(copyMe.ptrReceiverDesignator_a));
    ptrSender1Designator_a
        = new string(*(copyMe.ptrSender1Designator_a));
    ptrSender2Designator_a
        = new string(*(copyMe.ptrSender2Designator_a));
    return;
}

ConjunctionPeriod&
ConjunctionPeriod::operator = (const ConjunctionPeriod &copyMe)
{
    TimePeriod::operator = (copyMe);
    numberOfSimultaneousSenders_a = copyMe.numberOfSimultaneousSenders_a;
    ptrReceiverDesignator_a
        = new string(*(copyMe.ptrReceiverDesignator_a));
    ptrSender1Designator_a
        = new string(*(copyMe.ptrSender1Designator_a));
    ptrSender2Designator_a
        = new string(*(copyMe.ptrSender2Designator_a));
    return(*this);
}

bool
ConjunctionPeriod::operator == (const ConjunctionPeriod &compareMe) const
{
    return( (TimePeriod::operator == (compareMe)) &&
            (numberOfSimultaneousSenders_a == compareMe.numberOfSimultaneousSenders_a) &&
            (*ptrReceiverDesignator_a == *(compareMe.ptrReceiverDesignator_a)) &&
            (*ptrSender1Designator_a == *(compareMe.ptrSender1Designator_a)) &&
            (*ptrSender2Designator_a == *(compareMe.ptrSender2Designator_a)) );
}

bool
ConjunctionPeriod::operator != (const ConjunctionPeriod &compareMe) const
{
    return(!(operator == (compareMe)));
}

ConjunctionPeriod::~ConjunctionPeriod()
{
    if (ptrReceiverDesignator_a != NULL) {
        delete ptrReceiverDesignator_a;
        ptrReceiverDesignator_a = NULL;
    }

    if (ptrSender1Designator_a != NULL) {
        delete ptrSender1Designator_a;
        ptrSender1Designator_a = NULL;
    }

    if (ptrSender2Designator_a != NULL) {
        delete ptrSender2Designator_a;
        ptrSender2Designator_a = NULL;
    }

    return;
}
