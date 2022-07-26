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
 * Earth.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef EARTH_H
#define EARTH_H "Earth"

#include <fstream>
#include <iostream>
#include <memory>

#include "general/array/ArrayOfLinkLists.h"
#include "general/utility/Constant.h"

using namespace std;

class InputDataElement;
class Region;
class Requirement;
class SpaceVector;

/**
 * Class which represents a the Earth.  It mainly is a container for the Region
 * objects which represent WACs or ATCs.
 *
 * Name                           Description (units)
 * -------------                  ----------------------------------------------
 * ptrLowestRegionNumberArray_s*  array of the lowest region number in each
 *                                  latitude bin in ptrRegionLatitudeBinList_a
 * ptrHighestRegionNumberArray_s* array of the highest region number in each
 *                                  latitude bin in ptrRegionLatitudeBinList_a
 * latitudeDivisionsPerRegion_s   used to define WAC (0), ATC (5), or WTM (20)
 *                                  level
 * longitudeDivisionsPerRegion_s  used to define WAC (0), ATC (5), or WTM (20)
 *                                  level
 * RADIUS_NMI_s                   radius of the earth in nautical miles
 * MU_NMI_SEC_s                   earth's gravitational constant
 * PERIOD_SEC_s                   period of the earth's rotation
 * ptrRegionLatitudeBinList_s*    array by number of latitude bins of lists of
 *                                  instances of Region
 *
 * @author Brian Fowler
 * @date 09/01/11
 */
class Earth
{
public:

    virtual ~Earth();

    /**
     * Adds regionts between the upper/lower latitude and within the earth angle parameters
     *
     * @param *ptrRegionList the region list to "return"
     * @param upperLatitude the upper latitude constraint
     * @param lowerLatitude the lower latitude constraint
     * @param &vehiclePosition the vehicle position
     * @param maximumEarthAngle the earth angle constraint (for getting regions in the cone)
     */
    static void   AddRegions(LinkList<Region> *ptrRegionList,
                             double upperLatitude,
                             double lowerLatitude,
                             const SpaceVector &vehiclePosition,
                             double maximumEarthAngle);
    static void   AddAllRegions(LinkList<Region> *ptrRegionList,
                                double upperLatitude,
                                double lowerLatitude);

    static void   CreateRegions(shared_ptr<InputDataElement> ptrInputDataElement);
    static void   DestroyRegions();

    static int  GetAccessLevel(int regionNumber,
                               int serviceDefinition,
		                       int subRegionNumber = 1);

    static double GetRadius(const DISTANCE_UNIT &distanceUnits);
    static double GetStoreRadius();
    static double GetMU(const DISTANCE_UNIT &distanceUnits,
                        const TIME_UNIT &timeUnits);
    static double GetStoreMU();
    static double GetPeriod(const TIME_UNIT &timeUnits);
    static double GetStorePeriod();

    static Region* GetRegion(double fullRegionNumber);
    static Region* GetRegion(int regionNumber,
                             int subRegionNumber);
    static Region* GetRegion(double latitude,
                             double longitude);
    static Region* GetRegion(const SpaceVector &inputVector);

    static const LinkList<Region>* GetRegionLatitudeBinList(int index);
    static const LinkList<Region>* GetRegionLatitudeList(double latitude);

    static inline int   GetNumberOfLatitudeBins();

    static inline int GetLatitudeDivisionsPerRegion();
    static inline int GetLongitudeDivisionsPerRegion();
	static inline int GetNumberOfServiceTierDefinitions();
    static inline int GetSubRegionsPerRegion();

    static inline double GetRadius();
    static inline double GetMU();
    static inline double GetPeriod();

    static inline ArrayOfLinkLists<Region>* GetRegionLatitudeBinListArray();

protected:

private:

    Earth();

    static inline string GetClassName();

    static Array<double>             *ptrLowestRegionNumberArray_s;
    static Array<double>             *ptrHighestRegionNumberArray_s;
    static int                      latitudeDivisionsPerRegion_s;
    static int                      longitudeDivisionsPerRegion_s;
	static int                      numberOfServiceTierDefinitions_s;
    static ArrayOfLinkLists<Region> *ptrRegionLatitudeBinList_s;
};

inline
int
Earth::GetLatitudeDivisionsPerRegion()
{
    return(latitudeDivisionsPerRegion_s);
}

inline
int
Earth::GetLongitudeDivisionsPerRegion()
{
    return(longitudeDivisionsPerRegion_s);
}

//RAGUSA 11/27/2018
inline
int
Earth::GetNumberOfServiceTierDefinitions()
{
	return(numberOfServiceTierDefinitions_s);
}

inline
int
Earth::GetSubRegionsPerRegion()
{
    return(latitudeDivisionsPerRegion_s * longitudeDivisionsPerRegion_s);
}

inline
double
Earth::GetRadius()
{
    return(RADIUS_NMI);
}

inline
double
Earth::GetMU()
{
    return(MU_NMI_SEC);
}

inline
double
Earth::GetPeriod()
{
    return(PERIOD_SEC);
}

inline
int
Earth::GetNumberOfLatitudeBins()
{
    int numberOfLatitudeBins = 0;

    if (ptrRegionLatitudeBinList_s != NULL) {
        numberOfLatitudeBins = ptrRegionLatitudeBinList_s->Size();
    }

    return(numberOfLatitudeBins);
}

inline
ArrayOfLinkLists<Region>*
Earth::GetRegionLatitudeBinListArray()
{
    return(ptrRegionLatitudeBinList_s);
}

inline
string
Earth::GetClassName()
{
    return(EARTH_H);
}

#endif
