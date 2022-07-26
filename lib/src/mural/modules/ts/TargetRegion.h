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
 * TargetRegion.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef TARGET_REGION_H
#define TARGET_REGION_H "TargetRegion"

#include <string>
#include "general/array/Array.h"

using namespace std;

class Region;
class Sensor;
class SpaceVector;
class Target;

/**
 * Data class to hold information about a "sub region" within the
 * larget Target for targets which overlap region boundaries.
 *
 * @author Brian Fowler
 * @date 12/20/2011
 */
class TargetRegion
{
public:

    TargetRegion();




    TargetRegion(const TargetRegion &targetRegion);

    TargetRegion(double area,
                 int numberOfSensors,
                 Region *ptrRegion,
                 Target *ptrTarget,
                 const bool &scheduleTargets);
    virtual ~TargetRegion();


    void        SetHighestQuality(int sensorIndex,
                                  int quality);
    void        UpdateCollectedArea(int area,
                                    int sensorIndex);

    int       GetQualityLevel(int sensorIndex) const;
    int       GetRequiredQuality(int sensorIndex) const;

    int        GetDeckIndex() const;
    int        GetMissionIndex() const;
    int        GetMissionIndex(int sensorIndex) const;
    int        GetQualityIndex(int sensorIndex) const;
    int        GetModeIndex(int sensorIndex) const;
    int        GetPriorityIndex(int sensorIndex) const;
    int        GetNumberOfCPSs() const;
    int        GetScoreValueForSort() const;

    double       GetRegionNumber() const;
    double       GetRemainingArea(int sensorIndex) const;
    double       GetSensorScore(int sensorIndex) const;
    double       GetScoreEfficiency(const Sensor *ptrSensor,
                                    double remainingTTC) const;

    bool        CanCollectWithSensor(int sensorIndex) const;
    bool        IsPointTarget() const;
    bool        IsAreaTarget() const;

    SpaceVector GetPosition(int timeIndex) const;

    inline double   GetTargetArea() const;
    inline double   GetCollectedArea(int sensorIndex) const;
    inline double   GetTotalCollectedArea() const;

    inline Region* GetRegion() const;

    inline Target* GetTarget() const;

protected:

private:




    double               targetArea_a;
    Array<double>       *ptrCollectedArea_a;
    Region             *ptrRegion_a;
    Target             *ptrTarget_a;
};

inline
double
TargetRegion::GetTargetArea() const
{
    return(targetArea_a);
}

inline
double
TargetRegion::GetCollectedArea(int sensorIndex) const
{
    return(ptrCollectedArea_a->ElementAt(sensorIndex));
}

inline
double
TargetRegion::GetTotalCollectedArea() const
{
    return(ptrCollectedArea_a->GetSum());
}

inline
Region*
TargetRegion::GetRegion() const
{
    return(ptrRegion_a);
}

inline
Target*
TargetRegion::GetTarget() const
{
    return(ptrTarget_a);
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
TargetRegion::GetClassName()
{
    return(TARGET_REGION_H);
}
*/

#endif
