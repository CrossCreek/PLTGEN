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
 * Requirement.h
 *
 * PLTGEN - Positions, Links, Timeline GENerator
 *
 * POC: Anthony Ragusa | Joe DiBruno | Ken Ivory (2018- )
 *============================================================================================*/


#ifndef REQUIREMENT_H
#define REQUIREMENT_H "Requirement"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>


#include "general/array/Array.h"

using namespace std;

class Sensor;

/**
 * Data class which represents a MURAL collection requirement generated from a requirement
 * or target deck
 *
 * Name                    Description (units)
 * -------------           -----------------------------------------------------
 * numberOfRequirements_s  total number of requirements
 * missionIndex_a          mission index of target(s)
 * qualityIndex_a          required quality (NIIRS or NRIS) index of target(s)
 * priorityIndex_a         index of the priority level assigned this requirement
 * subPriorityLevel_a      highest sub-priority level of target(s)
 * numberOfPoints_a        number of point targets in this requirement (if
 *                           this is a 'point' mission)
 * requirementIndex_a      sequential index assigned each requirement used for
 *                           any target type scheduling (requested or allocated
 *                           MVR or collection impact assessment)
 * score_a                 aggregate of raw score of the target(s)
 * area_a                  aggregate of collection area of the target(s) (nmi^2)
 * ptrRemainingScore_a*    score left to be collected on this requirement by
 *                           resource (one if not resource specific)
 * ptrRemainingArea_a*     area left to be collected on this requirement by
 *                           resource (one if not resource specific)
 *
 * @author Brian Fowler
 * @date 09/07/2011
 */
class Requirement
{
public:

    Requirement();
    Requirement(int sensorIndex,
                int missionIndex,
                int qualityIndex,
                int priorityIndex,
                int subPriority,
                double score,
                double area,
                int numberOfPoints,
                int numberOfResources);
    Requirement(const Requirement &copyMe);
    virtual ~Requirement();

    Requirement& operator =  (const Requirement &copyMe);
    bool         operator == (const Requirement &compareMe) const;
    bool         operator != (const Requirement &compareMe) const;

    void   AddScoreAndArea(double score,
                           double area,
                           int priorityIndex,
                           int subPriority);
    void   PrintRequirement(int sensorIndex,
                            int regionNumber,
                            int subRegionNumber,
                            ofstream &outfile) const;
    void   ResetScoreAndArea();

    int  GetMissionNumber() const;
    int  GetQualityLevel(int sensorIndex) const;
    int  GetPriorityLevel() const;
    int  GetAchievableNumberOfPoints(const Sensor *ptrSensor,
                                     double maximumSeconds,
                                     int resourceIndex = -1) const;
    int  GetRemainingNumberOfPoints(int resourceIndex = -1) const;

    double  GetScore(const Sensor *ptrSensor) const;
    double  GetTimeToCollect(const Sensor *ptrSensor) const;
    double  GetRemainingArea(int resourceIndex = -1) const;
    double  GetRemainingScore(const Sensor *ptrSensor,
                              int resourceIndex = -1) const;
    double  GetRemainingScoreEfficiency(const Sensor *ptrSensor,
                                        int resourceIndex = -1) const;
    double  GetRemainingTimeToCollect(const Sensor *ptrSensor,
                                      int resourceIndex = -1) const;
    double  GetAchievableCollectionTime(const Sensor *ptrSensor,
                                        double maximumSeconds,
                                        int resourceIndex = -1) const;
    double  GetAchievableArea(const Sensor *ptrSensor,
                              double maximumSeconds,
                              int resourceIndex = -1) const;
    double  GetAchievableScore(const Sensor *ptrSensor,
                               double maximumSeconds,
                               int resourceIndex = -1) const;
    double  UpdateRemainingScoreAndArea(const Sensor *ptrSensor,
                                        double maximumSeconds,
                                        int resourceIndex = -1);

    bool   IsPointRequirement() const;

    string GetMissionString() const;

    static inline int   GetNumberOfRequirements();

    inline int  GetSubPriorityLevel() const;
    inline int  GetNumberOfPoints() const;

    inline int   GetMissionIndex() const;
    inline int   GetQualityIndex() const;
    inline int   GetPriorityIndex() const;
    inline int   GetIndex() const;

    inline double  GetArea() const;
    inline double  GetScore() const;

    inline bool   EqualRequirement(int missionIndex,
                                   int qualityIndex,
                                   int priorityIndex) const;

protected:

private:

    double  GetResourceWeight(const Sensor *ptrSensor) const;
    double  GetCollectionRate(const Sensor *ptrSensor) const;
    double  GetSecondsPerPoint(const Sensor *ptrSensor) const;




    static int         numberOfRequirements_s;

    int               missionIndex_a;
    int               qualityIndex_a;
    int               priorityIndex_a;
    int               subPriorityLevel_a;
    int               numberOfPoints_a;
    int                requirementIndex_a;
    double               score_a;
    double               area_a;
    Array<double>       *ptrRemainingScore_a;
    Array<double>       *ptrRemainingArea_a;
};

inline
int
Requirement::GetNumberOfRequirements()
{
    return(numberOfRequirements_s);
}

inline
int
Requirement::GetMissionIndex() const
{
    return(missionIndex_a);
}

inline
int
Requirement::GetQualityIndex() const
{
    return(qualityIndex_a);
}

inline
int
Requirement::GetPriorityIndex() const
{
    return(priorityIndex_a);
}

inline
int
Requirement::GetSubPriorityLevel() const
{
    return(subPriorityLevel_a);
}

inline
int
Requirement::GetNumberOfPoints() const
{
    return(numberOfPoints_a);
}

inline
int
Requirement::GetIndex() const
{
    return(requirementIndex_a);
}

inline
double
Requirement::GetArea() const
{
    return(area_a);
}

inline
double
Requirement::GetScore() const
{
    return(score_a);
}

inline
bool
Requirement::EqualRequirement(int missionIndex,
                              int qualityIndex,
                              int priorityIndex) const
{
    return( (missionIndex_a == missionIndex) &&
            (qualityIndex_a == qualityIndex) &&
            (priorityIndex_a == priorityIndex) );
}

//CPPCHECK_RELATED
//Removed GetClassName() since it wasn't called in code base
/*
inline
string
Requirement::GetClassName()
{
    return(REQUIREMENT_H);
}
*/

#endif
