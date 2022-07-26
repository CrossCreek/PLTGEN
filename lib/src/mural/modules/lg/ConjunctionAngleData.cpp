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
 * ConjunctionAngleData.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "ConjunctionAngleData.h"

ConjunctionAngleData::ConjunctionAngleData()
    : numberOfSimultaneousSenders_a(0), conjunctionAngle_a(0.00),
      ptrNearDesignator_a(NULL), ptrFarDesignator_a(NULL)
{
    ptrNearDesignator_a = new string("UNKNOWN");
    ptrFarDesignator_a = new string("UNKNOWN");
    return;
}

ConjunctionAngleData::ConjunctionAngleData(double newAngle,
        const string &newActionTagNear,
        const string &newActionTagFar,
        int simultaneousSenders)
    : numberOfSimultaneousSenders_a(simultaneousSenders), conjunctionAngle_a(newAngle),
      ptrNearDesignator_a(NULL), ptrFarDesignator_a(NULL)
{
    ptrNearDesignator_a = new string(newActionTagNear);
    ptrFarDesignator_a = new string(newActionTagFar);
    return;
}

ConjunctionAngleData::ConjunctionAngleData(const ConjunctionAngleData &copyMe)
    : numberOfSimultaneousSenders_a(copyMe.numberOfSimultaneousSenders_a),
      conjunctionAngle_a(copyMe.conjunctionAngle_a),
      ptrNearDesignator_a(NULL), ptrFarDesignator_a(NULL)
{
    ptrNearDesignator_a = new string(*(copyMe.ptrNearDesignator_a));
    ptrFarDesignator_a = new string(*(copyMe.ptrFarDesignator_a));
    return;
}

ConjunctionAngleData&
ConjunctionAngleData::operator = (const ConjunctionAngleData &copyMe)
{
    numberOfSimultaneousSenders_a = copyMe.numberOfSimultaneousSenders_a;
    conjunctionAngle_a            = copyMe.conjunctionAngle_a;
    ptrNearDesignator_a           = new string(*(copyMe.ptrNearDesignator_a));
    ptrFarDesignator_a            = new string(*(copyMe.ptrFarDesignator_a));
    return(*this);
}

bool
ConjunctionAngleData::operator == (const ConjunctionAngleData &compareMe) const
{
    return( (numberOfSimultaneousSenders_a == compareMe.numberOfSimultaneousSenders_a) &&
            (conjunctionAngle_a == compareMe.conjunctionAngle_a) &&
            (*ptrNearDesignator_a == *(compareMe.ptrNearDesignator_a)) &&
            (*ptrFarDesignator_a == *(compareMe.ptrFarDesignator_a)) );
}

bool
ConjunctionAngleData::operator!=(const ConjunctionAngleData &compareMe) const
{
    return(!(operator == (compareMe)));
}

ConjunctionAngleData::~ConjunctionAngleData()
{
    if (ptrNearDesignator_a != NULL) {
        delete ptrNearDesignator_a;
        ptrNearDesignator_a = NULL;
    }

    if (ptrFarDesignator_a != NULL) {
        delete ptrFarDesignator_a;
        ptrFarDesignator_a = NULL;
    }

    return;
}
