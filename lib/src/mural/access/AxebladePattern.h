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
 * AxebladePattern.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#ifndef AXEBLADE_PATTERN_H
#define AXEBLADE_PATTERN_H "AxebladePattern"

#include <iostream>
#include <iomanip>
#include <map>

#include "AccessPattern.h"

using namespace std;

template<class Type> class LinkList;
class ConeAngleData;
class InputDataElement;
class Region;
class SpaceVector;

/**
 *  AxebladePattern is the class that represents a axeblade access pattern utilizing both the
 *  outer and inner cone elevation angles to create a donut shape.
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */
class AxebladePattern : public AccessPattern
{
public:

    AxebladePattern();
    AxebladePattern(const AxebladePattern &copyMe);
    virtual ~AxebladePattern();

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

    /**
     * Sets the Broadside angle
     *
     * @param broadsideAngle broadside angle
     */
    void SetBroadSideAngle(double broadsideAngle);

    /**
     * Gets the Broadside angle
     *
     * @return double broadside angle
     */
    double GetBroadSideAngle() const;

protected:

private:

    static inline string GetClassName();

    double                   broadsideAngle_a;
};

inline
string
AxebladePattern::GetClassName()
{
    return(AXEBLADE_PATTERN_H);
}

#endif
