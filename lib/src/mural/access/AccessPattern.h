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
 * AccessPattern.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#ifndef ACCESS_PATTERN_H
#define ACCESS_PATTERN_H "AccessPattern"

#include <iostream>
#include <iomanip>
#include <map>
//RAGUSA 7/24/2017 - Needed to resolve shared_ptr removal from std library.
#include <memory>

using namespace std;

template<class Type> class LinkList;
class ConeAngleData;
class InputDataElement;
class Region;
class SpaceVector;

/**
 *  AccessPattern is the class that represents a cone of points
 *  that can be seen from a given vantage point.
 *
 * Class Attribute Descriptions
 *
 * Name                              Description (units)
 * -------------                     -------------------------------------------
 * broadsideAngle_a                  angle to define the pattern symmetrically
 *                                     to both sides of the ground track
 *                                     ('squint' for BUTTERFLY, 'doppler cone'
 *                                     for AXEBLADE)
 * alwaysIncludeSubSatelliteRegion_a indicates if the sub-satellite Region
 *                                     should be included if it is not in the
 *                                     access pattern
 * patternType_a                     type of access pattern (PATTERN_TYPE enum)
 * ptrConeAngleDataList_a*           list of the outer elevation angle
 *                                     (mandatory) and inner elevaltion angle
 *                                     (optional) data for default and mission
 *                                     specific patterns
 */
class AccessPattern
{
public:

    enum PATTERN {
        DEFAULT_PATTERN = 0,
        MISSION_SPECIFIC = 1,
        LARGEST_PATTERN = 2,
        REVISIT = 3,
        //RAGUSA 1/9/2018 - Add CRISIS mission pattern for using Crisis Region Mode, this is
        //a slightly different way this enum is used but it works for our purposes here
        //as simply a secondary access pattern for Crisis Region Mode
        CRISIS_PATTERN = 4
    };

    enum PATTERN_TYPE {
        UNKNOWN_TYPE = -1,
        VISIBILITY_CONE = 0,
        DOUGHNUT = 1,
        BUTTERFLY = 2,
        AXEBLADE = 3
    };

    AccessPattern();
    AccessPattern(const AccessPattern &copyMe);
    virtual ~AccessPattern();

    AccessPattern&  operator =  (const AccessPattern &copyMe);
    bool            operator == (const AccessPattern &compareMe) const;
    bool            operator != (const AccessPattern &compareMe) const;

    /**
     * MEthod to traslate a pattern type from a string to the enum
     *
     * @param &inputString the input string to translate from
     * @return PATTERN_TYPE the converted pattern type
     */
    static PATTERN_TYPE GetPatternTypeFromString(const string &inputString);

    /**
     * Method to translate a pattern type to a string representation
     *
     * @param PATTERN_TYPE the pattern type
     * @return string
     */
    static string GetPatternTypeString(const PATTERN_TYPE &patternType);

    /**
     * Prints the attributes of a access pattern
     */
    static void PrintAttributes(const shared_ptr<AccessPattern> &accessPattern, ostream &outfile);

    /**
     * Method which gets all the contianed regions in the access pattern
     *
     * @param *ptrRegionList the full region list
     * @param &vehiclePosition the vehicle position
     * @param &vehicleVelocity the vehicle velocity
     * @param &timeStep the simulation time step
     * @param &pattern the pattern type
     * @param &the specific mission string
     */
    virtual void   GetAllContainedRegions(LinkList<Region> *ptrRegionList,
                                          const SpaceVector &vehiclePosition,
                                          const SpaceVector &vehicleVelocity,
                                          const PATTERN &pattern,
                                          const string &missionString = "") const = 0;

    virtual bool   InsideAccessPattern(const string &missionString,
                                       const SpaceVector &targetPosition,
                                       const SpaceVector &vehiclePosition,
                                       const SpaceVector &vehicleVelocity) const = 0;



    double GetOuterConeElevationAngle(const PATTERN &pattern,
                                      const string &missionString = "") const;
    double GetOuterConeElevationAngle(const string &missionString) const;

    double GetInnerConeElevationAngle(const PATTERN &pattern,
                                      const string &missionString = "") const;
    double GetInnerConeElevationAngle(const string &missionString) const;


    bool   MissionSpecificEqualsDefault(const string &missionString) const;
    bool   MissionSpecificEqualsLargest(const string &missionString) const;



    inline double GetBroadsideAngle() const;

    /**
     * Gets the Pattern Type
     *
     * @return PATTERN_TYPE the pattern
     */
    PATTERN_TYPE GetPatternType() const;

    /**
     * Sets the Pattern Type
     *
     * @param patternType the pattern type
     */
    void  SetPatternType(PATTERN_TYPE patternType);

    /**
     * Sets the always include sub sat boolean.
     *
     * @param alwaysInclde the boolean to set
     */
    void   SetAlwaysIncludeSubSatelliteRegion(bool alwaysInclude);

    /**
     * Gets the always include sub sat boolean.
     *
     * @return bool always Include sub sat boolean
     */
    bool   GetAlwaysIncludeSubSatelliteRegion() const;

    /**
     * Gets the Outer Cone Angle Data
     *
     * @return  Outer Cone angle data
     */
    shared_ptr<ConeAngleData>   GetOuterConeAngleData() const;

    /**
     * Gets the Inner Cone Angle Data
     *
     * @return  Inner Cone angle data
     */
    shared_ptr<ConeAngleData>   GetInnerConeAngleData() const;

    /**
     * Sets the Outer Cone Angle Data
     *
     * @param coneAngleData outer cone angle data
     */
    void   SetOuterConeAngleData(const shared_ptr<ConeAngleData> &coneAngleData);

    /**
    * Sets the Inner Cone Angle Data
    *
    * @param coneAngleData inner cone angle data
    */
    void   SetInnerConeAngleData(const shared_ptr<ConeAngleData> &coneAngleData);

protected:

    PATTERN_TYPE                patternType_a;
    bool                        alwaysIncludeSubSatelliteRegion_a;
    shared_ptr<ConeAngleData>   ptrOuterConeAngleData_a;
    shared_ptr<ConeAngleData>   ptrInnerConeAngleData_a;

    /**
     * Method to get the regions in the cone.  Common among the filter routines.
     * Uses the method on the Earth to calculate this based on the vehicle
     * position and the earth angle
     *
     * @param vehiclePosition the vehicle position
     * @param radiusInDegrees the ground range radius
     * @param maximumEarthAngle earth angle of the trig calculation
     * @param j2000Time optional the time index
     * @return LinkList<Region> the list of regions
     */
    LinkList<Region>* GetRegionsInTheCone(const SpaceVector &vehiclePosition,
                                          double radiusInDegrees,
                                          double maximumEarthAngle) const;

    /**
     * Method to calculate the squint angle by getting the speration angle of the
     * intrack and velocity normal vectors.  Where:
     *
     * intrack         = vehicleToTargetLOS cross vehiclePosition.Negated
     * velocity normal = vehicleVelcoity    cross vehiclePosition.Negated
     *
     * @param &vehicleToTargetLOS the target line of sight vector
     * @param &vehiclePosition the position vector
     * @param &vehicleVelocity the velocity vector
     * @return double the squint angle
     */
    double CalculateSquintAngle(const SpaceVector &vehicleToTargetLOS,
                                const SpaceVector &vehiclePosition,
                                const SpaceVector &vehicleVelocity) const;

private:

    static inline string GetClassName();


};

inline
string
AccessPattern::GetClassName()
{
    return(ACCESS_PATTERN_H);
}

#endif
