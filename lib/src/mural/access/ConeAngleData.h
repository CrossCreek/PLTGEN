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
 * ConeAngleData.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/

#ifndef CONE_ANGLE_DATA_H
#define CONE_ANGLE_DATA_H "ConeAngleData"

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
//RAGUSA 7/24/2017 - Needed to resolve shared_ptr removal from std library.
#include <memory>

using namespace std;

/**
 * Data class which holds information regarding cone elevation angles.  There is a default value
 * and values based on specific mission strings.  The default will be used if no specific mission
 * value is specified.
 *
 * @author Brian Fowler
 * @date 09/09/2011
 */
class ConeAngleData
{
public:

    ConeAngleData();



    explicit ConeAngleData(const double defaultValue);

    ConeAngleData(const ConeAngleData &copyMe);
    virtual ~ConeAngleData();

    ConeAngleData&  operator =  (const ConeAngleData &copyMe);
    bool            operator == (const ConeAngleData &compareMe) const;
    bool            operator != (const ConeAngleData &compareMe) const;

    /**
     * Adds a specific mission and angle value to the hash map
     *
     * @param missionKey the mission to add
     * @param angleValue the angle to add
     */
    void   AddSpecificMission(const string &missionKey, double angleValue);

    /**
     * Method to return the desired elevation angle value.  If the mission key
     * is not found the default will be returned
     *
     * @param missionKey the mission key to look for
     * @return the angle value (default if the mission is not found)
     */
    double GetAngleValue(const string &missionKey) const;

    /**
     * Method to simply return the default value
     *
     * @return the default value
     */
    double GetDefaultValue() const;

    /**
     * Method which finds the minimum angle across all missions including the default value.  Used
     * when figuring out the Largest access pattern accross mission types for the outer cone.
     * However this excludes the revisit string mission key because this is a special mission key
     * used for the revist calculation and not standard timeline accesses.
     *
     * @return the min angle
     */
    double GetMinimumAngle() const;

    /**
     * Method which finds the maximum angle across all missions including the default value.  Used
     * when figuring out the Largest access pattern accross mission types for the inner cone.
     * However this excludes the revisit string mission key because this is a special mission key
     * used for the revist calculation and not standard timeline accesses.
     *
     * @return the max angle
     */
    double GetMaximumAngle() const;

    /**
     * Method to check if the spcified mission key is in the map or not
     *
     * @param missionKey the key to check
     * @return bool
     */
    bool HasSpecificMission(const string &missionKey) const;

    /**
     * Method to return a vector of all the specific mission tags in the map
     *
     * @return vector of the tags
     */
    shared_ptr<std::vector<string>>  GetSpecificTags() const;

protected:

private:




    double                               defaultElevationAngle_a;
    shared_ptr<std::map<string, double>> ptrSpecficMissionAngleMap_a;
    shared_ptr<std::vector<string>>      ptrSpecficMissionTags_a;
};

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
ConeAngleData::GetClassName()
{
    return(CONE_ANGLE_DATA_H);
}
*/

#endif
