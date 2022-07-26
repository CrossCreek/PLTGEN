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
 * OffRegionData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef OFF_REGION_DATA
#define OFF_REGION_DATA "OffRegionData"

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

/**
 * OffRegionData is the class that models an visible sensor that
 *  can be attached to a data provider.
 *
 * Name                  Description (units)
 * -------------         -------------------------------------------------------
 * latitude_a            latitude that divides the off region from the on
 * startLongitude_a      longitude starting the off region
 * endLongitude_a        longitude ending the off region
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */

class OffRegionData
{
public:

    OffRegionData();
    OffRegionData(double latitude,
                  double startLongitude,
                  double endLongitude);
    OffRegionData(const OffRegionData &copyMe);
    virtual ~OffRegionData();

    OffRegionData&  operator =  (const OffRegionData &copyMe);
    bool            operator == (const OffRegionData &compareMe) const;
    bool            operator != (const OffRegionData &compareMe) const;

    inline double GetLatitude() const;
    inline double GetStartLongitude() const;
    inline double GetEndLongitude() const;

    inline bool UseOffRegion(double longitude) const;

protected:

private:

    static inline string GetClassName();

    double              latitude_a;
    double              startLongitude_a;
    double              endLongitude_a;
};

inline
double
OffRegionData::GetLatitude() const
{
    return(latitude_a);
}

inline
double
OffRegionData::GetStartLongitude() const
{
    return(startLongitude_a);
}

inline
double
OffRegionData::GetEndLongitude() const
{
    return(endLongitude_a);
}

inline
bool
OffRegionData::UseOffRegion(double longitude) const
{
    return((longitude >= startLongitude_a) && (longitude < endLongitude_a));
}

inline
string
OffRegionData::GetClassName()
{
    return(OFF_REGION_DATA);
}

#endif
