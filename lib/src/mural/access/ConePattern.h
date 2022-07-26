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
 * ConePattern.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#ifndef CONE_PATTERN_H
#define CONE_PATTERN_H "ConePattern"

#include <iostream>
#include <iomanip>
#include "general/array/LinkList.h"
#include "AccessPattern.h"

using namespace std;

class ConeAngleData;
class InputDataElement;
class Region;
class SpaceVector;

/**
 *  ConePattern is the class that represents a basic cone
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */
class ConePattern : public AccessPattern
{
public:

    ConePattern();
    ConePattern(const ConePattern &copyMe);
    virtual ~ConePattern();

    ConePattern&  operator =  (const ConePattern &copyMe);
    bool            operator == (const ConePattern &compareMe) const;
    bool            operator != (const ConePattern &compareMe) const;

    /**
     * Override from AccessPattern
     */
    virtual void   GetAllContainedRegions(LinkList<Region> *ptrRegionList,
                                          const SpaceVector &vehiclePosition,
                                          const SpaceVector &vehicleVelocity,
                                          const PATTERN &pattern,
                                          const string &missionString = "") const;

    /**
     * Override from AccessPattern
     */
    virtual bool   InsideAccessPattern(const string &missionString,
                                       const SpaceVector &targetPosition,
                                       const SpaceVector &vehiclePosition,
                                       const SpaceVector &vehicleVelocity) const;

protected:

private:

    static inline string GetClassName();
};

inline
string
ConePattern::GetClassName()
{
    return(CONE_PATTERN_H);
}

#endif
