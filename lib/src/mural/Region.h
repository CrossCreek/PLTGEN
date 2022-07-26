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
 * Region.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef REGION_H
#define REGION_H "Region"

#include <fstream>
#include <iostream>
#include <string>

#include "general/array/Array.h"
#include "general/array/Array2DOfLinkLists.h"
#include "general/utility/Constant.h"
#include "general/data/UnitData.h"

using namespace std;

class SpaceVector;

/**
 * Class which represents a rectangular region on the the Earth.  Typically this is a WAC, but
 * can be configured to represent ATC's.
 *
 * Region has a sub region attribute which is used if the region is sub divied via the
 * region.input file.  For WAC 1 ATC 1 the region number would be 1.1
 *
 * Class Attribute Descriptions
 *
 * Name                            Description (units)
 * -------------                   ---------------------------------------------
 * SUB_REGION_DIVIDER_s            value used to calculate the sub-region (only
 *                                   if used) as a fractional value (i.e. 7.011
 *                                   is Region 7, Sub-region 11)
 * usingSubRegions_s               indicates if sub-regions (i.e. ATCs or WTMs)
 *                                   are being used
 * ptrAreaOfInterestArray_s*       array of AOI region numbers - DEPRECATED
 * ptrAccessDefintionsArray_s*     array of serivce tier definitions for this region (including AOIs)
 * ptrAccessLevelArray_s*          array of the access level(s)
 * ptrGeoRegionLabelArray_s*       array of geo-region designators
 * regionNumber_a                  number of this region (i.e. WAC)
 * subRegionNumber_a               sub-region (i.e. ATC) of the region (if used)
 * northernLatitude_a              upper latitude of this region
 * southernLatitude_a              lower latitude of this region
 * westernLongitude_a              left most longitude of this region
 * easternLongitude_a              right most longitude of this region
 * containsLandMass_a              indicates if this region has land mass
 * revisitRegion_a                 flag to include region in Revisit calculations
 * accessLevel_a                   defines the access value (Tier 1, Tier 2, AOI)
 * geoRegionLabel_a                name identification of the geo-region this
 *                                   region is included in (used for grouping)
 * ptrRequirementList_a            list of requirements (aggregate of targets)
 *                                   contained in this region
 * ptrTargetRegionList_a           list of targets (either entirely or partially)
 *                                   contained in this region
 *
 * @author Brian Fowler
 * @date 09/01/11
 */
class Region
{
public:

    Region();
    Region(int regionNumber,
           int subRegionNumber,
           double northernLatitude,
           double southernLatitude,
           double westernLongitude,
           double easternLongitude,
           const bool &containsLand,
           const string &geoRegionLabel,
           const bool &revisitRegion,
           Array<int> *accessTierArray,
		   Array<int> *accessAOIArray);
    Region(const Region &copyMe);
    virtual ~Region();

    Region&   operator =  (const Region &copyMe);
    bool      operator == (const Region &compareMe) const;
    bool      operator != (const Region &compareMe) const;

    static void  DestroyRegion();
    static void  PrintRegionNumber(ostream &outfile,
                                   double regionNumber);
    static void  PrintFixedWidthRegionNumber(ostream &outfile,
            double regionNumber);

    void         PrintRegionNumber(ostream &outfile) const;
    void         PrintFixedWidthRegionNumber(ostream &outfile) const;

    /**
     * Gets the Center Earth Angle representing the angle between the position
     * and the region center
     *
     * @param &vehiclePosition the vehicle position
     * @param j2000Time to convert to ECI if desired
     * @return double the earth angle
     */
    double       GetCenterEarthAngle(const SpaceVector &vehiclePosition,
                                     double j2000Time = -1.0) const;
    double       GetMinimumEarthAngle(const SpaceVector &vehiclePosition,
                                      double j2000Time = -1.0) const;
    double       GetMaximumEarthAngle(const SpaceVector &vehiclePosition,
                                      double j2000Time = -1.0) const;
    /**
     * Method to return the Region Center in a specified reference frame.  If the time
     *
     * @param j2000Time to convert to ECI if desired
     * @param REFERENCE_FRAME a reference frame to convert to
     * @return SpaceVector vector represnting the region center in desired reference frame
     */
    SpaceVector  GetRegionCenter(double j2000Time = -1.0,
                                 REFERENCE_FRAME returnFrame = UnitData::GetInternalReferenceFrame()) const;
	//RAGUSA 11/27/2018 - Removed 
    //static inline void   SetAreaOfInterest(int regionNumber);
    static inline void   SetUsingSubRegions(const bool &usingSubRegions);

    static inline int  GetAccessLevel(int accessLevelIndex);

    static inline int   GetAccessLevelIndex(int accessLevel);
    static inline int   GetNumberOfAccessLevels();
    static inline int   GetNumberOfRegions();
    static inline int   GetNumberOfGeoRegions();

    static inline double  GetSubRegionDivider();
    static inline double  GetFullRegionNumber(int regionNumber,
            int subRegionNumber);

	//RAGUSA 11/27/2018
    inline bool   IsAreaOfInterest(int serviceDefinition) const;
    static inline bool   UsingSubRegions();

    static inline string GetGeoRegionLabel(int geoRegionIndex);

    inline int  GetRegionAccessLevel(int serviceDefinition) const;
    inline int  GetSubRegionNumber() const;

    inline int   GetRegionNumber() const;
	//RAGUSA 11/27/2018 - Not Used?
    //inline int   GetNumberOfTargetRegions() const;
    inline int   GetGeoRegionIndex() const;

    inline double  GetFullRegionNumber() const;
    inline double  GetNorthernLatitude() const;
    inline double  GetSouthernLatitude() const;
    inline double  GetWesternLongitude() const;
    inline double  GetEasternLongitude() const;
    inline double  GetMiddleLatitude() const;
    inline double  GetMiddleLongitude() const;
    inline double  GetLatitudeRange() const;
    inline double  GetLongitudeRange() const;
    inline bool   ContainsLandMass() const;
    inline bool   IsRevisitRegion() const;
    inline bool   ContainsLatitude(double latitude) const;
    inline bool   ContainsLongitude(double longitude) const;

    inline string GetGeoRegionLabel() const;

    static const int      SUB_REGION_DIVIDER_s;

protected:

private:

    inline void SetRegionNumber(int newNumber);
    inline void SetNorthernLatitude(double newValue);
    inline void SetSouthernLatitude(double newValue);
    inline void SetWesternLongitude(double newValue);
    inline void SetEasternLongitude(double newValue);

    static inline string GetClassName();

    static int               numberOfRegions_s;
    static bool              usingSubRegions_s;
    static Array<int>       *ptrAccessLevelArray_s;
	//RAGUSA 11/27/2018 - Each Region has an array of defintions
    Array<int>              *ptrAccessTierArray_a;
	Array<int>              *ptrAccessAOIArray_a;
    //static Array<int>     *ptrAreaOfInterestArray_s;
	//int                   accessLevel_a;
	//========================================================
    static Array<string>    *ptrGeoRegionLabelArray_s;
    int                     subRegionNumber_a;
    int                     regionNumber_a;
    double                  northernLatitude_a;
    double                  southernLatitude_a;
    double                  westernLongitude_a;
    double                  easternLongitude_a;
    bool                    containsLandMass_a;
    bool                    revisitRegion_a;
    string                  geoRegionLabel_a;
};

inline
int
Region::GetAccessLevel(int accessLevelIndex)
{
    int accessLevel = 0;

    if (ptrAccessLevelArray_s != NULL) {
        accessLevel = ptrAccessLevelArray_s->ElementAt(accessLevelIndex);
    }

    return(accessLevel);
}

inline
int
Region::GetAccessLevelIndex(int accessLevel)
{
    int accessLevelIndex = -1;

    if (ptrAccessLevelArray_s != NULL) {
        accessLevelIndex = ptrAccessLevelArray_s->FindIndex(accessLevel);
    }

    return(accessLevelIndex);
}

inline
int
Region::GetNumberOfAccessLevels()
{
    int numberOfAccessLevels = 0;

    if (ptrAccessLevelArray_s != NULL) {
        numberOfAccessLevels = ptrAccessLevelArray_s->Size();
    }

    return(numberOfAccessLevels);
}

inline
int
Region::GetNumberOfRegions()
{
    return(numberOfRegions_s);
}

inline
int
Region::GetNumberOfGeoRegions()
{
    int numberOfGeoRegions = 0;

    if (ptrGeoRegionLabelArray_s != NULL) {
        numberOfGeoRegions = ptrGeoRegionLabelArray_s->Size();
    }

    return(numberOfGeoRegions);
}

inline
string
Region::GetGeoRegionLabel(int geoRegionIndex)
{
    string geoRegionLabel = "";

    if (ptrGeoRegionLabelArray_s != NULL) {
        if ( (geoRegionIndex >= 0) &&
                (geoRegionIndex < ptrGeoRegionLabelArray_s->Size()) ) {
            geoRegionLabel = ptrGeoRegionLabelArray_s->ElementAt(geoRegionIndex);
        }
    }

    return(geoRegionLabel);
}

/*RAGUSA 11/27/218 - Removed for Column Input Method
inline
void
Region::SetAreaOfInterest(int regionNumber)
{
    if (ptrAreaOfInterestArray_s == NULL) {
        ptrAreaOfInterestArray_s = new Array<int>();
    }

    ptrAreaOfInterestArray_s->AddUniqueElement(regionNumber);
}
*/

inline
void
Region::SetUsingSubRegions(const bool &usingSubRegions)
{
    usingSubRegions_s = usingSubRegions;
}

inline
double
Region::GetSubRegionDivider()
{
    return(SUB_REGION_DIVIDER_s);
}

inline
double
Region::GetFullRegionNumber(int regionNumber,
                            int subRegionNumber)
{
    double fullRegionNumber = (double)regionNumber;

    if ((usingSubRegions_s == true) && (subRegionNumber > 0)) {
        fullRegionNumber += (double)subRegionNumber / SUB_REGION_DIVIDER_s;
    }

    return(fullRegionNumber);
}

inline
bool
Region::UsingSubRegions()
{
    return(usingSubRegions_s);
}

//RAGUSA 11/27/2018 - Reimplemented method to determine if this region is an AOI
//Requires the service definition number to index the array
inline
bool
Region::IsAreaOfInterest(int serviceDefinition) const
{
    bool isAreaOfInterest = false;
	//Definition 1 is at element 0
	int definition = serviceDefinition - 1;
    if (ptrAccessAOIArray_a->ElementAt(definition) != NULL && ptrAccessAOIArray_a->ElementAt(definition) == 1){
        isAreaOfInterest = true;
    }
    return(isAreaOfInterest);
}
//RAGUSA 11/27/2018 - Reimplemented method to return Access Level of this Region
//Requires the service definition number to index the array
inline
int
Region::GetRegionAccessLevel(int serviceDefinition) const
{
	//Definition 1 is at element 0
	int definition = serviceDefinition - 1;
    return(ptrAccessTierArray_a->ElementAt(definition));
}
//================================================================================
inline
int
Region::GetSubRegionNumber() const
{
    return(subRegionNumber_a);
}

inline
int
Region::GetRegionNumber() const
{
    return(regionNumber_a);
}

inline
int
Region::GetGeoRegionIndex() const
{
    int geoRegionIndex = -1;

    if (ptrGeoRegionLabelArray_s != NULL) {
        geoRegionIndex = ptrGeoRegionLabelArray_s->FindIndex(geoRegionLabel_a);
    }

    return(geoRegionIndex);
}

inline
double
Region::GetNorthernLatitude() const
{
    return(northernLatitude_a);
}

inline
double
Region::GetSouthernLatitude() const
{
    return(southernLatitude_a);
}

inline
double
Region::GetWesternLongitude() const
{
    return(westernLongitude_a);
}

inline
double
Region::GetEasternLongitude() const
{
    return(easternLongitude_a);
}

inline
double
Region::GetMiddleLatitude() const
{
    return((northernLatitude_a + southernLatitude_a) / 2.0);
}

inline
double
Region::GetMiddleLongitude() const
{
    double middleLongitude = (westernLongitude_a + easternLongitude_a) / 2.0;

    // check to see if this region crosses the date line
    if (westernLongitude_a > easternLongitude_a) {
        if (middleLongitude <= 0.0) {
            middleLongitude += DATE_LINE_DEGREES;
        } else {
            middleLongitude -= DATE_LINE_DEGREES;
        }
    }

    return(middleLongitude);
}

inline
double
Region::GetLatitudeRange() const
{
    return(northernLatitude_a - southernLatitude_a);
}

inline
double
Region::GetLongitudeRange() const
{
    double longitudeRange = easternLongitude_a - westernLongitude_a;

    // check to see if this region crosses the date line
    if (westernLongitude_a > easternLongitude_a) {
        longitudeRange += (2.0 * DATE_LINE_DEGREES);
    }

    return(longitudeRange);
}

inline
double
Region::GetFullRegionNumber() const
{
    return(GetFullRegionNumber(regionNumber_a, subRegionNumber_a));
}

inline
bool
Region::ContainsLandMass() const
{
    return(containsLandMass_a);
}

inline
bool
Region::IsRevisitRegion() const
{
    return(revisitRegion_a);
}

inline
bool
Region::ContainsLatitude(double latitude) const
{
    return((latitude <= northernLatitude_a) && (latitude >= southernLatitude_a));
}

inline
bool
Region::ContainsLongitude(double longitude) const
{
    bool containsLongitude = false;

    if (westernLongitude_a < easternLongitude_a) {
        if ((longitude >= westernLongitude_a) && (longitude <= easternLongitude_a)) {
            containsLongitude = true;
        }
    } else if ( (longitude >= westernLongitude_a) ||
                (longitude <= easternLongitude_a) ) {
        containsLongitude = true;
    }

    return(containsLongitude);
}

inline
string
Region::GetGeoRegionLabel() const
{
    return(geoRegionLabel_a);
}

inline
void
Region::SetRegionNumber(int newNumber)
{
    regionNumber_a = newNumber;
}

inline
void
Region::SetNorthernLatitude(double newValue)
{
    northernLatitude_a = newValue;
}

inline
void
Region::SetSouthernLatitude(double newValue)
{
    southernLatitude_a = newValue;
}

inline
void
Region::SetWesternLongitude(double newValue)
{
    westernLongitude_a = newValue;
}

inline
void
Region::SetEasternLongitude(double newValue)
{
    easternLongitude_a = newValue;
}

inline
string
Region::GetClassName()
{
    return(REGION_H);
}

#endif
