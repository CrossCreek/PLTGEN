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
 * OverheadData.cpp
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#include "OverheadData.h"

OverheadData::OverheadData()
    : acquisitionTime_a(-1), droplinkTime_a(-1), makeBeforeBreakDroplinkTime_a(-1),
      makeBeforeBreakStartUpTime_a(-1), ptrWithReourceDesignator_a(NULL)
{
    return;
}

OverheadData::OverheadData(const string &withDesignator,
                           int acquireTime, int dropTime,
                           int makeBeforeBreakDropTime,
                           int makeBeforeBreakStartUpTime)
    : acquisitionTime_a(acquireTime), droplinkTime_a(dropTime),
      makeBeforeBreakDroplinkTime_a(makeBeforeBreakDropTime),
      makeBeforeBreakStartUpTime_a(makeBeforeBreakStartUpTime),
      ptrWithReourceDesignator_a(NULL)
{
    SetWithResourceDesignator(withDesignator);
    return;
}

OverheadData::OverheadData(const OverheadData &copyMe)
    : acquisitionTime_a(copyMe.acquisitionTime_a),
      droplinkTime_a(copyMe.droplinkTime_a),
      makeBeforeBreakDroplinkTime_a(copyMe.makeBeforeBreakDroplinkTime_a),
      makeBeforeBreakStartUpTime_a(copyMe.makeBeforeBreakStartUpTime_a),
      ptrWithReourceDesignator_a(NULL)
{
    if (copyMe.ptrWithReourceDesignator_a != NULL) {
        ptrWithReourceDesignator_a
            = new string(*(copyMe.ptrWithReourceDesignator_a));
    }

    return;
}

OverheadData&
OverheadData::operator = (const OverheadData &copyMe)
{
    acquisitionTime_a             = copyMe.acquisitionTime_a;
    droplinkTime_a                = copyMe.droplinkTime_a;
    makeBeforeBreakDroplinkTime_a = copyMe.makeBeforeBreakDroplinkTime_a;
    makeBeforeBreakStartUpTime_a  = copyMe.makeBeforeBreakStartUpTime_a;
    ptrWithReourceDesignator_a = NULL;

    if (copyMe.ptrWithReourceDesignator_a != NULL) {
        ptrWithReourceDesignator_a
            = new string(*(copyMe.ptrWithReourceDesignator_a));
    }

    return(*this);
}

bool
OverheadData::operator == (const OverheadData &compareMe) const
{
    return( (*ptrWithReourceDesignator_a == *(compareMe.ptrWithReourceDesignator_a)) &&
            (acquisitionTime_a == compareMe.acquisitionTime_a) &&
            (droplinkTime_a == compareMe.droplinkTime_a) );
}

bool
OverheadData::operator != (const OverheadData &compareMe) const
{
    return(!(operator == (compareMe)));
}

OverheadData::~OverheadData()
{
    if (ptrWithReourceDesignator_a != NULL) {
        delete ptrWithReourceDesignator_a;
        ptrWithReourceDesignator_a = NULL;
    }

    return;
}
